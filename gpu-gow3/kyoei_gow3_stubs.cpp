// Kyoei: pieces of the GoW3 port outside its GPU folder. Its in-game menu (Gow3Overlay)
// is not used: Kyoei has its own overlay. Runtime services map to the Kyoei runtime.
// SPDX-License-Identifier: GPL-2.0-or-later
#include <cstddef>
#include <string>
#include "gow3_overlay.h"
extern "C" {
#include "gz.h"
void gz_thread_attach_host(const char*);
}

namespace Gow3Overlay {
bool Visible() { return false; }
void Init(const Vulkan::Instance&, vk::Format, u32) {}
void Render(vk::CommandBuffer, vk::ImageView, vk::Extent2D) {}
void SetBackgroundLoading(u32, u32) {}
void SetLoading(bool, u32, u32) {}
void ShowNotice(const std::string&, int) {}
}

extern "C" int runtime_file_translate(const char* guest, char* out, size_t size) {
    return gz_file_translate(guest, out, size); // 0 on success in both
}
extern "C" void runtime_thread_attach_host(const char* name) { gz_thread_attach_host(name); }
