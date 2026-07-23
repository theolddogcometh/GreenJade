/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room subset of Vulkan 1.0 / KHR_swapchain types and entry points.
 * Sized for GreenJade ICD T0 (virtio-gpu 2D present). Not a full SDK.
 */
#pragma once

#include <vulkan/vk_platform.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t VkFlags;
typedef uint32_t VkBool32;
typedef uint64_t VkDeviceSize;
typedef uint32_t VkSampleMask;

#define VK_TRUE  1u
#define VK_FALSE 0u
#define VK_NULL_HANDLE 0

#define VK_MAKE_VERSION(major, minor, patch) \
    ((((uint32_t)(major)) << 22) | (((uint32_t)(minor)) << 12) | ((uint32_t)(patch)))
#define VK_API_VERSION_1_0 VK_MAKE_VERSION(1, 0, 0)
#define VK_HEADER_VERSION 1

#define VK_MAX_PHYSICAL_DEVICE_NAME_SIZE 256
#define VK_UUID_SIZE                     16
#define VK_MAX_EXTENSION_NAME_SIZE       256
#define VK_MAX_DESCRIPTION_SIZE          256
#define VK_MAX_MEMORY_TYPES              32
#define VK_MAX_MEMORY_HEAPS              16

typedef enum VkResult {
    VK_SUCCESS = 0,
    VK_NOT_READY = 1,
    VK_TIMEOUT = 2,
    VK_EVENT_SET = 3,
    VK_EVENT_RESET = 4,
    VK_INCOMPLETE = 5,
    VK_ERROR_OUT_OF_HOST_MEMORY = -1,
    VK_ERROR_OUT_OF_DEVICE_MEMORY = -2,
    VK_ERROR_INITIALIZATION_FAILED = -3,
    VK_ERROR_DEVICE_LOST = -4,
    VK_ERROR_MEMORY_MAP_FAILED = -5,
    VK_ERROR_LAYER_NOT_PRESENT = -6,
    VK_ERROR_EXTENSION_NOT_PRESENT = -7,
    VK_ERROR_FEATURE_NOT_PRESENT = -8,
    VK_ERROR_INCOMPATIBLE_DRIVER = -9,
    VK_ERROR_TOO_MANY_OBJECTS = -10,
    VK_ERROR_FORMAT_NOT_SUPPORTED = -11,
    VK_ERROR_FRAGMENTED_POOL = -12,
    VK_ERROR_UNKNOWN = -13,
    VK_ERROR_SURFACE_LOST_KHR = -1000000000,
    VK_ERROR_NATIVE_WINDOW_IN_USE_KHR = -1000000001,
    VK_SUBOPTIMAL_KHR = 1000001003,
    VK_ERROR_OUT_OF_DATE_KHR = -1000001004
} VkResult;

typedef enum VkStructureType {
    VK_STRUCTURE_TYPE_APPLICATION_INFO = 0,
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO = 1,
    VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO = 2,
    VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO = 3,
    VK_STRUCTURE_TYPE_SUBMIT_INFO = 4,
    VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO = 5,
    VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE = 6,
    VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO = 12,
    VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO = 14,
    VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO = 15,
    VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO = 16,
    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO = 18,
    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO = 19,
    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO = 20,
    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO = 22,
    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO = 23,
    VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO = 28,
    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO = 30,
    VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO = 31,
    VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO = 37,
    VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO = 38,
    VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO = 39,
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO = 40,
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO = 42,
    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO = 43,
    VK_STRUCTURE_TYPE_FENCE_CREATE_INFO = 8,
    VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO = 9,
    VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR = 1000001000,
    VK_STRUCTURE_TYPE_PRESENT_INFO_KHR = 1000001001,
    VK_STRUCTURE_TYPE_MAX_ENUM = 0x7FFFFFFF
} VkStructureType;

/* Shader stages */
#define VK_SHADER_STAGE_VERTEX_BIT   0x00000001u
#define VK_SHADER_STAGE_FRAGMENT_BIT 0x00000010u
#define VK_SHADER_STAGE_ALL_GRAPHICS 0x0000001Fu

/* Buffer usage */
#define VK_BUFFER_USAGE_TRANSFER_SRC_BIT 0x00000001u
#define VK_BUFFER_USAGE_TRANSFER_DST_BIT 0x00000002u
#define VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT 0x00000010u
#define VK_BUFFER_USAGE_INDEX_BUFFER_BIT 0x00000040u
#define VK_BUFFER_USAGE_VERTEX_BUFFER_BIT 0x00000080u

/* Primitive topology */
#define VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST 3u

/* SPIR-V magic (Khronos) */
#define GJ_SPV_MAGIC 0x07230203u

typedef enum VkSystemAllocationScope {
    VK_SYSTEM_ALLOCATION_SCOPE_COMMAND = 0,
    VK_SYSTEM_ALLOCATION_SCOPE_OBJECT = 1,
    VK_SYSTEM_ALLOCATION_SCOPE_CACHE = 2,
    VK_SYSTEM_ALLOCATION_SCOPE_DEVICE = 3,
    VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE = 4
} VkSystemAllocationScope;

typedef enum VkInternalAllocationType {
    VK_INTERNAL_ALLOCATION_TYPE_EXECUTABLE = 0
} VkInternalAllocationType;

typedef enum VkPhysicalDeviceType {
    VK_PHYSICAL_DEVICE_TYPE_OTHER = 0,
    VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU = 1,
    VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU = 2,
    VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU = 3,
    VK_PHYSICAL_DEVICE_TYPE_CPU = 4
} VkPhysicalDeviceType;

typedef enum VkFormat {
    VK_FORMAT_UNDEFINED = 0,
    VK_FORMAT_R8G8B8A8_UNORM = 37,
    VK_FORMAT_B8G8R8A8_UNORM = 44,
    VK_FORMAT_B8G8R8A8_SRGB = 50
} VkFormat;

typedef enum VkColorSpaceKHR {
    VK_COLOR_SPACE_SRGB_NONLINEAR_KHR = 0
} VkColorSpaceKHR;

typedef enum VkPresentModeKHR {
    VK_PRESENT_MODE_IMMEDIATE_KHR = 0,
    VK_PRESENT_MODE_MAILBOX_KHR = 1,
    VK_PRESENT_MODE_FIFO_KHR = 2,
    VK_PRESENT_MODE_FIFO_RELAXED_KHR = 3
} VkPresentModeKHR;

typedef enum VkSharingMode {
    VK_SHARING_MODE_EXCLUSIVE = 0,
    VK_SHARING_MODE_CONCURRENT = 1
} VkSharingMode;

typedef enum VkImageUsageFlagBits {
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT = 0x00000001,
    VK_IMAGE_USAGE_TRANSFER_DST_BIT = 0x00000002,
    VK_IMAGE_USAGE_SAMPLED_BIT = 0x00000004,
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT = 0x00000010
} VkImageUsageFlagBits;
typedef VkFlags VkImageUsageFlags;

typedef enum VkSurfaceTransformFlagBitsKHR {
    VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR = 0x00000001
} VkSurfaceTransformFlagBitsKHR;
typedef VkFlags VkSurfaceTransformFlagsKHR;

typedef enum VkCompositeAlphaFlagBitsKHR {
    VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR = 0x00000001
} VkCompositeAlphaFlagBitsKHR;
typedef VkFlags VkCompositeAlphaFlagsKHR;

typedef enum VkQueueFlagBits {
    VK_QUEUE_GRAPHICS_BIT = 0x00000001,
    VK_QUEUE_COMPUTE_BIT = 0x00000002,
    VK_QUEUE_TRANSFER_BIT = 0x00000004
} VkQueueFlagBits;
typedef VkFlags VkQueueFlags;

typedef enum VkCommandBufferLevel {
    VK_COMMAND_BUFFER_LEVEL_PRIMARY = 0,
    VK_COMMAND_BUFFER_LEVEL_SECONDARY = 1
} VkCommandBufferLevel;

typedef enum VkCommandBufferUsageFlagBits {
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT = 0x00000001
} VkCommandBufferUsageFlagBits;
typedef VkFlags VkCommandBufferUsageFlags;

typedef enum VkFenceCreateFlagBits {
    VK_FENCE_CREATE_SIGNALED_BIT = 0x00000001
} VkFenceCreateFlagBits;
typedef VkFlags VkFenceCreateFlags;

typedef enum VkPipelineStageFlagBits {
    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT = 0x00000001,
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = 0x00000400,
    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT = 0x00002000,
    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT = 0x00010000
} VkPipelineStageFlagBits;
typedef VkFlags VkPipelineStageFlags;

typedef enum VkAccessFlagBits {
    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT = 0x00000100
} VkAccessFlagBits;
typedef VkFlags VkAccessFlags;

/* Handles as opaque pointers (dispatchable) or uint64 (non-dispatchable) */
typedef struct VkInstance_T *VkInstance;
typedef struct VkPhysicalDevice_T *VkPhysicalDevice;
typedef struct VkDevice_T *VkDevice;
typedef struct VkQueue_T *VkQueue;
typedef struct VkCommandBuffer_T *VkCommandBuffer;
typedef uint64_t VkDeviceMemory;
typedef uint64_t VkCommandPool;
typedef uint64_t VkFence;
typedef uint64_t VkSemaphore;
typedef uint64_t VkBuffer;
typedef uint64_t VkImage;
typedef uint64_t VkImageView;
typedef uint64_t VkShaderModule;
typedef uint64_t VkPipelineLayout;
typedef uint64_t VkRenderPass;
typedef uint64_t VkPipeline;
typedef uint64_t VkFramebuffer;
typedef uint64_t VkSurfaceKHR;
typedef uint64_t VkSwapchainKHR;
typedef uint64_t VkEvent;
typedef uint64_t VkQueryPool;
typedef uint64_t VkPipelineCache;
typedef uint64_t VkBufferView;
typedef uint64_t VkDescriptorSet;
typedef uint64_t VkDescriptorSetLayout;
typedef uint64_t VkDescriptorPool;
typedef uint64_t VkSampler;

typedef void *(VKAPI_PTR *PFN_vkAllocationFunction)(void *pUserData, size_t size,
                                                    size_t alignment,
                                                    VkSystemAllocationScope scope);
typedef void *(VKAPI_PTR *PFN_vkReallocationFunction)(void *pUserData, void *pOriginal,
                                                      size_t size, size_t alignment,
                                                      VkSystemAllocationScope scope);
typedef void(VKAPI_PTR *PFN_vkFreeFunction)(void *pUserData, void *pMemory);
typedef void(VKAPI_PTR *PFN_vkInternalAllocationNotification)(
    void *pUserData, size_t size, VkInternalAllocationType type,
    VkSystemAllocationScope scope);
typedef void(VKAPI_PTR *PFN_vkInternalFreeNotification)(
    void *pUserData, size_t size, VkInternalAllocationType type,
    VkSystemAllocationScope scope);

typedef struct VkAllocationCallbacks {
    void *pUserData;
    PFN_vkAllocationFunction pfnAllocation;
    PFN_vkReallocationFunction pfnReallocation;
    PFN_vkFreeFunction pfnFree;
    PFN_vkInternalAllocationNotification pfnInternalAllocation;
    PFN_vkInternalFreeNotification pfnInternalFree;
} VkAllocationCallbacks;

typedef struct VkExtent2D {
    uint32_t width;
    uint32_t height;
} VkExtent2D;

typedef struct VkExtent3D {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
} VkExtent3D;

typedef struct VkOffset2D {
    int32_t x;
    int32_t y;
} VkOffset2D;

typedef struct VkRect2D {
    VkOffset2D offset;
    VkExtent2D extent;
} VkRect2D;

typedef struct VkApplicationInfo {
    VkStructureType sType;
    const void *pNext;
    const char *pApplicationName;
    uint32_t applicationVersion;
    const char *pEngineName;
    uint32_t engineVersion;
    uint32_t apiVersion;
} VkApplicationInfo;

typedef struct VkInstanceCreateInfo {
    VkStructureType sType;
    const void *pNext;
    VkFlags flags;
    const VkApplicationInfo *pApplicationInfo;
    uint32_t enabledLayerCount;
    const char *const *ppEnabledLayerNames;
    uint32_t enabledExtensionCount;
    const char *const *ppEnabledExtensionNames;
} VkInstanceCreateInfo;

typedef struct VkExtensionProperties {
    char extensionName[VK_MAX_EXTENSION_NAME_SIZE];
    uint32_t specVersion;
} VkExtensionProperties;

typedef struct VkLayerProperties {
    char layerName[VK_MAX_EXTENSION_NAME_SIZE];
    uint32_t specVersion;
    uint32_t implementationVersion;
    char description[VK_MAX_DESCRIPTION_SIZE];
} VkLayerProperties;

typedef struct VkPhysicalDeviceLimits {
    uint32_t maxImageDimension2D;
    uint32_t maxFramebufferWidth;
    uint32_t maxFramebufferHeight;
    /* rest zeroed for T0 */
    uint32_t aPad[64];
} VkPhysicalDeviceLimits;

typedef struct VkPhysicalDeviceSparseProperties {
    VkBool32 residencyStandard2DBlockShape;
    VkBool32 residencyStandard2DMultisampleBlockShape;
    VkBool32 residencyStandard3DBlockShape;
    VkBool32 residencyAlignedMipSize;
    VkBool32 residencyNonResidentStrict;
} VkPhysicalDeviceSparseProperties;

typedef struct VkPhysicalDeviceProperties {
    uint32_t apiVersion;
    uint32_t driverVersion;
    uint32_t vendorID;
    uint32_t deviceID;
    VkPhysicalDeviceType deviceType;
    char deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
    uint8_t pipelineCacheUUID[VK_UUID_SIZE];
    VkPhysicalDeviceLimits limits;
    VkPhysicalDeviceSparseProperties sparseProperties;
} VkPhysicalDeviceProperties;

typedef struct VkPhysicalDeviceFeatures {
    VkBool32 robustBufferAccess;
    VkBool32 aPad[55];
} VkPhysicalDeviceFeatures;

typedef struct VkQueueFamilyProperties {
    VkQueueFlags queueFlags;
    uint32_t queueCount;
    uint32_t timestampValidBits;
    VkExtent3D minImageTransferGranularity;
} VkQueueFamilyProperties;

typedef struct VkDeviceQueueCreateInfo {
    VkStructureType sType;
    const void *pNext;
    VkFlags flags;
    uint32_t queueFamilyIndex;
    uint32_t queueCount;
    const float *pQueuePriorities;
} VkDeviceQueueCreateInfo;

typedef struct VkDeviceCreateInfo {
    VkStructureType sType;
    const void *pNext;
    VkFlags flags;
    uint32_t queueCreateInfoCount;
    const VkDeviceQueueCreateInfo *pQueueCreateInfos;
    uint32_t enabledLayerCount;
    const char *const *ppEnabledLayerNames;
    uint32_t enabledExtensionCount;
    const char *const *ppEnabledExtensionNames;
    const VkPhysicalDeviceFeatures *pEnabledFeatures;
} VkDeviceCreateInfo;

typedef struct VkSurfaceCapabilitiesKHR {
    uint32_t minImageCount;
    uint32_t maxImageCount;
    VkExtent2D currentExtent;
    VkExtent2D minImageExtent;
    VkExtent2D maxImageExtent;
    uint32_t maxImageArrayLayers;
    VkSurfaceTransformFlagsKHR supportedTransforms;
    VkSurfaceTransformFlagBitsKHR currentTransform;
    VkCompositeAlphaFlagsKHR supportedCompositeAlpha;
    VkImageUsageFlags supportedUsageFlags;
} VkSurfaceCapabilitiesKHR;

typedef struct VkSurfaceFormatKHR {
    VkFormat format;
    VkColorSpaceKHR colorSpace;
} VkSurfaceFormatKHR;

typedef struct VkSwapchainCreateInfoKHR {
    VkStructureType sType;
    const void *pNext;
    VkFlags flags;
    VkSurfaceKHR surface;
    uint32_t minImageCount;
    VkFormat imageFormat;
    VkColorSpaceKHR imageColorSpace;
    VkExtent2D imageExtent;
    uint32_t imageArrayLayers;
    VkImageUsageFlags imageUsage;
    VkSharingMode imageSharingMode;
    uint32_t queueFamilyIndexCount;
    const uint32_t *pQueueFamilyIndices;
    VkSurfaceTransformFlagBitsKHR preTransform;
    VkCompositeAlphaFlagBitsKHR compositeAlpha;
    VkPresentModeKHR presentMode;
    VkBool32 clipped;
    VkSwapchainKHR oldSwapchain;
} VkSwapchainCreateInfoKHR;

typedef struct VkPresentInfoKHR {
    VkStructureType sType;
    const void *pNext;
    uint32_t waitSemaphoreCount;
    const VkSemaphore *pWaitSemaphores;
    uint32_t swapchainCount;
    const VkSwapchainKHR *pSwapchains;
    const uint32_t *pImageIndices;
    VkResult *pResults;
} VkPresentInfoKHR;

typedef struct VkCommandPoolCreateInfo {
    VkStructureType sType;
    const void *pNext;
    VkFlags flags;
    uint32_t queueFamilyIndex;
} VkCommandPoolCreateInfo;

typedef struct VkCommandBufferAllocateInfo {
    VkStructureType sType;
    const void *pNext;
    VkCommandPool commandPool;
    VkCommandBufferLevel level;
    uint32_t commandBufferCount;
} VkCommandBufferAllocateInfo;

typedef struct VkCommandBufferBeginInfo {
    VkStructureType sType;
    const void *pNext;
    VkCommandBufferUsageFlags flags;
    const void *pInheritanceInfo;
} VkCommandBufferBeginInfo;

typedef struct VkSubmitInfo {
    VkStructureType sType;
    const void *pNext;
    uint32_t waitSemaphoreCount;
    const VkSemaphore *pWaitSemaphores;
    const VkPipelineStageFlags *pWaitDstStageMask;
    uint32_t commandBufferCount;
    const VkCommandBuffer *pCommandBuffers;
    uint32_t signalSemaphoreCount;
    const VkSemaphore *pSignalSemaphores;
} VkSubmitInfo;

typedef struct VkFenceCreateInfo {
    VkStructureType sType;
    const void *pNext;
    VkFenceCreateFlags flags;
} VkFenceCreateInfo;

typedef struct VkSemaphoreCreateInfo {
    VkStructureType sType;
    const void *pNext;
    VkFlags flags;
} VkSemaphoreCreateInfo;

typedef struct VkShaderModuleCreateInfo {
    VkStructureType sType;
    const void *pNext;
    VkFlags flags;
    size_t codeSize; /* bytes */
    const uint32_t *pCode;
} VkShaderModuleCreateInfo;

typedef struct VkPipelineShaderStageCreateInfo {
    VkStructureType sType;
    const void *pNext;
    VkFlags flags;
    uint32_t stage; /* VK_SHADER_STAGE_* */
    VkShaderModule module;
    const char *pName;
    const void *pSpecializationInfo;
} VkPipelineShaderStageCreateInfo;

typedef struct VkViewport {
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;
} VkViewport;

/* Integer viewport for freestanding (no FP required on call path) */
typedef struct VkViewportGJ {
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
} VkViewportGJ;

typedef struct VkGraphicsPipelineCreateInfo {
    VkStructureType sType;
    const void *pNext;
    VkFlags flags;
    uint32_t stageCount;
    const VkPipelineShaderStageCreateInfo *pStages;
    const void *pVertexInputState;
    const void *pInputAssemblyState;
    const void *pTessellationState;
    const void *pViewportState;
    const void *pRasterizationState;
    const void *pMultisampleState;
    const void *pDepthStencilState;
    const void *pColorBlendState;
    const void *pDynamicState;
    VkPipelineLayout layout;
    VkRenderPass renderPass;
    uint32_t subpass;
    VkPipeline basePipelineHandle;
    int32_t basePipelineIndex;
} VkGraphicsPipelineCreateInfo;

typedef struct VkBufferCreateInfo {
    VkStructureType sType;
    const void *pNext;
    VkFlags flags;
    VkDeviceSize size;
    VkFlags usage;
    VkSharingMode sharingMode;
    uint32_t queueFamilyIndexCount;
    const uint32_t *pQueueFamilyIndices;
} VkBufferCreateInfo;

typedef struct VkMemoryRequirements {
    VkDeviceSize size;
    VkDeviceSize alignment;
    uint32_t memoryTypeBits;
} VkMemoryRequirements;

typedef struct VkMemoryAllocateInfo {
    VkStructureType sType;
    const void *pNext;
    VkDeviceSize allocationSize;
    uint32_t memoryTypeIndex;
} VkMemoryAllocateInfo;

typedef struct VkMemoryHeap {
    VkDeviceSize size;
    VkFlags flags;
} VkMemoryHeap;

typedef struct VkMemoryType {
    VkFlags propertyFlags;
    uint32_t heapIndex;
} VkMemoryType;

typedef struct VkPhysicalDeviceMemoryProperties {
    uint32_t memoryTypeCount;
    VkMemoryType memoryTypes[VK_MAX_MEMORY_TYPES];
    uint32_t memoryHeapCount;
    VkMemoryHeap memoryHeaps[VK_MAX_MEMORY_HEAPS];
} VkPhysicalDeviceMemoryProperties;

typedef struct VkFormatProperties {
    VkFlags linearTilingFeatures;
    VkFlags optimalTilingFeatures;
    VkFlags bufferFeatures;
} VkFormatProperties;

typedef struct VkImageFormatProperties {
    VkExtent3D maxExtent;
    uint32_t maxMipLevels;
    uint32_t maxArrayLayers;
    VkFlags sampleCounts;
    VkDeviceSize maxResourceSize;
} VkImageFormatProperties;

typedef struct VkSubresourceLayout {
    VkDeviceSize offset;
    VkDeviceSize size;
    VkDeviceSize rowPitch;
    VkDeviceSize arrayPitch;
    VkDeviceSize depthPitch;
} VkSubresourceLayout;

typedef struct VkImageSubresource {
    uint32_t aspectMask;
    uint32_t mipLevel;
    uint32_t arrayLayer;
} VkImageSubresource;

typedef struct VkMappedMemoryRange {
    VkStructureType sType;
    const void *pNext;
    VkDeviceMemory memory;
    VkDeviceSize offset;
    VkDeviceSize size;
} VkMappedMemoryRange;

typedef struct VkBufferCopy {
    VkDeviceSize srcOffset;
    VkDeviceSize dstOffset;
    VkDeviceSize size;
} VkBufferCopy;

typedef struct VkBufferImageCopy {
    VkDeviceSize bufferOffset;
    uint32_t bufferRowLength;
    uint32_t bufferImageHeight;
    uint32_t imageSubresource; /* aspect/mip/layer packed loosely for T0 */
    VkOffset2D imageOffset;
    VkExtent3D imageExtent;
} VkBufferImageCopy;

typedef struct VkImageCopy {
    uint32_t srcSubresource;
    VkOffset2D srcOffset;
    uint32_t dstSubresource;
    VkOffset2D dstOffset;
    VkExtent3D extent;
} VkImageCopy;

typedef struct VkEventCreateInfo {
    VkStructureType sType;
    const void *pNext;
    VkFlags flags;
} VkEventCreateInfo;

typedef struct VkPipelineCacheCreateInfo {
    VkStructureType sType;
    const void *pNext;
    VkFlags flags;
    size_t initialDataSize;
    const void *pInitialData;
} VkPipelineCacheCreateInfo;

typedef struct VkQueryPoolCreateInfo {
    VkStructureType sType;
    const void *pNext;
    VkFlags flags;
    uint32_t queryType;
    uint32_t queryCount;
    uint32_t pipelineStatistics;
} VkQueryPoolCreateInfo;

/* Format feature bits (subset) */
#define VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT              0x00000001u
#define VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT           0x00000080u
#define VK_FORMAT_FEATURE_TRANSFER_SRC_BIT               0x00004000u
#define VK_FORMAT_FEATURE_TRANSFER_DST_BIT               0x00008000u
#define VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT              0x00000040u

#define VK_SAMPLE_COUNT_1_BIT 0x00000001u

#define VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT 0x00000001u
#define VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT 0x00000002u
#define VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 0x00000004u

/* Whole-size sentinel used by map/flush ranges */
#define VK_WHOLE_SIZE (~(VkDeviceSize)0)

typedef void(VKAPI_PTR *PFN_vkVoidFunction)(void);

typedef VkResult(VKAPI_PTR *PFN_vkCreateInstance)(
    const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
    VkInstance *pInstance);
typedef void(VKAPI_PTR *PFN_vkDestroyInstance)(VkInstance instance,
                                               const VkAllocationCallbacks *pAllocator);
typedef PFN_vkVoidFunction(VKAPI_PTR *PFN_vkGetInstanceProcAddr)(VkInstance instance,
                                                                 const char *pName);
typedef PFN_vkVoidFunction(VKAPI_PTR *PFN_vkGetDeviceProcAddr)(VkDevice device,
                                                               const char *pName);

/* Implemented entry points */
VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo,
                                                const VkAllocationCallbacks *pAllocator,
                                                VkInstance *pInstance);
VKAPI_ATTR void VKAPI_CALL vkDestroyInstance(VkInstance instance,
                                             const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pPropertyCount,
                                        VkExtensionProperties *pProperties);
VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceLayerProperties(uint32_t *pPropertyCount,
                                   VkLayerProperties *pProperties);
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance,
                                                               const char *pName);
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice device,
                                                             const char *pName);
VKAPI_ATTR VkResult VKAPI_CALL
vkEnumeratePhysicalDevices(VkInstance instance, uint32_t *pPhysicalDeviceCount,
                           VkPhysicalDevice *pPhysicalDevices);
VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice,
                              VkPhysicalDeviceProperties *pProperties);
VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice,
                            VkPhysicalDeviceFeatures *pFeatures);
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice physicalDevice, uint32_t *pQueueFamilyPropertyCount,
    VkQueueFamilyProperties *pQueueFamilyProperties);
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceMemoryProperties(
    VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties *pMemoryProperties);
VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice, const char *pLayerName, uint32_t *pPropertyCount,
    VkExtensionProperties *pProperties);
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(VkPhysicalDevice physicalDevice,
                                              const VkDeviceCreateInfo *pCreateInfo,
                                              const VkAllocationCallbacks *pAllocator,
                                              VkDevice *pDevice);
VKAPI_ATTR void VKAPI_CALL vkDestroyDevice(VkDevice device,
                                           const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex,
                                            uint32_t queueIndex, VkQueue *pQueue);
VKAPI_ATTR VkResult VKAPI_CALL vkDeviceWaitIdle(VkDevice device);
VKAPI_ATTR VkResult VKAPI_CALL vkQueueWaitIdle(VkQueue queue);
VKAPI_ATTR VkResult VKAPI_CALL vkQueueSubmit(VkQueue queue, uint32_t submitCount,
                                             const VkSubmitInfo *pSubmits,
                                             VkFence fence);
VKAPI_ATTR VkResult VKAPI_CALL
vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo *pCreateInfo,
                    const VkAllocationCallbacks *pAllocator, VkCommandPool *pCommandPool);
VKAPI_ATTR void VKAPI_CALL
vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool,
                     const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL
vkAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo *pAllocateInfo,
                         VkCommandBuffer *pCommandBuffers);
VKAPI_ATTR void VKAPI_CALL
vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool,
                     uint32_t commandBufferCount, const VkCommandBuffer *pCommandBuffers);
VKAPI_ATTR VkResult VKAPI_CALL
vkBeginCommandBuffer(VkCommandBuffer commandBuffer,
                     const VkCommandBufferBeginInfo *pBeginInfo);
VKAPI_ATTR VkResult VKAPI_CALL vkEndCommandBuffer(VkCommandBuffer commandBuffer);
VKAPI_ATTR VkResult VKAPI_CALL vkCreateFence(VkDevice device,
                                             const VkFenceCreateInfo *pCreateInfo,
                                             const VkAllocationCallbacks *pAllocator,
                                             VkFence *pFence);
VKAPI_ATTR void VKAPI_CALL vkDestroyFence(VkDevice device, VkFence fence,
                                          const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL vkWaitForFences(VkDevice device, uint32_t fenceCount,
                                               const VkFence *pFences, VkBool32 waitAll,
                                               uint64_t timeout);
VKAPI_ATTR VkResult VKAPI_CALL vkResetFences(VkDevice device, uint32_t fenceCount,
                                             const VkFence *pFences);
VKAPI_ATTR VkResult VKAPI_CALL
vkCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo *pCreateInfo,
                  const VkAllocationCallbacks *pAllocator, VkSemaphore *pSemaphore);
VKAPI_ATTR void VKAPI_CALL
vkDestroySemaphore(VkDevice device, VkSemaphore semaphore,
                   const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR void VKAPI_CALL vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface,
                                               const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface,
    VkBool32 *pSupported);
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
    VkSurfaceCapabilitiesKHR *pSurfaceCapabilities);
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
    uint32_t *pSurfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormats);
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t *pPresentModeCount,
    VkPresentModeKHR *pPresentModes);
VKAPI_ATTR VkResult VKAPI_CALL
vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR *pCreateInfo,
                     const VkAllocationCallbacks *pAllocator, VkSwapchainKHR *pSwapchain);
VKAPI_ATTR void VKAPI_CALL
vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain,
                      const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL
vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain,
                        uint32_t *pSwapchainImageCount, VkImage *pSwapchainImages);
VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImageKHR(
    VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore,
    VkFence fence, uint32_t *pImageIndex);
VKAPI_ATTR VkResult VKAPI_CALL vkQueuePresentKHR(VkQueue queue,
                                                 const VkPresentInfoKHR *pPresentInfo);

/* GreenJade extensions (not Khronos) for headless + CPU map of swap images */
VKAPI_ATTR VkResult VKAPI_CALL
vkCreateHeadlessSurfaceGJ(VkInstance instance, uint32_t width, uint32_t height,
                          const VkAllocationCallbacks *pAllocator,
                          VkSurfaceKHR *pSurface);
VKAPI_ATTR void *VKAPI_CALL vkMapSwapchainImageGJ(VkDevice device,
                                                  VkSwapchainKHR swapchain,
                                                  uint32_t imageIndex,
                                                  uint32_t *pStride);

VKAPI_ATTR void VKAPI_CALL vkCmdClearColorImage(VkCommandBuffer commandBuffer,
                                                VkImage image, uint32_t imageLayout,
                                                const void *pColor, uint32_t rangeCount,
                                                const void *pRanges);
VKAPI_ATTR void VKAPI_CALL vkCmdClearColorImageGJ(VkCommandBuffer commandBuffer,
                                                  VkImage image, uint32_t rgba);
VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderPass(VkCommandBuffer commandBuffer,
                                                const void *pRenderPassBegin,
                                                uint32_t contents);
VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderPass(VkCommandBuffer commandBuffer);
VKAPI_ATTR void VKAPI_CALL vkCmdPipelineBarrier(
    VkCommandBuffer commandBuffer, uint32_t srcStageMask, uint32_t dstStageMask,
    uint32_t dependencyFlags, uint32_t memoryBarrierCount, const void *pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount, const void *pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount, const void *pImageMemoryBarriers);
VKAPI_ATTR void VKAPI_CALL vkCmdBindPipeline(VkCommandBuffer commandBuffer,
                                             uint32_t pipelineBindPoint,
                                             VkPipeline pipeline);
VKAPI_ATTR void VKAPI_CALL vkCmdBindFramebufferImageGJ(VkCommandBuffer commandBuffer,
                                                       VkImage image);
VKAPI_ATTR void VKAPI_CALL vkCmdBindTextureImageGJ(VkCommandBuffer commandBuffer,
                                                   VkImage image);
VKAPI_ATTR VkResult VKAPI_CALL vkCreateSampler(VkDevice device, const void *pCreateInfo,
                                               const VkAllocationCallbacks *pAllocator,
                                               VkSampler *pSampler);
VKAPI_ATTR void VKAPI_CALL vkDestroySampler(VkDevice device, VkSampler sampler,
                                            const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL vkCreateImage(VkDevice device, const void *pCreateInfo,
                                             const VkAllocationCallbacks *pAllocator,
                                             VkImage *pImage);
VKAPI_ATTR void VKAPI_CALL vkDestroyImage(VkDevice device, VkImage image,
                                          const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR void VKAPI_CALL
vkGetImageMemoryRequirements(VkDevice device, VkImage image,
                             VkMemoryRequirements *pMemoryRequirements);
VKAPI_ATTR VkResult VKAPI_CALL
vkAllocateMemory(VkDevice device, const VkMemoryAllocateInfo *pAllocateInfo,
                 const VkAllocationCallbacks *pAllocator, VkDeviceMemory *pMemory);
VKAPI_ATTR void VKAPI_CALL vkFreeMemory(VkDevice device, VkDeviceMemory memory,
                                        const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory(VkDevice device, VkImage image,
                                                 VkDeviceMemory memory,
                                                 VkDeviceSize memoryOffset);
VKAPI_ATTR VkResult VKAPI_CALL vkMapMemory(VkDevice device, VkDeviceMemory memory,
                                           VkDeviceSize offset, VkDeviceSize size,
                                           VkFlags flags, void **ppData);
VKAPI_ATTR void VKAPI_CALL vkUnmapMemory(VkDevice device, VkDeviceMemory memory);
VKAPI_ATTR VkResult VKAPI_CALL
vkCreateRenderPass(VkDevice device, const void *pCreateInfo,
                   const VkAllocationCallbacks *pAllocator, VkRenderPass *pRenderPass);
VKAPI_ATTR void VKAPI_CALL
vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass,
                    const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL
vkCreateFramebuffer(VkDevice device, const void *pCreateInfo,
                    const VkAllocationCallbacks *pAllocator, VkFramebuffer *pFramebuffer);
VKAPI_ATTR void VKAPI_CALL
vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer,
                     const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL vkCreateGraphicsPipelines(
    VkDevice device, uint64_t pipelineCache, uint32_t createInfoCount,
    const void *pCreateInfos, const VkAllocationCallbacks *pAllocator,
    VkPipeline *pPipelines);
VKAPI_ATTR void VKAPI_CALL vkDestroyPipeline(VkDevice device, VkPipeline pipeline,
                                             const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR void VKAPI_CALL vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount,
                                     uint32_t instanceCount, uint32_t firstVertex,
                                     uint32_t firstInstance);
VKAPI_ATTR void VKAPI_CALL vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer,
                                                VkBuffer buffer, VkDeviceSize offset,
                                                uint32_t indexType);
VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexed(VkCommandBuffer commandBuffer,
                                            uint32_t indexCount, uint32_t instanceCount,
                                            uint32_t firstIndex, int32_t vertexOffset,
                                            uint32_t firstInstance);

#define VK_INDEX_TYPE_UINT16 0u
#define VK_INDEX_TYPE_UINT32 1u
VKAPI_ATTR VkResult VKAPI_CALL
vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo *pCreateInfo,
                     const VkAllocationCallbacks *pAllocator,
                     VkShaderModule *pShaderModule);
VKAPI_ATTR void VKAPI_CALL
vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule,
                      const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL vkCreateImageView(VkDevice device, const void *pCreateInfo,
                                                 const VkAllocationCallbacks *pAllocator,
                                                 VkImageView *pView);
VKAPI_ATTR void VKAPI_CALL vkDestroyImageView(VkDevice device, VkImageView imageView,
                                              const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL
vkCreatePipelineLayout(VkDevice device, const void *pCreateInfo,
                       const VkAllocationCallbacks *pAllocator,
                       VkPipelineLayout *pPipelineLayout);
VKAPI_ATTR void VKAPI_CALL
vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout,
                        const VkAllocationCallbacks *pAllocator);

/* Buffers + vertex path (SPIR-V software raster T0) */
VKAPI_ATTR VkResult VKAPI_CALL
vkCreateBuffer(VkDevice device, const VkBufferCreateInfo *pCreateInfo,
               const VkAllocationCallbacks *pAllocator, VkBuffer *pBuffer);
VKAPI_ATTR void VKAPI_CALL vkDestroyBuffer(VkDevice device, VkBuffer buffer,
                                           const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR void VKAPI_CALL
vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer,
                              VkMemoryRequirements *pMemoryRequirements);
VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory(VkDevice device, VkBuffer buffer,
                                                  VkDeviceMemory memory,
                                                  VkDeviceSize memoryOffset);
VKAPI_ATTR void VKAPI_CALL vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer,
                                                  uint32_t firstBinding,
                                                  uint32_t bindingCount,
                                                  const VkBuffer *pBuffers,
                                                  const VkDeviceSize *pOffsets);
VKAPI_ATTR void VKAPI_CALL vkCmdPushConstants(VkCommandBuffer commandBuffer,
                                              VkPipelineLayout layout,
                                              uint32_t stageFlags, uint32_t offset,
                                              uint32_t size, const void *pValues);
VKAPI_ATTR void VKAPI_CALL vkCmdSetViewport(VkCommandBuffer commandBuffer,
                                            uint32_t firstViewport,
                                            uint32_t viewportCount,
                                            const VkViewport *pViewports);
/* Freestanding-friendly integer viewport (no float) */
VKAPI_ATTR void VKAPI_CALL vkCmdSetViewportGJ(VkCommandBuffer commandBuffer,
                                              int32_t x, int32_t y, uint32_t w,
                                              uint32_t h);
VKAPI_ATTR void VKAPI_CALL vkCmdSetScissor(VkCommandBuffer commandBuffer,
                                           uint32_t firstScissor, uint32_t scissorCount,
                                           const VkRect2D *pScissors);

/* Descriptor set layout / pool (stubs grown for A1+) */
VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDescriptorSetLayout(VkDevice device, const void *pCreateInfo,
                            const VkAllocationCallbacks *pAllocator,
                            VkDescriptorSetLayout *pSetLayout);
VKAPI_ATTR void VKAPI_CALL
vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout,
                             const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDescriptorPool(VkDevice device, const void *pCreateInfo,
                       const VkAllocationCallbacks *pAllocator, VkDescriptorPool *pPool);
VKAPI_ATTR void VKAPI_CALL
vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool,
                        const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL
vkAllocateDescriptorSets(VkDevice device, const void *pAllocateInfo,
                         VkDescriptorSet *pDescriptorSets);
VKAPI_ATTR VkResult VKAPI_CALL
vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool,
                     uint32_t descriptorSetCount, const VkDescriptorSet *pDescriptorSets);
VKAPI_ATTR void VKAPI_CALL vkUpdateDescriptorSets(VkDevice device, uint32_t writeCount,
                                                   const void *pWrites,
                                                   uint32_t copyCount,
                                                   const void *pCopies);
VKAPI_ATTR void VKAPI_CALL vkCmdBindDescriptorSets(
    VkCommandBuffer commandBuffer, uint32_t pipelineBindPoint, VkPipelineLayout layout,
    uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet *pDescriptorSets,
    uint32_t dynamicOffsetCount, const uint32_t *pDynamicOffsets);

/*
 * Product-shaped deepen stubs:
 *   soft success  — host-coherent / no-op shapes (flush, reset, copies, events)
 *   FEATURE_NOT_PRESENT — sparse, query pools, compute pipelines, multi-draw indirect
 */
VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceVersion(uint32_t *pApiVersion);
VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t *pPropertyCount,
                                 VkLayerProperties *pProperties);
VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format,
                                    VkFormatProperties *pFormatProperties);
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice physicalDevice, VkFormat format, uint32_t type, uint32_t tiling,
    VkImageUsageFlags usage, uint32_t flags, VkImageFormatProperties *pImageFormatProperties);
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties(
    VkPhysicalDevice physicalDevice, VkFormat format, uint32_t type,
    VkSampleMask samples, uint32_t usage, uint32_t tiling, uint32_t *pPropertyCount,
    void *pProperties);
VKAPI_ATTR VkResult VKAPI_CALL vkGetFenceStatus(VkDevice device, VkFence fence);
VKAPI_ATTR VkResult VKAPI_CALL
vkResetCommandBuffer(VkCommandBuffer commandBuffer, VkFlags flags);
VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandPool(VkDevice device, VkCommandPool commandPool,
                                                  VkFlags flags);
VKAPI_ATTR VkResult VKAPI_CALL
vkFlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount,
                          const VkMappedMemoryRange *pMemoryRanges);
VKAPI_ATTR VkResult VKAPI_CALL
vkInvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount,
                               const VkMappedMemoryRange *pMemoryRanges);
VKAPI_ATTR void VKAPI_CALL
vkGetDeviceMemoryCommitment(VkDevice device, VkDeviceMemory memory,
                            VkDeviceSize *pCommittedMemoryInBytes);
VKAPI_ATTR void VKAPI_CALL
vkGetImageSubresourceLayout(VkDevice device, VkImage image,
                            const VkImageSubresource *pSubresource,
                            VkSubresourceLayout *pLayout);
VKAPI_ATTR void VKAPI_CALL
vkCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer,
                uint32_t regionCount, const VkBufferCopy *pRegions);
VKAPI_ATTR void VKAPI_CALL
vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage,
                       uint32_t dstImageLayout, uint32_t regionCount,
                       const VkBufferImageCopy *pRegions);
VKAPI_ATTR void VKAPI_CALL
vkCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage,
                       uint32_t srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount,
                       const VkBufferImageCopy *pRegions);
VKAPI_ATTR void VKAPI_CALL
vkCmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, uint32_t srcImageLayout,
               VkImage dstImage, uint32_t dstImageLayout, uint32_t regionCount,
               const VkImageCopy *pRegions);
VKAPI_ATTR void VKAPI_CALL
vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, uint32_t srcImageLayout,
               VkImage dstImage, uint32_t dstImageLayout, uint32_t regionCount,
               const void *pRegions, uint32_t filter);
VKAPI_ATTR void VKAPI_CALL
vkCmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset,
                VkDeviceSize size, uint32_t data);
VKAPI_ATTR void VKAPI_CALL
vkCmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset,
                  VkDeviceSize dataSize, const void *pData);
VKAPI_ATTR void VKAPI_CALL
vkCmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount,
                     const VkCommandBuffer *pCommandBuffers);
VKAPI_ATTR void VKAPI_CALL
vkCmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY,
              uint32_t groupCountZ);
VKAPI_ATTR void VKAPI_CALL
vkCmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset);
VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset,
                  uint32_t drawCount, uint32_t stride);
VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset,
                         uint32_t drawCount, uint32_t stride);

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateEvent(VkDevice device, const VkEventCreateInfo *pCreateInfo,
              const VkAllocationCallbacks *pAllocator, VkEvent *pEvent);
VKAPI_ATTR void VKAPI_CALL vkDestroyEvent(VkDevice device, VkEvent event,
                                          const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL vkGetEventStatus(VkDevice device, VkEvent event);
VKAPI_ATTR VkResult VKAPI_CALL vkSetEvent(VkDevice device, VkEvent event);
VKAPI_ATTR VkResult VKAPI_CALL vkResetEvent(VkDevice device, VkEvent event);
VKAPI_ATTR void VKAPI_CALL
vkCmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask);
VKAPI_ATTR void VKAPI_CALL
vkCmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask);
VKAPI_ATTR void VKAPI_CALL
vkCmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent *pEvents,
                VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                uint32_t memoryBarrierCount, const void *pMemoryBarriers,
                uint32_t bufferMemoryBarrierCount, const void *pBufferMemoryBarriers,
                uint32_t imageMemoryBarrierCount, const void *pImageMemoryBarriers);

VKAPI_ATTR VkResult VKAPI_CALL
vkCreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo *pCreateInfo,
                      const VkAllocationCallbacks *pAllocator,
                      VkPipelineCache *pPipelineCache);
VKAPI_ATTR void VKAPI_CALL
vkDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache,
                       const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL
vkGetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache, size_t *pDataSize,
                       void *pData);
VKAPI_ATTR VkResult VKAPI_CALL
vkMergePipelineCaches(VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount,
                      const VkPipelineCache *pSrcCaches);

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateBufferView(VkDevice device, const void *pCreateInfo,
                   const VkAllocationCallbacks *pAllocator, VkBufferView *pView);
VKAPI_ATTR void VKAPI_CALL
vkDestroyBufferView(VkDevice device, VkBufferView bufferView,
                    const VkAllocationCallbacks *pAllocator);

/* Unsupported product features → VK_ERROR_FEATURE_NOT_PRESENT */
VKAPI_ATTR VkResult VKAPI_CALL
vkCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache,
                         uint32_t createInfoCount, const void *pCreateInfos,
                         const VkAllocationCallbacks *pAllocator, VkPipeline *pPipelines);
VKAPI_ATTR VkResult VKAPI_CALL
vkQueueBindSparse(VkQueue queue, uint32_t bindInfoCount, const void *pBindInfo,
                  VkFence fence);
VKAPI_ATTR VkResult VKAPI_CALL
vkCreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo *pCreateInfo,
                  const VkAllocationCallbacks *pAllocator, VkQueryPool *pQueryPool);
VKAPI_ATTR void VKAPI_CALL
vkDestroyQueryPool(VkDevice device, VkQueryPool queryPool,
                   const VkAllocationCallbacks *pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL
vkGetQueryPoolResults(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery,
                      uint32_t queryCount, size_t dataSize, void *pData, uint64_t stride,
                      VkFlags flags);
VKAPI_ATTR void VKAPI_CALL
vkCmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query,
                VkFlags flags);
VKAPI_ATTR void VKAPI_CALL
vkCmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query);
VKAPI_ATTR void VKAPI_CALL
vkCmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool,
                    uint32_t firstQuery, uint32_t queryCount);
VKAPI_ATTR void VKAPI_CALL
vkCmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlags pipelineStage,
                    VkQueryPool queryPool, uint32_t query);
VKAPI_ATTR void VKAPI_CALL
vkCmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool,
                          uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer,
                          VkDeviceSize dstOffset, VkDeviceSize stride, VkFlags flags);

/* Query SPIR-V module metadata (ICD test / tools) */
VKAPI_ATTR uint32_t VKAPI_CALL vkGetShaderModuleWordCountGJ(VkShaderModule module);
VKAPI_ATTR uint32_t VKAPI_CALL vkGetShaderModuleSpecColorGJ(VkShaderModule module);
VKAPI_ATTR uint32_t VKAPI_CALL vkGetShaderModuleExecMaskGJ(VkShaderModule module);
/* T0 host-visible buffer pointer (software vertex path) */
VKAPI_ATTR void *VKAPI_CALL vkGetBufferHostPointerGJ(VkBuffer buffer);

#ifdef __cplusplus
}
#endif
