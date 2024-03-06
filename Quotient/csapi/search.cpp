// THIS FILE IS GENERATED - ANY EDITS WILL BE OVERWRITTEN

#include "search.h"

using namespace Quotient;

auto queryToSearch(const QString& nextBatch)
{
    QUrlQuery _q;
    addParam<IfNotEmpty>(_q, QStringLiteral("next_batch"), nextBatch);
    return _q;
}

SearchJob::SearchJob(const Categories& searchCategories, const QString& nextBatch)
    : BaseJob(HttpVerb::Post, QStringLiteral("SearchJob"),
              makePath("/_matrix/client/v3", "/search"), queryToSearch(nextBatch))
{
    QJsonObject _dataJson;
    addParam<>(_dataJson, QStringLiteral("search_categories"), searchCategories);
    setRequestData({ _dataJson });
    addExpectedKey("search_categories");
}
