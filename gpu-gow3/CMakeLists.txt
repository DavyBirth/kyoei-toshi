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
# MSVC's std::array iterator is a class, not a pointer.
# KYOEI_UPLOAD_REPORT=1 (kyoei_upload_report.h): the images re-uploaded most, every 5 s.
gow3_adapt(VIDEO_CORE shadps4/video_core/texture_cache/texture_cache.cpp
    "auto* slot = std::ranges::find(readback_images" "auto slot = std::ranges::find(readback_images"
    "#include \"video_core/texture_cache/texture_cache.h\"" "#include \"video_core/texture_cache/texture_cache.h\"
#include \"kyoei_upload_report.h\""
    "    Gow3Stats::image_upload_bytes.fetch_add(image.info.guest_size, std::memory_order_relaxed)"
    "    KyoeiUploadReport::Note(image), Gow3Stats::image_upload_bytes.fetch_add(image.info.guest_size, std::memory_order_relaxed)")
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
# Reach compat/common/ntapi.h instead of the sibling header.
gow3_adapt(COMMON shadps4/common/thread.cpp "#include \"ntapi.h\"" "#include \"common/ntapi.h\"")
# Godzilla's host pieces: window, pad, logging into the runtime log, paths.
set(GZ "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/godzilla-core")
set(HOST "${GZ}/gpu/window.cpp" "${GZ}/gpu/pad.cpp" "${GZ}/gpu/logging.cpp" "${GZ}/gpu/paths.cpp"
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
    "#include \"video_core/renderer_vulkan/vk_rasterizer.h\"" "#include \"video_core/renderer_vulkan/vk_rasterizer.h\"\n#include \"kyoei_upload_report.h\""
    "                    texture_cache.InvalidateMemoryFromGPU(vsharp.base_address, size)"
    "                    KyoeiUploadReport::NoteWrite(stage.pgm_hash, vsharp.base_address, size), texture_cache.InvalidateMemoryFromGPU(vsharp.base_address, size)"
    "    texture_cache.InvalidateMemoryFromGPU(dst, num_bytes)"
    "    KyoeiUploadReport::NoteWrite(0, dst, num_bytes), texture_cache.InvalidateMemoryFromGPU(dst, num_bytes)")
# FixedRenderSession() ran getenv on every draw (TemporalUpscaler::OnFrameStart): ~10% of
# the draw recording thread on Windows, where getenv scans the environment. Read it once.
gow3_adapt(SHIM shim/gow3_settings.cpp
    "    const char* size = std::getenv(\"GOW3_RENDER_RES\")"
    "    static const char* const size = std::getenv(\"GOW3_RENDER_RES\")")
add_library(gow3_core STATIC ${VIDEO_CORE} ${RECOMPILER} ${COMMON} ${AVPLAYER} ${SHIM} ${DRIVERS} ${HOST} ${AJM} ${AUDIO}
    kyoei_gow3_bridge.cpp kyoei_gow3_stubs.cpp)
# The core's own Sirit (newer SPIR-V ops) and FSR-Vulkan (MIT), as in the GoW3 build.
set(SIRIT_USE_SYSTEM_SPIRV_HEADERS OFF CACHE BOOL "" FORCE)
add_subdirectory("${GOW3}/third_party/sirit" sirit EXCLUDE_FROM_ALL)
set(FFX_VK_PORTABLE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FFX_VK_PORTABLE_BUILD_PROBE OFF CACHE BOOL "" FORCE)
add_subdirectory("${FSR_VULKAN}" fsr-vulkan EXCLUDE_FROM_ALL)
# MSVC-ABI enums are int: FFX error codes above INT_MAX narrow in switch cases (same values).
target_compile_options(ffx_vulkan_portable PRIVATE -Wno-c++11-narrowing -Wno-microsoft-enum-value)
target_link_libraries(gow3_core PUBLIC sirit ffx-vulkan::portable ffx-vulkan::fsr4-v07-vulkan)
add_dependencies(gow3_core gow3_host_shaders)
# compat/a/b makes the GoW3 shim's "../../src/cheat_hook.h" resolve to compat/src.
target_include_directories(gow3_core PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/compat" "${CMAKE_CURRENT_SOURCE_DIR}/compat/a/b"
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
