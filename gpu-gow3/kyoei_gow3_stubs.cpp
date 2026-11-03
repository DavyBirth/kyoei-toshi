// Kyoei: pieces of the GoW3 port outside its GPU folder. Its in-game menu (Gow3Overlay)
// is replaced by Kyoei's overlay (dialogs, title exit, photo HUD), drawn by the GoW3
// presenter at the same point. Runtime services map to the Kyoei runtime.
// SPDX-License-Identifier: GPL-2.0-or-later
#include <cstddef>
#include <string>
#include "gow3_overlay.h"
#include "kyoei_overlay.h"
#include "kyoei_pause_photo.h"
#include "kyoei_overlay_gpu.h"
extern "C" {
#include "gz.h"
#include "kyoei_title_exit.h"
int kyoei_photo_hud_hidden(void);
void gz_thread_attach_host(const char*);
}

namespace Gow3Overlay {
// The presenter asks this for every frame. A frame without the overlay already shows its
// closed state: acknowledge it, or the driver kept redrawing the last frame on every idle
// vblank after the first dialog closed (~60 -> ~33 FPS).
bool Visible() {
    const bool visible =
        kyoei_overlay_kind() != 0 || kyoei_title_exit_state() != 0 || kyoei_photo_hud_hidden() || kyoei_pause_photo_visible();
    if (!visible) {
        kyoei_overlay_presented(kyoei_overlay_revision());
    }
    return visible;
}
void Init(const Vulkan::Instance& instance, vk::Format format, u32 images) {
    KyoeiOverlay::Initialize(instance, format, images);
}
// Also called for the start-up loading frame; the Kyoei overlay draws only its own state.
void Render(vk::CommandBuffer command, vk::ImageView view, vk::Extent2D extent) {
    const auto revision = kyoei_overlay_revision();
    KyoeiOverlay::Render(command, view, extent);
    kyoei_overlay_presented(revision);
}
void SetBackgroundLoading(u32, u32) {}
void SetLoading(bool, u32, u32) {}
void ShowNotice(const std::string&, int) {}
}

extern "C" int runtime_file_translate(const char* guest, char* out, size_t size) {
    return gz_file_translate(guest, out, size); // 0 on success in both
}
extern "C" void runtime_thread_attach_host(const char* name) { gz_thread_attach_host(name); }

// The loader's GPU test mode (gz_gpu_self_test) has no test on this core.
extern "C" void gz_gpu_self_test(void) { gz_log("GPU TEST: not available on the GoW3 core"); }
