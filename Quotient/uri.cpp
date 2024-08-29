// SPDX-FileCopyrightText: 2020 Kitsune Ral <kitsune-ral@users.sf.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "uri.h"

#include "logging_categories_p.h"
#include "util.h"

#include <QtCore/QRegularExpression>

using namespace Quotient;

namespace {

struct ReplacePair { QLatin1String uriString; char sigil; };
/// \brief Defines bi-directional mapping of path prefixes and sigils
///
/// When there are two prefixes for the same sigil, the first matching
/// entry for a given sigil is used.
const ReplacePair replacePairs[] = {
    { "u/"_L1, '@' },
    { "user/"_L1, '@' },
    { "roomid/"_L1, '!' },
    { "r/"_L1, '#' },
    { "room/"_L1, '#' },
    // The notation for bare event ids is not proposed in MSC2312 but there's
    // https://github.com/matrix-org/matrix-doc/pull/2644
    { "e/"_L1, '$' },
    { "event/"_L1, '$' }
};

}

Uri::Uri(QByteArray primaryId, QByteArray secondaryId, QString query)
{
    if (primaryId.isEmpty())
        primaryType_ = Empty;
    else {
        setScheme("matrix"_L1);
        QString pathToBe;
        primaryType_ = Invalid;
        if (primaryId.size() < 2) // There should be something after sigil
            return;
        for (const auto& p: replacePairs)
            if (primaryId[0] == p.sigil) {
                primaryType_ = Type(p.sigil);
                auto safePrimaryId = primaryId.mid(1);
                safePrimaryId.replace('/', "%2F");
                pathToBe = p.uriString + QString::fromUtf8(safePrimaryId);
                break;
            }
        if (!secondaryId.isEmpty()) {
            if (secondaryId.size() < 2) {
                primaryType_ = Invalid;
                return;
            }
            auto safeSecondaryId = secondaryId.mid(1);
            safeSecondaryId.replace('/', "%2F");
            pathToBe += "/event/"_L1 + QString::fromUtf8(safeSecondaryId);
        }
        setPath(pathToBe, QUrl::TolerantMode);
    }
    if (!query.isEmpty())
        setQuery(query);
}

static inline auto encodedPath(const QUrl& url)
{
    return url.path(QUrl::EncodeDelimiters | QUrl::EncodeUnicode);
}

static QString pathSegment(const QUrl& url, int which)
{
    return QUrl::fromPercentEncoding(
        encodedPath(url).section(u'/', which, which).toUtf8());
}

static auto decodeFragmentPart(QStringView part)
{
    return QUrl::fromPercentEncoding(part.toLatin1()).toUtf8();
}

static inline auto matrixToUrlRegexInit()
{
    // See https://matrix.org/docs/spec/appendices#matrix-to-navigation
    const QRegularExpression MatrixToUrlRE {
        "^/(?<main>[^:]+(:|%3A|%3a)[^/?]+)(/(?<sec>(\\$|%24)[^?]+))?(\\?(?<query>.+))?$"_L1
    };
    Q_ASSERT(MatrixToUrlRE.isValid());
    return MatrixToUrlRE;
}

Uri::Uri(QUrl url) : QUrl(std::move(url))
{
    // NB: don't try to use `url` from here on, it's moved-from and empty
    if (isEmpty())
        return; // primaryType_ == Empty

    primaryType_ = Invalid;
    if (!QUrl::isValid()) // MatrixUri::isValid() checks primaryType_
        return;

    if (scheme() == "matrix"_L1) {
        // Check sanity as per https://github.com/matrix-org/matrix-doc/pull/2312
        const auto& urlPath = encodedPath(*this);
        const auto& splitPath = urlPath.split(u'/');
        switch (splitPath.size()) {
        case 2:
            break;
        case 4:
            if (splitPath[2] == "event"_L1 || splitPath[2] == "e"_L1)
                break;
            [[fallthrough]];
        default:
            return; // Invalid
        }

        for (const auto& p: replacePairs)
            if (urlPath.startsWith(p.uriString)) {
                primaryType_ = Type(p.sigil);
                return; // The only valid return path for matrix: URIs
            }
        qCDebug(MAIN) << "The matrix: URI is not recognised:"
                      << toDisplayString();
        return;
    }

    primaryType_ = NonMatrix; // Default, unless overridden by the code below
    if (scheme() == "https"_L1 && authority() == "matrix.to"_L1) {
        static const auto MatrixToUrlRE = matrixToUrlRegexInit();
        // matrix.to accepts both literal sigils (as well as & and ? used in
        // its "query" substitute) and their %-encoded forms;
        // so force QUrl to decode everything.
        auto f = fragment(QUrl::EncodeUnicode);
        if (auto&& m = MatrixToUrlRE.match(f); m.hasMatch())
            *this = Uri { decodeFragmentPart(m.capturedView(u"main")),
                          decodeFragmentPart(m.capturedView(u"sec")),
                          QString::fromUtf8(decodeFragmentPart(m.capturedView(u"query"))) };
    }
}

Uri::Uri(const QString& uriOrId) : Uri(fromUserInput(uriOrId)) {}

Uri Uri::fromUserInput(const QString& uriOrId)
{
    if (uriOrId.isEmpty())
        return {}; // type() == None

    // A quick check if uriOrId is a plain Matrix id
    // Bare event ids cannot be resolved without a room scope as per the current
    // spec but there's a movement towards making them navigable (see, e.g.,
    // https://github.com/matrix-org/matrix-doc/pull/2644) - so treat them
    // as valid
    if (QStringLiteral("!@#+$").contains(uriOrId[0]))
        return Uri { uriOrId.toUtf8() };

    return Uri { QUrl::fromUserInput(uriOrId) };
}

Uri::Type Uri::type() const { return primaryType_; }

Uri::SecondaryType Uri::secondaryType() const
{
    const auto& type2 = pathSegment(*this, 2);
    return type2 == "event"_L1 || type2 == "e"_L1 ? EventId : NoSecondaryId;
}

QUrl Uri::toUrl(UriForm form) const
{
    if (!isValid())
        return {};

    if (form == CanonicalUri || type() == NonMatrix)
        return SLICE(*this, QUrl);

    QUrl url;
    url.setScheme("https"_L1);
    url.setHost("matrix.to"_L1);
    url.setPath("/"_L1);
    auto fragment = u'/' + primaryId();
    if (const auto& secId = secondaryId(); !secId.isEmpty())
        fragment += u'/' + secId;
    if (const auto& q = query(); !q.isEmpty())
        fragment += u'?' + q;
    url.setFragment(fragment);
    return url;
}

QString Uri::primaryId() const
{
    if (primaryType_ == Empty || primaryType_ == Invalid)
        return {};

    auto idStem = pathSegment(*this, 1);
    if (!idStem.isEmpty())
        idStem.push_front(QChar::fromLatin1(primaryType_));
    return idStem;
}

QString Uri::secondaryId() const
{
    auto idStem = pathSegment(*this, 3);
    if (!idStem.isEmpty())
        idStem.push_front(QChar::fromLatin1(secondaryType()));
    return idStem;
}

static const auto ActionKey = QStringLiteral("action");

QString Uri::action() const
{
    return type() == NonMatrix || !isValid()
               ? QString()
               : QUrlQuery { query() }.queryItemValue(ActionKey);
}

void Uri::setAction(const QString& newAction)
{
    if (!isValid()) {
        qCWarning(MAIN) << "Cannot set an action on an invalid Quotient::Uri";
        return;
    }
    QUrlQuery q { query() };
    q.removeQueryItem(ActionKey);
    q.addQueryItem(ActionKey, newAction);
    setQuery(q);
}

QStringList Uri::viaServers() const
{
    return QUrlQuery { query() }.allQueryItemValues(QStringLiteral("via"),
                                                    QUrl::EncodeReserved);
}

bool Uri::isValid() const
{
    return primaryType_ != Empty && primaryType_ != Invalid;
}
