cmake_minimum_required(VERSION 3.28)
project(kyoei_native C CXX)
set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type")
if(NOT CMAKE_C_COMPILER_ID STREQUAL "Clang")
  message(FATAL_ERROR "Use Clang: the guest uses sysv_abi")
endif()
set(CMAKE_C_STANDARD 11)
set(KYOEI_CORE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/godzilla-core")
include("${KYOEI_CORE}/prebuilt/libraries.cmake")
set(CORE_FILES rt_core rt_sync rt_thread rt_memory rt_vmem rt_sema
  rt_file rt_savedata rt_kernel rt_rtc)
list(TRANSFORM CORE_FILES PREPEND "${KYOEI_CORE}/src/")
list(TRANSFORM CORE_FILES APPEND ".c")
# Kyoei uses the PS4 NONE message type while checking installed content.
# Adapt build copies only; keep the fixed Godzilla snapshot unchanged.
file(READ "${KYOEI_CORE}/src/rt_services.c" KYOEI_SERVICES)
file(READ "${KYOEI_CORE}/src/dialog_ui.c" KYOEI_DIALOG_UI)
function(kyoei_replace_checked variable before after)
  string(FIND "${${variable}}" "${before}" position)
  if(position EQUAL -1)
    message(FATAL_ERROR "Review message dialog adapter: missing anchor ${before}")
  endif()
  string(REPLACE "${before}" "${after}" adapted "${${variable}}")
  set(${variable} "${adapted}" PARENT_SCOPE)
endfunction()
kyoei_replace_checked(KYOEI_DIALOG_UI
  "int gz_dialog_input(uint32_t buttons) {"
  "int kyoei_ime_input(uint32_t buttons);\nint gz_dialog_input(uint32_t buttons) {\n    if (kyoei_ime_input(buttons)) { release_gate = 1; return 1; }")
kyoei_replace_checked(KYOEI_SERVICES
  "case 0: case 1: case 3: case 7: case 8: break;"
  "case 0: case 1: case 2: case 3: case 7: case 8: break;")
kyoei_replace_checked(KYOEI_SERVICES
  "msg_result_code = msg_button ? 0 : 1;"
  "msg_result_code = work->type == 2 ? 0 : msg_button ? 0 : 1;")
kyoei_replace_checked(KYOEI_SERVICES
  "{NULL, \"sceMsgDialogUpdateStatus\", (void *)msg_status}"
  "{NULL, \"sceMsgDialogGetStatus\", (void *)msg_status}, {NULL, \"sceMsgDialogUpdateStatus\", (void *)msg_status}")
kyoei_replace_checked(KYOEI_DIALOG_UI
  "type == 256 ? 0 : type == 0 ? 1 : 2"
  "type == 256 || type == 2 ? 0 : type == 0 ? 1 : 2")
kyoei_replace_checked(KYOEI_DIALOG_UI
  "d->type == 256 ? L\"Aguarde...\""
  "d->type == 256 || d->type == 2 ? L\"Aguarde...\"")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_services.c" CONTENT "${KYOEI_SERVICES}" @ONLY)
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_dialog_ui.c" CONTENT "${KYOEI_DIALOG_UI}" @ONLY)
list(APPEND CORE_FILES "${CMAKE_CURRENT_BINARY_DIR}/kyoei_services.c" "${CMAKE_CURRENT_BINARY_DIR}/kyoei_dialog_ui.c")
file(READ "${KYOEI_CORE}/src/loader.c" KYOEI_LOADER)
set(TLS_REJECTION "    if (main_tls[2]) fail(\"this eboot has static TLS; the Windows loader does not support it yet\");")
string(FIND "${KYOEI_LOADER}" "${TLS_REJECTION}" TLS_ANCHOR)
if(TLS_ANCHOR EQUAL -1)
  message(FATAL_ERROR "Review TLS adapter: shared loader changed")
endif()
string(REPLACE "${TLS_REJECTION}" "    /* Main TLS is validated and initialized by the Kyoei adapter below. */" KYOEI_LOADER "${KYOEI_LOADER}")
string(REPLACE "#include \"gz.h\"" "#include \"gz.h\"\n#include \"kyoei_tls.h\"" KYOEI_LOADER "${KYOEI_LOADER}")
string(REPLACE "#include \"kyoei_tls.h\"" "#include \"kyoei_tls.h\"\n#include \"kyoei_modules.h\"" KYOEI_LOADER "${KYOEI_LOADER}")
string(REPLACE "    DWORD code = e->ExceptionRecord->ExceptionCode;" "    if (kyoei_tls_fault(e)) return EXCEPTION_CONTINUE_EXECUTION;\n    DWORD code = e->ExceptionRecord->ExceptionCode;" KYOEI_LOADER "${KYOEI_LOADER}")
string(REPLACE "    fclose(f);\n    snprintf(path, sizeof(path), \"%s\\\\imports.tsv\", out);" "    fclose(f);\n    kyoei_tls_configure(image, image_size, main_tls);\n    snprintf(path, sizeof(path), \"%s\\\\imports.tsv\", out);" KYOEI_LOADER "${KYOEI_LOADER}")
string(REPLACE "    kyoei_tls_configure(image, image_size, main_tls);" "    kyoei_tls_configure(image, image_size, main_tls);\n    kyoei_modules_configure(out, image, image_size);" KYOEI_LOADER "${KYOEI_LOADER}")
string(REPLACE "        gz_log(\"Starting %s (init at image+0x%\"" "        if (kyoei_module_deferred(modules[m].base)) continue;\n        gz_log(\"Starting %s (init at image+0x%\"" KYOEI_LOADER "${KYOEI_LOADER}")
foreach(REQUIRED_ADAPTER "kyoei_tls_configure(image, image_size, main_tls);" "if (kyoei_tls_fault(e))")
  string(FIND "${KYOEI_LOADER}" "${REQUIRED_ADAPTER}" ADAPTER_POSITION)
  if(ADAPTER_POSITION EQUAL -1)
    message(FATAL_ERROR "Review TLS adapter: missing integration ${REQUIRED_ADAPTER}")
  endif()
endforeach()
kyoei_replace_checked(KYOEI_LOADER
  "#include \"kyoei_tls.h\""
  "#include \"kyoei_tls.h\"\n#include \"kyoei_crash.h\"")
kyoei_replace_checked(KYOEI_LOADER
  "    gz_stop(GZ_EXIT_FAULT, \"STOP: fault\");"
  "    kyoei_capture_failure(\"fault\", e);\n    gz_stop(GZ_EXIT_FAULT, \"STOP: fault\");")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_loader.c" CONTENT "${KYOEI_LOADER}" @ONLY)
add_executable(kyoei ${CORE_FILES} "${CMAKE_CURRENT_BINARY_DIR}/kyoei_loader.c" src/kyoei_kernel.c src/kyoei_tls.c src/kyoei_modules.c src/kyoei_ime.c)
target_sources(kyoei PRIVATE src/kyoei_crash.c)
target_compile_definitions(kyoei PRIVATE KYOEI_CRASH_CAPTURE=1)
set_source_files_properties("${KYOEI_CORE}/src/rt_core.c" PROPERTIES
  COMPILE_DEFINITIONS "gz_resolve=kyoei_shared_resolve")
set_source_files_properties("${KYOEI_CORE}/src/rt_kernel.c" PROPERTIES
  COMPILE_DEFINITIONS "gz_kernel_resolve=kyoei_shared_kernel_resolve")
target_include_directories(kyoei PRIVATE "${KYOEI_CORE}/src" src)
target_compile_definitions(kyoei PRIVATE GZ_GPU=1 WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS)
target_compile_options(kyoei PRIVATE -Wall -Wextra)
set_target_properties(kyoei PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(kyoei PRIVATE ${KYOEI_GPU_LIBRARIES} ${KYOEI_SYSTEM_LIBRARIES})
target_link_libraries(kyoei PRIVATE dbghelp)
# Compile adapted GPU units from the fixed snapshot locally. The
# external cache supplies dependency headers only; no sibling sources are built.
set(KYOEI_GPU_HEADERS "G:/godzilla-port-codex/build/_deps" CACHE PATH "Existing GPU dependency header cache")
set(KYOEI_SHAD "${KYOEI_CORE}/third_party/shadPS4")
file(READ "${KYOEI_SHAD}/src/core/libraries/videoout/video_out.cpp" KYOEI_VIDEOOUT)
kyoei_replace_checked(KYOEI_VIDEOOUT
  "    Platform::IrqC::Instance()->RegisterOnce("
  "    const auto kyoei_request = KyoeiFlipTrace::Record(false, 0, buf_id, port->buffer_labels[buf_id]);\n    Platform::IrqC::Instance()->RegisterOnce(")
kyoei_replace_checked(KYOEI_VIDEOOUT
  "            ASSERT_MSG(irq == Platform::InterruptId::GfxFlip,"
  "            KyoeiFlipTrace::Record(true, kyoei_request, buf_id, port->buffer_labels[buf_id]);\n            ASSERT_MSG(irq == Platform::InterruptId::GfxFlip,")
kyoei_replace_checked(KYOEI_VIDEOOUT
  "#include \"common/assert.h\""
  "extern \"C\" {\n#include \"gz.h\"\n}\n#include \"kyoei_flip_trace.h\"\n#include \"common/assert.h\"")
kyoei_replace_checked(KYOEI_VIDEOOUT
  "    driver = std::make_unique<VideoOutDriver>"
  "    if (presenter) driver = std::make_unique<VideoOutDriver>")
kyoei_replace_checked(KYOEI_VIDEOOUT
  "            ASSERT_MSG(port->buffer_labels[buf_id] == 1, \"Out of order flip IRQ\");"
  "            if (port->buffer_labels[buf_id] != 1) {\n                KyoeiFlipTrace::Dump();\n                gz_log(\"Kyoei flip failure: buffer=%u label=0x%llx previous=%d count=%llu pending=%d gc=%d arg=%lld\", buf_id, (unsigned long long)port->buffer_labels[buf_id], port->prev_index, (unsigned long long)port->flip_status.count, port->flip_status.flip_pending_num, port->flip_status.gc_queue_num, (long long)flip_arg);\n            }\n            ASSERT_MSG(port->buffer_labels[buf_id] == 1, \"Out of order flip IRQ\");")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_videoout.cpp" CONTENT "${KYOEI_VIDEOOUT}" @ONLY)
add_library(kyoei_videoout OBJECT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_videoout.cpp")
# Release the old surface before publishing completion. Otherwise a guest
# woken by the event can lock it for a new flip before the late reset to zero.
file(READ "${KYOEI_SHAD}/src/core/libraries/videoout/driver.cpp" KYOEI_VIDEOOUT_DRIVER)
set(KYOEI_RELEASE_OLD [=[    // Reset prev flip label
    if (port->prev_index != -1) {
        port->buffer_labels[port->prev_index] = 0;
        port->SignalVoLabel();
    }
    // save to prev buf index
    port->prev_index = req.index;
]=])
kyoei_replace_checked(KYOEI_VIDEOOUT_DRIVER "${KYOEI_RELEASE_OLD}" "")
kyoei_replace_checked(KYOEI_VIDEOOUT_DRIVER
  "        auto& flip_status = port->flip_status;"
  "        // Finish surface release before publishing completion to the guest.\n        if (port->prev_index != -1) {\n            port->buffer_labels[port->prev_index] = 0;\n            port->SignalVoLabel();\n        }\n        port->prev_index = req.index;\n        auto& flip_status = port->flip_status;")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_videoout_driver.cpp" CONTENT "${KYOEI_VIDEOOUT_DRIVER}" @ONLY)
target_sources(kyoei_videoout PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/kyoei_videoout_driver.cpp")
# The emulated command processor can reach the next flip's WRITE_DATA while
# the presentation thread still owns that surface. Respect its label before
# writing the new lock; never overwrite or fabricate the expected label.
file(READ "${KYOEI_SHAD}/src/video_core/amdgpu/liverpool.cpp" KYOEI_LIVERPOOL)
kyoei_replace_checked(KYOEI_LIVERPOOL
  "                u64* address = write_data->Address<u64*>();"
  "                u64* address = write_data->Address<u64*>();\n                if (vo_port && vo_port->IsVoLabel(address) && data_size == 4 && write_data->data[0] == 1) {\n                    while (*address != 0) {\n                        YIELD_GFX();\n                    }\n                }")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_liverpool.cpp" CONTENT "${KYOEI_LIVERPOOL}" @ONLY)
target_sources(kyoei_videoout PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/kyoei_liverpool.cpp")
target_sources(kyoei PRIVATE $<TARGET_OBJECTS:kyoei_videoout>)
target_compile_features(kyoei_videoout PRIVATE cxx_std_23)
target_compile_options(kyoei_videoout PRIVATE -O2 -DNDEBUG -U_DEBUG -D_ITERATOR_DEBUG_LEVEL=0)
target_compile_definitions(kyoei_videoout PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX _CRT_SECURE_NO_WARNINGS VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
target_include_directories(kyoei_videoout PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${KYOEI_CORE}/gpu/shim" "${KYOEI_SHAD}/src" "${KYOEI_SHAD}" "${KYOEI_CORE}/src"
  "${KYOEI_SHAD}/externals/gcn/include"
  "${KYOEI_GPU_HEADERS}/gz_vulkan-src/include" "${KYOEI_GPU_HEADERS}/gz_fmt-src/include"
  "${KYOEI_GPU_HEADERS}/gz_magic-src/include" "${KYOEI_GPU_HEADERS}/gz_robin-src/include"
  "${KYOEI_GPU_HEADERS}/gz_vma-src/include" "${KYOEI_GPU_HEADERS}/gz_sdl-src/include"
  "${KYOEI_GPU_HEADERS}/gz_boost-src"
  "${KYOEI_GPU_HEADERS}/gz_sdl-build/include-revision" "${KYOEI_GPU_HEADERS}/gz_xbyak-src"
  "${KYOEI_GPU_HEADERS}/gz_xxhash-src")
file(GLOB KYOEI_BOOST_INCLUDES "${KYOEI_GPU_HEADERS}/gz_boost-src/libs/*/include")
target_include_directories(kyoei_videoout SYSTEM PRIVATE ${KYOEI_BOOST_INCLUDES})
add_executable(test_kyoei_tls src/test_kyoei_tls.c src/kyoei_tls.c)
target_include_directories(test_kyoei_tls PRIVATE "${KYOEI_CORE}/src" src)
file(GLOB GPU_DLLS "${KYOEI_CORE}/prebuilt/*.dll")
foreach(DLL IN LISTS GPU_DLLS)
  add_custom_command(TARGET kyoei POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${DLL}" "$<TARGET_FILE_DIR:kyoei>")
endforeach()
