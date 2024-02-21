/******************************************************************************
 * THIS FILE IS GENERATED - ANY EDITS WILL BE OVERWRITTEN
 */

#include "third_party_lookup.h"

using namespace Quotient;

QUrl GetProtocolsJob::makeRequestUrl(QUrl baseUrl)
{
    return BaseJob::makeRequestUrl(std::move(baseUrl),
                                   makePath("/_matrix/client/v3",
                                            "/thirdparty/protocols"));
}

GetProtocolsJob::GetProtocolsJob()
    : BaseJob(HttpVerb::Get, QStringLiteral("GetProtocolsJob"),
              makePath("/_matrix/client/v3", "/thirdparty/protocols"))
{}

QUrl GetProtocolMetadataJob::makeRequestUrl(QUrl baseUrl,
                                            const QString& protocol)
{
    return BaseJob::makeRequestUrl(std::move(baseUrl),
                                   makePath("/_matrix/client/v3",
                                            "/thirdparty/protocol/", protocol));
}

GetProtocolMetadataJob::GetProtocolMetadataJob(const QString& protocol)
    : BaseJob(HttpVerb::Get, QStringLiteral("GetProtocolMetadataJob"),
              makePath("/_matrix/client/v3", "/thirdparty/protocol/", protocol))
{}

auto queryToQueryLocationByProtocol(const QString& searchFields)
{
    QUrlQuery _q;
    addParam<IfNotEmpty>(_q, QStringLiteral("searchFields"), searchFields);
    return _q;
}

QUrl QueryLocationByProtocolJob::makeRequestUrl(QUrl baseUrl,
                                                const QString& protocol,
                                                const QString& searchFields)
{
    return BaseJob::makeRequestUrl(std::move(baseUrl),
                                   makePath("/_matrix/client/v3",
                                            "/thirdparty/location/", protocol),
                                   queryToQueryLocationByProtocol(searchFields));
}

QueryLocationByProtocolJob::QueryLocationByProtocolJob(
    const QString& protocol, const QString& searchFields)
    : BaseJob(HttpVerb::Get, QStringLiteral("QueryLocationByProtocolJob"),
              makePath("/_matrix/client/v3", "/thirdparty/location/", protocol),
              queryToQueryLocationByProtocol(searchFields))
{}

auto queryToQueryUserByProtocol(const QHash<QString, QString>& fields)
{
    QUrlQuery _q;
    addParam<IfNotEmpty>(_q, QStringLiteral("fields"), fields);
    return _q;
}

QUrl QueryUserByProtocolJob::makeRequestUrl(
    QUrl baseUrl, const QString& protocol, const QHash<QString, QString>& fields)
{
    return BaseJob::makeRequestUrl(std::move(baseUrl),
                                   makePath("/_matrix/client/v3",
                                            "/thirdparty/user/", protocol),
                                   queryToQueryUserByProtocol(fields));
}

QueryUserByProtocolJob::QueryUserByProtocolJob(
    const QString& protocol, const QHash<QString, QString>& fields)
    : BaseJob(HttpVerb::Get, QStringLiteral("QueryUserByProtocolJob"),
              makePath("/_matrix/client/v3", "/thirdparty/user/", protocol),
              queryToQueryUserByProtocol(fields))
{}

auto queryToQueryLocationByAlias(const QString& alias)
{
    QUrlQuery _q;
    addParam<>(_q, QStringLiteral("alias"), alias);
    return _q;
}

QUrl QueryLocationByAliasJob::makeRequestUrl(QUrl baseUrl, const QString& alias)
{
    return BaseJob::makeRequestUrl(std::move(baseUrl),
                                   makePath("/_matrix/client/v3",
                                            "/thirdparty/location"),
                                   queryToQueryLocationByAlias(alias));
}

QueryLocationByAliasJob::QueryLocationByAliasJob(const QString& alias)
    : BaseJob(HttpVerb::Get, QStringLiteral("QueryLocationByAliasJob"),
              makePath("/_matrix/client/v3", "/thirdparty/location"),
              queryToQueryLocationByAlias(alias))
{}

auto queryToQueryUserByID(const QString& userid)
{
    QUrlQuery _q;
    addParam<>(_q, QStringLiteral("userid"), userid);
    return _q;
}

QUrl QueryUserByIDJob::makeRequestUrl(QUrl baseUrl, const QString& userid)
{
    return BaseJob::makeRequestUrl(std::move(baseUrl),
                                   makePath("/_matrix/client/v3",
                                            "/thirdparty/user"),
                                   queryToQueryUserByID(userid));
}

QueryUserByIDJob::QueryUserByIDJob(const QString& userid)
    : BaseJob(HttpVerb::Get, QStringLiteral("QueryUserByIDJob"),
              makePath("/_matrix/client/v3", "/thirdparty/user"),
              queryToQueryUserByID(userid))
{}
