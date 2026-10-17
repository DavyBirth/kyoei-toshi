#include "kyoei_vulkan_recording.h"
#include "vulkan_recording/queue.h"
#include <cassert>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <vector>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace {
std::vector<uint32_t> observed;
std::thread::id producer;
std::atomic<bool> delegated{};
unsigned transfers{};
void Transfer() { assert(std::this_thread::get_id()!=producer); ++transfers; }
void VKAPI_CALL ToImage(VkCommandBuffer,VkBuffer,VkImage,VkImageLayout,uint32_t n,const VkBufferImageCopy* p) {
    Transfer(); assert(n==1 && p->bufferOffset==13 && p->imageExtent.width==17);
}
void VKAPI_CALL FromImage(VkCommandBuffer,VkImage,VkImageLayout,VkBuffer,uint32_t n,const VkBufferImageCopy* p) {
    Transfer(); assert(n==1 && p->bufferOffset==13 && p->imageExtent.width==17);
}
void VKAPI_CALL CopyImage(VkCommandBuffer,VkImage,VkImageLayout,VkImage,VkImageLayout,uint32_t n,const VkImageCopy* p) {
    Transfer(); assert(n==1 && p->srcOffset.x==-2 && p->dstSubresource.layerCount==3);
}
void VKAPI_CALL Blit(VkCommandBuffer,VkImage,VkImageLayout,VkImage,VkImageLayout,uint32_t n,const VkImageBlit* p,VkFilter filter) {
    Transfer(); assert(n==1 && p->srcOffsets[1].x==42 && filter==VK_FILTER_LINEAR);
}
void VKAPI_CALL ClearColor(VkCommandBuffer,VkImage,VkImageLayout,const VkClearColorValue* p,uint32_t n,const VkImageSubresourceRange* r) {
    Transfer(); assert(n==1 && p->uint32[2]==0x12345678 && r->levelCount==4);
}
void VKAPI_CALL ClearDepth(VkCommandBuffer,VkImage,VkImageLayout,const VkClearDepthStencilValue* p,uint32_t n,const VkImageSubresourceRange* r) {
    Transfer(); assert(n==1 && p->depth==0.5f && p->stencil==7 && r->levelCount==4);
}
void VKAPI_CALL Viewport(VkCommandBuffer,uint32_t first,uint32_t n,const VkViewport* p) {
    Transfer(); assert(first==1 && n==1 && p->width==640);
}
void VKAPI_CALL Scissor(VkCommandBuffer,uint32_t first,uint32_t n,const VkRect2D* p) {
    Transfer(); assert(first==2 && n==1 && p->extent.width==1280);
}
void VKAPI_CALL Draw(VkCommandBuffer, uint32_t value, uint32_t, uint32_t, uint32_t) {
    delegated.store(std::this_thread::get_id()!=producer);
    observed.push_back(value);
}
void VKAPI_CALL Push(VkCommandBuffer,VkPipelineLayout,VkShaderStageFlags,uint32_t,uint32_t size,const void* data) {
    assert(size==4); observed.push_back(*static_cast<const uint32_t*>(data));
}
void VKAPI_CALL Barrier(VkCommandBuffer,const VkDependencyInfo* info) {
    observed.push_back(info->pBufferMemoryBarriers[0].srcQueueFamilyIndex);
}
void VKAPI_CALL Rendering(VkCommandBuffer,const VkRenderingInfo* info) {
    assert(info->pColorAttachments[0].clearValue.color.uint32[0]==31);
    assert(info->pDepthAttachment->clearValue.depthStencil.depth==0.5f);
    assert(!info->pStencilAttachment);
    observed.push_back(info->renderArea.extent.width);
}
void VKAPI_CALL Descriptors(VkCommandBuffer,VkPipelineBindPoint,VkPipelineLayout,uint32_t,
 uint32_t count,const VkWriteDescriptorSet* writes) {
    assert(count==2);
    observed.push_back(uint32_t(writes[0].pBufferInfo->offset));
    observed.push_back(uint32_t(writes[1].pImageInfo->imageLayout));
}
// Region arrays must also survive mutation immediately after the call.
void VKAPI_CALL Copy(VkCommandBuffer,VkBuffer,VkBuffer,uint32_t,const VkBufferCopy* data) {
    assert(std::this_thread::get_id()!=producer);
    observed.push_back(uint32_t(data->size));
}
void VKAPI_CALL ColorMask(VkCommandBuffer,uint32_t first,uint32_t count,const VkColorComponentFlags* values) {
    assert(std::this_thread::get_id()!=producer);
    assert(first==3 && count==2 && values[0]==4 && values[1]==8);
    observed.push_back(12);
}
}
int main() {
    producer=std::this_thread::get_id();
    {
        KyoeiVulkanRecording::Queue q;
        std::vector<unsigned> values;
        for(unsigned i=0;i<100000;++i) q.Record([i,&values] { values.push_back(i); });
        q.Drain();
        assert(values.size()==100000);
        for(unsigned i=0;i<values.size();++i) assert(values[i]==i);
        values.clear();
        for(unsigned i=0;i<100000;++i) {
            assert(q.Reserve(sizeof(i)));
            unsigned source=i;
            auto snapshot=q.Copy(&source,1);
            q.Record([snapshot,&values] { values.push_back(snapshot[0]); });
            source=~i;
        }
        q.Drain();
        assert(values.size()==100000);
        for(unsigned i=0;i<values.size();++i) assert(values[i]==i);
        assert(!q.Reserve(65536));
    }
    _putenv_s("KYOEI_VULKAN_RECORDING","1");
    auto& d=VULKAN_HPP_DEFAULT_DISPATCHER;
    d.vkCmdDraw=Draw; d.vkCmdPushConstants=Push; d.vkCmdPipelineBarrier2=Barrier;
    d.vkCmdBeginRendering=Rendering; d.vkCmdPushDescriptorSetKHR=Descriptors; d.vkCmdCopyBuffer=Copy;
    d.vkCmdSetColorWriteMaskEXT=ColorMask;
    d.vkCmdCopyBufferToImage=ToImage; d.vkCmdCopyImageToBuffer=FromImage;
    d.vkCmdCopyImage=CopyImage; d.vkCmdBlitImage=Blit;
    d.vkCmdClearColorImage=ClearColor; d.vkCmdClearDepthStencilImage=ClearDepth;
    d.vkCmdSetViewport=Viewport; d.vkCmdSetScissor=Scissor;
    KyoeiVulkanRecording::Initialize();
    const auto cmd=reinterpret_cast<VkCommandBuffer>(uintptr_t(0x100));
    int owner;
    KyoeiVulkanRecording::Attach(&owner,cmd);
    d.vkCmdDraw(cmd,1,1,0,0);
    uint32_t data=2;
    d.vkCmdPushConstants(cmd,{},0,0,4,&data); data=999;
    VkBufferMemoryBarrier2 buffer{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2};
    buffer.srcQueueFamilyIndex=3;
    VkDependencyInfo dep{VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
    dep.bufferMemoryBarrierCount=1; dep.pBufferMemoryBarriers=&buffer;
    d.vkCmdPipelineBarrier2(cmd,&dep); buffer.srcQueueFamilyIndex=999;
    VkRenderingAttachmentInfo color{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
    color.clearValue.color.uint32[0]=31;
    VkRenderingAttachmentInfo depth{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
    depth.clearValue.depthStencil.depth=0.5f;
    VkRenderingInfo rendering{VK_STRUCTURE_TYPE_RENDERING_INFO};
    rendering.renderArea.extent.width=4; rendering.colorAttachmentCount=1;
    rendering.pColorAttachments=&color; rendering.pDepthAttachment=&depth;
    d.vkCmdBeginRendering(cmd,&rendering);
    color.clearValue={}; depth.clearValue={}; rendering.renderArea.extent.width=999;
    VkDescriptorBufferInfo db{}; db.offset=5;
    VkDescriptorImageInfo di{}; di.imageLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    VkWriteDescriptorSet writes[2]{};
    writes[0].descriptorType=VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; writes[0].descriptorCount=1;
    writes[0].pBufferInfo=&db;
    // Poison unused fields: Vulkan does not require them to be initialized.
    writes[0].pImageInfo=reinterpret_cast<const VkDescriptorImageInfo*>(uintptr_t(1));
    writes[1].descriptorType=VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE; writes[1].descriptorCount=1;
    writes[1].pImageInfo=&di;
    d.vkCmdPushDescriptorSetKHR(cmd,VK_PIPELINE_BIND_POINT_GRAPHICS,{},0,2,writes);
    db.offset=999; di.imageLayout=VK_IMAGE_LAYOUT_UNDEFINED;
    VkBufferCopy copy{}; copy.size=7;
    d.vkCmdCopyBuffer(cmd,{},{},1,&copy);
    copy.size=999;
    KyoeiVulkanRecording::Drain(&owner);
    assert((observed==std::vector<uint32_t>{1,2,3,4,5,uint32_t(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),7}));
    assert(delegated.load());
    // pNext is deliberately unsupported: its backing storage is not copied.
    VkMemoryBarrier2 extra{VK_STRUCTURE_TYPE_MEMORY_BARRIER_2};
    buffer.srcQueueFamilyIndex=8; dep.pNext=&extra;
    d.vkCmdDraw(cmd,9,1,0,0);
    d.vkCmdPipelineBarrier2(cmd,&dep);
    assert(observed[7]==9 && observed[8]==8);
    VkColorComponentFlags masks[]{4,8};
    d.vkCmdSetColorWriteMaskEXT(cmd,3,2,masks);
    masks[0]=masks[1]=999;
    // Simulates an external renderer with its own private Vulkan entry points.
    KyoeiVulkanRecording::DrainCommand(cmd);
    assert(observed.back()==12);
    VkBufferImageCopy region{}; region.bufferOffset=13; region.imageExtent.width=17;
    d.vkCmdCopyBufferToImage(cmd,{},{},VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&region);
    d.vkCmdCopyImageToBuffer(cmd,{},VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,{},1,&region);
    region={};
    VkImageCopy image_copy{}; image_copy.srcOffset.x=-2; image_copy.dstSubresource.layerCount=3;
    d.vkCmdCopyImage(cmd,{},VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,{},VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&image_copy);
    image_copy={};
    VkImageBlit blit{}; blit.srcOffsets[1].x=42;
    d.vkCmdBlitImage(cmd,{},VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,{},VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&blit,VK_FILTER_LINEAR);
    blit={};
    VkImageSubresourceRange range{}; range.levelCount=4;
    VkClearColorValue clear{}; clear.uint32[2]=0x12345678;
    VkClearDepthStencilValue ds{0.5f,7};
    d.vkCmdClearColorImage(cmd,{},VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,&clear,1,&range);
    d.vkCmdClearDepthStencilImage(cmd,{},VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,&ds,1,&range);
    range={}; clear={}; ds={};
    VkViewport viewport{}; viewport.width=640;
    VkRect2D scissor{}; scissor.extent.width=1280;
    d.vkCmdSetViewport(cmd,1,1,&viewport); d.vkCmdSetScissor(cmd,2,1,&scissor);
    viewport={}; scissor={};
    KyoeiVulkanRecording::Drain(&owner);
    assert(transfers==8);
    KyoeiVulkanRecording::Detach(&owner);
    // The same command handle after unregistering goes directly to the driver.
    d.vkCmdDraw(cmd,10,1,0,0);
    assert(!delegated.load() && observed.back()==10);
    // Reusing the scheduler address must replace stale TLS registry entries.
    KyoeiVulkanRecording::Attach(&owner,cmd);
    d.vkCmdDraw(cmd,11,1,0,0);
    KyoeiVulkanRecording::Drain(&owner);
    assert(delegated.load() && observed.back()==11);
    KyoeiVulkanRecording::Detach(&owner);
    std::puts("Vulkan recording: order, ownership, fallback, lifetime and backlog PASS");
}
