#pragma once

#include "common/macro.h"
#include "containers/array.hpp"
#include "containers/dict.hpp"
#include "containers/hash32.hpp"
#include "rendering/vulkan/vkr.h"
#include <string.h>

namespace vkr
{
    class RenderGraph;
    class RenderPass;
    class Resource;

    using PassId = Hash32;
    using ResourceId = Hash32;

    enum ResourceType
    {
        ResourceType_NULL = 0,

        ResourceType_Buffer,
        ResourceType_Image,
    };

    enum QueueFlagBit
    {
        QueueFlagBit_Graphics = 1 << 0,
        QueueFlagBit_Compute = 1 << 1,
        QueueFlagBit_Transfer = 1 << 2,
        QueueFlagBit_Present = 1 << 3,
    };
    using QueueFlags = u32;

    enum AttachmentScaling
    {
        AttachmentScaling_Absolute,
        AttachmentScaling_DisplayRelative,
        AttachmentScaling_InputRelative,
    };

    struct BufferInfo
    {
        i32 size = 0;
        VkBufferUsageFlags usage = 0x0;
        u8 transient = 0;
        u8 persistent = 1;

        bool operator==(const BufferInfo& other) const
        {
            return memcmp(this, &other, sizeof(*this)) == 0;
        }
    };

    struct AttachmentInfo
    {
        VkFormat format = VK_FORMAT_UNDEFINED;
        VkImageUsageFlags usage = 0x0;
        AttachmentScaling scaling = AttachmentScaling_DisplayRelative;
        Hash32 scaleName;
        float scaleX = 1.0f;
        float scaleY = 1.0f;
        float scaleZ = 1.0f;
        u8 samples = 1;
        u8 levels = 1;
        u8 layers = 1;
        u8 transient = 0;
        u8 persistent = 1;

        bool operator==(const AttachmentInfo& other) const
        {
            return memcmp(this, &other, sizeof(*this)) == 0;
        }
    };

    struct ImageInfo
    {
        VkFormat format = VK_FORMAT_UNDEFINED;
        VkImageUsageFlags usage = 0x0;
        u16 width = 0;
        u16 height = 0;
        u16 depth = 1;
        u8 layers = 1;
        u8 levels = 1;
        u8 samples = 1;
        u8 transient = 0;
        u8 persistent = 1;

        bool operator==(const ImageInfo& other) const
        {
            return memcmp(this, &other, sizeof(*this)) == 0;
        }
    };

    struct ResourceAccess
    {
        Resource* resource = NULL;
        VkPipelineStageFlags stage = 0x0;
        VkAccessFlags access = 0x0;
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

        bool operator==(const ResourceAccess& other) const
        {
            return memcmp(this, &other, sizeof(*this)) == 0;
        }
    };

    class Resource
    {
    protected:
        ResourceType m_type;
        ResourceId m_id;
        Array<PassId> m_writePasses;
        Array<PassId> m_readPasses;
        QueueFlags m_queueFlags;
    public:
        explicit Resource(ResourceType type, ResourceId id) :
            m_type(type),
            m_id(id),
            m_writePasses(EAlloc_Temp),
            m_readPasses(EAlloc_Temp),
            m_queueFlags(0x0)
        {}
        virtual ~Resource() = default;

        ResourceType GetType() const { return m_type; }
        ResourceId GetId() const { return m_id; }

        void WriteInPass(PassId id) { m_writePasses.FindAdd(id); }
        void ReadInPass(PassId id) { m_readPasses.FindAdd(id); }
        const Array<PassId>& GetWritePasses() const { return m_writePasses; }
        const Array<PassId>& GetReadPasses() const { return m_readPasses; }

        QueueFlags GetQueues() const { return m_queueFlags; }
        void AddQueue(QueueFlags queue) { m_queueFlags |= queue; }
    };

    class BufferResource : public Resource
    {
    protected:
        BufferInfo m_info;
    public:
        explicit BufferResource(ResourceId id)
            : Resource(ResourceType_Buffer, id)
        {}

        void SetBufferInfo(const BufferInfo& info) { m_info = info; }
        const BufferInfo& GetBufferInfo() const { return m_info; }

        VkBufferUsageFlags GetBufferUsage() const { return m_info.usage; }
        void AddBufferUsage(VkBufferUsageFlags usage) { m_info.usage |= usage; }
    };

    class ImageResource : public Resource
    {
    protected:
        AttachmentInfo m_info;
    public:
        explicit ImageResource(ResourceId id)
            : Resource(ResourceType_Image, id)
        {}

        const AttachmentInfo& GetAttachmentInfo() const { return m_info; }
        void SetAttachmentInfo(const AttachmentInfo& info) { m_info = info; }

        bool GetIsTransient() const { return m_info.transient; }
        void SetIsTransient(bool transient) { m_info.transient = transient; }

        VkImageUsageFlags GetImageUsage() const { return m_info.usage; }
        void AddImageUsage(VkImageUsageFlags usage) { m_info.usage |= usage; }
    };

    class RenderPass
    {
    private:
        RenderGraph& m_graph;
        PassId m_id;
        QueueFlags m_queueFlags;
        Array<ResourceAccess> m_inputs;
        Array<ResourceAccess> m_outputs;
    public:
        explicit RenderPass(
            RenderGraph& graph,
            PassId id,
            QueueFlags queueFlags) :
            m_graph(graph),
            m_id(id),
            m_queueFlags(queueFlags),
            m_inputs(EAlloc_Temp),
            m_outputs(EAlloc_Temp)
        {}
        virtual ~RenderPass() = default;

        virtual bool Setup() = 0;
        virtual void Execute() = 0;

        PassId GetId() const { return m_id; }

        const Array<ResourceAccess>& GetInputs() const { return m_inputs; }
        const Array<ResourceAccess>& GetOutputs() const { return m_outputs; }

        ImageResource& AddImageInput(
            const char* name,
            VkPipelineStageFlags stage,
            VkAccessFlags access,
            VkImageLayout layout,
            VkImageUsageFlags usage);
        ImageResource& AddImageOutput(
            const char* name,
            VkPipelineStageFlags stage,
            VkAccessFlags access,
            VkImageLayout layout,
            const AttachmentInfo& info);

        BufferResource& AddBufferInput(
            const char* name,
            VkPipelineStageFlags stage,
            VkAccessFlags access,
            VkBufferUsageFlags usage);
        BufferResource& AddBufferOutput(
            const char* name,
            VkPipelineStageFlags stage,
            VkAccessFlags access,
            const BufferInfo& info);
    };

    class RenderGraph final
    {
    private:
        Array<PassId> m_passIds;
        Array<RenderPass*> m_passes;
        Array<ResourceId> m_resourceIds;
        Array<Resource*> m_resources;
    public:
        RenderGraph() :
            m_passIds(EAlloc_Temp),
            m_passes(EAlloc_Temp),
            m_resourceIds(EAlloc_Temp),
            m_resources(EAlloc_Temp)
        {}

        ImageResource& GetImageResource(const char* name);
        BufferResource& GetBufferResource(const char* name);

        template<class PassType>
        PassType& AddPass(const char* name, QueueFlags queueFlags)
        {
            ResourceId id(name);
            i32 index = m_passIds.Find(id);
            if (index >= 0)
            {
                return *static_cast<PassType*>(m_passes[index]);
            }
            PassType* ptr = new(tmp_calloc(sizeof(*ptr))) PassType(*this, id, queueFlags);
            m_passIds.Grow() = id;
            m_passes.Grow() = ptr;
            return *ptr;
        }
    };
};
