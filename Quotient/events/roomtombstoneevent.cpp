// SPDX-FileCopyrightText: 2019 Kitsune Ral <Kitsune-Ral@users.sf.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "roomtombstoneevent.h"

using namespace Quotient;

QString RoomTombstoneEvent::serverMessage() const
{
    return contentPart<QString>("body"_L1);
}

QString RoomTombstoneEvent::successorRoomId() const
{
    return contentPart<QString>("replacement_room"_L1);
}
