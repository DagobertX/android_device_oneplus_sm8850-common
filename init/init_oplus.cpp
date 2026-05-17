/*
 * Copyright (C) 2022-2025 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/logging.h>
#include <android-base/parseint.h>
#include <android-base/properties.h>

#include <fs_mgr.h>

#include <string>
#include <unordered_map>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

using android::base::GetProperty;
namespace android {
namespace init {
uint32_t InitPropertySet(const std::string& name, const std::string& value);
}  // namespace init
}  // namespace android

using android::base::ParseInt;
using android::fs_mgr::GetKernelCmdline;
using android::init::InitPropertySet;

namespace {
constexpr std::string kCmdlineRegion = "oplus_region";
const std::unordered_map<int, std::string> kRegionMap = {
        {27, "IN"},
        {68, "EU"},
        {151, "CN"},
        {161, "NA"},
        {167, "ROW"},
};
}  // anonymous namespace

/*
 * Only for read-only properties. Properties that can be wrote to more
 * than once should be set in a typical init script (e.g. init.oplus.hw.rc)
 * after the original property has been set.
 */
void vendor_process_bootenv() {
    std::string buf;
    if (!GetKernelCmdline(kCmdlineRegion, &buf)) {
        LOG(ERROR) << kCmdlineRegion << " not found in /proc/cmdline";
        return;
    }

    int region_id;
    if (!ParseInt(buf, &region_id)) {
        LOG(ERROR) << "Region ID [" << buf << "] is invalid";
        return;
    }

    auto it = kRegionMap.find(region_id);
    if (it == kRegionMap.end()) {
        LOG(ERROR) << "Unexpected region ID: " << region_id;
    } else {
        InitPropertySet("ro.boot.hardware.revision", it->second);
    }
}

/*
 * SetProperty does not allow updating read only properties and as a result
 * does not work for our use case. Write "OverrideProperty" to do practically
 * the same thing as "SetProperty" without this restriction.
 */
void OverrideProperty(const char* name, const char* value) {
    size_t valuelen = strlen(value);

    prop_info* pi = (prop_info*)__system_property_find(name);
    if (pi != nullptr) {
        __system_property_update(pi, value, valuelen);
    } else {
        __system_property_add(name, strlen(name), value, valuelen);
    }
}

/*
 * Only for read-only properties. Properties that can be wrote to more
 * than once should be set in a typical init script (e.g. init.oplus.hw.rc)
 * after the original property has been set.
 */
void vendor_load_properties() {
    auto device = GetProperty("ro.product.product.device", "");
    auto prjname = std::stoi(GetProperty("ro.boot.prjname", "0"));
    auto rf_version = std::stoi(GetProperty("ro.boot.rf_version", "0"));

    switch (rf_version) {
        case 151: // CN
            if (device == "OP60FFL1") {
                OverrideProperty("ro.product.product.model", "PLK110");
            }
            break;
        case 27: // IN
            if (device == "OP611FL1") {
                OverrideProperty("ro.product.product.model", "CPH2745");
            }
            break;
        case 68: // EU
            if (device == "OP611FL1") {
                OverrideProperty("ro.product.product.model", "CPH2747");
            }
            break;
        case 161: // NA
            if (device == "OP611FL1") {
                OverrideProperty("ro.product.product.model", "CPH2749");
            }
            break;
        case 167: // ROW
            if (device == "OP611FL1") {
                OverrideProperty("ro.product.product.model", "CPH2747");
            }
            break;
        default:
            LOG(ERROR) << "Unexpected RF version: " << rf_version;
    }
}
