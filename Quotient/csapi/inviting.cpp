// THIS FILE IS GENERATED - ANY EDITS WILL BE OVERWRITTEN

#include "inviting.h"

using namespace Quotient;

InviteUserJob::InviteUserJob(const QString& roomId, const QString& userId, const QString& reason)
    : BaseJob(HttpVerb::Post, QStringLiteral("InviteUserJob"),
              makePath("/_matrix/client/v3", "/rooms/", roomId, "/invite"))
{
    QJsonObject _dataJson;
    addParam<>(_dataJson, QStringLiteral("user_id"), userId);
    addParam<IfNotEmpty>(_dataJson, QStringLiteral("reason"), reason);
    setRequestData({ _dataJson });
}
