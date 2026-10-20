// Host-only grading; original guest images are never modified.
// SPDX-License-Identifier: GPL-2.0-or-later
#include "kyoei_photo_post.h"
#include "kyoei_photo_settings.h"
#include "kyoei_photo_hud.h"
#include "kyoei_overlay_barriers.h"
#include "video_core/renderer_vulkan/vk_instance.h"
#include "video_core/renderer_vulkan/vk_scheduler.h"
#include <array>
#include <unordered_map>
#include <stdexcept>
#include <mutex>
namespace KyoeiPhoto {
namespace {
vk::Device device;
vk::Format target_format;
vk::DescriptorSetLayout descriptor_layout;
vk::DescriptorPool pool;
vk::Sampler sampler;
vk::PipelineLayout layout;
vk::Pipeline pipeline;
std::unordered_map<void*,vk::DescriptorSet> sets;
struct Push {Grade grade;int encoded;};
static_assert(sizeof(Push)==20);
static constexpr uint32_t vertex[]=
#include "photo.vert.inc"
;
static constexpr uint32_t fragment[]=
#include "photo.frag.inc"
;
template<class T>T checked(vk::ResultValue<T> result){if(result.result!=vk::Result::eSuccess)throw std::runtime_error("Photo grading Vulkan allocation failed");return result.value;}
void initialize(const Vulkan::Instance& instance,vk::Format format) {
    if(pipeline && target_format==format)return;
    if(device){std::scoped_lock lock{Vulkan::Scheduler::submit_mutex};auto result=device.waitIdle();if(result!=vk::Result::eSuccess)throw std::runtime_error("Photo grading device wait failed");ShutdownPost();}
    device=instance.GetDevice();target_format=format;
    const vk::DescriptorSetLayoutBinding binding{0,vk::DescriptorType::eCombinedImageSampler,1,vk::ShaderStageFlagBits::eFragment};
    descriptor_layout=checked(device.createDescriptorSetLayout({.bindingCount=1,.pBindings=&binding}));
    const vk::DescriptorPoolSize size{vk::DescriptorType::eCombinedImageSampler,32};
    pool=checked(device.createDescriptorPool({.maxSets=32,.poolSizeCount=1,.pPoolSizes=&size}));
    sampler=checked(device.createSampler({.magFilter=vk::Filter::eLinear,.minFilter=vk::Filter::eLinear,
        .mipmapMode=vk::SamplerMipmapMode::eNearest,.addressModeU=vk::SamplerAddressMode::eClampToEdge,
        .addressModeV=vk::SamplerAddressMode::eClampToEdge,.addressModeW=vk::SamplerAddressMode::eClampToEdge,.maxLod=0}));
    const vk::PushConstantRange push{vk::ShaderStageFlagBits::eFragment,0,sizeof(Push)};
    layout=checked(device.createPipelineLayout({.setLayoutCount=1,.pSetLayouts=&descriptor_layout,.pushConstantRangeCount=1,.pPushConstantRanges=&push}));
    auto vs=checked(device.createShaderModule({.codeSize=sizeof(vertex),.pCode=vertex}));
    auto fs=checked(device.createShaderModule({.codeSize=sizeof(fragment),.pCode=fragment}));
    const std::array stages{
        vk::PipelineShaderStageCreateInfo{.stage=vk::ShaderStageFlagBits::eVertex,.module=vs,.pName="main"},
        vk::PipelineShaderStageCreateInfo{.stage=vk::ShaderStageFlagBits::eFragment,.module=fs,.pName="main"}};
    const vk::PipelineVertexInputStateCreateInfo input{};
    const vk::PipelineInputAssemblyStateCreateInfo assembly{.topology=vk::PrimitiveTopology::eTriangleList};
    const vk::PipelineViewportStateCreateInfo viewport{.viewportCount=1,.scissorCount=1};
    const vk::PipelineRasterizationStateCreateInfo raster{.polygonMode=vk::PolygonMode::eFill,
        .cullMode=vk::CullModeFlagBits::eNone,.frontFace=vk::FrontFace::eCounterClockwise,.lineWidth=1};
    const vk::PipelineMultisampleStateCreateInfo samples{.rasterizationSamples=vk::SampleCountFlagBits::e1};
    const vk::PipelineColorBlendAttachmentState color{.colorWriteMask=vk::ColorComponentFlagBits::eR|
        vk::ColorComponentFlagBits::eG|vk::ColorComponentFlagBits::eB|vk::ColorComponentFlagBits::eA};
    const vk::PipelineColorBlendStateCreateInfo blend{.attachmentCount=1,.pAttachments=&color};
    const std::array dynamics{vk::DynamicState::eViewport,vk::DynamicState::eScissor};
    const vk::PipelineDynamicStateCreateInfo dynamic{.dynamicStateCount=uint32_t(dynamics.size()),.pDynamicStates=dynamics.data()};
    const vk::PipelineRenderingCreateInfo rendering{.colorAttachmentCount=1,.pColorAttachmentFormats=&format};
    auto result=device.createGraphicsPipeline({},vk::GraphicsPipelineCreateInfo{.pNext=&rendering,
        .stageCount=uint32_t(stages.size()),.pStages=stages.data(),.pVertexInputState=&input,
        .pInputAssemblyState=&assembly,.pViewportState=&viewport,.pRasterizationState=&raster,
        .pMultisampleState=&samples,.pColorBlendState=&blend,.pDynamicState=&dynamic,.layout=layout});
    device.destroyShaderModule(vs);device.destroyShaderModule(fs);pipeline=checked(result);
}
}
void ShutdownPost() {
    if(!device)return;
    device.destroyPipeline(pipeline);device.destroyPipelineLayout(layout);
    device.destroyDescriptorPool(pool);device.destroySampler(sampler);device.destroyDescriptorSetLayout(descriptor_layout);
    pipeline=nullptr;layout=nullptr;pool=nullptr;sampler=nullptr;descriptor_layout=nullptr;device=nullptr;sets.clear();
}
void RenderPost(const Vulkan::Instance& instance,vk::CommandBuffer cmd,void* slot,vk::ImageView source,
    unsigned width,unsigned height,vk::ImageView target,vk::Extent2D extent,vk::Format format) {
    const Grade grade=GetGrade();
    if(!kyoei_photo_hud_hidden() || Neutral(grade))return;
    const bool srgb=format==vk::Format::eB8G8R8A8Srgb || format==vk::Format::eR8G8B8A8Srgb;
    if(!srgb && format!=vk::Format::eB8G8R8A8Unorm && format!=vk::Format::eR8G8B8A8Unorm)return;
    initialize(instance,format);
    auto& set=sets[slot];
    if(!set)set=checked(device.allocateDescriptorSets({.descriptorPool=pool,.descriptorSetCount=1,.pSetLayouts=&descriptor_layout})).front();
    const vk::DescriptorImageInfo image{sampler,source,vk::ImageLayout::eGeneral};
    const vk::WriteDescriptorSet write{.dstSet=set,.descriptorCount=1,.descriptorType=vk::DescriptorType::eCombinedImageSampler,.pImageInfo=&image};
    device.updateDescriptorSets(write,{});
    const auto attachment=KyoeiOverlay::Attachment(target);
    cmd.beginRendering({.renderArea={{0,0},extent},.layerCount=1,.colorAttachmentCount=1,.pColorAttachments=&attachment});
    const float scale=std::min(float(extent.width)/width,float(extent.height)/height);
    const float w=width*scale,h=height*scale;
    const vk::Viewport viewport{(extent.width-w)*.5f,(extent.height-h)*.5f,w,h,0,1};
    const vk::Rect2D scissor{{0,0},extent};
    cmd.setViewport(0,viewport);cmd.setScissor(0,scissor);
    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics,pipeline);
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,layout,0,set,{});
    const Push push{grade,srgb?0:1};
    cmd.pushConstants(layout,vk::ShaderStageFlagBits::eFragment,0,sizeof(push),&push);
    cmd.draw(3,1,0,0);cmd.endRendering();
    const vk::MemoryBarrier barrier{.srcAccessMask=vk::AccessFlagBits::eColorAttachmentWrite,
        .dstAccessMask=vk::AccessFlagBits::eColorAttachmentRead|vk::AccessFlagBits::eColorAttachmentWrite};
    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,vk::PipelineStageFlagBits::eColorAttachmentOutput,{},barrier,{},{});
}
}
