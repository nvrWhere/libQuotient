// THIS FILE IS GENERATED - ANY EDITS WILL BE OVERWRITTEN

#pragma once

#include <Quotient/converters.h>

namespace Quotient {
//! A client device
struct QUOTIENT_API Device {
    //! Identifier of this device.
    QString deviceId;

    //! Display name set by the user for this device. Absent if no name has been
    //! set.
    QString displayName{};

    //! The IP address where this device was last seen. (May be a few minutes out
    //! of date, for efficiency reasons).
    QString lastSeenIp{};

    //! The timestamp (in milliseconds since the unix epoch) when this devices
    //! was last seen. (May be a few minutes out of date, for efficiency
    //! reasons).
    std::optional<qint64> lastSeenTs{};
};

template <>
struct JsonObjectConverter<Device> {
    static void dumpTo(QJsonObject& jo, const Device& pod)
    {
        addParam<>(jo, "device_id"_L1, pod.deviceId);
        addParam<IfNotEmpty>(jo, "display_name"_L1, pod.displayName);
        addParam<IfNotEmpty>(jo, "last_seen_ip"_L1, pod.lastSeenIp);
        addParam<IfNotEmpty>(jo, "last_seen_ts"_L1, pod.lastSeenTs);
    }
    static void fillFrom(const QJsonObject& jo, Device& pod)
    {
        fillFromJson(jo.value("device_id"_L1), pod.deviceId);
        fillFromJson(jo.value("display_name"_L1), pod.displayName);
        fillFromJson(jo.value("last_seen_ip"_L1), pod.lastSeenIp);
        fillFromJson(jo.value("last_seen_ts"_L1), pod.lastSeenTs);
    }
};

} // namespace Quotient
