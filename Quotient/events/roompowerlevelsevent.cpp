// SPDX-FileCopyrightText: 2019 Black Hat <bhat@encom.eu.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "roompowerlevelsevent.h"

using namespace Quotient;

// The default values used below are defined in
// https://spec.matrix.org/v1.3/client-server-api/#mroompower_levels
PowerLevelsEventContent::PowerLevelsEventContent(const QJsonObject& json) :
    invite(json["invite"_L1].toInt(50)),
    kick(json["kick"_L1].toInt(50)),
    ban(json["ban"_L1].toInt(50)),
    redact(json["redact"_L1].toInt(50)),
    events(fromJson<QHash<QString, int>>(json["events"_L1])),
    eventsDefault(json["events_default"_L1].toInt(0)),
    stateDefault(json["state_default"_L1].toInt(0)),
    users(fromJson<QHash<QString, int>>(json["users"_L1])),
    usersDefault(json["users_default"_L1].toInt(0)),
    notifications(Notifications{json["notifications"_L1].toObject()["room"_L1].toInt(50)})
{}

QJsonObject PowerLevelsEventContent::toJson() const
{
    QJsonObject o;
    o.insert(QStringLiteral("invite"), invite);
    o.insert(QStringLiteral("kick"), kick);
    o.insert(QStringLiteral("ban"), ban);
    o.insert(QStringLiteral("redact"), redact);
    o.insert(QStringLiteral("events"), Quotient::toJson(events));
    o.insert(QStringLiteral("events_default"), eventsDefault);
    o.insert(QStringLiteral("state_default"), stateDefault);
    o.insert(QStringLiteral("users"), Quotient::toJson(users));
    o.insert(QStringLiteral("users_default"), usersDefault);
    o.insert(QStringLiteral("notifications"),
             QJsonObject { { "room"_L1, notifications.room } });
    return o;
}

int RoomPowerLevelsEvent::powerLevelForEvent(const QString& eventTypeId) const
{
    return events().value(eventTypeId, eventsDefault());
}

int RoomPowerLevelsEvent::powerLevelForState(const QString& eventTypeId) const
{
    return events().value(eventTypeId, stateDefault());
}

int RoomPowerLevelsEvent::powerLevelForUser(const QString& userId) const
{
    return users().value(userId, usersDefault());
}
