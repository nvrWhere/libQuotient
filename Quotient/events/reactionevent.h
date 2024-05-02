// SPDX-FileCopyrightText: 2019 Kitsune Ral <kitsune-ral@users.sf.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "roomevent.h"
#include "eventrelation.h"

namespace Quotient {

class QUOTIENT_API ReactionEvent
    : public EventTemplate<
          ReactionEvent, RoomEvent,
          EventContent::SingleKeyValue<EventRelation, RelatesToKey>> {
public:
    QUO_EVENT(ReactionEvent, "m.reaction")
    static bool isValid(const QJsonObject& fullJson)
    {
        return fullJson[ContentKey][RelatesToKey][RelTypeKey].toString()
               == EventRelation::AnnotationType;
    }

    ReactionEvent(const QString& eventId, const QString& reactionKey)
        : EventTemplate(EventRelation::annotate(eventId, reactionKey))
    {}

    QString eventId() const { return content().value.eventId; }
    QString key() const { return content().value.key; }

private:
    explicit ReactionEvent(const QJsonObject& json) : EventTemplate(json) {}
};

} // namespace Quotient
