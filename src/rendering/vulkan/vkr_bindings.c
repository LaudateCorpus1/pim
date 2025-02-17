#include "rendering/vulkan/vkr_bindings.h"
#include "rendering/vulkan/vkr_desc.h"
#include "rendering/vulkan/vkr_textable.h"
#include "rendering/vulkan/vkr_pipeline.h"

#include "common/profiler.h"

// ----------------------------------------------------------------------------

typedef struct vkrBinding
{
    VkDescriptorType type;
    union
    {
        VkDescriptorBufferInfo buffer;
        VkDescriptorImageInfo image;
    };
} vkrBinding;

// ----------------------------------------------------------------------------

static void vkrBindings_Flush(void);

// ----------------------------------------------------------------------------

static const VkDescriptorSetLayoutBinding kSetLayoutBindings[] =
{
    {
        .binding = vkrBindId_CameraData,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
    },
    {
        .binding = vkrBindId_LumTexture,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    },
    {
        .binding = vkrBindId_HistogramBuffer,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    },
    {
        .binding = vkrBindId_ExposureBuffer,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
    },
    {
        .binding = vkrBindTableId_Texture1D,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = kTextureTable1DSize,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
    },
    {
        .binding = vkrBindTableId_Texture2D,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = kTextureTable2DSize,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
    },
    {
        .binding = vkrBindTableId_Texture3D,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = kTextureTable3DSize,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
    },
    {
        .binding = vkrBindTableId_TextureCube,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = kTextureTableCubeSize,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
    },
    {
        .binding = vkrBindTableId_Texture2DArray,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = kTextureTable2DArraySize,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
    },
};

static const VkDescriptorPoolSize kPoolSizes[] =
{
    {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 8,
    },
    {
        .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .descriptorCount = 8,
    },
    {
        .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .descriptorCount = 8,
    },
    {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount =
            kTextureTable1DSize +
            kTextureTable2DSize +
            kTextureTable3DSize +
            kTextureTableCubeSize +
            kTextureTable2DArraySize,
    },
};

// ----------------------------------------------------------------------------

static VkDescriptorSetLayout ms_layout;
static VkDescriptorPool ms_pool;
static VkDescriptorSet ms_sets[kResourceSets];
static VkWriteDescriptorSet ms_writes[vkrBindId_COUNT];
static vkrBinding ms_bindings[vkrBindId_COUNT];
static bool ms_dirty[vkrBindId_COUNT];

// ----------------------------------------------------------------------------

bool vkrBindings_Init(void)
{
    ms_layout = vkrDescSetLayout_New(NELEM(kSetLayoutBindings), kSetLayoutBindings, 0x0);
    if (!ms_layout)
    {
        vkrBindings_Shutdown();
        return false;
    }

    ms_pool = vkrDescPool_New(kResourceSets, NELEM(kPoolSizes), kPoolSizes);
    if (!ms_pool)
    {
        vkrBindings_Shutdown();
        return false;
    }

    for (i32 i = 0; i < kResourceSets; ++i)
    {
        ms_sets[i] = vkrDescSet_New(ms_pool, ms_layout);
        if (!ms_sets[i])
        {
            vkrBindings_Shutdown();
            return false;
        }
    }

    return true;
}

ProfileMark(pm_update, vkrBindings_Update)
void vkrBindings_Update(void)
{
    ProfileBegin(pm_update);

    VkDescriptorSet set = vkrBindings_GetSet();
    if (set)
    {
        vkrTexTable_Write(set);
        vkrBindings_Flush();
    }

    ProfileEnd(pm_update);
}

void vkrBindings_Shutdown(void)
{
    if (ms_pool)
    {
        for (i32 i = 0; i < kResourceSets; ++i)
        {
            vkrDescSet_Del(ms_pool, ms_sets[i]);
            ms_sets[i] = NULL;
        }
        vkrDescPool_Del(ms_pool);
        ms_pool = NULL;
    }
    vkrDescSetLayout_Del(ms_layout);
    ms_layout = NULL;
}

VkDescriptorSetLayout vkrBindings_GetSetLayout(void)
{
    ASSERT(ms_layout);
    return ms_layout;
}

VkDescriptorSet vkrBindings_GetSet(void)
{
    u32 syncIndex = vkrSys_SyncIndex();
    ASSERT(syncIndex < NELEM(ms_sets));
    ASSERT(ms_sets[syncIndex]);
    return ms_sets[syncIndex];
}

void vkrBindings_BindImage(
    vkrBindId id,
    VkDescriptorType type,
    VkSampler sampler,
    VkImageView view,
    VkImageLayout layout)
{
    ASSERT(id >= 0);
    ASSERT(id < vkrBindId_COUNT);
    ms_bindings[id].type = type;
    ms_bindings[id].image.imageLayout = layout;
    ms_bindings[id].image.imageView = view;
    ms_bindings[id].image.sampler = sampler;
    ms_dirty[id] = true;
}

void vkrBindings_BindBuffer(
    vkrBindId id,
    VkDescriptorType type,
    vkrBuffer const *const buffer)
{
    ASSERT(id >= 0);
    ASSERT(id < vkrBindId_COUNT);
    ms_bindings[id].type = type;
    ms_bindings[id].buffer.buffer = buffer->handle;
    ms_bindings[id].buffer.offset = 0;
    ms_bindings[id].buffer.range = buffer->size;
    ms_dirty[id] = true;
}

ProfileMark(pm_flush, vkrBindings_Flush)
static void vkrBindings_Flush(void)
{
    ProfileBegin(pm_flush);

    VkDescriptorSet set = vkrBindings_GetSet();
    if (set)
    {
        i32 writeCount = 0;
        for (i32 i = 0; i < NELEM(ms_bindings); ++i)
        {
            if (!ms_dirty[i])
            {
                continue;
            }
            ms_dirty[i] = false;

            VkWriteDescriptorSet *const write = &ms_writes[writeCount++];
            write->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write->descriptorType = ms_bindings[i].type;
            write->dstSet = set;
            write->dstBinding = i;
            write->dstArrayElement = 0;
            write->descriptorCount = 1;

            switch (ms_bindings[i].type)
            {
            default:
                ASSERT(false);
                break;
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
            {
                write->pBufferInfo = &ms_bindings[i].buffer;
            }
            break;
            case VK_DESCRIPTOR_TYPE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
            {
                write->pImageInfo = &ms_bindings[i].image;
            }
            break;
            }
        }
        if (writeCount > 0)
        {
            vkUpdateDescriptorSets(g_vkr.dev, writeCount, ms_writes, 0, NULL);
        }
    }

    ProfileEnd(pm_flush);
}
