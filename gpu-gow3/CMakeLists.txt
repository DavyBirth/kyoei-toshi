# Kyoei on the GoW3 PC GPU core (third_party/gow3-gpu, bbport-derived, GPL-2.0).
# The fixed copy is not edited; Kyoei adaptations live here and in src/.
# Stage 1: compile the core with Kyoei's toolchain (clang, MSVC ABI) and the
# dependency headers the current renderer already uses.
cmake_minimum_required(VERSION 3.28)
project(kyoei_gow3gpu C CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(GOW3 "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/gow3-gpu")
set(FSR_VULKAN "${CMAKE_CURRENT_SOURCE_DIR}/../build/fsr-vulkan-src") # FSR-Vulkan source (MIT)
set(DEPS "G:/godzilla-port-codex/build/_deps" CACHE PATH "Existing GPU dependency header cache")
find_program(GLSLANG_COMPILER NAMES glslangValidator glslang HINTS "$ENV{VULKAN_SDK}/Bin" REQUIRED)

# Host shaders -> headers, same list as the GoW3 build.
set(SHADER_SRC "${GOW3}/shadps4/video_core/host_shaders")
set(SHADER_INCLUDE "${CMAKE_CURRENT_BINARY_DIR}/include")
set(SHADER_DIR "${SHADER_INCLUDE}/video_core/host_shaders")
set(SHADER_HEADERS "")
function(add_host_shader FILENAME SHADER_NAME)
    set(OUT "${SHADER_DIR}/${SHADER_NAME}.h")
    string(TOUPPER ${SHADER_NAME} SYMBOL)
    set(DEFINES ${ARGN})
    list(TRANSFORM DEFINES PREPEND "-D")
    add_custom_command(OUTPUT "${OUT}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_DIR}"
        COMMAND "${GLSLANG_COMPILER}" --target-env vulkan1.3 -o "${OUT}" --vn ${SYMBOL} ${DEFINES} "${SHADER_SRC}/${FILENAME}"
        COMMAND ${CMAKE_COMMAND} "-DHEADER_FILE=${OUT}" -P "${SHADER_SRC}/AddShaderHeaderInclude.cmake"
        DEPENDS "${SHADER_SRC}/${FILENAME}" VERBATIM)
    set(SHADER_HEADERS ${SHADER_HEADERS} "${OUT}" PARENT_SCOPE)
endfunction()
add_host_shader(color_to_ms_depth.frag color_to_ms_depth_frag)
add_host_shader(color_to_ms_depth.frag color_to_ms_depth_fallback_frag SAMPLE_FALLBACK=1)
add_host_shader(ms_image_blit.frag ms_image_blit_frag)
add_host_shader(ms_image_blit.frag ms_image_blit_msaa_frag SRC_MSAA=1)
add_host_shader(fault_buffer_process.comp fault_buffer_process_comp)
add_host_shader(fs_tri.vert fs_tri_vert)
add_host_shader(fsr.comp fsr_easu_comp SAMPLE_EASU=1)
add_host_shader(fsr.comp fsr_rcas_comp SAMPLE_RCAS=1)
add_host_shader(post_process.frag post_process_frag)
add_host_shader(camera_motion.comp camera_motion_comp)
add_host_shader(upscale_merge.comp upscale_merge_comp)
add_host_shader(upscale_reactive.comp upscale_reactive_comp)
add_host_shader(taa.comp taa_comp)
add_host_shader(taa_sharpen.comp taa_sharpen_comp)
add_host_shader(taa_sharpen.comp taa_sharpen_ldr_comp LDR_OUTPUT=1)
add_host_shader(buffer_multi_copy.comp buffer_multi_copy_comp)
add_host_shader(depth_resample.frag depth_resample_frag)
add_host_shader(depth_resample.frag depth_stencil_resample_frag HAS_STENCIL=1)
add_host_shader(depth_resample.frag depth_stencil_bits_frag STENCIL_BITS=1)
add_host_shader(camera_motion.comp camera_motion_debug_comp DEBUG_OVERLAY=1)
foreach(BPP IN ITEMS 8 16 32 64 96 128)
    add_host_shader(tiling.comp tiling_micro_${BPP}_comp BITS_PER_PIXEL=${BPP} IS_MACRO_TILED=0)
    add_host_shader(tiling.comp tiling_macro_${BPP}_comp BITS_PER_PIXEL=${BPP} IS_MACRO_TILED=1)
endforeach()
add_custom_target(gow3_host_shaders DEPENDS ${SHADER_HEADERS})

file(GLOB_RECURSE VIDEO_CORE CONFIGURE_DEPENDS "${GOW3}/shadps4/video_core/*.cpp")
file(GLOB_RECURSE RECOMPILER CONFIGURE_DEPENDS "${GOW3}/shadps4/shader_recompiler/*.cpp")
list(FILTER VIDEO_CORE EXCLUDE REGEX "renderdoc\\.cpp$")
# Checked source adaptations, written to the build tree; the fixed copy stays as is.
function(gow3_adapt list_var relative)
    set(source "${GOW3}/${relative}")
    file(READ "${source}" text)
    set(pairs ${ARGN})
    list(LENGTH pairs count)
    math(EXPR last "${count} - 1")
    foreach(i RANGE 0 ${last} 2)
        math(EXPR j "${i} + 1")
        list(GET pairs ${i} before)
        list(GET pairs ${j} after)
        string(FIND "${text}" "${before}" position)
        if(position EQUAL -1)
            message(FATAL_ERROR "GoW3 core changed; review adaptation of ${relative}: ${before}")
        endif()
        string(REPLACE "${before}" "${after}" text "${text}")
    endforeach()
    set(out "${CMAKE_CURRENT_BINARY_DIR}/adapted/${relative}")
    file(CONFIGURE OUTPUT "${out}" CONTENT "${text}" @ONLY)
    set(files ${${list_var}})
    list(REMOVE_ITEM files "${source}")
    list(APPEND files "${out}")
    set(${list_var} ${files} PARENT_SCOPE)
endfunction()
# Larger adaptations kept as patch files (gpu-gow3/patches): blocks of
#   <<<<<<< (line)  text to find  ======= (line)  replacement  >>>>>>> (line)
# applied in order to the adapted copy (or the fixed one), each block found exactly once.
# Patch text may hold semicolons, unlike gow3_adapt's arguments. Call after gow3_adapt.
function(gow3_patch list_var relative patch)
    set(source "${GOW3}/${relative}")
    set(out "${CMAKE_CURRENT_BINARY_DIR}/adapted/${relative}")
    set(files ${${list_var}})
    list(FIND files "${out}" adapted)
    if(adapted EQUAL -1)
        file(READ "${source}" text)
    else()
        file(READ "${out}" text)
    endif()
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/patches/${patch}" blocks)
    string(REPLACE "

" "
" text "${text}")
    string(REPLACE "

" "
" blocks "${blocks}")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/patches/${patch}")
    while(TRUE)
        string(FIND "${blocks}" "<<<<<<<
" start)
        if(start EQUAL -1)
            break()
        endif()
        string(FIND "${blocks}" "
=======
" middle)
        string(FIND "${blocks}" "
>>>>>>>" end)
        if(middle LESS start OR end LESS middle)
            message(FATAL_ERROR "Malformed block in ${patch}")
        endif()
        math(EXPR before_at "${start} + 8")
        math(EXPR before_length "${middle} - ${before_at}")
        math(EXPR after_at "${middle} + 9")
        math(EXPR after_length "${end} - ${after_at}")
        string(SUBSTRING "${blocks}" ${before_at} ${before_length} before)
        string(SUBSTRING "${blocks}" ${after_at} ${after_length} after)
        math(EXPR rest "${end} + 8")
        string(SUBSTRING "${blocks}" ${rest} -1 blocks)
        string(FIND "${text}" "${before}" first)
        string(FIND "${text}" "${before}" last REVERSE)
        if(first EQUAL -1 OR NOT first EQUAL last)
            message(FATAL_ERROR "GoW3 core changed; ${patch} block not found once in ${relative}:
${before}")
        endif()
        string(SUBSTRING "${text}" 0 ${first} head)
        string(LENGTH "${before}" before_size)
        math(EXPR tail_at "${first} + ${before_size}")
        string(SUBSTRING "${text}" ${tail_at} -1 tail)
        set(text "${head}${after}${tail}")
    endwhile()
    file(CONFIGURE OUTPUT "${out}" CONTENT "${text}" @ONLY)
    list(REMOVE_ITEM files "${source}" "${out}")
    list(APPEND files "${out}")
    set(${list_var} ${files} PARENT_SCOPE)
endfunction()
# Header copies with Kyoei changes, found before the fixed ones (adapted/shadps4 comes first
# in the include path). The image flag marks Kyoei button prompts (see texture_cache.cpp).
set(GOW3_ADAPTED_HEADERS "")
gow3_adapt(GOW3_ADAPTED_HEADERS shadps4/video_core/texture_cache/image.h
    "    Aliased = 1 << 8,     ///< gow3: another image has the same guest address (alias_states)"
    "    Aliased = 1 << 8,     ///< gow3: another image has the same guest address (alias_states)
    KyoeiPromptImage = 1 << 30, ///< Kyoei: button prompt artwork, selected on every binding")
gow3_adapt(GOW3_ADAPTED_HEADERS shadps4/video_core/texture_cache/texture_cache.h
    "constexpr u32 Aliased = static_cast<u32>(ImageFlagBits::Aliased);"
    "constexpr u32 Aliased = static_cast<u32>(ImageFlagBits::Aliased | ImageFlagBits::KyoeiPromptImage);"
    "    /// Runs the garbage collector."
    "#include \"kyoei_gow3_image_lookup.inc\"
    /// Runs the garbage collector.")
# MSVC's std::array iterator is a class, not a pointer.
# KYOEI_UPLOAD_REPORT=1 (kyoei_upload_report.h): the images re-uploaded most, every 5 s.
gow3_adapt(VIDEO_CORE shadps4/video_core/texture_cache/texture_cache.cpp
    "    ForEachImageInRegion(address, size, [&](ImageId image_id, Image& image) {
        if (image.info.guest_address != address) {"
    "    KyoeiForEachImageWithAddress(address, [&](ImageId image_id, Image& image) {
        if (image.info.guest_address != address) {"
    "auto* slot = std::ranges::find(readback_images" "auto slot = std::ranges::find(readback_images"
    "#include \"video_core/texture_cache/texture_cache.h\"" "#include \"video_core/texture_cache/texture_cache.h\"
#include \"kyoei_upload_report.h\""
    "    Gow3Stats::image_upload_bytes.fetch_add(image.info.guest_size, std::memory_order_relaxed)"
    "    KyoeiUploadReport::Note(image), Gow3Stats::image_upload_bytes.fetch_add(image.info.guest_size, std::memory_order_relaxed)"
    "void TextureCache::UntrackImage(ImageId image_id) {"
    "void TextureCache::UntrackImage(ImageId image_id) {
    if (KyoeiUploadReport::NoteUntrack(slot_images[image_id], __builtin_return_address(0), 0)) {}"
    "void TextureCache::UntrackImageHead(ImageId image_id) {"
    "void TextureCache::UntrackImageHead(ImageId image_id) {
    if (KyoeiUploadReport::NoteUntrack(slot_images[image_id], __builtin_return_address(0), 1)) {}"
    "void TextureCache::UntrackImageTail(ImageId image_id) {"
    "void TextureCache::UntrackImageTail(ImageId image_id) {
    if (KyoeiUploadReport::NoteUntrack(slot_images[image_id], __builtin_return_address(0), 2)) {}"
    # Kyoei button prompts: gpu-gow3/kyoei_gow3_prompts.h (CMake lists split on semicolons,
    # so the code lives there).
    "#include \"video_core/texture_cache/texture_cache.h\""
    "#include \"video_core/texture_cache/texture_cache.h\"
#include \"kyoei_gow3_prompts.h\""
    "void TextureCache::RefreshImage(Image& image) {
    if (False(image.flags & ImageFlagBits::Dirty) || image.info.guest_samples > 1) {"
    "void TextureCache::RefreshImage(Image& image) {
    KYOEI_PROMPT_SELECT(image)
    if (False(image.flags & ImageFlagBits::Dirty) || image.info.guest_samples > 1) {"
    "if (is_gpu_modified && !is_gpu_dirty) {"
    "if (is_gpu_modified && !is_gpu_dirty && !prompt) {"
    "    const auto [in_buffer, in_offset] =
        buffer_cache.ObtainBufferForImage(image.info.guest_address, image.info.guest_size)"
    "    KYOEI_UPLOAD_OVERLAPS(image)
    KYOEI_PROMPT_SOURCE(image)"
    "tile_manager.DetileImage(in_buffer, in_offset, image.info)"
    "prompt ? std::pair<const Buffer*, u64>{in_buffer, in_offset}
                                         : tile_manager.DetileImage(in_buffer, in_offset, image.info)"
    "ImageView& TextureCache::FindRenderTarget(ImageId image_id, const ImageDesc& desc) {
    Image& image = slot_images[image_id]"
    "ImageView& TextureCache::FindRenderTarget(ImageId image_id, const ImageDesc& desc) {
    Image& image = (KyoeiUploadReport::NoteTarget(slot_images[image_id]),
        KyoeiUploadReport::WatchImage(\"target\", slot_images[image_id]), slot_images[image_id])"
    "                                     bool refresh) {
    Image& image = slot_images[image_id]"
    "                                     bool refresh) {
    Image& image = (KyoeiUploadReport::WatchImage(desc.type == BindingType::Storage ? \"storage\" : \"texture\",
        slot_images[image_id]), slot_images[image_id])"
    "void TextureCache::UnregisterImage(ImageId image_id) {
    Image& image = slot_images[image_id]"
    "void TextureCache::UnregisterImage(ImageId image_id) {
    Image& image = (KyoeiPrompts::states.erase(slot_images[image_id].image_uid), slot_images[image_id])")
set(COMMON decoder error io_file native_clock number_utils path_util rdtsc spin_lock
    recursive_lock string_util thread signal_context)
list(TRANSFORM COMMON PREPEND "${GOW3}/shadps4/common/")
list(TRANSFORM COMMON APPEND ".cpp")
file(GLOB AVPLAYER CONFIGURE_DEPENDS "${GOW3}/shadps4/core/libraries/avplayer/*.cpp")
# GoW3 game features (orbs, actors, cheats), its menu and its runtime glue stay out;
# gow3gpu.cpp is replaced by Kyoei's glue to the Godzilla runtime.
file(GLOB SHIM CONFIGURE_DEPENDS "${GOW3}/shim/*.cpp" "${GOW3}/shim/*/*.cpp")
list(FILTER SHIM EXCLUDE REGEX "gow3_(orbs|actors|overlay)\.cpp$|gow3gpu\.cpp$|shim/window\.cpp$|shim/common/(assert|logging/log)\.cpp$")
set(DRIVERS "${GOW3}/shadps4/core/libraries/gnmdriver/gnmdriver.cpp"
    "${GOW3}/shadps4/core/libraries/videoout/driver.cpp" "${GOW3}/shadps4/core/libraries/videoout/video_out.cpp"
    "${GOW3}/shadps4/core/libraries/kernel/equeue.cpp" "${GOW3}/shadps4/core/libraries/videodec/video_utils.cpp")
# Kyoei's overlay asks for frames while a dialog is open and the game stops flipping
# (the old core's ImGui::Core::MustKeepDrawing adaptation).
gow3_adapt(DRIVERS shadps4/core/libraries/videoout/driver.cpp
    "#include \"common/assert.h\"" "#include \"kyoei_overlay.h\"
#include \"kyoei_gow3_flip_rate.h\"
#include \"common/assert.h\""
    "} else if (ImGui::Core::MustKeepDrawing()) {"
    "} else if (ImGui::Core::MustKeepDrawing() || kyoei_overlay_needs_redraw()) {"
    "    RunPresenter([this, frame = req.frame, hdr = req.port->is_hdr] {"
    "    KyoeiGow3FlipRate::Flip(), RunPresenter([this, frame = req.frame, hdr = req.port->is_hdr] {")
gow3_adapt(VIDEO_CORE shadps4/video_core/buffer_cache/buffer_cache.cpp
    "#include \"video_core/buffer_cache/buffer_cache.h\""
    "#include \"video_core/buffer_cache/buffer_cache.h\"
#include \"kyoei_gow3_experiments.h\""
    "if (is_texel_buffer && !is_written) {"
    "if ((is_texel_buffer || KyoeiGow3Experiments::SyncAllReads()) && !is_written) {")
# Reach compat/common/ntapi.h instead of the sibling header.
gow3_adapt(COMMON shadps4/common/thread.cpp "#include \"ntapi.h\"" "#include \"common/ntapi.h\"")
# Godzilla's host pieces: window, pad, logging into the runtime log, paths.
set(GZ "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/godzilla-core")
# Benchmarks: while the file named by KYOEI_BENCH_TURN_FILE exists, the right stick is held
# right. Turns the camera from inside the game, whatever window has focus (SDL clears held
# keys when the window loses focus). Unset: no effect.
# kyoei.exe's adapted pad/window (keyboard map, mouse camera, window size) when the parent
# build provides them (KYOEI_GOW3_PAD/KYOEI_GOW3_WINDOW); Godzilla's otherwise.
if(NOT KYOEI_GOW3_PAD)
    set(KYOEI_GOW3_PAD "${GZ}/gpu/pad.cpp")
endif()
if(NOT KYOEI_GOW3_WINDOW)
    set(KYOEI_GOW3_WINDOW "${GZ}/gpu/window.cpp")
endif()
file(READ "${KYOEI_GOW3_PAD}" PAD_SOURCE)
if(KYOEI_GOW3_PAD STREQUAL "${GZ}/gpu/pad.cpp")
    set(PAD_ANCHOR "                       keyboard_axis(SDL_SCANCODE_I,SDL_SCANCODE_K,data.rightStick.y)};")
else()
    set(PAD_ANCHOR "    // Kyoei: no keyboard camera; the mouse and the gamepad turn it.")
endif()
string(FIND "${PAD_SOURCE}" "${PAD_ANCHOR}" PAD_POSITION)
if(PAD_POSITION EQUAL -1)
    message(FATAL_ERROR "Godzilla pad changed; review the benchmark turn adaptation")
endif()
string(REPLACE "${PAD_ANCHOR}" "${PAD_ANCHOR}
    {
        static const char* turn_file = std::getenv(\"KYOEI_BENCH_TURN_FILE\");
        if (turn_file && GetFileAttributesA(turn_file) != INVALID_FILE_ATTRIBUTES) data.rightStick.x = 255;
    }" PAD_SOURCE "${PAD_SOURCE}")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/adapted/pad.cpp" CONTENT "#include <windows.h>
${PAD_SOURCE}" @ONLY)
set(HOST "${KYOEI_GOW3_WINDOW}" "${CMAKE_CURRENT_BINARY_DIR}/adapted/pad.cpp" "${GZ}/gpu/logging.cpp" "${GZ}/gpu/paths.cpp"
    "${GZ}/gpu/windows_api.cpp")
# Audio and pad libraries the GoW3 core does not carry, from Godzilla's shadPS4. Its
# headers come last in the include order, after the GoW3 ones.
set(SHAD "${GZ}/third_party/shadPS4")
file(GLOB AJM CONFIGURE_DEPENDS "${SHAD}/src/core/libraries/ajm/*.cpp")
list(REMOVE_ITEM AJM "${SHAD}/src/core/libraries/ajm/ajm_aac.cpp")
file(READ "${SHAD}/src/core/libraries/ajm/ajm_aac.cpp" AJM_AAC_SOURCE)
string(REPLACE "externals/aacdec/fdk-aac/" "${DEPS}/gz_aac-src/" AJM_AAC_SOURCE "${AJM_AAC_SOURCE}")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/ajm_aac.cpp" CONTENT "${AJM_AAC_SOURCE}" @ONLY)
list(APPEND AJM "${CMAKE_CURRENT_BINARY_DIR}/ajm_aac.cpp")
file(READ "${SHAD}/src/core/libraries/audio/audioout.cpp" AUDIO_SOURCE)
string(REPLACE "if (EmulatorSettings.GetAudioBackend() == AudioBackend::OpenAL) {
        audio = std::make_unique<OpenALAudioOut>();
    } else {
        audio = std::make_unique<SDLAudioOut>();
    }" "audio = std::make_unique<SDLAudioOut>();" AUDIO_SOURCE "${AUDIO_SOURCE}")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/audioout.cpp" CONTENT "${AUDIO_SOURCE}" @ONLY)
set(AUDIO "${CMAKE_CURRENT_BINARY_DIR}/audioout.cpp" "${SHAD}/src/core/libraries/audio/sdl_audio_out.cpp")
set_source_files_properties("${SHAD}/src/core/libraries/audio/sdl_audio_out.cpp" PROPERTIES
    COMPILE_OPTIONS "-msse4.1;-include;core/libraries/kernel/time.h")
gow3_adapt(VIDEO_CORE shadps4/video_core/renderer_vulkan/vk_rasterizer.cpp
    "#include \"video_core/renderer_vulkan/vk_rasterizer.h\"" "#include \"video_core/renderer_vulkan/vk_rasterizer.h\"\n#include \"kyoei_upload_report.h\"\n#include \"kyoei_gow3_experiments.h\""
    "                         mrt1_desc.view_info.range)"
    "                         mrt1_desc.view_info.range), KyoeiGow3Experiments::ResolveWritten(mrt1_image)"
    "                    texture_cache.InvalidateMemoryFromGPU(vsharp.base_address, size)"
    "                    KyoeiUploadReport::NoteWrite(stage.pgm_hash, vsharp.base_address, size), texture_cache.InvalidateMemoryFromGPU(vsharp.base_address, size)"
    "    texture_cache.InvalidateMemoryFromGPU(dst, num_bytes)"
    "    KyoeiUploadReport::NoteWrite(0, dst, num_bytes), texture_cache.InvalidateMemoryFromGPU(dst, num_bytes)"
    "void Rasterizer::CopyBuffer(VAddr dst, VAddr src, u32 num_bytes, bool dst_gds, bool src_gds) {
    DrainDrawPipe()"
    "void Rasterizer::CopyBuffer(VAddr dst, VAddr src, u32 num_bytes, bool dst_gds, bool src_gds) {
    DrainDrawPipe(), KyoeiUploadReport::CopyEnabled() && KyoeiUploadReport::NoteCopy(dst, src, num_bytes,
        dst_gds || src_gds, !src_gds && buffer_cache.IsRegionGpuModified(src, num_bytes),
        !src_gds && bool(texture_cache.FindImageFromRange(src, num_bytes)))"
    "bool Rasterizer::ReadMemory(VAddr addr, u64 size, bool assume_locks) {
    DrainDrawPipe()"
    "bool Rasterizer::ReadMemory(VAddr addr, u64 size, bool assume_locks) {
    DrainDrawPipe(), KyoeiUploadReport::NoteRead(addr, size)")
# Kyoei photo mode: capture of the guest display buffer, the photo grade under the overlay, and the
# overlay/grade pipelines released with the presenter.
gow3_adapt(VIDEO_CORE shadps4/video_core/renderer_vulkan/vk_presenter.cpp
    "#include \"video_core/texture_cache/image.h\""
    "#include \"video_core/texture_cache/image.h\"
#include \"kyoei_overlay_gpu.h\"
#include \"kyoei_photo_post.h\"
#include \"kyoei_gow3_photo.h\""
    "        if (frame_dump) {
            runtime.Transit(&image,"
    "        if (KyoeiGow3Photo::Capture(instance, draw_scheduler, cmdbuf, runtime, image, view_info.format, ScreenshotToRgba8),
            frame_dump) {
            runtime.Transit(&image,"
    "            Gow3Overlay::Render(cmdbuf, swapchain.ImageView(), extent)"
    "            KyoeiPhoto::RenderPost(instance, cmdbuf, frame, frame->image_view, frame->width,
                                   frame->height, swapchain.ImageView(), extent,
                                   swapchain.GetSurfaceFormat().format),
                Gow3Overlay::Render(cmdbuf, swapchain.ImageView(), extent)"
    "    Check(draw_scheduler.CommandBuffer().reset())"
    "    KyoeiPhoto::ShutdownPost(), KyoeiOverlay::Shutdown(), Check(draw_scheduler.CommandBuffer().reset())")
foreach(stage vert frag)
    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/photo.${stage}.inc"
        COMMAND "${KYOEI_GLSLC}" -mfmt=c "${CMAKE_CURRENT_SOURCE_DIR}/../src/shaders/photo.${stage}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/photo.${stage}.inc"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../src/shaders/photo.${stage}" VERBATIM)
endforeach()
# FixedRenderSession() ran getenv on every draw (TemporalUpscaler::OnFrameStart): ~10% of
# the draw recording thread on Windows, where getenv scans the environment. Read it once.
gow3_adapt(SHIM shim/gow3_settings.cpp
    "    const char* size = std::getenv(\"GOW3_RENDER_RES\")"
    "    static const char* const size = std::getenv(\"GOW3_RENDER_RES\")")
# SamplerSetMemo from the inFamous port: a memoized texture set reuses its resolved samplers.
gow3_patch(GOW3_ADAPTED_HEADERS shadps4/video_core/texture_cache/texture_cache.h sampler_set_memo_texture_cache_h.patch)
gow3_patch(GOW3_ADAPTED_HEADERS shadps4/video_core/renderer_vulkan/vk_rasterizer.h sampler_set_memo_rasterizer_h.patch)
gow3_patch(VIDEO_CORE shadps4/video_core/texture_cache/texture_cache.cpp sampler_set_memo_texture_cache_cpp.patch)
gow3_patch(VIDEO_CORE shadps4/video_core/renderer_vulkan/vk_rasterizer.cpp sampler_set_memo_rasterizer_cpp.patch)
add_library(gow3_core STATIC ${VIDEO_CORE} ${RECOMPILER} ${COMMON} ${AVPLAYER} ${SHIM} ${DRIVERS} ${HOST} ${AJM} ${AUDIO}
    kyoei_gow3_bridge.cpp kyoei_gow3_stubs.cpp kyoei_upload_report.cpp kyoei_thread_sampler.cpp
    ../src/kyoei_overlay_gpu.cpp ../src/kyoei_photo_post.cpp "${KYOEI_IMGUI_DIR}/backends/imgui_impl_vulkan.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/photo.vert.inc" "${CMAKE_CURRENT_BINARY_DIR}/photo.frag.inc")
set_source_files_properties("${KYOEI_IMGUI_DIR}/backends/imgui_impl_vulkan.cpp" PROPERTIES
    COMPILE_DEFINITIONS IMGUI_IMPL_VULKAN_NO_PROTOTYPES=1)
set_source_files_properties(../src/kyoei_overlay_gpu.cpp PROPERTIES
    COMPILE_DEFINITIONS "IMGUI_IMPL_VULKAN_NO_PROTOTYPES=1;KYOEI_GUI_FONT_DIR=\"assets/fonts\"")
# The core's own Sirit (newer SPIR-V ops) and FSR-Vulkan (MIT), as in the GoW3 build.
set(SIRIT_USE_SYSTEM_SPIRV_HEADERS OFF CACHE BOOL "" FORCE)
add_subdirectory("${GOW3}/third_party/sirit" sirit EXCLUDE_FROM_ALL)
set(FFX_VK_PORTABLE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FFX_VK_PORTABLE_BUILD_PROBE OFF CACHE BOOL "" FORCE)
add_subdirectory("${FSR_VULKAN}" fsr-vulkan EXCLUDE_FROM_ALL)
# MSVC-ABI enums are int: FFX error codes above INT_MAX narrow in switch cases (same values).
target_compile_options(ffx_vulkan_portable PRIVATE -Wno-c++11-narrowing -Wno-microsoft-enum-value)
target_link_libraries(gow3_core PUBLIC kyoei_imgui dbghelp winmm sirit ffx-vulkan::portable ffx-vulkan::fsr4-v07-vulkan)
add_dependencies(gow3_core gow3_host_shaders)
# compat/a/b makes the GoW3 shim's "../../src/cheat_hook.h" resolve to compat/src.
target_include_directories(gow3_core PRIVATE "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/adapted/shadps4" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/../src" "${KYOEI_IMGUI_DIR}/backends" "${GZ}/gpu" "${CMAKE_CURRENT_SOURCE_DIR}/compat" "${CMAKE_CURRENT_SOURCE_DIR}/compat/a/b"
    "${FSR_VULKAN}/include" "${DEPS}/gz_sirit-src/externals/SPIRV-Headers/include" "${GOW3}/shim" "${GOW3}/shadps4" "${SHADER_INCLUDE}"
    "${GOW3}/third_party/gcn/include" "${GOW3}/third_party/half/include" "${GOW3}/third_party/sirit/include"
    "$ENV{VULKAN_SDK}/Include" "${DEPS}/gz_fmt-src/include" "${DEPS}/gz_magic-src/include"
    "${DEPS}/gz_magic-src/include/magic_enum" "${DEPS}/gz_robin-src/include" "${DEPS}/gz_vma-src/include"
    "${DEPS}/gz_sdl-src/include" "${DEPS}/gz_sdl-build/include-revision" "${DEPS}/gz_boost-src"
    "${DEPS}/gz_xbyak-src" "${DEPS}/gz_xxhash-src" "${DEPS}/gz_zydis-src/include"
    "${DEPS}/gz_zydis-build" "${DEPS}/gz_zydis-src/dependencies/zycore/include" "${DEPS}/gz_zydis-build/zycore"
    "${DEPS}/gz_miniz-src" "${DEPS}/gz_miniz-build"
    "${GOW3}/shadps4/core/libraries" "${GOW3}/shadps4/core/libraries/gnmdriver" "${DEPS}/gz_ffmpeg-src/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/godzilla-core/prebuilt/externals/ffmpeg-94dde08/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/godzilla-core/src"
    "${SHAD}/src" "${SHAD}/src/core/libraries/ajm" "${DEPS}/gz_mp3-src"
    "${DEPS}/gz_atrac9-src/C/src" "${DEPS}/gz_aac-src/libAACdec/include" "${DEPS}/gz_aac-src/libAACenc/include" "${DEPS}/gz_aac-src/libArithCoding/include" "${DEPS}/gz_aac-src/libDRCdec/include" "${DEPS}/gz_aac-src/libFDK/include" "${DEPS}/gz_aac-src/libMpegTPDec/include" "${DEPS}/gz_aac-src/libMpegTPEnc/include" "${DEPS}/gz_aac-src/libPCMutils/include" "${DEPS}/gz_aac-src/libSACdec/include" "${DEPS}/gz_aac-src/libSACenc/include" "${DEPS}/gz_aac-src/libSBRdec/include" "${DEPS}/gz_aac-src/libSBRenc/include" "${DEPS}/gz_aac-src/libSYS/include")
file(GLOB BOOST_INCLUDES "${DEPS}/gz_boost-src/libs/*/include")
target_include_directories(gow3_core SYSTEM PRIVATE ${BOOST_INCLUDES})
target_compile_definitions(gow3_core PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX _CRT_SECURE_NO_WARNINGS
    BOOST_ASIO_STANDALONE NDEBUG VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
target_compile_options(gow3_core PRIVATE -O2 -D_ITERATOR_DEBUG_LEVEL=0 -Wno-deprecated-declarations
    -Wno-missing-field-initializers -include "${GOW3}/shim/gow3_win_compat.h")
# Link-time optimization of the renderer (the GoW3 build does the same: 1.8% less GPU
# thread time per draw in Bloodborne). KYOEI_GOW3_LTO=OFF builds without it.
option(KYOEI_GOW3_LTO "Link-time optimization of the GoW3 GPU core" ON)
if(KYOEI_GOW3_LTO)
  set_property(TARGET gow3_core sirit PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
endif()
