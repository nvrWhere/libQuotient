// SPDX-FileCopyrightText: 2019 Kitsune Ral <Kitsune-Ral@users.sf.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "quotient_export.h"

#include <qobjectdefs.h>
#include "util.h"

#include <array>


//! \brief Quotient replacement for the Q_FLAG/Q_DECLARE_FLAGS combination
//!
//! Although the comment in QTBUG-82295 says that Q_FLAG[_NS] "should" be
//! applied to the enum type only, Qt then doesn't allow to wrap the
//! corresponding flag type (defined with Q_DECLARE_FLAGS) into a QVariant.
//! This macro defines Q_FLAG and on top of that adds Q_ENUM_IMPL which is
//! a part of Q_ENUM() macro that enables the metatype data but goes under
//! the moc radar to avoid double registration of the same data in the map
//! defined in moc_*.cpp.
//!
//! Simply put, instead of using Q_FLAG/Q_DECLARE_FLAGS combo (and struggling
//! to figure out what you should pass to Q_FLAG if you want to make it
//! wrappable in a QVariant) use the macro below, and things will just work.
//!
//! \sa https://bugreports.qt.io/browse/QTBUG-82295
#define QUO_DECLARE_FLAGS(Flags, Enum) \
    Q_DECLARE_FLAGS(Flags, Enum)       \
    Q_ENUM_IMPL(Enum)                  \
    Q_FLAG(Flags)

//! \brief Quotient replacement for the Q_FLAG_NS/Q_DECLARE_FLAGS combination
//!
//! This is the equivalent of QUO_DECLARE_FLAGS for enums declared at the
//! namespace level (be sure to provide Q_NAMESPACE _in the same file_
//! as the enum definition and this macro).
//! \sa QUO_DECLARE_FLAGS
#define QUO_DECLARE_FLAGS_NS(Flags, Enum) \
    Q_DECLARE_FLAGS(Flags, Enum)          \
    Q_ENUM_NS_IMPL(Enum)                  \
    Q_FLAG_NS(Flags)

namespace Quotient {
Q_NAMESPACE_EXPORT(QUOTIENT_API)

// TODO: code like this should be generated from the CS API definition

//! \brief Membership states
//!
//! These are used for member events. The names here are case-insensitively
//! equal to state names used on the wire.
//! \sa MemberEventContent, RoomMemberEvent
enum class Membership : uint16_t {
    // Specific power-of-2 values (1,2,4,...) are important here as syncdata.cpp
    // depends on that, as well as Join being the first in line
    Invalid = 0x0,
    Join = 0x1,
    Leave = 0x2,
    Invite = 0x4,
    Knock = 0x8,
    Ban = 0x10,
    Undefined = Invalid
};
QUO_DECLARE_FLAGS_NS(MembershipMask, Membership)

constexpr std::array MembershipStrings {
    // The order MUST be the same as the order in the Membership enum
    "join"_L1, "leave"_L1, "invite"_L1, "knock"_L1, "ban"_L1
};

//! \brief Local user join-state names
//!
//! This represents a subset of Membership values that may arrive as the local
//! user's state grouping for the sync response.
//! \sa SyncData
enum class JoinState : std::underlying_type_t<Membership> {
    Invalid = std::underlying_type_t<Membership>(Membership::Invalid),
    Join = std::underlying_type_t<Membership>(Membership::Join),
    Leave = std::underlying_type_t<Membership>(Membership::Leave),
    Invite = std::underlying_type_t<Membership>(Membership::Invite),
    Knock = std::underlying_type_t<Membership>(Membership::Knock),
};
QUO_DECLARE_FLAGS_NS(JoinStates, JoinState)

[[maybe_unused]] constexpr std::array JoinStateStrings {
    MembershipStrings[0], MembershipStrings[1], MembershipStrings[2],
    MembershipStrings[3] /* same as MembershipStrings, sans "ban" */
};

//! \brief Network job running policy flags
//!
//! So far only background/foreground flags are available.
//! \sa Connection::callApi, Connection::run
enum RunningPolicy { ForegroundRequest = 0x0, BackgroundRequest = 0x1 };
Q_ENUM_NS(RunningPolicy)

//! \brief The result of URI resolution using UriResolver
//! \sa UriResolver
enum UriResolveResult : int8_t {
    StillResolving = -1,
    UriResolved = 0,
    CouldNotResolve,
    IncorrectAction,
    InvalidUri,
    NoAccount
};
Q_ENUM_NS(UriResolveResult)

enum class RoomType : uint8_t {
    Space = 0,
    Undefined = 0xFF,
};
Q_ENUM_NS(RoomType)

[[maybe_unused]] constexpr std::array RoomTypeStrings { "m.space"_L1 };

enum class EncryptionType : uint8_t {
    MegolmV1AesSha2 = 0,
    Undefined = 0xFF,
};
Q_ENUM_NS(EncryptionType)

} // namespace Quotient
Q_DECLARE_OPERATORS_FOR_FLAGS(Quotient::MembershipMask)
Q_DECLARE_OPERATORS_FOR_FLAGS(Quotient::JoinStates)
