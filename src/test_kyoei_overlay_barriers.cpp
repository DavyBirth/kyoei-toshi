// Construct actual Vulkan descriptors; no instance, device, driver or game.
#include "kyoei_overlay_barriers.h"
#include <cstdio>
#include <cstdlib>
#define CHECK(x) do { if (!(x)) { std::fprintf(stderr,"line %d: %s\n",__LINE__,#x); std::exit(1); } } while(0)
int main() {
    vk::Image image(reinterpret_cast<VkImage>(uintptr_t(0x1234)));
    vk::ImageView view(reinterpret_cast<VkImageView>(uintptr_t(0x5678)));
    vk::ImageSubresourceRange range{vk::ImageAspectFlagBits::eColor,0,1,0,1};
    auto direct=KyoeiOverlay::BeforeDraw(image,range,false);
    CHECK(direct.oldLayout==vk::ImageLayout::eTransferDstOptimal && direct.newLayout==vk::ImageLayout::ePresentSrcKHR);
    CHECK(direct.srcAccessMask==vk::AccessFlagBits::eTransferWrite && direct.dstAccessMask==vk::AccessFlagBits::eNone);
    auto before=KyoeiOverlay::BeforeDraw(image,range,true);
    CHECK(before.newLayout==vk::ImageLayout::eColorAttachmentOptimal);
    CHECK(before.dstAccessMask==(vk::AccessFlagBits::eColorAttachmentRead|vk::AccessFlagBits::eColorAttachmentWrite));
    auto attachment=KyoeiOverlay::Attachment(view);
    CHECK(attachment.imageView==view && attachment.imageLayout==before.newLayout);
    CHECK(attachment.loadOp==vk::AttachmentLoadOp::eLoad && attachment.storeOp==vk::AttachmentStoreOp::eStore);
    auto after=KyoeiOverlay::AfterDraw(image,range);
    CHECK(after.oldLayout==attachment.imageLayout && after.newLayout==vk::ImageLayout::ePresentSrcKHR);
    CHECK(after.srcAccessMask==vk::AccessFlagBits::eColorAttachmentWrite && after.dstAccessMask==vk::AccessFlagBits::eNone);
    CHECK(before.image==image && after.image==image && before.subresourceRange==range && after.subresourceRange==range);
    CHECK(before.srcQueueFamilyIndex==VK_QUEUE_FAMILY_IGNORED && after.dstQueueFamilyIndex==VK_QUEUE_FAMILY_IGNORED);
    std::puts("OVERLAY BARRIERS PASS: direct/overlay transitions, frame Load, Store and final Present");
}
