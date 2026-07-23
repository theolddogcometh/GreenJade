/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Vulkan ICD loader interface (names from public loader ABI docs).
 */
#pragma once

#include <vulkan/vulkan_core_gj.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ICD interface versions used by the Vulkan loader.
 * GreenJade implements version-5-shaped negotiate + GetInstanceProcAddr.
 */
#define CURRENT_LOADER_ICD_INTERFACE_VERSION 5
#define MIN_SUPPORTED_LOADER_ICD_INTERFACE_VERSION 0

typedef VkResult(VKAPI_PTR *PFN_vk_icdNegotiateLoaderICDInterfaceVersion)(
    uint32_t *pVersion);
typedef PFN_vkVoidFunction(VKAPI_PTR *PFN_vk_icdGetInstanceProcAddr)(
    VkInstance instance, const char *pName);
typedef PFN_vkVoidFunction(VKAPI_PTR *PFN_vk_icdGetPhysicalDeviceProcAddr)(
    VkInstance instance, const char *pName);

VKAPI_ATTR VkResult VKAPI_CALL
vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t *pSupportedVersion);

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vk_icdGetInstanceProcAddr(VkInstance instance, const char *pName);

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vk_icdGetPhysicalDeviceProcAddr(VkInstance instance, const char *pName);

#ifdef __cplusplus
}
#endif
