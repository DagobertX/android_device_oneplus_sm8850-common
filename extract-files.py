#!/usr/bin/env -S PYTHONPATH=../../../tools/extract-utils python3
#
# SPDX-FileCopyrightText: 2024 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

from extract_utils.fixups_blob import (
    blob_fixup,
    blob_fixups_user_type,
)
from extract_utils.fixups_lib import (
    lib_fixups,
    lib_fixups_user_type,
)
from extract_utils.main import (
    ExtractUtils,
    ExtractUtilsModule,
)

namespace_imports = [
    'device/oneplus/sm8850-common',
    'hardware/qcom-caf/sm8850',
    'hardware/qcom-caf/wlan',
    'hardware/oplus',
    'vendor/qcom/opensource/commonsys/display',
    'vendor/qcom/opensource/commonsys-intf/display',
    'vendor/qcom/opensource/dataservices',
]

def lib_fixup_odm_suffix(lib: str, partition: str, *args, **kwargs):
    return f'{lib}_{partition}' if partition == 'odm' else None

def lib_fixup_vendor_suffix(lib: str, partition: str, *args, **kwargs):
    return f'{lib}_{partition}' if partition == 'vendor' else None

lib_fixups: lib_fixups_user_type = {
    **lib_fixups,
    (
        'com.qualcomm.qti.dpm.api@1.0',
        'libosensenativeproxy_client',
        'vendor.qti.ImsRtpService-V2-ndk',
        'vendor.qti.hardware.dpmaidlservice-V1-ndk',
        'vendor.qti.hardware.wifidisplaysession_aidl-V1-ndk',
        'vendor.qti.qccvndhal_aidl-V1-ndk',
    ): lib_fixup_vendor_suffix,
}

blob_fixups: blob_fixups_user_type = {
    'odm/bin/hw/vendor.oplus.hardware.biometrics.fingerprint@2.1-service_uff': blob_fixup()
        .add_needed('libshims_aidl_fingerprint_v3.oplus.so'),
    'product/etc/sysconfig/com.android.hotwordenrollment.common.util.xml': blob_fixup()
        .regex_replace('/my_product', '/product'),
}  # fmt: skip

module = ExtractUtilsModule(
    'sm8850-common',
    'oneplus',
    blob_fixups=blob_fixups,
    lib_fixups=lib_fixups,
    namespace_imports=namespace_imports,
)
module.add_proprietary_file('proprietary-files-phone.txt').add_copy_files_guard(
    'TARGET_IS_TABLET', 'true', invert=True
)

if __name__ == '__main__':
    utils = ExtractUtils.device(module)
    utils.run()
