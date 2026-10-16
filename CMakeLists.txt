cmake_minimum_required(VERSION 3.28)
project(kyoei_native C CXX)
set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type")
if(NOT CMAKE_C_COMPILER_ID STREQUAL "Clang")
  message(FATAL_ERROR "Use Clang: the guest uses sysv_abi")
endif()
set(CMAKE_C_STANDARD 11)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  include(cmake/LinuxBootstrap.cmake)
  return()
endif()
set(KYOEI_CORE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/godzilla-core")
include("${KYOEI_CORE}/prebuilt/libraries.cmake")
set(CORE_FILES rt_core rt_sync rt_memory rt_sema
  rt_kernel rt_rtc)
list(TRANSFORM CORE_FILES PREPEND "${KYOEI_CORE}/src/")
list(TRANSFORM CORE_FILES APPEND ".c")
# Kyoei uses the PS4 NONE message type while checking installed content.
# Adapt build copies only; keep the fixed Godzilla snapshot unchanged.
file(READ "${KYOEI_CORE}/src/rt_services.c" KYOEI_SERVICES)
file(READ "${KYOEI_CORE}/src/dialog_ui.c" KYOEI_DIALOG_UI)
set(KYOEI_DIALOG_UI "#include \"kyoei_overlay.h\"\n${KYOEI_DIALOG_UI}")
function(kyoei_replace_checked variable before after)
  string(FIND "${${variable}}" "${before}" position)
  if(position EQUAL -1)
    message(FATAL_ERROR "Review message dialog adapter: missing anchor ${before}")
  endif()
  string(REPLACE "${before}" "${after}" adapted "${${variable}}")
  set(${variable} "${adapted}" PARENT_SCOPE)
endfunction()
kyoei_replace_checked(KYOEI_SERVICES
  "getenv(\"GZ_USER_NAME\") : \"Davy\""
  "getenv(\"GZ_USER_NAME\") : \"Player\"")
file(READ "${KYOEI_CORE}/src/rt_file.c" KYOEI_FILES)
kyoei_replace_checked(KYOEI_FILES
  "void gz_file_configure(const char *app0, const char *user) {"
  "#include \"kyoei_translation_mount.h\"\nvoid gz_file_configure(const char *app0, const char *user) {")
kyoei_replace_checked(KYOEI_FILES
  "    gz_file_mount(\"/hostapp\", app0);"
  "    gz_file_mount(\"/hostapp\", app0);\n    kyoei_translation_mount();")
kyoei_replace_checked(KYOEI_FILES
  "typedef struct { int used, append; HANDLE host; int64_t position; Listing *dir; char path[512]; } File;"
  "typedef struct { int used, append; HANDLE host; int64_t position; Listing *dir; char path[512]; const struct KyoeiDelta *delta; } File;")
kyoei_replace_checked(KYOEI_FILES
  "#include \"kyoei_translation_mount.h\"\n"
  "#include \"kyoei_translation_mount.h\"\n#include \"kyoei_archive_delta.h\"\n")
kyoei_replace_checked(KYOEI_FILES
  "    snprintf(files[fd].path, sizeof(files[fd].path), \"%s\", guest);"
  "    snprintf(files[fd].path, sizeof(files[fd].path), \"%s\", guest);\n    if (!dir) files[fd].delta = kyoei_delta_for(guest, host);")
kyoei_replace_checked(KYOEI_FILES
  "    int64_t n = read_at(f->host, buffer, size, f->position);"
  "    int64_t n = kyoei_delta_read(f->delta, f->host, buffer, size, f->position);")
kyoei_replace_checked(KYOEI_FILES
  "    return read_at(f->host, buffer, size, offset);"
  "    return kyoei_delta_read(f->delta, f->host, buffer, size, offset);")
kyoei_replace_checked(KYOEI_FILES
  "    int64_t base = whence == 0 ? 0 : whence == 1 ? f->position : file_size(f->host);"
  "    int64_t base = whence == 0 ? 0 : whence == 1 ? f->position : f->delta ? (int64_t)f->delta->virtual_size : file_size(f->host);")
kyoei_replace_checked(KYOEI_FILES
  "    out->ino = i.nFileIndexLow;"
  "    out->ino = i.nFileIndexLow;\n    if (f->delta) { out->size = (int64_t)f->delta->virtual_size; out->blocks = (out->size + 511) / 512; }")
kyoei_replace_checked(KYOEI_FILES
  "    convert_stat(a.dwFileAttributes, (uint64_t)a.nFileSizeHigh << 32 | a.nFileSizeLow, a.ftLastAccessTime, a.ftLastWriteTime, a.ftCreationTime, out);\n    return 0;"
  "    convert_stat(a.dwFileAttributes, (uint64_t)a.nFileSizeHigh << 32 | a.nFileSizeLow, a.ftLastAccessTime, a.ftLastWriteTime, a.ftCreationTime, out);\n    kyoei_delta_stat(guest, wide, out);\n    return 0;")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_files.c" CONTENT "${KYOEI_FILES}" @ONLY)
list(APPEND CORE_FILES "${CMAKE_CURRENT_BINARY_DIR}/kyoei_files.c")
file(READ "${KYOEI_CORE}/src/rt_vmem.c" KYOEI_VMEM)
kyoei_replace_checked(KYOEI_VMEM
  "    Region *r = &regions[placeholder(address, address + size)];"
  "    /* placeholder can realloc the table; read its base only afterwards. */\n    size_t index = placeholder(address, address + size);\n    Region *r = &regions[index];")
kyoei_replace_checked(KYOEI_VMEM
  "        vmem_fail(\"MapViewOfFile3\", r->base, r->size);"
  "        {\n            DWORD error=GetLastError(); MEMORY_BASIC_INFORMATION state={0};\n            VirtualQuery((void*)r->base,&state,sizeof(state));\n            gz_log(\"Kyoei map failure: requested=%p size=0x%llx phys=0x%llx OSbase=%p allocation=%p size=0x%llx state=0x%lx type=0x%lx regions=%zu\",(void*)r->base,(unsigned long long)r->size,(unsigned long long)r->phys,state.BaseAddress,state.AllocationBase,(unsigned long long)state.RegionSize,state.State,state.Type,region_count);\n            SetLastError(error); vmem_fail(\"MapViewOfFile3\",r->base,r->size);\n        }")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_vmem.c" CONTENT "${KYOEI_VMEM}" @ONLY)
list(APPEND CORE_FILES "${CMAKE_CURRENT_BINARY_DIR}/kyoei_vmem.c")
kyoei_replace_checked(KYOEI_DIALOG_UI
  "    if (pressed & (0x4000 | 8)) choose(window, d, d->selected + 1);\n    else if (pressed & 0x2000) choose(window, d, d->choices == 2 ? 2 : 1);"
  "    if (pressed & (0x2000 | 8)) choose(window, d, d->selected + 1);\n    else if (pressed & 0x4000) choose(window, d, d->choices == 2 ? 2 : 0);")
kyoei_replace_checked(KYOEI_DIALOG_UI
  "w == VK_RETURN || w == VK_SPACE ? 0x4000 : w == VK_ESCAPE ? 0x2000"
  "w == VK_RETURN || w == VK_SPACE ? 0x2000 : w == VK_ESCAPE ? 0x4000")
kyoei_replace_checked(KYOEI_DIALOG_UI
  "X / Enter: confirmar     Setas: escolher     Círculo / Esc: voltar"
  "Círculo / Enter: confirmar     Setas: escolher     X / Esc: voltar")
file(READ "${KYOEI_CORE}/src/rt_thread.c" KYOEI_THREADS)
kyoei_replace_checked(KYOEI_THREADS
  "static const GzExport exports[] = {"
  "#include \"kyoei_thread_finalize.h\"\nstatic const GzExport exports[] = {\n    {\"libkernel\", \"__pthread_cxa_finalize\", (void *)kyoei_pthread_finalize},")
kyoei_replace_checked(KYOEI_THREADS
  "    t->result = t->entry(t->argument);"
  "    extern void kyoei_tls_enter(void);\n    kyoei_tls_enter();\n    t->result = t->entry(t->argument);")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_threads.c" CONTENT "${KYOEI_THREADS}" @ONLY)
list(APPEND CORE_FILES "${CMAKE_CURRENT_BINARY_DIR}/kyoei_threads.c")
file(READ "${KYOEI_CORE}/src/rt_savedata.c" KYOEI_SAVEDATA)
kyoei_replace_checked(KYOEI_SAVEDATA
  "#define ERR_NOT_MOUNTED ((int32_t)0x809F0004)"
  "#define ERR_BUSY ((int32_t)0x809F0003)\n#define ERR_NOT_MOUNTED ((int32_t)0x809F0004)")
set(KYOEI_OLD_SAVE_TERMINATE [=[static ABI int32_t save_terminate(void) {
    if (!initialized) return ERR_NOT_INITIALIZED;
    initialized = 0;
    return 0;
}]=])
kyoei_replace_checked(KYOEI_SAVEDATA "${KYOEI_OLD_SAVE_TERMINATE}"
  "static ABI int32_t save_terminate(void) {\n    AcquireSRWLockExclusive(&lock);\n    if (!initialized) { ReleaseSRWLockExclusive(&lock); return ERR_NOT_INITIALIZED; }\n    for (unsigned i=0;i<SLOTS;++i) if (slots[i].used) { ReleaseSRWLockExclusive(&lock); return ERR_BUSY; }\n    initialized = 0;\n    ReleaseSRWLockExclusive(&lock);\n    return 0;\n}")
kyoei_replace_checked(KYOEI_SAVEDATA
  "    AcquireSRWLockExclusive(&lock);\n    int slot = -1;"
  "    AcquireSRWLockExclusive(&lock);\n    if (!initialized) { ReleaseSRWLockExclusive(&lock); return ERR_NOT_INITIALIZED; }\n    int slot = -1;")
kyoei_replace_checked(KYOEI_SAVEDATA
  "static struct { int used; char host[700], meta[700]; } slots[SLOTS];"
  "static struct { int used, read_only; char host[700], meta[700]; } slots[SLOTS];")
kyoei_replace_checked(KYOEI_SAVEDATA
  "    slots[slot].used = 1;"
  "    slots[slot].used = 1; slots[slot].read_only = (mode & MODE_RDONLY) != 0;")
set(KYOEI_OLD_ICON_WRITE [=[        char path[760];
        snprintf(path, sizeof(path), "%s\\icon0.png", slots[slot].meta);
        FILE *f = open_utf8(path, L"wb");
        r = f && fwrite(icon->buffer, 1, icon->data_size, f) == icon->data_size ? 0 : ERR_INTERNAL;
        if (f) fclose(f);]=])
kyoei_replace_checked(KYOEI_SAVEDATA "${KYOEI_OLD_ICON_WRITE}"
  "        if (slots[slot].read_only) r = ERR_BAD_MOUNTED;\n        else {\n            uint64_t bytes = icon->data_size < icon->buffer_size ? icon->data_size : icon->buffer_size;\n            r = bytes > SIZE_MAX || kyoei_write_save_metadata(slots[slot].meta, \"icon0.png\", icon->buffer, (size_t)bytes) ? ERR_INTERNAL : 0;\n        }")
set(KYOEI_OLD_WRITE_PARAM [=[static void write_param(const char *meta, const Param *p) {
    char path[760];
    snprintf(path, sizeof(path), "%s\\param.bin", meta);
    FILE *f = open_utf8(path, L"wb");
    if (f) { Param copy = *p; copy.mtime = (int64_t)time(NULL); fwrite(&copy, sizeof(copy), 1, f); fclose(f); }
}]=])
kyoei_replace_checked(KYOEI_SAVEDATA "${KYOEI_OLD_WRITE_PARAM}" "#include \"kyoei_save_metadata.h\"\n#include \"kyoei_save_slot.h\"")
kyoei_replace_checked(KYOEI_SAVEDATA
  "    if (!exists) { Param empty = {0}; write_param(meta, &empty); }"
  "    if (!exists) { Param empty = {0}; if (write_param(meta, &empty)) { kyoei_discard_empty_save(host,meta); ReleaseSRWLockExclusive(&lock); return ERR_INTERNAL; } }")
kyoei_replace_checked(KYOEI_SAVEDATA
  "        if (!r) write_param(slots[slot].meta, &p);"
  "        if (!r && write_param(slots[slot].meta, &p)) r = ERR_INTERNAL;")
kyoei_replace_checked(KYOEI_SAVEDATA
  "    int slot = atoi(point->data + 9);"
  "    int slot = kyoei_save_slot_number(point->data,sizeof(point->data));")
kyoei_replace_checked(KYOEI_SAVEDATA
  "static const GzExport exports[] = {"
  "/* No asynchronous backups are scheduled by this folder backend. */\nstatic ABI int32_t save_clear_progress(void) { return initialized ? 0 : ERR_NOT_INITIALIZED; }\nstatic ABI int32_t save_get_progress(float *progress) {\n    if (!initialized) return ERR_NOT_INITIALIZED;\n    if (!progress) return ERR_PARAMETER;\n    *progress = 0.0f; return 0;\n}\nstatic const GzExport exports[] = {\n    {NULL, \"sceSaveDataGetProgress\", (void *)save_get_progress},\n    {NULL, \"sceSaveDataClearProgress\", (void *)save_clear_progress},")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_savedata_backend.c" CONTENT "${KYOEI_SAVEDATA}" @ONLY)
list(APPEND CORE_FILES "${CMAKE_CURRENT_BINARY_DIR}/kyoei_savedata_backend.c")
kyoei_replace_checked(KYOEI_DIALOG_UI
  "#include \"dialog_ui.h\""
  "#include \"dialog_ui.h\"\n#include \"kyoei_dialog_list.h\"")
kyoei_replace_checked(KYOEI_DIALOG_UI
  "    if (pressed & (0x10 | 0x20 | 0x40 | 0x80))"
  "    if (d->type == 512) kyoei_list_move(window, pressed);\n    else if (pressed & (0x10 | 0x20 | 0x40 | 0x80))")
kyoei_replace_checked(KYOEI_DIALOG_UI
  "        if (d->type == 256) {"
  "        if (d->type == 512) kyoei_list_draw(dc, &r);\n        if (d->type == 256) {")
kyoei_replace_checked(KYOEI_SERVICES
  "static ABI int32_t save_open(const SaveParam *p) {"
  "#include \"kyoei_save_list.h\"\nstatic ABI int32_t save_open(const SaveParam *p) {")
kyoei_replace_checked(KYOEI_SERVICES
  "    if (p->mode != 2 && p->mode != 4 && p->mode != 5) {"
  "    if (p->mode == 1) {\n        KyoeiSaveList *list = kyoei_save_list_copy(p);\n        if (!list) return (int32_t)0x80B8000A;\n        if (save_worker) CloseHandle(save_worker);\n        save_mode = 1; save_user_data = p->user_data; save_button = 0; save_result_code = 1;\n        memset(kyoei_save_selected, 0, 32);\n        InterlockedExchange(&save_closed, 0);\n        gz_dialog_prepare(); InterlockedExchange(&save_running, 1); dialog_state[DLG_SAVE] = 2;\n        save_worker = CreateThread(NULL, 0, kyoei_save_list_show, list, 0, NULL);\n        if (!save_worker) { kyoei_save_list_free(list); InterlockedExchange(&save_running, 0); dialog_state[DLG_SAVE] = 1; return (int32_t)0x80B80009; }\n        return 0;\n    }\n    if (p->mode != 2 && p->mode != 4 && p->mode != 5) {")
kyoei_replace_checked(KYOEI_SERVICES
  "    r->user_data = save_user_data;"
  "    r->user_data = save_user_data;\n    if (save_mode == 1 && r->directory) memcpy(r->directory, kyoei_save_selected, 32);")
kyoei_replace_checked(KYOEI_SERVICES
  "    if (p->mode != 2 && p->mode != 4 && p->mode != 5) {"
  "    if (p->mode != 2 && p->mode != 3 && p->mode != 4 && p->mode != 5) {")
kyoei_replace_checked(KYOEI_SERVICES
  "    if (p->mode == 2) {"
  "    if (p->mode == 3) {\n        const uint32_t *system = p->system;\n        if (!system || p->display < 1 || p->display > 3 || (system[0] != 2 && system[0] != 3)) { free(work); return (int32_t)0x80B8000F; }\n        work->type = 1;\n        wcscpy(work->text, system[0] == 3 ? L\"Substituir este save?\" : p->display == 1 ? L\"Salvar neste slot?\" : p->display == 2 ? L\"Carregar este save?\" : L\"Excluir este save?\");\n    } else if (p->mode == 2) {")
kyoei_replace_checked(KYOEI_DIALOG_UI
  "int gz_dialog_input(uint32_t buttons) {"
  "int kyoei_ime_input(uint32_t buttons);\nint gz_dialog_input(uint32_t buttons) {\n    if (kyoei_overlay_input(buttons) || kyoei_ime_input(buttons)) { release_gate = 1; return 1; }")
kyoei_replace_checked(KYOEI_DIALOG_UI
  "void gz_dialog_cancel(void) {"
  "void gz_dialog_cancel(void) {\n    kyoei_overlay_cancel_dialog();")
kyoei_replace_checked(KYOEI_DIALOG_UI
  "void gz_dialog_progress(unsigned percent) {"
  "void gz_dialog_progress(unsigned percent) { kyoei_overlay_progress(percent);")
kyoei_replace_checked(KYOEI_DIALOG_UI
  "void gz_dialog_prepare(void) {"
  "void gz_dialog_prepare(void) { kyoei_overlay_prepare_dialog();")
kyoei_replace_checked(KYOEI_DIALOG_UI
  "int gz_dialog_show(const wchar_t *text, unsigned type) {"
  "int gz_dialog_show(const wchar_t *text, unsigned type) {\n    if (!InterlockedCompareExchange(&canceled,0,0)) {\n        int overlay=kyoei_overlay_message(text,type);\n        if (overlay!=INT_MIN) return overlay;\n    }")
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
kyoei_replace_checked(KYOEI_LOADER
  "    gz_log(\"Exit code %d\", code);"
  "    extern void kyoei_gpu_cache_shutdown(void);\n    if(code==0) kyoei_gpu_cache_shutdown();\n    gz_log(\"Exit code %d\", code);")
kyoei_replace_checked(KYOEI_LOADER
  "        gz_gpu_self_test(); gz_stop(0, \"GPU TEST: PASS\");"
  "        const char *cache_test=getenv(\"KYOEI_CACHE_SELF_TEST\");\n        if(cache_test && !strcmp(cache_test,\"1\")) { extern void kyoei_gpu_cache_self_test(void); kyoei_gpu_cache_self_test(); }\n        gz_gpu_self_test(); gz_stop(0, \"GPU TEST: PASS\");")
kyoei_replace_checked(KYOEI_LOADER
  "    if (!no_gpu) gz_gpu_start(gpu_sdk, list_stubs);"
  "    if (!no_gpu && !list_stubs && !getenv(\"GZ_READBACK_LINEAR\") && _putenv_s(\"GZ_READBACK_LINEAR\", \"1\")) fail(\"cannot configure linear image readback\");\n    if (!no_gpu) gz_gpu_start(gpu_sdk, list_stubs);")
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
  "#include \"kyoei_crash.h\""
  "#include \"kyoei_crash.h\"\n#include \"kyoei_gpu_wait.h\"")
kyoei_replace_checked(KYOEI_LOADER
  "    kyoei_tls_configure(image, image_size, main_tls);"
  "    kyoei_tls_configure(image, image_size, main_tls);\n    kyoei_gpu_wait_configure(image, image_size);")
kyoei_replace_checked(KYOEI_LOADER
  "    gz_stop(GZ_EXIT_FAULT, \"STOP: fault\");"
  "    kyoei_capture_failure(\"fault\", e);\n    gz_stop(GZ_EXIT_FAULT, \"STOP: fault\");")
kyoei_replace_checked(KYOEI_LOADER
  "    kyoei_tls_configure(image, image_size, main_tls);"
  "    kyoei_tls_configure(image, image_size, main_tls);\n    kyoei_tls_optimize(out);")
kyoei_replace_checked(KYOEI_LOADER
  "    gz_thread_attach_main();"
  "    gz_thread_attach_main();\n    kyoei_tls_enter();")
kyoei_replace_checked(KYOEI_LOADER
  "#include \"kyoei_gpu_wait.h\""
  "#include \"kyoei_gpu_wait.h\"\n#include \"kyoei_hud_trace.h\"\n#include \"kyoei_title_exit.h\"\n#include \"kyoei_text_wrap.h\"")
kyoei_replace_checked(KYOEI_LOADER
  "    kyoei_gpu_wait_configure(image, image_size);"
  "    kyoei_gpu_wait_configure(image, image_size);\n    kyoei_hud_trace_configure(image, image_size);\n    kyoei_title_exit_configure(image, image_size);\n    kyoei_text_wrap_configure(image, image_size);")
set(KYOEI_LOADER "#include \"kyoei_log_filter.h\"\n${KYOEI_LOADER}")
set(KYOEI_LOADER "#include \"kyoei_names.h\"\n${KYOEI_LOADER}")
kyoei_replace_checked(KYOEI_LOADER
  "    protect(traps, round_page(import_count * 32), 5);"
  "    if (import_count>28 && !strcmp(imports[28].library,\"libc\") && !strcmp(imports[28].symbol,\"wcstombs_s\") && bindings[28]) kyoei_names_configure(image,image_size,(uintptr_t)image+bindings[28]);\n    protect(traps, round_page(import_count * 32), 5);")
kyoei_replace_checked(KYOEI_LOADER
  "void gz_log(const char *format, ...) { va_list a;"
  "void gz_log(const char *format, ...) { if (!kyoei_log_visible(format)) return; va_list a;")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_loader.c" CONTENT "${KYOEI_LOADER}" @ONLY)
add_executable(kyoei ${CORE_FILES} "${CMAKE_CURRENT_BINARY_DIR}/kyoei_loader.c" src/kyoei_kernel.c src/kyoei_tls.c src/kyoei_modules.c src/kyoei_ime.c)
set(KYOEI_PORT_EXE_NAME "kyoei" CACHE STRING "Prepared port binary: kyoei or kyoei-orb-trace")
if(NOT KYOEI_PORT_EXE_NAME MATCHES "^(kyoei|kyoei-orb-trace|kyoei-orb-trace-next)$")
  message(FATAL_ERROR "Unsupported port executable name")
endif()
set_target_properties(kyoei PROPERTIES OUTPUT_NAME "${KYOEI_PORT_EXE_NAME}")
target_sources(kyoei PRIVATE src/kyoei_crash.c)
target_sources(kyoei PRIVATE src/kyoei_hud_trace.c src/kyoei_title_exit.c src/kyoei_text_wrap.c)
target_sources(kyoei PRIVATE src/kyoei_gpu_wait.c)
target_sources(kyoei PRIVATE src/kyoei_png.c)
target_sources(kyoei PRIVATE src/kyoei_savedata.c)
target_sources(kyoei PRIVATE src/kyoei_input_mode.c)
target_sources(kyoei PRIVATE src/kyoei_prompt_state.c)
target_sources(kyoei PRIVATE src/kyoei_ui_text.c)
target_sources(kyoei PRIVATE src/kyoei_names.c)
target_sources(kyoei PRIVATE src/kyoei_pc_names.c)
set(KYOEI_IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui")
set(KYOEI_IMGUI_SOURCES "${KYOEI_IMGUI_DIR}/imgui.cpp" "${KYOEI_IMGUI_DIR}/imgui_draw.cpp"
  "${KYOEI_IMGUI_DIR}/imgui_tables.cpp" "${KYOEI_IMGUI_DIR}/imgui_widgets.cpp")
add_library(kyoei_imgui STATIC ${KYOEI_IMGUI_SOURCES})
target_include_directories(kyoei_imgui PUBLIC "${KYOEI_IMGUI_DIR}")
target_compile_definitions(kyoei_imgui PRIVATE _CRT_SECURE_NO_WARNINGS)
target_sources(kyoei PRIVATE src/kyoei_overlay_model.cpp)
target_compile_features(kyoei PRIVATE cxx_std_23)
target_compile_definitions(kyoei PRIVATE NOMINMAX)
target_link_libraries(kyoei PRIVATE kyoei_imgui)
target_compile_definitions(kyoei PRIVATE KYOEI_SAVE_ADAPTER=1)
target_compile_definitions(kyoei PRIVATE KYOEI_PNG_ENCODER=1 MINIZ_STATIC_DEFINE=1)
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
target_include_directories(kyoei PRIVATE "${KYOEI_GPU_HEADERS}/gz_miniz-src" "${KYOEI_GPU_HEADERS}/gz_miniz-build")
set(KYOEI_SHAD "${KYOEI_CORE}/third_party/shadPS4")
file(READ "${KYOEI_CORE}/gpu/bridge.cpp" KYOEI_BRIDGE)
kyoei_replace_checked(KYOEI_BRIDGE "info.game_serial = \"CUSA02046\";" "info.game_serial = \"CUSA03562\";")
kyoei_replace_checked(KYOEI_BRIDGE "info.title = \"Godzilla\";" "info.title = \"Kyoei Toshi\";")
kyoei_replace_checked(KYOEI_BRIDGE "info.app_ver = \"01.01\";" "info.app_ver = \"01.03\";")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_bridge.cpp" CONTENT "${KYOEI_BRIDGE}" @ONLY)
file(READ "${KYOEI_SHAD}/src/core/libraries/videoout/video_out.cpp" KYOEI_VIDEOOUT)
kyoei_replace_checked(KYOEI_VIDEOOUT
  "    *label_addr = reinterpret_cast<uintptr_t>(port->buffer_labels.data());"
  "    KyoeiFlipTrace::Bind(port->buffer_labels.data(), port->buffer_labels.size());\n    *label_addr = reinterpret_cast<uintptr_t>(port->buffer_labels.data());")
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
kyoei_replace_checked(KYOEI_VIDEOOUT
  "    driver->GetPort(handle)->flip_rate = rate;"
  "    if (driver->GetPort(handle)->flip_rate != rate) gz_log(\"Kyoei videoout: SetFlipRate handle=%d rate=%d\", handle, rate);\n    driver->GetPort(handle)->flip_rate = rate;")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_videoout.cpp" CONTENT "${KYOEI_VIDEOOUT}" @ONLY)
add_library(kyoei_videoout OBJECT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_videoout.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/kyoei_bridge.cpp")
# Release the old surface before publishing completion. Otherwise a guest
# woken by the event can lock it for a new flip before the late reset to zero.
file(READ "${KYOEI_SHAD}/src/core/libraries/videoout/driver.cpp" KYOEI_VIDEOOUT_DRIVER)
set(KYOEI_VIDEOOUT_DRIVER "#include \"kyoei_overlay.h\"\n${KYOEI_VIDEOOUT_DRIVER}")
kyoei_replace_checked(KYOEI_VIDEOOUT_DRIVER
  "ImGui::Core::MustKeepDrawing()"
  "(ImGui::Core::MustKeepDrawing() || kyoei_overlay_needs_redraw())")
kyoei_replace_checked(KYOEI_VIDEOOUT_DRIVER
  "#include \"common/assert.h\""
  "extern \"C\" {\n#include \"gz.h\"\n}\n#include \"kyoei_flip_trace.h\"\n#include \"common/assert.h\"")
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
kyoei_replace_checked(KYOEI_VIDEOOUT_DRIVER
  "            port->buffer_labels[port->prev_index] = 0;"
  "            KyoeiFlipTrace::Action(\"release-before\", port->prev_index, port->buffer_labels[port->prev_index], &port->buffer_labels[port->prev_index], req.index);\n            port->buffer_labels[port->prev_index] = 0;\n            KyoeiFlipTrace::Action(\"release-after\", port->prev_index, port->buffer_labels[port->prev_index], &port->buffer_labels[port->prev_index], req.index);")
kyoei_replace_checked(KYOEI_VIDEOOUT_DRIVER
  "#include \"kyoei_flip_trace.h\""
  "#include \"kyoei_flip_trace.h\"\n#include \"kyoei_frame_stats.h\"")
kyoei_replace_checked(KYOEI_VIDEOOUT_DRIVER
  "    presenter->Present(req.frame);"
  "    { KyoeiFrameStats::Work measurement(1); presenter->Present(req.frame); }\n    KyoeiFrameStats::Presented();")
kyoei_replace_checked(KYOEI_VIDEOOUT_DRIVER
  "    Vulkan::Frame* frame;"
  "    KyoeiFrameStats::Work preparation_measurement(0);\n    Vulkan::Frame* frame;")
kyoei_replace_checked(KYOEI_VIDEOOUT_DRIVER
  "#include \"kyoei_frame_stats.h\""
  "#include \"kyoei_frame_stats.h\"\n#include \"kyoei_display_diagnostics.h\"")
kyoei_replace_checked(KYOEI_VIDEOOUT_DRIVER
  "void VideoOutDriver::DrawBlankFrame() {"
  "void VideoOutDriver::DrawBlankFrame() {\n    KyoeiDisplayDiagnostics::ClosedPort();")
kyoei_replace_checked(KYOEI_VIDEOOUT_DRIVER
  "    if (index == -1) {\n        frame = presenter->PrepareBlankFrame(false);"
  "    if (index == -1) {\n        KyoeiDisplayDiagnostics::Submitted(index,is_eop,flip_arg);\n        frame = presenter->PrepareBlankFrame(false);")
kyoei_replace_checked(KYOEI_VIDEOOUT_DRIVER
  "        frame = presenter->PrepareFrame(group, buffer.address_left);"
  "        KyoeiDisplayDiagnostics::Submitted(index,is_eop,flip_arg,group.attrib.width,group.attrib.height,group.attrib.pitch_in_pixel,static_cast<u32>(group.attrib.pixel_format),static_cast<u32>(group.attrib.tiling_mode));\n        frame = presenter->PrepareFrame(group, buffer.address_left);")
kyoei_replace_checked(KYOEI_VIDEOOUT_DRIVER
  "        timer.Start();"
  "        { KyoeiFrameStats::Work measurement(13); timer.Start(); }")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_videoout_driver.cpp" CONTENT "${KYOEI_VIDEOOUT_DRIVER}" @ONLY)
target_sources(kyoei_videoout PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/kyoei_videoout_driver.cpp")
# The emulated command processor can reach the next flip's WRITE_DATA while
# the presentation thread still owns that surface. Respect its label before
# writing the new lock; never overwrite or fabricate the expected label.
file(READ "${KYOEI_SHAD}/src/video_core/amdgpu/liverpool.cpp" KYOEI_LIVERPOOL)
set(KYOEI_SUBMIT_BORROWED [=[    if (EmulatorSettings.IsCopyGpuBuffers()) {
        std::tie(dcb, ccb) = CopyCmdBuffers(dcb, ccb);
    }

    auto task = ProcessGraphics(dcb, ccb);
]=])
set(KYOEI_SUBMIT_OWNED [=[    Task task{};
    {
        // Parameters live in the coroutine frame. Each submission owns its
        // snapshot until processing finishes, independently of SubmitDone.
        const auto make_owned_task = [](Liverpool* owner, std::vector<u32> owned_dcb,
                                       std::vector<u32> owned_ccb) -> Task {
            auto inner = owner->ProcessGraphics(owned_dcb, owned_ccb);
            while (!inner.handle.done()) {
                inner.handle.resume();
                if (!inner.handle.done()) co_yield {};
            }
            inner.handle.destroy();
        };
        task = make_owned_task(this, {dcb.begin(), dcb.end()}, {ccb.begin(), ccb.end()});
        KyoeiFlipTrace::Action("submit-owned", 0, dcb.size_bytes(), dcb.data(), ccb.size_bytes());
    }
]=])
kyoei_replace_checked(KYOEI_LIVERPOOL "${KYOEI_SUBMIT_BORROWED}" "${KYOEI_SUBMIT_OWNED}")
kyoei_replace_checked(KYOEI_LIVERPOOL
  "                    Platform::IrqC::Instance()->Signal(Platform::InterruptId::GfxFlip);"
  "                    KyoeiFlipTrace::Action(\"patched-flip\", 0, nop->header.raw, nop, base_addr);\n                    Platform::IrqC::Instance()->Signal(Platform::InterruptId::GfxFlip);")
kyoei_replace_checked(KYOEI_LIVERPOOL
  "#include \"common/assert.h\""
  "extern \"C\" {\n#include \"gz.h\"\n}\n#include \"kyoei_flip_trace.h\"\n#include \"common/assert.h\"")
kyoei_replace_checked(KYOEI_LIVERPOOL
  "                u64* address = write_data->Address<u64*>();"
  "                u64* address = write_data->Address<u64*>();\n                if (vo_port && vo_port->IsVoLabel(address) && data_size == 4 && write_data->data[0] == 1) {\n                    while (*address != 0) {\n                        YIELD_GFX();\n                    }\n                }")
kyoei_replace_checked(KYOEI_LIVERPOOL
  "                if (!write_data->wr_one_addr.Value()) {"
  "                if (const auto kyoei_index = KyoeiFlipTrace::LabelIndex(address); kyoei_index >= 0) {\n                    KyoeiFlipTrace::Action(\"write-before\", kyoei_index, *address, address, (static_cast<u64>(data_size) << 32) | write_data->data[0]);\n                    KyoeiFlipTrace::Action(vo_port && vo_port->IsVoLabel(address) ? \"port-match\" : \"port-mismatch\", kyoei_index, *address, address);\n                }\n                if (!write_data->wr_one_addr.Value()) {")
kyoei_replace_checked(KYOEI_LIVERPOOL
  "                    std::memcpy(address, write_data->data, data_size);"
  "                    std::memcpy(address, write_data->data, data_size);\n                    if (const auto kyoei_index = KyoeiFlipTrace::LabelIndex(address); kyoei_index >= 0) {\n                        KyoeiFlipTrace::Action(\"write-after\", kyoei_index, *address, address);\n                        KyoeiFlipTrace::StressAfterWrite();\n                    }")
kyoei_replace_checked(KYOEI_LIVERPOOL
  "#include \"kyoei_flip_trace.h\""
  "#include \"kyoei_flip_trace.h\"\n#include \"kyoei_frame_stats.h\"")
# The scope lives on the worker stack, outside every coroutine. It includes
# nested GPU waits, so it must not be added to scheduler/readback timings.
kyoei_replace_checked(KYOEI_LIVERPOOL
  "        while (num_submits || num_commands) {"
  "        {\n        KyoeiFrameStats::Work measurement(12);\n        while (num_submits || num_commands) {")
kyoei_replace_checked(KYOEI_LIVERPOOL
  "        if (submit_done) {"
  "        }\n        if (submit_done) {")
# Synchronous calls only: no timer may span co_yield or a coroutine lifetime.
kyoei_replace_checked(KYOEI_LIVERPOOL "        callback();"
  "        { KyoeiFrameStats::Work callback_measurement(14); callback(); }")
kyoei_replace_checked(KYOEI_LIVERPOOL
  "                    vo_port->WaitVoLabel([&] { return wait_reg_mem->Test(regs.reg_array); });"
  "                    { KyoeiFrameStats::Work label_measurement(15); vo_port->WaitVoLabel([&] { return wait_reg_mem->Test(regs.reg_array); }); }")
foreach(KYOEI_MEASURE_METHOD IN ITEMS Draw DrawIndirect DispatchDirect DispatchIndirect CpSync)
  if(KYOEI_MEASURE_METHOD MATCHES "^Draw")
    set(KYOEI_MEASURE_CATEGORY 16)
  elseif(KYOEI_MEASURE_METHOD MATCHES "^Dispatch")
    set(KYOEI_MEASURE_CATEGORY 17)
  else()
    set(KYOEI_MEASURE_CATEGORY 18)
  endif()
  set(KYOEI_MEASURE_PATTERN "rasterizer->${KYOEI_MEASURE_METHOD}\\([^;]*\\);")
  if(NOT KYOEI_LIVERPOOL MATCHES "${KYOEI_MEASURE_PATTERN}")
    message(FATAL_ERROR "Missing synchronous measurement anchor ${KYOEI_MEASURE_METHOD}")
  endif()
  string(REGEX REPLACE "${KYOEI_MEASURE_PATTERN}"
    "{ KyoeiFrameStats::Work raster_measurement(${KYOEI_MEASURE_CATEGORY}); \\0 }"
    KYOEI_LIVERPOOL "${KYOEI_LIVERPOOL}")
endforeach()
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_liverpool.cpp" CONTENT "${KYOEI_LIVERPOOL}" @ONLY)
target_sources(kyoei_videoout PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/kyoei_liverpool.cpp")
# Patch only a private snapshot. The guest can reuse NOP storage by changing
# its header without clearing its old payload, so host markers must never
# remain in that storage after this API returns.
file(READ "${KYOEI_SHAD}/src/core/libraries/gnmdriver/gnmdriver.cpp" KYOEI_GNM)
kyoei_replace_checked(KYOEI_GNM
  "liverpool = std::make_unique<AmdGpu::Liverpool>();"
  "if (g_window) liverpool = std::make_unique<AmdGpu::Liverpool>();")
kyoei_replace_checked(KYOEI_GNM
  "presenter = std::make_unique<Vulkan::Presenter>(*g_window, liverpool.get());"
  "if (g_window) presenter = std::make_unique<Vulkan::Presenter>(*g_window, liverpool.get());")
kyoei_replace_checked(KYOEI_GNM
  "    if (EmulatorSettings.IsCopyGpuBuffers()) {\n        liverpool->ReserveCopyBufferSpace();\n    }"
  "    /* Kyoei SubmitGfx owns each command snapshot; no shared copy arena. */")
kyoei_replace_checked(KYOEI_GNM
  "    auto* cmdbuf = dcb_gpu_addrs[count - 1];\n    const auto size_dw = dcb_sizes_in_bytes[count - 1] / 4;"
  "    const auto size_dw = dcb_sizes_in_bytes[count - 1] / 4;\n    std::vector<u32> kyoei_flip_commands(dcb_gpu_addrs[count - 1], dcb_gpu_addrs[count - 1] + size_dw);\n    std::vector<u32*> kyoei_dcb_addresses(dcb_gpu_addrs, dcb_gpu_addrs + count);\n    kyoei_dcb_addresses[count - 1] = kyoei_flip_commands.data();\n    auto* cmdbuf = kyoei_flip_commands.data();")
kyoei_replace_checked(KYOEI_GNM
  "    return sceGnmSubmitCommandBuffers(count, const_cast<const u32**>(dcb_gpu_addrs),"
  "    return sceGnmSubmitCommandBuffers(count, const_cast<const u32**>(kyoei_dcb_addresses.data()),")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_gnmdriver.cpp" CONTENT "${KYOEI_GNM}" @ONLY)
target_sources(kyoei_videoout PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/kyoei_gnmdriver.cpp")
file(READ "${KYOEI_SHAD}/src/video_core/buffer_cache/buffer_cache.cpp" KYOEI_BUFFER_CACHE)
set(KYOEI_BUFFER_CACHE "#include \"kyoei_raster_trace.h\"\n${KYOEI_BUFFER_CACHE}")
set(KYOEI_VERTEX_INPUTS [=[    pipeline.GetVertexInputs(attributes, bindings, divisors, guest_buffers,
                             regs.vgt_instance_step_rate_0, regs.vgt_instance_step_rate_1);]=])
set(KYOEI_VERTEX_PROBE [=[
    if (KyoeiRasterTrace::Capturing()) {
        KyoeiRasterTrace::current_vertices={};KyoeiRasterTrace::current_vertex_count=0;
        u32 blend_word{},depth_word{};std::memcpy(&blend_word,&regs.blend_control[0],4);std::memcpy(&depth_word,&regs.depth_control,4);
        if ((blend_word || regs.primitive_type==AmdGpu::PrimitiveType::RectList ||
            (regs.primitive_type==AmdGpu::PrimitiveType::TriangleStrip && depth_word==0)) &&
            (std::fabs(regs.viewports[0].xscale)>=64 || regs.IsClipDisabled())) {
            for (size_t slot=0;slot<guest_buffers.size() && slot<4;++slot) {
                const auto &buffer=guest_buffers[slot];
                auto &probe=KyoeiRasterTrace::current_vertices[slot];
                probe.address=buffer.base_address;probe.bytes=buffer.GetSize();probe.stride=buffer.GetStride();
                probe.location=attributes[slot].location;probe.format=static_cast<u32>(attributes[slot].format);
                if (bindings[slot].inputRate!=vk::VertexInputRate::eVertex) probe.status=6;
                else KyoeiRasterTrace::SampleSelectedXY(probe,KyoeiRasterTrace::current_selection,
                    [this](uint64_t addr,uint64_t bytes) { return memory->ClampRangeSize(addr,bytes)==bytes; },
                    [this](uint64_t addr,uint64_t bytes) { return IsRegionGpuModified(addr,bytes); },
                    [](uint64_t addr,void *dest,uint64_t bytes) { return gz_memory_copy(static_cast<uintptr_t>(addr),dest,bytes,0)!=0; });
                ++KyoeiRasterTrace::current_vertex_count;
            }
        }
    }
]=])
kyoei_replace_checked(KYOEI_BUFFER_CACHE "${KYOEI_VERTEX_INPUTS}" "${KYOEI_VERTEX_INPUTS}\n${KYOEI_VERTEX_PROBE}")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_buffer_cache.cpp" CONTENT "${KYOEI_BUFFER_CACHE}" @ONLY)
target_sources(kyoei_videoout PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/kyoei_buffer_cache.cpp")
file(READ "${KYOEI_SHAD}/src/video_core/texture_cache/texture_cache.cpp" KYOEI_TEXTURE_CACHE)
# Same-range dirty comparison, also identified in bloodborne_pc e5d04e2.
# MarkAsMaybeDirty hashes guest_size; comparing only the first 8x8 pixels
# invalidates unchanged images and can miss later CPU writes.
set(KYOEI_PARTIAL_DIRTY_HASH [=[        // The image size should be less than page size to be considered MaybeCpuDirty
        // So this calculation should be very uncommon and reasonably fast
        // For now we'll just check up to 64 first pixels
        const auto addr = std::bit_cast<u8*>(image.info.guest_address);
        const u32 w = std::min(image.info.size.width, u32(8));
        const u32 h = std::min(image.info.size.height, u32(8));

        const u32 s_w = image.info.props.is_block ? Common::DivCeil(w, 4u) : w;
        const u32 s_h = image.info.props.is_block ? Common::DivCeil(h, 4u) : h;
        const u32 size = s_w * s_h * (image.info.num_bits / 8);
        const u64 hash = XXH3_64bits(addr, size);]=])
kyoei_replace_checked(KYOEI_TEXTURE_CACHE "${KYOEI_PARTIAL_DIRTY_HASH}"
  "        // Match the complete range hashed by MarkAsMaybeDirty.\n        const u64 hash = XXH3_64bits(std::bit_cast<const u8*>(image.info.guest_address), image.info.guest_size);")
set(KYOEI_TEXTURE_CACHE "extern \"C\" {\n#include \"gz.h\"\n}\n#include \"kyoei_frame_stats.h\"\n#include \"kyoei_readback_batch.h\"\n${KYOEI_TEXTURE_CACHE}")
kyoei_replace_checked(KYOEI_TEXTURE_CACHE "namespace VideoCore {" "#include <vk_mem_alloc.h>\nnamespace VideoCore {")
kyoei_replace_checked(KYOEI_TEXTURE_CACHE
  "void TextureCache::ProcessDownloadImages() {"
  "void TextureCache::ProcessDownloadImages() {\n    if (download_images.empty()) return;\n    KyoeiFrameStats::Work download_measurement(2);\n    KyoeiReadback::Batch batch([this] { scheduler.Finish(); });\n    KyoeiReadback::Activation activation(batch);")
kyoei_replace_checked(KYOEI_TEXTURE_CACHE
  "    download_images.clear();"
  "    batch.Flush();\n    if (batch.images && KyoeiFrameStats::Enabled()) {\n        static unsigned reports=0;\n        if (++reports % 120 == 0) gz_log(\"Kyoei readback batch: images=%u waits=%u bytes=%llu\",batch.images,batch.waits,(unsigned long long)batch.bytes);\n    }\n    download_images.clear();")
kyoei_replace_checked(KYOEI_TEXTURE_CACHE
  "    const auto [download, offset] = download_buffer.Map(download_size);"
  "    auto mapping = download_buffer.Map(download_size, 0, !KyoeiReadback::active);\n    if (!mapping.first && KyoeiReadback::active) {\n        KyoeiReadback::active->Flush();\n        mapping=download_buffer.Map(download_size);\n    }\n    ASSERT_MSG(mapping.first, \"Download image exceeds available stream buffer\");\n    const auto [download, offset] = mapping;\n    if (KyoeiReadback::active) KyoeiReadback::active->BeforeReuse(offset,download_size);")
set(KYOEI_DOWNLOAD_ORIGINAL [=[    if (sync) {
        scheduler.Finish();
        Core::Memory::Instance()->TryWriteBacking(std::bit_cast<u8*>(image.info.guest_address),
                                                  download, download_size);
    } else {
        scheduler.DeferPriorityOperation(
            [this, device_addr = image.info.guest_address, download, download_size] {
                Core::Memory::Instance()->TryWriteBacking(std::bit_cast<u8*>(device_addr), download,
                                                          download_size);
            });
    }]=])
set(KYOEI_DOWNLOAD_ADAPTED [=[    // Host cache invalidation belongs after GPU completion, before CPU reads.
    // StreamBuffer::Commit alone happens before this copy is recorded.
    auto publish = [allocator=instance.GetAllocator(), allocation=download_buffer.buffer.allocation,
                    coherent=download_buffer.is_coherent, offset,
                    device_addr=image.info.guest_address, download, download_size] {
        if (!coherent) {
            const auto result=vmaInvalidateAllocation(allocator,allocation,offset,download_size);
            ASSERT_MSG(result == VK_SUCCESS, "Download cache invalidation failed");
        }
        Core::Memory::Instance()->TryWriteBacking(std::bit_cast<u8*>(device_addr),download,download_size);
    };
    if (sync && KyoeiReadback::active) {
        KyoeiReadback::active->Add(offset,download_size,std::move(publish));
    } else if (sync) {
        KyoeiReadback::RecordSingle(download_size);
        scheduler.Finish();
        publish();
    } else {
        scheduler.DeferPriorityOperation(std::move(publish));
    }]=])
kyoei_replace_checked(KYOEI_TEXTURE_CACHE "${KYOEI_DOWNLOAD_ORIGINAL}" "${KYOEI_DOWNLOAD_ADAPTED}")
# Break down the existing wall-clock download scope without changing fences,
# publication order or the default batching policy.
kyoei_replace_checked(KYOEI_TEXTURE_CACHE
  "scheduler.Finish();"
  "{ KyoeiFrameStats::Work finish_measurement(3); scheduler.Finish(); }")
kyoei_replace_checked(KYOEI_TEXTURE_CACHE
  "KyoeiReadback::RecordSingle(download_size);\n        { KyoeiFrameStats::Work finish_measurement(3); scheduler.Finish(); }"
  "KyoeiReadback::RecordSingle(download_size);\n        { KyoeiFrameStats::Work finish_measurement(3,download_size,image.info.size.width,image.info.size.height); scheduler.Finish(); }")
kyoei_replace_checked(KYOEI_TEXTURE_CACHE
  "    auto mapping = download_buffer.Map(download_size, 0, !KyoeiReadback::active);"
  "    auto map_download = [&](bool wait) { KyoeiFrameStats::Work map_measurement(5); return download_buffer.Map(download_size, 0, wait); };\n    auto mapping = map_download(!KyoeiReadback::active);")
kyoei_replace_checked(KYOEI_TEXTURE_CACHE
  "mapping=download_buffer.Map(download_size);"
  "mapping=map_download(true);")
kyoei_replace_checked(KYOEI_TEXTURE_CACHE
  "device_addr=image.info.guest_address, download, download_size] {"
  "device_addr=image.info.guest_address, download, download_size] {\n        KyoeiFrameStats::Work publish_measurement(4);")
set(KYOEI_TEXTURE_CACHE "#include \"kyoei_live_prompts.h\"\n${KYOEI_TEXTURE_CACHE}")
kyoei_replace_checked(KYOEI_TEXTURE_CACHE
  "void TextureCache::RefreshImage(Image& image) {"
  "void TextureCache::RefreshImage(Image& image) {\n    bool prompt_changed=false; unsigned prompt_mode=0;\n    const auto* prompt=False(image.flags & ImageFlagBits::GpuModified) ? KyoeiPrompts::Select(image,True(image.flags & ImageFlagBits::CpuDirty),prompt_changed,prompt_mode) : nullptr;\n    if (prompt_changed) image.flags |= ImageFlagBits::CpuDirty;")
kyoei_replace_checked(KYOEI_TEXTURE_CACHE
  "    const auto [in_buffer, in_offset] =\n        buffer_cache.ObtainBufferForImage(image.info.guest_address, image.info.guest_size);"
  "    auto source=buffer_cache.ObtainBufferForImage(image.info.guest_address,image.info.guest_size);\n    if (prompt) {\n        auto& staging=buffer_cache.GetUtilityBuffer(MemoryUsage::Upload);\n        const auto [pixels,at]=staging.Map(image.info.guest_size,16);\n        ASSERT(pixels); std::memset(pixels,0,image.info.guest_size);\n        const auto& data=prompt_mode==KYOEI_INPUT_CONTROLLER?prompt->controller:prompt->keyboard;\n        const bool copied=KyoeiPrompts::Rows(*prompt,image.info,[&](size_t to,size_t from,size_t bytes){std::memcpy(pixels+to,data.data()+from,bytes);return true;});\n        ASSERT(copied); staging.Commit(); source={&staging,at};\n    }\n    const auto [in_buffer,in_offset]=source;")
kyoei_replace_checked(KYOEI_TEXTURE_CACHE
  "tile_manager.DetileImage(in_buffer->Handle(), in_offset, image.info);"
  "(prompt ? TileManager::Result{in_buffer->Handle(),in_offset} : tile_manager.DetileImage(in_buffer->Handle(), in_offset, image.info));")
kyoei_replace_checked(KYOEI_TEXTURE_CACHE
  "False(image.flags & ImageFlagBits::GpuModified) ? KyoeiPrompts::Select"
  "!image.usage.render_target ? KyoeiPrompts::Select")
kyoei_replace_checked(KYOEI_TEXTURE_CACHE
  "if (is_gpu_modified && !is_gpu_dirty) {"
  "if (is_gpu_modified && !is_gpu_dirty && !prompt) {")
kyoei_replace_checked(KYOEI_TEXTURE_CACHE
  "void TextureCache::UnregisterImage(ImageId image_id) {\n    Image& image = slot_images[image_id];"
  "void TextureCache::UnregisterImage(ImageId image_id) {\n    Image& image = slot_images[image_id];\n    KyoeiPrompts::states.erase(image.image_uid);")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_texture_cache.cpp" CONTENT "${KYOEI_TEXTURE_CACHE}" @ONLY)

# Kyoei-only driver cache. Keep the frozen dependency intact. Create the
# Vulkan cache before WarmUp so preloaded pipelines benefit from its contents.
file(READ "${KYOEI_SHAD}/src/video_core/renderer_vulkan/vk_pipeline_cache.cpp" KYOEI_PIPELINE_CACHE)
set(KYOEI_PIPELINE_CACHE "extern \"C\" {\n#include \"gz.h\"\n}\n#include \"kyoei_frame_stats.h\"\n#include \"kyoei_pipeline_cache.h\"\n${KYOEI_PIPELINE_CACHE}")
kyoei_replace_checked(KYOEI_PIPELINE_CACHE
  "        GraphicsPipeline::SerializationSupport sdata{};"
  "        KyoeiFrameStats::Work kyoei_compile_work(7);\n        GraphicsPipeline::SerializationSupport sdata{};")
kyoei_replace_checked(KYOEI_PIPELINE_CACHE
  "        ComputePipeline::SerializationSupport sdata{};"
  "        KyoeiFrameStats::Work kyoei_compile_work(8);\n        ComputePipeline::SerializationSupport sdata{};")
kyoei_replace_checked(KYOEI_PIPELINE_CACHE
  "    LOG_INFO(Render_Vulkan, \"Compiling {} shader {:#x} {}\", info.stage, info.pgm_hash,"
  "    KyoeiFrameStats::Work kyoei_compile_work(6);\n    LOG_INFO(Render_Vulkan, \"Compiling {} shader {:#x} {}\", info.stage, info.pgm_hash,")
kyoei_replace_checked(KYOEI_PIPELINE_CACHE
  "    WarmUp();\n\n    auto [cache_result, cache] = instance.GetDevice().createPipelineCacheUnique({});"
  "    const auto properties=instance.GetPhysicalDevice().getProperties();\n    KyoeiPipelineCache::Identity identity{properties.vendorID,properties.deviceID,{}};\n    std::memcpy(identity.uuid.data(),properties.pipelineCacheUUID.data(),16);\n    auto initial=KyoeiPipelineCache::Load(identity);\n    auto [cache_result, cache] = instance.GetDevice().createPipelineCacheUnique({.initialDataSize=initial.size(),.pInitialData=initial.data()});\n    if(cache_result!=vk::Result::eSuccess && !initial.empty()) {\n        auto retry=instance.GetDevice().createPipelineCacheUnique({});\n        cache_result=retry.result; cache=std::move(retry.value);\n    }")
kyoei_replace_checked(KYOEI_PIPELINE_CACHE
  "    pipeline_cache = std::move(cache);"
  "    pipeline_cache = std::move(cache);\n    WarmUp();\n    KyoeiPipelineCache::Checkpoint(instance,pipeline_cache,uint64_t(num_new_pipelines)+1,true);")
kyoei_replace_checked(KYOEI_PIPELINE_CACHE
  "const GraphicsPipeline* PipelineCache::GetGraphicsPipeline() {"
  "const GraphicsPipeline* PipelineCache::GetGraphicsPipeline() {\n    KyoeiPipelineCache::Checkpoint(instance,pipeline_cache,uint64_t(num_new_pipelines)+1);")
kyoei_replace_checked(KYOEI_PIPELINE_CACHE
  "const ComputePipeline* PipelineCache::GetComputePipeline() {"
  "const ComputePipeline* PipelineCache::GetComputePipeline() {\n    KyoeiPipelineCache::Checkpoint(instance,pipeline_cache,uint64_t(num_new_pipelines)+1);")
kyoei_replace_checked(KYOEI_PIPELINE_CACHE
  "PipelineCache::~PipelineCache() = default;"
  "PipelineCache::~PipelineCache() { KyoeiPipelineCache::Persist(instance,pipeline_cache); }")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_pipeline_cache.cpp" CONTENT "${KYOEI_PIPELINE_CACHE}" @ONLY)
file(READ "${KYOEI_SHAD}/src/video_core/renderer_vulkan/vk_pipeline_serialization.cpp" KYOEI_PIPELINE_SERIALIZATION)
set(KYOEI_PIPELINE_SERIALIZATION "extern \"C\" {\n#include \"gz.h\"\n}\n#include \"kyoei_pipeline_cache.h\"\n${KYOEI_PIPELINE_SERIALIZATION}")
kyoei_replace_checked(KYOEI_PIPELINE_SERIALIZATION
  "if (std::memcmp(profile_data.data(), &profile, sizeof(profile)) != 0)"
  "if (!KyoeiPipelineCache::ProfileMatches(std::span<const uint8_t>(profile_data),profile))")
kyoei_replace_checked(KYOEI_PIPELINE_SERIALIZATION
  "void PipelineCache::Sync() {\n    Storage::DataBase::Instance().Close();"
  "void PipelineCache::Sync() {\n    gz_log(\"Kyoei driver cache: collecting driver data\");\n    const bool saved=KyoeiPipelineCache::Persist(instance,pipeline_cache);\n    gz_log(\"Kyoei driver cache: driver data saved=%u; closing shader database\",unsigned(saved));\n    Storage::DataBase::Instance().Close();")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_pipeline_serialization.cpp" CONTENT "${KYOEI_PIPELINE_SERIALIZATION}" @ONLY)
target_sources(kyoei_videoout PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/kyoei_pipeline_cache.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/kyoei_pipeline_serialization.cpp" src/kyoei_gpu_cache_shutdown.cpp)
add_executable(test_kyoei_pipeline_cache src/test_kyoei_pipeline_cache.cpp)
target_compile_features(test_kyoei_pipeline_cache PRIVATE cxx_std_23)
target_compile_options(test_kyoei_pipeline_cache PRIVATE -UNDEBUG)
target_compile_definitions(test_kyoei_pipeline_cache PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS)
target_sources(kyoei_videoout PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/kyoei_texture_cache.cpp")
file(READ "${KYOEI_SHAD}/src/video_core/renderer_vulkan/vk_rasterizer.cpp" KYOEI_RASTERIZER)
set(KYOEI_RASTERIZER "#include \"kyoei_raster_trace.h\"\n${KYOEI_RASTERIZER}")
set(KYOEI_DRAW_SELECTION [=[
    if (KyoeiRasterTrace::Capturing()) {
        const auto &probe_vs=pipeline->GetStage(Shader::LogicalStage::Vertex);
        const auto [probe_vertex,probe_instance]=GetDrawOffsets(regs,probe_vs,pipeline->GetFetchShader());
        auto &selection=KyoeiRasterTrace::current_selection;
        selection={};selection.valid=1;selection.indexed=is_indexed;selection.count=regs.num_indices;
        selection.base_vertex=is_indexed?int64_t(static_cast<s32>(probe_vertex)):int64_t(probe_vertex);
        selection.index_bytes=regs.index_buffer_type.index_type==AmdGpu::IndexType::Index16?2:4;
        selection.address=regs.index_base_address.Address<VAddr>()+uint64_t(index_offset)*selection.index_bytes;
    }
]=])
kyoei_replace_checked(KYOEI_RASTERIZER
  "void Rasterizer::Draw(bool is_indexed, u32 index_offset) {"
  "void Rasterizer::Draw(bool is_indexed, u32 index_offset) {\n    KyoeiRasterTrace::BeginDraw();")
kyoei_replace_checked(KYOEI_RASTERIZER
  "                              u32 max_count, VAddr count_address) {"
  "                              u32 max_count, VAddr count_address) {\n    KyoeiRasterTrace::BeginDraw();")
set(KYOEI_UNIFORM_PROBE [=[
        if (KyoeiRasterTrace::Capturing() && KyoeiRasterTrace::CanSampleResource(desc) && size>0 &&
            (stage.l_stage==Shader::LogicalStage::Vertex || stage.l_stage==Shader::LogicalStage::Fragment)) {
            u32 probe_blend{};std::memcpy(&probe_blend,&liverpool->regs.blend_control[0],4);
            if(probe_blend && KyoeiRasterTrace::current_uniform_count<KyoeiRasterTrace::current_uniforms.size()) {
                auto &probe=KyoeiRasterTrace::current_uniforms[KyoeiRasterTrace::current_uniform_count++];
                probe.address=vsharp.base_address;probe.bytes=size;probe.stage=static_cast<unsigned>(stage.l_stage);probe.binding=i;
                KyoeiRasterTrace::SampleWords(probe,
                    [this](uint64_t addr,uint64_t bytes) { return memory->ClampRangeSize(addr,bytes)==bytes; },
                    [this](uint64_t addr,uint64_t bytes) { return buffer_cache.IsRegionGpuModified(addr,bytes); },
                    [](uint64_t addr,void *dest,uint64_t bytes) { return gz_memory_copy(static_cast<uintptr_t>(addr),dest,bytes,0)!=0; });
            }
        }
]=])
kyoei_replace_checked(KYOEI_RASTERIZER
  "        const bool is_storage = desc.IsStorage(vsharp);"
  "        const bool is_storage = desc.IsStorage(vsharp);\n${KYOEI_UNIFORM_PROBE}")
set(KYOEI_DIRECT_BIND [=[    const auto state = BeginRendering(pipeline);

    buffer_cache.BindVertexBuffers(*pipeline, buffer_barriers);
    if (is_indexed) {
        buffer_cache.BindIndexBuffer(index_offset, buffer_barriers);]=])
kyoei_replace_checked(KYOEI_RASTERIZER "${KYOEI_DIRECT_BIND}"
  "    const auto state = BeginRendering(pipeline);\n${KYOEI_DRAW_SELECTION}\n    buffer_cache.BindVertexBuffers(*pipeline, buffer_barriers);\n    if (is_indexed) {\n        buffer_cache.BindIndexBuffer(index_offset, buffer_barriers);")
set(KYOEI_SCISSOR_PUSH [=[        scissors.push_back({
            .offset = {vp_scsr.top_left_x, vp_scsr.top_left_y},
            .extent = {vp_scsr.GetWidth(), vp_scsr.GetHeight()},
        });]=])
kyoei_replace_checked(KYOEI_RASTERIZER "${KYOEI_SCISSOR_PUSH}"
  "${KYOEI_SCISSOR_PUSH}\n        if (KyoeiRasterTrace::Capturing()) KyoeiRasterTrace::Record(KyoeiRasterTrace::MakeProfile(regs,viewport,scissors.back(),i));")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_rasterizer.cpp" CONTENT "${KYOEI_RASTERIZER}" @ONLY)
target_sources(kyoei_videoout PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/kyoei_rasterizer.cpp")
file(READ "${KYOEI_CORE}/gpu/window.cpp" KYOEI_WINDOW)
file(READ "${KYOEI_SHAD}/src/video_core/renderer_vulkan/vk_scheduler.cpp" KYOEI_SCHEDULER)
set(KYOEI_SCHEDULER "extern \"C\" {\n#include \"gz.h\"\n}\n#include \"kyoei_frame_stats.h\"\n${KYOEI_SCHEDULER}")
kyoei_replace_checked(KYOEI_SCHEDULER
  "void Scheduler::Wait(u64 tick) {"
  "void Scheduler::Wait(u64 tick) {\n    const bool measuring=KyoeiFrameStats::Enabled();\n    const auto wait_started=measuring ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};\n    const auto diagnostic_before=measuring ? KyoeiDiagnosticActivity::Now() : KyoeiDiagnosticActivity::Stamp{};\n    const auto next_before=master_semaphore.CurrentTick();\n    const auto known_before=master_semaphore.KnownGpuTick();")
kyoei_replace_checked(KYOEI_SCHEDULER
  "    master_semaphore.Wait(tick);"
  "    master_semaphore.Wait(tick);\n    if (measuring) KyoeiFrameStats::GpuWait(tick,next_before,known_before,master_semaphore.KnownGpuTick(),tick>=next_before,std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-wait_started).count(),diagnostic_before);")
kyoei_replace_checked(KYOEI_SCHEDULER
  "    SubmitExecution(info);\n    Wait(presubmit_tick);"
  "    { KyoeiFrameStats::Work measurement(9); SubmitExecution(info); }\n    { KyoeiFrameStats::Work measurement(10); Wait(presubmit_tick); }")
kyoei_replace_checked(KYOEI_SCHEDULER
  "    std::scoped_lock lk{submit_mutex};"
  "    auto lk = [] { KyoeiFrameStats::Work measurement(11); return std::unique_lock{submit_mutex}; }();")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_scheduler.cpp" CONTENT "${KYOEI_SCHEDULER}" @ONLY)
target_sources(kyoei_videoout PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/kyoei_scheduler.cpp")
file(READ "${KYOEI_CORE}/gpu/pad.cpp" KYOEI_PAD)
set(KYOEI_PAD "#include \"kyoei_mouse.h\"\n${KYOEI_PAD}")
set(KYOEI_PAD "#include \"kyoei_pc_controls.h\"\n${KYOEI_PAD}")
set(KYOEI_OLD_KEYS [=[u32 key_mask(SDL_Scancode key) {
    switch (key) {
    case SDL_SCANCODE_UP: return 0x10; case SDL_SCANCODE_RIGHT: return 0x20;
    case SDL_SCANCODE_DOWN: return 0x40; case SDL_SCANCODE_LEFT: return 0x80;
    case SDL_SCANCODE_RETURN: return 8; case SDL_SCANCODE_SPACE: return 0x4000;
    case SDL_SCANCODE_X: return 0x2000; case SDL_SCANCODE_Z: return 0x8000;
    case SDL_SCANCODE_C: return 0x1000; case SDL_SCANCODE_Q: return 0x400;
    case SDL_SCANCODE_E: return 0x800; case SDL_SCANCODE_1: return 0x100;
    case SDL_SCANCODE_3: return 0x200; case SDL_SCANCODE_TAB: return 0x100000;
    default: return 0;
    }
}]=])
kyoei_replace_checked(KYOEI_PAD "${KYOEI_OLD_KEYS}"
  "u32 key_mask(SDL_Scancode key) { return KyoeiPcControls::KeyMask(key); }")
kyoei_replace_checked(KYOEI_PAD "    u32 buttons = keys;"
  "    keys=0; for (unsigned i=1;i<held.size();++i) if(held[i]) keys|=key_mask(static_cast<SDL_Scancode>(i));\n    u32 buttons = keys;\n    if(KyoeiMouse::captured && !kyoei_overlay_kind()) {\n        auto mouse=SDL_GetMouseState(nullptr,nullptr);\n        if(mouse&SDL_BUTTON_LMASK)buttons|=0x2000;\n        if(mouse&SDL_BUTTON_RMASK)buttons|=0x200;\n        if(mouse&SDL_BUTTON_MMASK)buttons|=4;\n    }")
kyoei_replace_checked(KYOEI_PAD
  "    if (buttons & 0x100) data.analogButtons.l2 = 255;"
  "    KyoeiMouse::Apply(data.rightStick);\n    if (buttons & 0x100) data.analogButtons.l2 = 255;")
set(KYOEI_PAD "#include \"kyoei_title_exit.h\"
${KYOEI_PAD}")
kyoei_replace_checked(KYOEI_PAD "    if (gz_dialog_input(buttons)) {"
  "    const int kyoei_title_consumed=kyoei_title_exit_input(buttons,data.leftStick.y);
    if (kyoei_title_exit_take_quit()) { SDL_Event quit{}; quit.type=SDL_EVENT_QUIT; SDL_PushEvent(&quit); }
    if (kyoei_title_consumed || gz_dialog_input(buttons)) {")
kyoei_replace_checked(KYOEI_PAD
  "keyboard_axis(SDL_SCANCODE_A,SDL_SCANCODE_D,data.leftStick.x)"
  "(kyoei_prompt_pause_active()?data.leftStick.x:keyboard_axis(SDL_SCANCODE_A,SDL_SCANCODE_D,data.leftStick.x))")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_pad.cpp" CONTENT "${KYOEI_PAD}" @ONLY)
target_sources(kyoei_videoout PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/kyoei_pad.cpp")
set(KYOEI_WINDOW "#include \"kyoei_mouse.h\"\n${KYOEI_WINDOW}")
kyoei_replace_checked(KYOEI_WINDOW
  "    SDL_Event event;"
  "    KyoeiMouse::Update(window);\n    SDL_Event event;")
kyoei_replace_checked(KYOEI_WINDOW
  "        gz_pad_event(event);"
  "        KyoeiMouse::Event(window,event);\n        gz_pad_event(event);")
kyoei_replace_checked(KYOEI_WINDOW
  "    gz_pad_poll();"
  "    KyoeiMouse::Update(window);\n    gz_pad_poll();")
set(KYOEI_WINDOW "#include \"kyoei_raster_trace.h\"\n#include \"kyoei_overlay_gpu.h\"\n${KYOEI_WINDOW}")
kyoei_replace_checked(KYOEI_WINDOW
  "    SDL_Event event;"
  "    KyoeiOverlay::UpdateTextInput(window);\n    SDL_Event event;")
set(KYOEI_WINDOW "#include \"kyoei_input_mode.h\"\n${KYOEI_WINDOW}")
kyoei_replace_checked(KYOEI_WINDOW
  "        gz_pad_event(event);"
  "        if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_TEXT_INPUT) kyoei_input_keyboard();\n        else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) kyoei_input_controller_button();\n        else if (event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION) kyoei_input_controller_axis(event.gaxis.axis,event.gaxis.value);\n        else if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST || event.type == SDL_EVENT_GAMEPAD_REMOVED) kyoei_input_release();\n        if (KyoeiOverlay::HandleEvent(event)) continue;\n        gz_pad_event(event);")
set(KYOEI_WINDOW "#include \"kyoei_window_options.h\"\n${KYOEI_WINDOW}")
kyoei_replace_checked(KYOEI_WINDOW
  "base_title{title}"
  "base_title{\"City Shrouded in Shadow - Kyoei Toshi\"}")
kyoei_replace_checked(KYOEI_WINDOW
  "    window = SDL_CreateWindow(title,"
  "    (void)title;\n    window = SDL_CreateWindow(base_title.c_str(),")
kyoei_replace_checked(KYOEI_WINDOW
  "    (void)title;\n    window = SDL_CreateWindow(base_title.c_str(), w, h, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);"
  "    (void)title;\n    const auto options=KyoeiWindowOptions::Read(w,h);\n    w=options.width; h=options.height; width.store(w); height.store(h);\n    window = SDL_CreateWindow(base_title.c_str(), w, h, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | (options.fullscreen ? SDL_WINDOW_FULLSCREEN : 0));")
kyoei_replace_checked(KYOEI_WINDOW
  "    gz_log(\"GPU: SDL window created (%dx%d)\", w, h);"
  "    int pixels_w=w,pixels_h=h; SDL_GetWindowSizeInPixels(window,&pixels_w,&pixels_h);\n    width.store(pixels_w); height.store(pixels_h);\n    const char* icon_path=std::getenv(\"KYOEI_WINDOW_ICON\");\n    if (icon_path && *icon_path) {\n        wchar_t icon_wide[1024];\n        if (!gz_widen(icon_path,icon_wide,1024)) {\n            static HICON game_icon=(HICON)LoadImageW(nullptr,icon_wide,IMAGE_ICON,0,0,LR_LOADFROMFILE|LR_DEFAULTSIZE);\n            if (game_icon) {\n                SendMessageW((HWND)window_info.render_surface,WM_SETICON,ICON_BIG,(LPARAM)game_icon);\n                SendMessageW((HWND)window_info.render_surface,WM_SETICON,ICON_SMALL,(LPARAM)game_icon);\n            }\n        }\n    }\n    gz_log(\"GPU: SDL window created (%dx%d), output=%dx%d fullscreen=%u\", w,h,pixels_w,pixels_h,options.fullscreen);")
kyoei_replace_checked(KYOEI_WINDOW
  "        if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_TEXT_INPUT)"
  "        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F9 && KyoeiRasterTrace::Enabled()) { if (!event.key.repeat) KyoeiRasterTrace::Request(); continue; }\n        if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_TEXT_INPUT)")
kyoei_replace_checked(KYOEI_WINDOW
  "        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F9"
  "        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F8 && std::getenv(\"KYOEI_INPUT_TEST\")) { if (!event.key.repeat) { SDL_Event controller{}; controller.type=SDL_EVENT_GAMEPAD_BUTTON_DOWN; controller.gbutton.button=SDL_GAMEPAD_BUTTON_SOUTH; SDL_PushEvent(&controller); } continue; }\n        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F9")
set(KYOEI_WINDOW "#include \"kyoei_prompt_state.h\"\n${KYOEI_WINDOW}")
kyoei_replace_checked(KYOEI_WINDOW
  "        KyoeiMouse::Event(window,event);"
  "        if (event.type==SDL_EVENT_KEY_DOWN && (event.key.key==SDLK_ESCAPE || event.key.key==SDLK_TAB || event.key.key==SDLK_SPACE || event.key.key==SDLK_BACKSPACE)) kyoei_prompt_reset();\n        KyoeiMouse::Event(window,event);")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_window.cpp" CONTENT "${KYOEI_WINDOW}" @ONLY)
target_sources(kyoei_videoout PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/kyoei_window.cpp")
file(READ "${KYOEI_CORE}/gpu/vendor/vk_presenter.cpp" KYOEI_PRESENTER)
set(KYOEI_PRESENTER "#include \"kyoei_title_exit.h\"
#include \"kyoei_overlay_gpu.h\"\n#include \"kyoei_overlay_barriers.h\"\n#include \"kyoei_overlay.h\"\n${KYOEI_PRESENTER}")
kyoei_replace_checked(KYOEI_PRESENTER
  "    pp_pass.Create(device, swapchain.GetSurfaceFormat().format);"
  "    pp_pass.Create(device, swapchain.GetSurfaceFormat().format);\n    KyoeiOverlay::Initialize(instance,swapchain.GetSurfaceFormat().format,swapchain.GetImageCount());")
kyoei_replace_checked(KYOEI_PRESENTER
  "    flip_scheduler.Finish();"
  "    flip_scheduler.Finish();\n    KyoeiOverlay::Shutdown();")
kyoei_replace_checked(KYOEI_PRESENTER
  "    Check(instance.GetDevice().resetFences(frame->present_done));"
  "    KyoeiOverlay::Initialize(instance,swapchain.GetSurfaceFormat().format,swapchain.GetImageCount());\n    const bool kyoei_modal=kyoei_overlay_kind()!=0 || kyoei_title_exit_state()!=0;\n    const auto kyoei_modal_revision=kyoei_overlay_revision();\n    Check(instance.GetDevice().resetFences(frame->present_done));")
set(KYOEI_OVERLAY_ORIGINAL_BARRIER [=[        vk::ImageMemoryBarrier{
            .srcAccessMask = vk::AccessFlagBits::eTransferWrite,
            .dstAccessMask = vk::AccessFlagBits::eNone,
            .oldLayout = vk::ImageLayout::eTransferDstOptimal,
            .newLayout = vk::ImageLayout::ePresentSrcKHR,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = swapchain.Image(), .subresourceRange = range}]=])
kyoei_replace_checked(KYOEI_PRESENTER "${KYOEI_OVERLAY_ORIGINAL_BARRIER}"
  "        KyoeiOverlay::BeforeDraw(swapchain.Image(),range,kyoei_modal)")
set(KYOEI_OVERLAY_PRESENT [=[    if (kyoei_modal) {
        KyoeiOverlay::Render(cmd,swapchain.ImageView(),{swapchain.GetWidth(),swapchain.GetHeight()});
        const auto to_present=KyoeiOverlay::AfterDraw(swapchain.Image(),range);
        cmd.pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,vk::PipelineStageFlagBits::eBottomOfPipe,{}, {}, {},to_present);
    }
    kyoei_overlay_presented(kyoei_modal_revision);
    SubmitInfo info{};
    info.AddWait(swapchain.GetImageAcquiredSemaphore());]=])
kyoei_replace_checked(KYOEI_PRESENTER
  "    SubmitInfo info{};\n    info.AddWait(swapchain.GetImageAcquiredSemaphore());"
  "${KYOEI_OVERLAY_PRESENT}")
set(KYOEI_PRESENTER "#include \"kyoei_raster_trace.h\"\n${KYOEI_PRESENTER}")
kyoei_replace_checked(KYOEI_PRESENTER
  "    kyoei_overlay_presented(kyoei_modal_revision);"
  "    kyoei_overlay_presented(kyoei_modal_revision);\n    KyoeiRasterTrace::Presented();\n    kyoei_prompt_presented();")
set(KYOEI_PRESENTER "#include \"kyoei_prompt_state.h\"\n${KYOEI_PRESENTER}")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_presenter.cpp" CONTENT "${KYOEI_PRESENTER}" @ONLY)
target_sources(kyoei_videoout PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/kyoei_presenter.cpp"
  src/kyoei_overlay_gpu.cpp "${KYOEI_IMGUI_DIR}/backends/imgui_impl_vulkan.cpp")
target_include_directories(kyoei_videoout PRIVATE "${KYOEI_IMGUI_DIR}" "${KYOEI_IMGUI_DIR}/backends")
set_source_files_properties(src/kyoei_overlay_gpu.cpp "${KYOEI_IMGUI_DIR}/backends/imgui_impl_vulkan.cpp"
  PROPERTIES COMPILE_DEFINITIONS IMGUI_IMPL_VULKAN_NO_PROTOTYPES=1)
set_source_files_properties(src/kyoei_overlay_gpu.cpp PROPERTIES
  COMPILE_DEFINITIONS "IMGUI_IMPL_VULKAN_NO_PROTOTYPES=1;KYOEI_GUI_FONT_DIR=\"assets/fonts\"")
target_include_directories(kyoei_videoout PRIVATE "${KYOEI_CORE}/gpu")
target_sources(kyoei PRIVATE $<TARGET_OBJECTS:kyoei_videoout>)
target_compile_features(kyoei_videoout PRIVATE cxx_std_23)
target_compile_options(kyoei_videoout PRIVATE -O2 -DNDEBUG -U_DEBUG -D_ITERATOR_DEBUG_LEVEL=0)
target_compile_definitions(kyoei_videoout PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX _CRT_SECURE_NO_WARNINGS VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
target_include_directories(kyoei_videoout PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${KYOEI_CORE}/gpu/shim" "${KYOEI_SHAD}/src" "${KYOEI_SHAD}" "${KYOEI_CORE}/src"
  "${KYOEI_SHAD}/externals/gcn/include"
  "${KYOEI_SHAD}/src/core/libraries/gnmdriver"
  "${KYOEI_GPU_HEADERS}/gz_vulkan-src/include" "${KYOEI_GPU_HEADERS}/gz_fmt-src/include"
  "${KYOEI_GPU_HEADERS}/gz_magic-src/include" "${KYOEI_GPU_HEADERS}/gz_robin-src/include"
  "${KYOEI_GPU_HEADERS}/gz_vma-src/include" "${KYOEI_GPU_HEADERS}/gz_sdl-src/include"
  "${KYOEI_GPU_HEADERS}/gz_boost-src"
  "${KYOEI_GPU_HEADERS}/gz_sdl-build/include-revision" "${KYOEI_GPU_HEADERS}/gz_xbyak-src"
  "${KYOEI_GPU_HEADERS}/gz_xxhash-src")
file(GLOB KYOEI_BOOST_INCLUDES "${KYOEI_GPU_HEADERS}/gz_boost-src/libs/*/include")
target_include_directories(kyoei_videoout SYSTEM PRIVATE ${KYOEI_BOOST_INCLUDES})
add_executable(test_kyoei_tls src/test_kyoei_tls.c src/kyoei_tls.c)
add_executable(test_kyoei_name_model src/test_kyoei_name_model.c)
add_executable(test_kyoei_names src/test_kyoei_names.c src/kyoei_names.c)
add_executable(test_kyoei_pc_names src/test_kyoei_pc_names.c src/kyoei_pc_names.c)
add_executable(test_kyoei_text_wrap src/test_kyoei_text_wrap.c src/kyoei_text_wrap.c)
target_compile_definitions(test_kyoei_text_wrap PRIVATE KYOEI_TEXT_WRAP_TEST=1)
add_executable(test_kyoei_title_exit src/test_kyoei_title_exit.c src/kyoei_title_exit.c)
target_compile_definitions(test_kyoei_title_exit PRIVATE KYOEI_TITLE_EXIT_TEST=1)
target_sources(test_kyoei_names PRIVATE src/kyoei_pc_names.c)
target_include_directories(test_kyoei_names PRIVATE src "${KYOEI_CORE}/src")
target_compile_definitions(test_kyoei_names PRIVATE KYOEI_NAMES_TEST=1 _CRT_SECURE_NO_WARNINGS)
add_executable(test_kyoei_input_mode src/test_kyoei_input_mode.c)
add_executable(test_kyoei_frame_stats src/test_kyoei_frame_stats.cpp)
target_include_directories(test_kyoei_frame_stats PRIVATE src)
target_compile_features(test_kyoei_frame_stats PRIVATE cxx_std_23)
target_compile_definitions(test_kyoei_frame_stats PRIVATE _CRT_SECURE_NO_WARNINGS)
add_executable(test_kyoei_list_model src/test_kyoei_list_model.c)
add_executable(test_kyoei_overlay src/test_kyoei_overlay.cpp src/kyoei_overlay_model.cpp
  src/kyoei_input_mode.c ${KYOEI_IMGUI_SOURCES})
target_compile_features(test_kyoei_overlay PRIVATE cxx_std_23)
target_sources(test_kyoei_overlay PRIVATE src/kyoei_pc_names.c src/kyoei_title_exit.c)
target_compile_definitions(test_kyoei_overlay PRIVATE KYOEI_TITLE_EXIT_TEST=1)
target_compile_options(test_kyoei_overlay PRIVATE -UNDEBUG)
target_compile_definitions(test_kyoei_overlay PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX _CRT_SECURE_NO_WARNINGS)
target_include_directories(test_kyoei_overlay PRIVATE src "${KYOEI_IMGUI_DIR}")
add_executable(test_kyoei_ime_overlay src/test_kyoei_ime_overlay.cpp src/kyoei_ime.c
  src/kyoei_overlay_model.cpp src/kyoei_input_mode.c)
target_compile_features(test_kyoei_ime_overlay PRIVATE cxx_std_23)
target_sources(test_kyoei_ime_overlay PRIVATE src/kyoei_pc_names.c src/kyoei_title_exit.c)
target_compile_definitions(test_kyoei_ime_overlay PRIVATE KYOEI_TITLE_EXIT_TEST=1)
target_compile_definitions(test_kyoei_ime_overlay PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX _CRT_SECURE_NO_WARNINGS)
target_include_directories(test_kyoei_ime_overlay PRIVATE src "${KYOEI_CORE}/src")
target_link_libraries(test_kyoei_ime_overlay PRIVATE kyoei_imgui user32 gdi32)
add_executable(test_kyoei_overlay_barriers src/test_kyoei_overlay_barriers.cpp)
get_target_property(KYOEI_OVERLAY_GPU_INCLUDES kyoei_videoout INCLUDE_DIRECTORIES)
add_executable(test_kyoei_mouse src/test_kyoei_mouse.cpp)
add_executable(test_kyoei_live_prompts src/test_kyoei_live_prompts.cpp src/kyoei_prompt_state.c)
target_include_directories(test_kyoei_live_prompts PRIVATE src)
target_compile_features(test_kyoei_live_prompts PRIVATE cxx_std_23)
target_compile_options(test_kyoei_live_prompts PRIVATE -UNDEBUG)
target_compile_definitions(test_kyoei_live_prompts PRIVATE _CRT_SECURE_NO_WARNINGS)
target_sources(test_kyoei_mouse PRIVATE src/kyoei_prompt_state.c)
target_include_directories(test_kyoei_mouse PRIVATE ${KYOEI_OVERLAY_GPU_INCLUDES})
target_compile_features(test_kyoei_mouse PRIVATE cxx_std_23)
target_compile_definitions(test_kyoei_mouse PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS)
target_link_libraries(test_kyoei_mouse PRIVATE "${KYOEI_CORE}/prebuilt/_deps/gz_sdl-build/SDL3-static.lib" ${KYOEI_SYSTEM_LIBRARIES})
add_executable(test_kyoei_log_filter src/test_kyoei_log_filter.c)
target_compile_definitions(test_kyoei_log_filter PRIVATE _CRT_SECURE_NO_WARNINGS)
target_include_directories(test_kyoei_overlay_barriers PRIVATE ${KYOEI_OVERLAY_GPU_INCLUDES})
target_compile_features(test_kyoei_overlay_barriers PRIVATE cxx_std_23)
target_compile_definitions(test_kyoei_overlay_barriers PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
add_executable(test_kyoei_raster_trace src/test_kyoei_raster_trace.cpp)
target_include_directories(test_kyoei_raster_trace PRIVATE src ${KYOEI_OVERLAY_GPU_INCLUDES})
target_compile_features(test_kyoei_raster_trace PRIVATE cxx_std_23)
target_compile_definitions(test_kyoei_raster_trace PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX _CRT_SECURE_NO_WARNINGS)
add_executable(test_kyoei_readback_batch src/test_kyoei_readback_batch.cpp)
target_compile_features(test_kyoei_readback_batch PRIVATE cxx_std_23)
target_compile_definitions(test_kyoei_readback_batch PRIVATE _CRT_SECURE_NO_WARNINGS)
add_executable(test_kyoei_gpu_wait src/test_kyoei_gpu_wait.c src/kyoei_gpu_wait.c)
add_executable(benchmark_kyoei_gpu_wait tools/benchmark_gpu_wait.c src/kyoei_gpu_wait.c)
target_include_directories(benchmark_kyoei_gpu_wait PRIVATE "${KYOEI_CORE}/src" src)
target_link_libraries(benchmark_kyoei_gpu_wait PRIVATE winmm)
add_executable(test_kyoei_hud_trace src/test_kyoei_hud_trace.c src/kyoei_hud_trace.c)
add_executable(test_kyoei_hud_layout src/test_kyoei_hud_layout.c)
target_include_directories(test_kyoei_hud_trace PRIVATE "${KYOEI_CORE}/src" src)
target_compile_definitions(test_kyoei_hud_trace PRIVATE _CRT_SECURE_NO_WARNINGS KYOEI_HUD_TRACE_TEST=1)
target_include_directories(test_kyoei_gpu_wait PRIVATE "${KYOEI_CORE}/src" src)
add_executable(test_kyoei_png src/test_kyoei_png.c src/kyoei_png.c)
add_executable(test_kyoei_modules src/test_kyoei_modules.c)
add_executable(test_kyoei_vmem src/test_kyoei_vmem.c)
target_include_directories(test_kyoei_vmem PRIVATE "${KYOEI_CORE}/src" "${CMAKE_CURRENT_BINARY_DIR}")
add_executable(test_kyoei_savedata src/test_kyoei_savedata.c)
target_include_directories(test_kyoei_savedata PRIVATE "${KYOEI_CORE}/src" src)
target_include_directories(test_kyoei_modules PRIVATE "${KYOEI_CORE}/src" src)
target_compile_definitions(test_kyoei_modules PRIVATE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(test_kyoei_modules PRIVATE bcrypt)
target_include_directories(test_kyoei_png PRIVATE "${KYOEI_CORE}/src" src "${KYOEI_GPU_HEADERS}/gz_miniz-src" "${KYOEI_GPU_HEADERS}/gz_miniz-build")
target_compile_definitions(test_kyoei_png PRIVATE MINIZ_STATIC_DEFINE=1)
target_link_libraries(test_kyoei_png PRIVATE "${KYOEI_CORE}/prebuilt/_deps/gz_miniz-build/miniz.lib")
target_include_directories(test_kyoei_tls PRIVATE "${KYOEI_CORE}/src" src)
file(GLOB GPU_DLLS "${KYOEI_CORE}/prebuilt/*.dll")
foreach(DLL IN LISTS GPU_DLLS)
  add_custom_command(TARGET kyoei POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${DLL}" "$<TARGET_FILE_DIR:kyoei>")
endforeach()
