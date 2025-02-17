#include "rendering/vulkan/vkr_opaquepass.h"
#include "rendering/vulkan/vkr_pass.h"
#include "rendering/vulkan/vkr_swapchain.h"
#include "rendering/vulkan/vkr_buffer.h"
#include "rendering/vulkan/vkr_textable.h"
#include "rendering/vulkan/vkr_shader.h"
#include "rendering/vulkan/vkr_renderpass.h"
#include "rendering/vulkan/vkr_context.h"
#include "rendering/vulkan/vkr_cmd.h"
#include "rendering/vulkan/vkr_desc.h"
#include "rendering/vulkan/vkr_bindings.h"
#include "rendering/vulkan/vkr_mesh.h"
#include "rendering/vulkan/vkr_im.h"

#include "rendering/drawable.h"
#include "rendering/mesh.h"
#include "rendering/texture.h"
#include "rendering/material.h"
#include "rendering/camera.h"
#include "rendering/lightmap.h"

#include "allocator/allocator.h"
#include "common/profiler.h"
#include "common/cvar.h"
#include "containers/table.h"
#include "math/box.h"
#include "math/frustum.h"
#include "threading/task.h"
#include <string.h>

typedef struct vkrPerCamera_s
{
    float4x4 worldToClip;
    float4 eye;

    float hdrEnabled;
    float whitepoint;
    float displayNits;
    float uiNits;
} vkrPerCamera;

typedef struct PushConstants_s
{
    float4x4 kLocalToWorld;
    float4 kIMc0;
    float4 kIMc1;
    float4 kIMc2;
    uint4 kTexInds;
} PushConstants;

static vkrPass ms_pass;
static VkRenderPass ms_renderPass;
static vkrBufferSet ms_perCameraBuffer;

bool vkrOpaquePass_New(void)
{
    bool success = true;

    const vkrRenderPassDesc renderPassDesc =
    {
        .srcStageMask =
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        .srcAccessMask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,

        .dstStageMask =
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccessMask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,

        .attachments[0] =
        {
            .format = g_vkr.chain.depthAttachments[0].format,
            .initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .load = VK_ATTACHMENT_LOAD_OP_LOAD,
            .store = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        },
        .attachments[1] =
        {
            .format = g_vkr.chain.colorFormat,
            .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .load = VK_ATTACHMENT_LOAD_OP_LOAD,
            .store = VK_ATTACHMENT_STORE_OP_STORE,
        },
        .attachments[2] =
        {
            .format = g_vkr.chain.lumAttachments[0].format,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .load = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .store = VK_ATTACHMENT_STORE_OP_STORE,
        },
    };
    ms_renderPass = vkrRenderPass_Get(&renderPassDesc);
    if (!ms_renderPass)
    {
        success = false;
        goto cleanup;
    }

    if (!vkrBufferSet_New(
        &ms_perCameraBuffer,
        sizeof(vkrPerCamera),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        vkrMemUsage_Dynamic))
    {
        success = false;
        goto cleanup;
    }

    VkPipelineShaderStageCreateInfo shaders[2] = { 0 };
    if (!vkrShader_New(&shaders[0], "brush.hlsl", "VSMain", vkrShaderType_Vert))
    {
        success = false;
        goto cleanup;
    }
    if (!vkrShader_New(&shaders[1], "brush.hlsl", "PSMain", vkrShaderType_Frag))
    {
        success = false;
        goto cleanup;
    }

    const VkVertexInputBindingDescription vertBindings[] =
    {
        // positionWS
        {
            .binding = 0,
            .stride = sizeof(float4),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        },
        // normalWS and lightmap index
        {
            .binding = 1,
            .stride = sizeof(float4),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        },
        // uv0 and uv1
        {
            .binding = 2,
            .stride = sizeof(float4),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        },
        // texture indices
        {
            .binding = 3,
            .stride = sizeof(int4),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        },
    };
    const VkVertexInputAttributeDescription vertAttributes[] =
    {
        // positionOS
        {
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
        },
        // normalOS and lightmap index
        {
            .binding = 1,
            .location = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
        },
        // uv0 and uv1
        {
            .binding = 2,
            .location = 2,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
        },
        // texture indices
        {
            .binding = 3,
            .location = 3,
            .format = VK_FORMAT_R32G32B32A32_UINT,
        }
    };

    const vkrPassDesc desc =
    {
        .pushConstantBytes = sizeof(PushConstants),
        .renderPass = ms_renderPass,
        .subpass = 0,
        .vertLayout =
        {
            .bindingCount = NELEM(vertBindings),
            .bindings = vertBindings,
            .attributeCount = NELEM(vertAttributes),
            .attributes = vertAttributes,
        },
        .fixedFuncs =
        {
            .viewport = vkrSwapchain_GetViewport(&g_vkr.chain),
            .scissor = vkrSwapchain_GetRect(&g_vkr.chain),
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .depthCompareOp = VK_COMPARE_OP_EQUAL,
            .scissorOn = false,
            .depthClamp = false,
            .depthTestEnable = true,
            .depthWriteEnable = false,
            .attachmentCount = 2,
            .attachments[0] =
            {
                .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
                .blendEnable = false,
            },
            .attachments[1] =
            {
                .colorWriteMask = VK_COLOR_COMPONENT_R_BIT,
                .blendEnable = false,
            },
        },
        .shaderCount = NELEM(shaders),
        .shaders = shaders,
    };

    if (!vkrPass_New(&ms_pass, &desc))
    {
        success = false;
        goto cleanup;
    }

cleanup:
    for (i32 i = 0; i < NELEM(shaders); ++i)
    {
        vkrShader_Del(&shaders[i]);
    }
    if (!success)
    {
        vkrOpaquePass_Del();
    }
    return success;
}

void vkrOpaquePass_Del(void)
{
    vkrPass_Del(&ms_pass);
    vkrBufferSet_Release(&ms_perCameraBuffer);
}

ProfileMark(pm_update, vkrOpaquePass_Setup)
void vkrOpaquePass_Setup(void)
{
    ProfileBegin(pm_update);

    {
        // TODO: Move this into some parameter provider standalone file
        Camera camera;
        Camera_Get(&camera);
        vkrPerCamera perCamera;
        perCamera.worldToClip = Camera_GetWorldToClip(&camera, vkrSwapchain_GetAspect(&g_vkr.chain));
        perCamera.eye = camera.position;
        perCamera.hdrEnabled = vkrSys_HdrEnabled() ? 1.0f : 0.0f;
        perCamera.whitepoint = vkrSys_GetWhitepoint();
        perCamera.displayNits = vkrSys_GetDisplayNitsMax();
        perCamera.uiNits = vkrSys_GetUiNits();
        vkrBufferSet_Write(&ms_perCameraBuffer, &perCamera, sizeof(perCamera));
    }

    vkrBindings_BindBuffer(
        vkrBindId_CameraData,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        vkrBufferSet_Current(&ms_perCameraBuffer));

    ProfileEnd(pm_update);
}

// ----------------------------------------------------------------------------

ProfileMark(pm_execute, vkrOpaquePass_Execute)
void vkrOpaquePass_Execute(vkrPassContext const *const ctx)
{
    ProfileBegin(pm_execute);

    VkCommandBuffer cmd = ctx->cmd;
    vkrCmdDefaultViewport(cmd);
    vkrCmdBindPass(cmd, &ms_pass);
    const VkClearValue clearValues[] =
    {
        {
            .depthStencil = { 1.0f, 0 },
        },
        {
            .color = { 0.0f, 0.0f, 0.0f, 1.0f },
        },
        {
            .color = { 0.0f, 0.0f, 0.0f, 1.0f },
        },
    };
    vkrCmdBeginRenderPass(
        cmd,
        ms_renderPass,
        ctx->framebuffer,
        vkrSwapchain_GetRect(&g_vkr.chain),
        NELEM(clearValues), clearValues,
        VK_SUBPASS_CONTENTS_INLINE);

    const Entities* ents = Entities_Get();
    for (i32 iEnt = 0; iEnt < ents->count; ++iEnt)
    {
        const Mesh* mesh = Mesh_Get(ents->meshes[iEnt]);
        if (mesh)
        {
            const Texture* albedo = Texture_Get(ents->materials[iEnt].albedo);
            const Texture* rome = Texture_Get(ents->materials[iEnt].rome);
            const Texture* normal = Texture_Get(ents->materials[iEnt].normal);
            PushConstants pc;
            pc.kLocalToWorld = ents->matrices[iEnt];
            pc.kIMc0 = ents->invMatrices[iEnt].c0;
            pc.kIMc1 = ents->invMatrices[iEnt].c1;
            pc.kIMc2 = ents->invMatrices[iEnt].c2;
            pc.kTexInds.x = albedo ? albedo->slot.index : 0;
            pc.kTexInds.y = rome ? rome->slot.index : 0;
            pc.kTexInds.z = normal ? normal->slot.index : 0;
            pc.kTexInds.w = 0;
            vkrCmdPushConstants(cmd, &ms_pass, &pc, sizeof(pc));
            vkrCmdDrawMesh(cmd, mesh->id);
        }
    }

    PushConstants pc;
    pc.kLocalToWorld = f4x4_id;
    pc.kIMc0 = f4_v(1.0f, 0.0f, 0.0f, 0.0f);
    pc.kIMc1 = f4_v(0.0f, 1.0f, 0.0f, 0.0f);
    pc.kIMc2 = f4_v(0.0f, 0.0f, 1.0f, 0.0f);
    pc.kTexInds = (uint4) { 0 };
    vkrCmdPushConstants(cmd, &ms_pass, &pc, sizeof(pc));
    vkrImSys_Draw(cmd);

    vkrCmdEndRenderPass(cmd);

    ProfileEnd(pm_execute);
}
