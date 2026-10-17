#include "kyoei_vulkan_recording.h"
#include "vulkan_recording/queue.h"

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <shared_mutex>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace KyoeiVulkanRecording {
namespace {
using Dispatcher = std::remove_reference_t<decltype(VULKAN_HPP_DEFAULT_DISPATCHER)>;
bool Enabled() {
    static const bool enabled = [] {
        const auto* value = std::getenv("KYOEI_VULKAN_RECORDING");
        return !value || std::strcmp(value, "1") == 0;
    }();
    return enabled;
}
bool Statistics() {
    static const bool enabled=[] {
        const auto* value=std::getenv("KYOEI_VULKAN_RECORDING_STATS");
        return value && value[0]=='1';
    }();
    return enabled;
}

struct State { const void* owner; Queue queue; bool active{true}; };
std::shared_mutex registry_mutex;
std::unordered_map<VkCommandBuffer, std::shared_ptr<State>> commands;
std::unordered_map<const void*, std::shared_ptr<State>> owners;
std::atomic<uint64_t> epoch{1};

State* Find(VkCommandBuffer command) {
    struct Cache {
        VkCommandBuffer command{};
        uint64_t epoch{};
        std::shared_ptr<State> state;
    };
    thread_local Cache cache;
    const auto generation = epoch.load(std::memory_order_acquire);
    if (cache.command == command && cache.epoch == generation) return cache.state.get();
    std::shared_lock lock(registry_mutex);
    const auto found = commands.find(command);
    cache = {command, generation, found == commands.end() ? nullptr : found->second};
    return cache.state.get();
}

template<auto Member> struct Tag {};
template<class T> constexpr bool IsValue = !std::is_pointer_v<T> ||
    (!std::is_void_v<std::remove_pointer_t<T>> &&
     !requires { sizeof(std::remove_pointer_t<T>); });

// Vulkan opaque handles are pointer-shaped values on 64-bit Windows. Actual
// data pointers require an explicit snapshot below. Unknown payloads run on
// the producer only after draining; they can never overtake queued commands.
template<auto M, class F, class... A>
bool Capture(Tag<M>, Queue& queue, F fn, VkCommandBuffer command, A... args) {
    if constexpr ((IsValue<A> && ...)) {
        queue.Record([=] { fn(command, args...); });
        return true;
    }
    return false;
}

template<class T> bool NoExtensions(const T& values) {
    for (const auto& value : values) if (value.pNext) return false;
    return true;
}

#define ARRAY_COMMAND(NAME, TYPE) \
template<class F> bool Capture(Tag<&Dispatcher::NAME>, Queue& q, F fn, \
 VkCommandBuffer c, uint32_t count, const TYPE* values) { \
 if(!q.Reserve(size_t(count)*sizeof(TYPE))) return false; \
 auto data = q.Copy(values, count); \
 q.Record([=, data=std::move(data)] { fn(c,count,data.data()); }); return true; }
ARRAY_COMMAND(vkCmdSetViewportWithCount, VkViewport)
ARRAY_COMMAND(vkCmdSetViewportWithCountEXT, VkViewport)
ARRAY_COMMAND(vkCmdSetScissorWithCount, VkRect2D)
ARRAY_COMMAND(vkCmdSetScissorWithCountEXT, VkRect2D)
#undef ARRAY_COMMAND

template<class F> bool Capture(Tag<&Dispatcher::vkCmdSetColorWriteMaskEXT>, Queue& q,
 F fn,VkCommandBuffer c,uint32_t first,uint32_t count,const VkColorComponentFlags* values) {
    if(!q.Reserve(size_t(count)*sizeof(*values))) return false;
    auto data=q.Copy(values,count);
    q.Record([=] { fn(c,first,count,data.data()); }); return true;
}

template<class F> bool Capture(Tag<&Dispatcher::vkCmdCopyBuffer>, Queue& q,
 F fn,VkCommandBuffer c,VkBuffer src,VkBuffer dst,uint32_t count,const VkBufferCopy* regions) {
    if(!q.Reserve(size_t(count)*sizeof(*regions))) return false;
    auto data=q.Copy(regions,count);
    q.Record([=] { fn(c,src,dst,count,data.data()); }); return true;
}

#define POSITIONED_ARRAY(NAME, TYPE) \
template<class F> bool Capture(Tag<&Dispatcher::NAME>,Queue& q,F fn,VkCommandBuffer c, \
 uint32_t first,uint32_t count,const TYPE* values) { \
 if(!q.Reserve(size_t(count)*sizeof(*values))) return false; auto data=q.Copy(values,count); \
 q.Record([=] { fn(c,first,count,data.data()); }); return true; }
POSITIONED_ARRAY(vkCmdSetViewport,VkViewport)
POSITIONED_ARRAY(vkCmdSetScissor,VkRect2D)
#undef POSITIONED_ARRAY

template<class F> bool Capture(Tag<&Dispatcher::vkCmdCopyBufferToImage>,Queue& q,F fn,VkCommandBuffer c,
 VkBuffer src,VkImage dst,VkImageLayout layout,uint32_t count,const VkBufferImageCopy* regions) {
    if(!q.Reserve(size_t(count)*sizeof(*regions))) return false;
    auto data=q.Copy(regions,count);
    q.Record([=] { fn(c,src,dst,layout,count,data.data()); }); return true;
}
template<class F> bool Capture(Tag<&Dispatcher::vkCmdCopyImageToBuffer>,Queue& q,F fn,VkCommandBuffer c,
 VkImage src,VkImageLayout layout,VkBuffer dst,uint32_t count,const VkBufferImageCopy* regions) {
    if(!q.Reserve(size_t(count)*sizeof(*regions))) return false;
    auto data=q.Copy(regions,count);
    q.Record([=] { fn(c,src,layout,dst,count,data.data()); }); return true;
}
template<class F> bool Capture(Tag<&Dispatcher::vkCmdCopyImage>,Queue& q,F fn,VkCommandBuffer c,
 VkImage src,VkImageLayout sl,VkImage dst,VkImageLayout dl,uint32_t count,const VkImageCopy* regions) {
    if(!q.Reserve(size_t(count)*sizeof(*regions))) return false;
    auto data=q.Copy(regions,count);
    q.Record([=] { fn(c,src,sl,dst,dl,count,data.data()); }); return true;
}
template<class F> bool Capture(Tag<&Dispatcher::vkCmdBlitImage>,Queue& q,F fn,VkCommandBuffer c,
 VkImage src,VkImageLayout sl,VkImage dst,VkImageLayout dl,uint32_t count,const VkImageBlit* regions,VkFilter filter) {
    if(!q.Reserve(size_t(count)*sizeof(*regions))) return false;
    auto data=q.Copy(regions,count);
    q.Record([=] { fn(c,src,sl,dst,dl,count,data.data(),filter); }); return true;
}
template<class F> bool Capture(Tag<&Dispatcher::vkCmdClearColorImage>,Queue& q,F fn,VkCommandBuffer c,
 VkImage image,VkImageLayout layout,const VkClearColorValue* value,uint32_t count,const VkImageSubresourceRange* ranges) {
    if(!q.Reserve(size_t(count)*sizeof(*ranges))) return false;
    auto data=q.Copy(ranges,count);
    q.Record([=, color=*value] { fn(c,image,layout,&color,count,data.data()); }); return true;
}
template<class F> bool Capture(Tag<&Dispatcher::vkCmdClearDepthStencilImage>,Queue& q,F fn,VkCommandBuffer c,
 VkImage image,VkImageLayout layout,const VkClearDepthStencilValue* value,uint32_t count,const VkImageSubresourceRange* ranges) {
    if(!q.Reserve(size_t(count)*sizeof(*ranges))) return false;
    auto data=q.Copy(ranges,count);
    q.Record([=, depth=*value] { fn(c,image,layout,&depth,count,data.data()); }); return true;
}

template<class F> bool Capture(Tag<&Dispatcher::vkCmdSetBlendConstants>,
 Queue& q, F fn, VkCommandBuffer c, const float* values) {
    std::array<float,4> data; std::copy_n(values,4,data.begin());
    q.Record([=] { fn(c,data.data()); }); return true;
}

template<class F> bool Capture(Tag<&Dispatcher::vkCmdSetVertexInputEXT>,
 Queue& q, F fn, VkCommandBuffer c, uint32_t bc,
 const VkVertexInputBindingDescription2EXT* bindings, uint32_t ac,
 const VkVertexInputAttributeDescription2EXT* attributes) {
    if (!NoExtensions(std::span(bindings,bc)) || !NoExtensions(std::span(attributes,ac))) return false;
    if(!q.Reserve(size_t(bc)*sizeof(*bindings)+size_t(ac)*sizeof(*attributes))) return false;
    auto b=q.Copy(bindings,bc); auto a=q.Copy(attributes,ac);
    q.Record([=, b=std::move(b), a=std::move(a)] { fn(c,bc,b.data(),ac,a.data()); }); return true;
}

template<class F> bool Capture(Tag<&Dispatcher::vkCmdBindVertexBuffers>,
 Queue& q, F fn, VkCommandBuffer c, uint32_t first, uint32_t count,
 const VkBuffer* buffers, const VkDeviceSize* offsets) {
    if(!q.Reserve(size_t(count)*(sizeof(VkBuffer)+3*sizeof(VkDeviceSize)))) return false;
    auto b=q.Copy(buffers,count); auto o=q.Copy(offsets,count);
    q.Record([=, b=std::move(b), o=std::move(o)] { fn(c,first,count,b.data(),o.data()); }); return true;
}
template<auto M, class F> bool VertexBuffers2(Tag<M>, Queue& q, F fn, VkCommandBuffer c,
 uint32_t first, uint32_t count, const VkBuffer* buffers, const VkDeviceSize* offsets,
 const VkDeviceSize* sizes, const VkDeviceSize* strides) {
    if(!q.Reserve(size_t(count)*(sizeof(VkBuffer)+3*sizeof(VkDeviceSize)))) return false;
    auto b=q.Copy(buffers,count); auto o=q.Copy(offsets,count);
    auto s=q.Copy(sizes,count); auto t=q.Copy(strides,count);
    q.Record([=, b=std::move(b), o=std::move(o), s=std::move(s), t=std::move(t)] {
        fn(c,first,count,b.data(),o.data(),sizes?s.data():nullptr,strides?t.data():nullptr);
    }); return true;
}
#define VERTEX2(NAME) template<class F> bool Capture( \
 Tag<&Dispatcher::NAME> tag, Queue& q, F fn, VkCommandBuffer c, uint32_t first, uint32_t count, \
 const VkBuffer* b, const VkDeviceSize* o, const VkDeviceSize* s, const VkDeviceSize* t) { \
 return VertexBuffers2(tag,q,fn,c,first,count,b,o,s,t); }
VERTEX2(vkCmdBindVertexBuffers2)
VERTEX2(vkCmdBindVertexBuffers2EXT)
#undef VERTEX2

template<class F> bool Capture(Tag<&Dispatcher::vkCmdPushConstants>, Queue& q,
 F fn, VkCommandBuffer c, VkPipelineLayout layout, VkShaderStageFlags stages,
 uint32_t offset, uint32_t size, const void* values) {
    if(!q.Reserve(size)) return false;
    auto data=q.Copy(static_cast<const std::byte*>(values),size);
    q.Record([=, data=std::move(data)] { fn(c,layout,stages,offset,size,data.data()); }); return true;
}

template<class F> bool Capture(Tag<&Dispatcher::vkCmdBindDescriptorSets>, Queue& q,
 F fn, VkCommandBuffer c, VkPipelineBindPoint point, VkPipelineLayout layout, uint32_t first,
 uint32_t count, const VkDescriptorSet* sets, uint32_t dc, const uint32_t* offsets) {
    if(!q.Reserve(size_t(count)*sizeof(VkDescriptorSet)+size_t(dc)*sizeof(uint32_t))) return false;
    auto s=q.Copy(sets,count); auto d=q.Copy(offsets,dc);
    q.Record([=, s=std::move(s), d=std::move(d)] { fn(c,point,layout,first,count,s.data(),dc,d.data()); });
    return true;
}

struct DescriptorWrites {
    std::span<VkWriteDescriptorSet> writes;
    bool Snapshot(Queue& q,uint32_t count,const VkWriteDescriptorSet* source) {
        size_t bytes=size_t(count)*(sizeof(VkWriteDescriptorSet)+alignof(std::max_align_t));
        for(size_t i=0;i<count;++i) {
            const auto& w=source[i];
            if(w.pNext) return false;
            // Only members corresponding to descriptorType are defined by Vulkan.
            switch(w.descriptorType) {
            case VK_DESCRIPTOR_TYPE_SAMPLER: case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE: case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
                bytes+=size_t(w.descriptorCount)*sizeof(VkDescriptorImageInfo); break;
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
                bytes+=size_t(w.descriptorCount)*sizeof(VkDescriptorBufferInfo); break;
            case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER: case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
                bytes+=size_t(w.descriptorCount)*sizeof(VkBufferView); break;
            default: return false;
            }
        }
        if(!q.Reserve(bytes)) return false;
        writes=q.Copy(source,count);
        for(auto& w:writes) {
            switch(w.descriptorType) {
            case VK_DESCRIPTOR_TYPE_SAMPLER: case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE: case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
                w.pImageInfo=q.Copy(w.pImageInfo,w.descriptorCount).data(); break;
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
                w.pBufferInfo=q.Copy(w.pBufferInfo,w.descriptorCount).data(); break;
            case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER: case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
                w.pTexelBufferView=q.Copy(w.pTexelBufferView,w.descriptorCount).data(); break;
            default: std::terminate();
            }
        }
        return true;
    }
};
template<class F> bool Capture(Tag<&Dispatcher::vkCmdPushDescriptorSetKHR>, Queue& q,
 F fn, VkCommandBuffer c, VkPipelineBindPoint point, VkPipelineLayout layout, uint32_t set,
 uint32_t count, const VkWriteDescriptorSet* writes) {
    DescriptorWrites data;
    if(!data.Snapshot(q,count,writes)) return false;
    q.Record([=] { fn(c,point,layout,set,count,data.writes.data()); }); return true;
}

template<auto M, class F> bool Barrier2(Tag<M>, Queue& q, F fn,
 VkCommandBuffer c, const VkDependencyInfo* info) {
    if(info->pNext) return false;
    if(!NoExtensions(std::span(info->pMemoryBarriers,info->memoryBarrierCount)) ||
       !NoExtensions(std::span(info->pBufferMemoryBarriers,info->bufferMemoryBarrierCount)) ||
       !NoExtensions(std::span(info->pImageMemoryBarriers,info->imageMemoryBarrierCount))) return false;
    if(!q.Reserve(size_t(info->memoryBarrierCount)*sizeof(VkMemoryBarrier2)+
       size_t(info->bufferMemoryBarrierCount)*sizeof(VkBufferMemoryBarrier2)+
       size_t(info->imageMemoryBarrierCount)*sizeof(VkImageMemoryBarrier2))) return false;
    auto m=q.Copy(info->pMemoryBarriers,info->memoryBarrierCount);
    auto b=q.Copy(info->pBufferMemoryBarriers,info->bufferMemoryBarrierCount);
    auto i=q.Copy(info->pImageMemoryBarriers,info->imageMemoryBarrierCount);
    q.Record([=, value=*info, m=std::move(m), b=std::move(b), i=std::move(i)]() mutable {
        value.pMemoryBarriers=m.data(); value.pBufferMemoryBarriers=b.data(); value.pImageMemoryBarriers=i.data();
        fn(c,&value);
    }); return true;
}
#define BARRIER2(NAME) template<class F> bool Capture(Tag<&Dispatcher::NAME> tag, \
 Queue& q, F fn, VkCommandBuffer c, const VkDependencyInfo* info) { return Barrier2(tag,q,fn,c,info); }
BARRIER2(vkCmdPipelineBarrier2)
BARRIER2(vkCmdPipelineBarrier2KHR)
#undef BARRIER2

template<class F> bool Capture(Tag<&Dispatcher::vkCmdPipelineBarrier>, Queue& q,
 F fn, VkCommandBuffer c, VkPipelineStageFlags src, VkPipelineStageFlags dst, VkDependencyFlags flags,
 uint32_t mc, const VkMemoryBarrier* memory, uint32_t bc, const VkBufferMemoryBarrier* buffers,
 uint32_t ic, const VkImageMemoryBarrier* images) {
    if(!NoExtensions(std::span(memory,mc))||!NoExtensions(std::span(buffers,bc))||!NoExtensions(std::span(images,ic))) return false;
    if(!q.Reserve(size_t(mc)*sizeof(*memory)+size_t(bc)*sizeof(*buffers)+size_t(ic)*sizeof(*images))) return false;
    auto m=q.Copy(memory,mc); auto b=q.Copy(buffers,bc); auto i=q.Copy(images,ic);
    q.Record([=, m=std::move(m), b=std::move(b), i=std::move(i)] {
        fn(c,src,dst,flags,mc,m.data(),bc,b.data(),ic,i.data());
    }); return true;
}

template<auto M, class F> bool BeginRendering(Tag<M>, Queue& q,F fn,VkCommandBuffer c,const VkRenderingInfo* info) {
    if(info->pNext) return false;
    if(!NoExtensions(std::span(info->pColorAttachments,info->colorAttachmentCount)) || (info->pDepthAttachment && info->pDepthAttachment->pNext) ||
       (info->pStencilAttachment && info->pStencilAttachment->pNext)) return false;
    if(!q.Reserve(size_t(info->colorAttachmentCount)*sizeof(VkRenderingAttachmentInfo))) return false;
    auto colors=q.Copy(info->pColorAttachments,info->colorAttachmentCount);
    const auto depth=info->pDepthAttachment?*info->pDepthAttachment:VkRenderingAttachmentInfo{};
    const auto stencil=info->pStencilAttachment?*info->pStencilAttachment:VkRenderingAttachmentInfo{};
    q.Record([=, value=*info, colors=std::move(colors)]() mutable {
        value.pColorAttachments=colors.data();
        value.pDepthAttachment=value.pDepthAttachment?&depth:nullptr;
        value.pStencilAttachment=value.pStencilAttachment?&stencil:nullptr;
        fn(c,&value);
    }); return true;
}
#define BEGIN_RENDERING(NAME) template<class F> bool Capture(Tag<&Dispatcher::NAME> tag, \
 Queue& q,F fn,VkCommandBuffer c,const VkRenderingInfo* info) { return BeginRendering(tag,q,fn,c,info); }
BEGIN_RENDERING(vkCmdBeginRendering)
BEGIN_RENDERING(vkCmdBeginRenderingKHR)
#undef BEGIN_RENDERING

// Unknown commands and extension payloads use the synchronous fallback.
// No change to queue submissions or GPU completion semantics.
template<class F, auto Member> struct Hook;
template<class R,class... A,auto Member>
struct Hook<R(VKAPI_PTR*)(VkCommandBuffer,A...), Member> {
    inline static R(VKAPI_PTR* original)(VkCommandBuffer,A...);
    inline static std::atomic<uint64_t> queued{},direct{};
    static R VKAPI_CALL Call(VkCommandBuffer command,A... args) {
        if(auto* state=Find(command)) {
            if(!state->active) return original(command,args...);
            if constexpr(std::is_void_v<R>) {
                if(Capture(Tag<Member>{},state->queue,original,command,args...)) {
                    if(Statistics()) queued.fetch_add(1,std::memory_order_relaxed);
                    return;
                }
            }
            if(Statistics()) direct.fetch_add(1,std::memory_order_relaxed);
            state->queue.Drain();
        }
        return original(command,args...);
    }
};
} // namespace

void Initialize() {
    if(!Enabled()) return;
    static std::once_flag installed;
    std::call_once(installed, [] {
        auto& dispatcher=VULKAN_HPP_DEFAULT_DISPATCHER;
#define KYOEI_VK_COMMAND(NAME) if(dispatcher.NAME) { \
        using H=Hook<PFN_##NAME,&Dispatcher::NAME>; \
        H::original=dispatcher.NAME; dispatcher.NAME=&H::Call; }
#include "vulkan_recording/commands.inc"
#undef KYOEI_VK_COMMAND
        std::printf("Kyoei: Vulkan recording worker enabled (experimental)\n");
    });
}
void Attach(const void* owner,vk::CommandBuffer command) {
    if(!Enabled()) return;
    std::unique_lock lock(registry_mutex);
    auto& state=owners[owner];
    if(!state) { state=std::make_shared<State>(); state->owner=owner; }
    // Optional A/B control is sampled only at command-buffer boundaries,
    // after the preceding submission drained. Never mix recording modes in
    // a pending buffer, and never read a control file in a hot vkCmd wrapper.
    if(const auto* path=std::getenv("KYOEI_VULKAN_RECORDING_CONTROL")) {
        if(auto* file=std::fopen(path,"rb")) {
            state->active=std::fgetc(file)=='1';
            std::fclose(file);
        } else state->active=false;
    }
    commands[command]=state;
    epoch.fetch_add(1,std::memory_order_release);
}
void Drain(const void* owner) {
    if(!Enabled()) return;
    std::shared_ptr<State> state;
    { std::shared_lock lock(registry_mutex); auto i=owners.find(owner); if(i!=owners.end()) state=i->second; }
    if(state) state->queue.Drain();
}
void DrainCommand(vk::CommandBuffer command) {
    if(!Enabled()) return;
    if(auto* state=Find(command)) state->queue.Drain();
}
void Report() {
    if(!Enabled()) return;
    if(Statistics()) {
#define KYOEI_VK_COMMAND(NAME) { using H=Hook<PFN_##NAME,&Dispatcher::NAME>; \
        const auto queued=H::queued.load(); const auto direct=H::direct.load(); \
        if(queued||direct) std::printf("Kyoei recording stats: command=%s queued=%llu direct=%llu\n", \
        #NAME,static_cast<unsigned long long>(queued),static_cast<unsigned long long>(direct)); }
#include "vulkan_recording/commands.inc"
#undef KYOEI_VK_COMMAND
    }
}
void Detach(const void* owner) {
    if(!Enabled()) return;
    Drain(owner);
    std::unique_lock lock(registry_mutex);
    for(auto i=commands.begin();i!=commands.end();) {
        if(i->second->owner==owner) i=commands.erase(i); else ++i;
    }
    owners.erase(owner);
    epoch.fetch_add(1,std::memory_order_release);
}
}
