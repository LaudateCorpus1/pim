#include "rendering/vulkan/vkr_debug.h"
#include "common/console.h"
#include <string.h>

VkDebugUtilsMessengerEXT vkrCreateDebugMessenger()
{
    ASSERT(g_vkr.inst);

    VkDebugUtilsMessengerEXT messenger = NULL;
#if VKR_DEBUG_MESSENGER_ON
    VkCheck(vkCreateDebugUtilsMessengerEXT(g_vkr.inst,
        &(VkDebugUtilsMessengerCreateInfoEXT)
    {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = vkrOnVulkanMessage,
    }, NULL, &messenger));
#endif // VKR_DEBUG_MESSENGER_ON
    return messenger;
}

void vkrDestroyDebugMessenger(VkDebugUtilsMessengerEXT messenger)
{
#if VKR_DEBUG_MESSENGER_ON
    if (messenger)
    {
        vkDestroyDebugUtilsMessengerEXT(g_vkr.inst, messenger, NULL);
    }
#endif // VKR_DEBUG_MESSENGER_ON
}

static VKAPI_ATTR VkBool32 VKAPI_CALL vkrOnVulkanMessage(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    bool shouldLog = false;
    LogSev sev = LogSev_Error;
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        sev = LogSev_Error;
        shouldLog = true;
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        sev = LogSev_Warning;
        shouldLog = true;
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        sev = LogSev_Info;
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    {
        sev = LogSev_Verbose;
    }
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
    {
        shouldLog = true;
    }
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
    {
        shouldLog = true;
    }

    if (shouldLog)
    {
        const char* pMessage = pCallbackData->pMessage;
        Con_Logf(sev, "vkr", pMessage);
        ASSERT(sev != LogSev_Error);
    }

    return VK_FALSE;
}
