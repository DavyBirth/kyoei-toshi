// SPDX-License-Identifier: GPL-2.0-or-later
// Vulkan initialization and attachment composition adapted from
// FelipePrado0/gow3_pc ec9b416f2415fc5a0469076aecc230c6d6027ac0,
// gpu/shim/gow3_overlay.cpp (GPL-2.0-or-later). Modal/state/UI implementation
// is separate and Kyoei-specific. Dear ImGui itself retains its MIT license.
#include "kyoei_overlay_gpu.h"
#include "kyoei_vulkan_recording.h"
#include "kyoei_overlay_barriers.h"
#include "kyoei_overlay.h"
#include "kyoei_input_mode.h"
#include "kyoei_title_exit.h"
#include "kyoei_photo.h"
#include "kyoei_photo_panel.h"
#include "kyoei_photo_dof.h"
#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "video_core/renderer_vulkan/vk_instance.h"
#include "video_core/renderer_vulkan/vk_scheduler.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <mutex>
extern "C" {
#include "gz.h"
}
namespace KyoeiOverlay {
namespace {
bool initialized{};
vk::Format surface_format{};
VkFormat color_format{};
unsigned images{};
std::chrono::steady_clock::time_point last_draw;
void checked(VkResult result) {
    if (result<0) gz_stop(23,"Kyoei overlay: Vulkan result %d",(int)result);
}
}
void Initialize(const Vulkan::Instance &instance,vk::Format format,unsigned image_count) {
    const char *setting=std::getenv("KYOEI_IN_GAME_DIALOGS"),*title=std::getenv("KYOEI_TITLE_EXIT"),*photo=std::getenv("KYOEI_PHOTO_HUD");
    if ((!setting || std::strcmp(setting,"1")) && (!title || std::strcmp(title,"1")) &&
        (!photo || std::strcmp(photo,"1"))) return;
    if (initialized && surface_format==format && images==image_count) return;
    if (initialized) {
        // Rebuilding backend allocations requires all previous draws to finish.
        { std::scoped_lock lock{Vulkan::Scheduler::submit_mutex};
          checked(VULKAN_HPP_DEFAULT_DISPATCHER.vkDeviceWaitIdle(instance.GetDevice())); }
        ImGui_ImplVulkan_Shutdown();
        ImGui::DestroyContext();
        initialized=false;
    }
    const char *font_directory=std::getenv("KYOEI_GUI_FONT_DIR");
    KyoeiOverlayModel::InitializeGui(font_directory && *font_directory ? font_directory : KYOEI_GUI_FONT_DIR,false);
    const vk::Instance handle=instance.GetInstance();
    if (!ImGui_ImplVulkan_LoadFunctions(instance.ApiVersion(),[](const char *name,void *user) {
        return VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr(*static_cast<const vk::Instance *>(user),name);
    },const_cast<vk::Instance *>(&handle))) {
        KyoeiOverlayModel::SetReady(false); KyoeiOverlayModel::ShutdownGui(); return;
    }
    color_format=static_cast<VkFormat>(format);
    ImGui_ImplVulkan_InitInfo info{};
    info.ApiVersion=instance.ApiVersion(); info.Instance=handle;
    info.PhysicalDevice=instance.GetPhysicalDevice(); info.Device=instance.GetDevice();
    info.QueueFamily=instance.GetGraphicsQueueFamilyIndex(); info.Queue=instance.GetGraphicsQueue();
    info.DescriptorPoolSize=32;
    info.MinImageCount=std::max(image_count,2u); info.ImageCount=std::max(image_count,2u);
    info.UseDynamicRendering=true; info.CheckVkResultFn=checked;
    info.PipelineInfoMain.PipelineRenderingCreateInfo={
        .sType=VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .colorAttachmentCount=1,.pColorAttachmentFormats=&color_format};
    if (!ImGui_ImplVulkan_Init(&info)) {
        KyoeiOverlayModel::SetReady(false); KyoeiOverlayModel::ShutdownGui(); return;
    }
    surface_format=format; images=image_count; initialized=true;
    KyoeiOverlayModel::SetReady(true);
    gz_log("Kyoei overlay: Vulkan modal backend ready; physical cross confirms, circle cancels");
}
void Shutdown() {
    if (!initialized) return;
    KyoeiOverlayModel::SetReady(false);
    // Presenter finishes its schedulers before calling this.
    ImGui_ImplVulkan_Shutdown(); KyoeiOverlayModel::ShutdownGui();
    initialized=false;
}
void UpdateTextInput(SDL_Window *window) {
    bool want=kyoei_overlay_wants_text()!=0;
    if (want!=SDL_TextInputActive(window)) {
        if (want) SDL_StartTextInput(window); else SDL_StopTextInput(window);
    }
}
bool HandleEvent(const SDL_Event &event) {
    if (event.type==SDL_EVENT_QUIT || event.type==SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
        kyoei_overlay_abort_all(); return false;
    }
    if (!kyoei_overlay_kind()) {
        if (!kyoei_title_exit_state()) return false;
        // The title releases the mouse pointer so the PC entry can be clicked.
        if (event.type==SDL_EVENT_MOUSE_MOTION || event.type==SDL_EVENT_MOUSE_BUTTON_DOWN) {
            auto id=event.type==SDL_EVENT_MOUSE_MOTION?event.motion.windowID:event.button.windowID;
            auto *window=SDL_GetWindowFromID(id);
            float density=window?SDL_GetWindowPixelDensity(window):1.f;
            float x=(event.type==SDL_EVENT_MOUSE_MOTION?event.motion.x:event.button.x)*density;
            float y=(event.type==SDL_EVENT_MOUSE_MOTION?event.motion.y:event.button.y)*density;
            int inside=kyoei_title_exit_hit(x,y);
            kyoei_title_exit_hover(inside);
            if (inside && event.type==SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button==SDL_BUTTON_LEFT)
            {
                int consumed=kyoei_title_exit_click();
                if (kyoei_title_exit_take_quit()) { SDL_Event quit{}; quit.type=SDL_EVENT_QUIT; SDL_PushEvent(&quit); }
                return consumed!=0;
            }
        }
        return false;
    }
    if (event.type==SDL_EVENT_TEXT_INPUT) { kyoei_overlay_text(event.text.text); return true; }
    if (event.type==SDL_EVENT_KEY_DOWN) {
        if (event.key.key==SDLK_ESCAPE && event.key.repeat) return true;
        int key=-1;
        switch (event.key.key) {
        case SDLK_LEFT: key=KYOEI_OVERLAY_KEY_LEFT; break;
        case SDLK_RIGHT: key=KYOEI_OVERLAY_KEY_RIGHT; break;
        case SDLK_UP: key=KYOEI_OVERLAY_KEY_UP; break;
        case SDLK_DOWN: key=KYOEI_OVERLAY_KEY_DOWN; break;
        case SDLK_BACKSPACE: key=KYOEI_OVERLAY_KEY_BACKSPACE; break;
        case SDLK_DELETE: key=KYOEI_OVERLAY_KEY_DELETE; break;
        case SDLK_HOME: key=KYOEI_OVERLAY_KEY_HOME; break;
        case SDLK_END: key=KYOEI_OVERLAY_KEY_END; break;
        case SDLK_RETURN: case SDLK_KP_ENTER: if (!event.key.repeat) key=KYOEI_OVERLAY_KEY_ACCEPT; break;
        case SDLK_ESCAPE: key=KYOEI_OVERLAY_KEY_CANCEL; break;
        case SDLK_A: if (event.key.mod&SDL_KMOD_CTRL) key=KYOEI_OVERLAY_KEY_SELECT_ALL; break;
        }
        if (key>=0) kyoei_overlay_key((unsigned)key);
        return true;
    }
    // Releases still reach the pad bridge to clear keys held before the modal.
    if (event.type==SDL_EVENT_KEY_UP) return false;
    if (event.type==SDL_EVENT_MOUSE_MOTION || event.type==SDL_EVENT_MOUSE_BUTTON_DOWN || event.type==SDL_EVENT_MOUSE_BUTTON_UP) {
        auto id=event.type==SDL_EVENT_MOUSE_MOTION?event.motion.windowID:event.button.windowID;
        auto *window=SDL_GetWindowFromID(id);
        float density=window?SDL_GetWindowPixelDensity(window):1.f;
        if (event.type==SDL_EVENT_MOUSE_MOTION) kyoei_overlay_mouse(0,event.motion.x*density,event.motion.y*density,0,0);
        else {
            int button=event.button.button==SDL_BUTTON_LEFT?0:event.button.button==SDL_BUTTON_RIGHT?1:event.button.button==SDL_BUTTON_MIDDLE?2:-1;
            kyoei_overlay_mouse(0,event.button.x*density,event.button.y*density,0,0);
            if (button>=0) kyoei_overlay_mouse(1,0,0,button,event.type==SDL_EVENT_MOUSE_BUTTON_DOWN);
        }
        return true;
    }
    if (event.type==SDL_EVENT_MOUSE_WHEEL) { kyoei_overlay_mouse(2,event.wheel.x,event.wheel.y,0,0); return true; }
    // Physical gamepad events keep flowing to gz_pad_poll, whose real button
    // state is consumed once by kyoei_overlay_input; no duplicate confirm.
    return false;
}
void Render(vk::CommandBuffer command,vk::ImageView view,vk::Extent2D extent) {
    if (!initialized) return;
    auto now=std::chrono::steady_clock::now();
    float elapsed=std::chrono::duration<float>(now-last_draw).count(); last_draw=now;
    ImGui_ImplVulkan_NewFrame();
    auto *draw=KyoeiOverlayModel::Draw(extent.width,extent.height,elapsed,[]{
        if(!kyoei_photo_hud_hidden() || !KyoeiPhoto::hints.load() || kyoei_overlay_kind())return;
        KyoeiPhoto::DrawPanel(kyoei_input_current_mode()==KYOEI_INPUT_CONTROLLER,kyoei_photo_player_hidden()!=0,
            kyoei_photo_scene_fov_value(),kyoei_photo_scene_roll_value(),kyoei_photo_dof_available()!=0);
    });
    const auto attachment=Attachment(view);
    command.beginRendering(vk::RenderingInfo{
        .renderArea={{0,0},extent},.layerCount=1,.colorAttachmentCount=1,.pColorAttachments=&attachment});
    {
        // ImGui font uploads submit to this same queue. Match the scheduler's
        // host synchronization rather than racing concurrent guest submissions.
        std::scoped_lock lock{Vulkan::Scheduler::submit_mutex};
        KyoeiVulkanRecording::DrainCommand(command);
        ImGui_ImplVulkan_RenderDrawData(draw,command);
    }
    command.endRendering();
}
}
