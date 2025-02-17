#pragma once

#include "rendering/vulkan/vkr.h"

PIM_C_BEGIN

VkRenderPass vkrRenderPass_GetFull(
    i32 attachmentCount,
    const VkAttachmentDescription* pAttachments,
    i32 subpassCount,
    const VkSubpassDescription* pSubpasses,
    i32 dependencyCount,
    const VkSubpassDependency* pDependencies);
void vkrRenderPass_Clear(void);

VkRenderPass vkrRenderPass_Get(const vkrRenderPassDesc* desc);

PIM_C_END
