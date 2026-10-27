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
# Image uploads may read a resident guest view while holding the original VMA
# lock. Keep generic copies on physical backing and preserve the fixed core.
file(READ "${KYOEI_CORE}/src/rt_memory.c" KYOEI_MEMORY)
set(KYOEI_MEMORY "#include \"kyoei_upload_source.h\"\n${KYOEI_MEMORY}")
kyoei_replace_checked(KYOEI_MEMORY
  "int gz_memory_copy(uintptr_t address, void *buffer, uint64_t size, int write) {"
  "static int kyoei_memory_copy_impl(uintptr_t address, void *buffer, uint64_t size, int write, int upload_alias) {")
kyoei_replace_checked(KYOEI_MEMORY
  "            else memcpy(out, physical, n);"
  "            else memcpy(out, kyoei_upload_source(address, physical, n, upload_alias), n);")
set(KYOEI_MEMORY_COPY_ENTRIES [=[int gz_memory_copy(uintptr_t address, void *buffer, uint64_t size, int write) {
    return kyoei_memory_copy_impl(address, buffer, size, write, 0);
}
/* GPU command thread only, after excluding GPU-modified image source data. */
int gz_memory_copy_upload(uintptr_t address, void *buffer, uint64_t size) {
    static _Thread_local int enabled = -1;
    if (enabled < 0) {
        const char *option = getenv("KYOEI_UPLOAD_ALIAS");
        enabled = option && option[0] == '1';
    }
    static _Thread_local int timed = -1;
    if (timed < 0) { const char *t = getenv("KYOEI_UPLOAD_TIMING"); timed = t && t[0] == '1'; }
    if (!timed) return kyoei_memory_copy_impl(address, buffer, size, 0, enabled);
    LARGE_INTEGER a, b, f; QueryPerformanceCounter(&a);
    int r = kyoei_memory_copy_impl(address, buffer, size, 0, enabled);
    QueryPerformanceCounter(&b); QueryPerformanceFrequency(&f);
    double ms = (double)(b.QuadPart - a.QuadPart) * 1000.0 / (double)f.QuadPart;
    if (ms >= 2.0) gz_log("Kyoei upload copy: bytes=%llu ms=%.2f MBps=%.0f dst=%p", (unsigned long long)size, ms, size / 1048576.0 / (ms / 1000.0), buffer);
    return r;
}
void gz_memory_gpu_protect]=])
kyoei_replace_checked(KYOEI_MEMORY "void gz_memory_gpu_protect" "${KYOEI_MEMORY_COPY_ENTRIES}")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_memory.c" CONTENT "${KYOEI_MEMORY}" @ONLY)
list(REMOVE_ITEM CORE_FILES "${KYOEI_CORE}/src/rt_memory.c")
list(APPEND CORE_FILES "${CMAKE_CURRENT_BINARY_DIR}/kyoei_memory.c")
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
# Host dialog hints describe physical American buttons; guest masks stay native.
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
  "    if (p->mode == 3) {\n        const uint32_t *system = p->system;\n        if (!system || p->display < 1 || p->display > 3) { free(work); return (int32_t)0x80B8000F; }\n        const int action = p->display - 1; /* save, load, delete */\n        int text;\n        work->type = 0; /* OK only, except the questions */\n        switch (system[0]) {\n        case 1: text = KYOEI_TEXT_NODATA; break;\n        case 2: work->type = 1; text = KYOEI_TEXT_ASK_SAVE + action; break;\n        case 3: work->type = 1; text = KYOEI_TEXT_ASK_OVERWRITE; break;\n        case 4: case 8: text = KYOEI_TEXT_NOSPACE; break;\n        case 6: text = KYOEI_TEXT_CORRUPTED; break;\n        case 7: text = KYOEI_TEXT_SAVED + action; break;\n        default:\n            gz_log(\"Runtime: SaveDataDialog system message %u not supported yet\", system[0]);\n            free(work); return (int32_t)0x80B8000F;\n        }\n        wcscpy(work->text, kyoei_system_text(text));\n    } else if (p->mode == 2) {")
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
# System dialog texts follow the launcher's language (Godzilla's are Portuguese).
kyoei_replace_checked(KYOEI_SERVICES
  "static ABI int32_t error_open(const ErrorParam *p) {"
  "#include \"kyoei_system_text.h\"\nstatic ABI int32_t error_open(const ErrorParam *p) {")
kyoei_replace_checked(KYOEI_SERVICES
  "L\"Ocorreu um erro.\\n\\nCódigo: 0x%08X\""
  "kyoei_system_text(KYOEI_TEXT_ERROR)")
kyoei_replace_checked(KYOEI_SERVICES
  "L\"Erro nos dados salvos.\\n\\nCódigo: 0x%08X\""
  "kyoei_system_text(KYOEI_TEXT_SAVE_ERROR)")
kyoei_replace_checked(KYOEI_SERVICES
  "p->progress->system == 2 ? L\"Restaurando dados salvos...\" :\n                p->progress->system == 0 ? L\"\" : p->display == 1 ? L\"Salvando...\" :\n                p->display == 2 ? L\"Carregando...\" : L\"Excluindo...\";"
  "p->progress->system == 2 ? kyoei_system_text(KYOEI_TEXT_RESTORING) :\n                p->progress->system == 0 ? L\"\" : kyoei_system_text(KYOEI_TEXT_SAVING + p->display - 1);")
kyoei_replace_checked(KYOEI_DIALOG_UI
  "(i ? L\"Não\" : L\"Sim\") : (i ? L\"Cancelar\" : L\"OK\")"
  "kyoei_system_text(i ? KYOEI_TEXT_NO : KYOEI_TEXT_YES) : (i ? kyoei_system_text(KYOEI_TEXT_CANCEL) : L\"OK\")")
kyoei_replace_checked(KYOEI_DIALOG_UI
  "L\"Aguarde...\" : L\"X / Enter: confirmar     Setas: escolher     Círculo / Esc: voltar\""
  "kyoei_system_text(KYOEI_TEXT_WAIT) : kyoei_system_text(KYOEI_TEXT_HINT)")
set(KYOEI_DIALOG_UI "#include \"kyoei_system_text.h\"\n${KYOEI_DIALOG_UI}")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_services.c" CONTENT "${KYOEI_SERVICES}" @ONLY)
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_dialog_ui.c" CONTENT "${KYOEI_DIALOG_UI}" @ONLY)
list(APPEND CORE_FILES "${CMAKE_CURRENT_BINARY_DIR}/kyoei_services.c" "${CMAKE_CURRENT_BINARY_DIR}/kyoei_dialog_ui.c")
file(READ "${KYOEI_CORE}/src/loader.c" KYOEI_LOADER)
# The guest's own thread priorities never reach Windows; keep the game ahead of
# background apps (browser, OBS) competing for the GPU command thread's core.
kyoei_replace_checked(KYOEI_LOADER
  "    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);"
  "    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);\n    { const char *priority=getenv(\"KYOEI_PRIORITY\"); if(!priority || strcmp(priority,\"normal\")) SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS); }")
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
set(KYOEI_LOADER "#include \"kyoei_photo_dof.h\"\n${KYOEI_LOADER}")
set(KYOEI_LOADER "#include \"kyoei_photo_lights.h\"\n${KYOEI_LOADER}")
# Install the opt-in photo HUD draw gate before starting guest/GPU threads.
set(KYOEI_LOADER "#include \"kyoei_photo_hud.h\"\n#include \"kyoei_photo_scene.h\"\n${KYOEI_LOADER}")
kyoei_replace_checked(KYOEI_LOADER
  "    kyoei_tls_optimize(out);"
  "    kyoei_photo_lights_configure(image, image_size);\n    kyoei_photo_dof_configure(image, image_size);\n    kyoei_photo_save_mark_configure(image, image_size);\n    kyoei_tls_optimize(out);")
kyoei_replace_checked(KYOEI_LOADER
  "    kyoei_gpu_wait_configure(image, image_size);"
  "    kyoei_gpu_wait_configure(image, image_size);\n    kyoei_photo_hud_configure(image, image_size);\n    kyoei_photo_scene_configure(image, image_size);\n    kyoei_photo_phone_configure(image, image_size);")
# The game prints ~1300 console lines when pausing; flushing each one stalls the
# frame. Guest lines stay buffered; the next runtime line flushes them.
kyoei_replace_checked(KYOEI_LOADER
  "if (log_file) { fputs(line, log_file); fputc('\\n', log_file); fflush(log_file); }"
  "if (log_file) { fputs(line, log_file); fputc('\\n', log_file); if (strncmp(line, \"Guest: \", 7)) fflush(log_file); }")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_loader.c" CONTENT "${KYOEI_LOADER}" @ONLY)
add_executable(kyoei ${CORE_FILES} "${CMAKE_CURRENT_BINARY_DIR}/kyoei_loader.c" src/kyoei_kernel.c src/kyoei_tls.c src/kyoei_modules.c src/kyoei_ime.c)
target_sources(kyoei PRIVATE src/kyoei_photo_dof.c src/kyoei_photo_hud.c src/kyoei_photo_scene.c src/kyoei_photo_phone.c src/kyoei_photo_save_mark.c)
target_sources(kyoei PRIVATE src/kyoei_photo_lights.c)
add_executable(test_kyoei_photo_lights src/test_kyoei_photo_lights.c)
target_include_directories(test_kyoei_photo_lights PRIVATE "${KYOEI_CORE}/src")
target_compile_options(test_kyoei_photo_lights PRIVATE -UNDEBUG)
target_compile_definitions(test_kyoei_photo_lights PRIVATE _CRT_SECURE_NO_WARNINGS)
add_executable(test_kyoei_photo_phone src/test_kyoei_photo_phone.c)
target_compile_options(test_kyoei_photo_phone PRIVATE -UNDEBUG)
add_executable(test_kyoei_photo_tips src/test_kyoei_photo_tips.c)
target_compile_options(test_kyoei_photo_tips PRIVATE -UNDEBUG)
target_sources(kyoei PRIVATE src/kyoei_photo_tips.c)
add_executable(test_kyoei_photo_tips_gate src/test_kyoei_photo_tips_gate.c src/kyoei_hud_trace.c)
target_include_directories(test_kyoei_photo_tips_gate PRIVATE "${KYOEI_CORE}/src")
target_compile_options(test_kyoei_photo_tips_gate PRIVATE -UNDEBUG)
target_compile_definitions(test_kyoei_photo_tips_gate PRIVATE _CRT_SECURE_NO_WARNINGS)
add_executable(test_kyoei_photo_player src/test_kyoei_photo_player.c)
target_include_directories(test_kyoei_photo_player PRIVATE "${KYOEI_CORE}/src")
target_compile_options(test_kyoei_photo_player PRIVATE -UNDEBUG)
target_compile_definitions(test_kyoei_photo_player PRIVATE _CRT_SECURE_NO_WARNINGS)
add_executable(test_kyoei_photo_dof src/test_kyoei_photo_dof.c)
target_include_directories(test_kyoei_photo_dof PRIVATE "${KYOEI_CORE}/src")
target_compile_options(test_kyoei_photo_dof PRIVATE -UNDEBUG)
target_compile_definitions(test_kyoei_photo_dof PRIVATE _CRT_SECURE_NO_WARNINGS)
add_executable(test_kyoei_photo_settings src/test_kyoei_photo_settings.cpp)
target_compile_features(test_kyoei_photo_settings PRIVATE cxx_std_17)
target_compile_options(test_kyoei_photo_settings PRIVATE -UNDEBUG)
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

# Compile adapted GPU units from the fixed snapshot locally. The
# external cache supplies dependency headers only; no sibling sources are built.
set(KYOEI_GPU_HEADERS "G:/godzilla-port-codex/build/_deps" CACHE PATH "Existing GPU dependency header cache")
target_include_directories(kyoei PRIVATE "${KYOEI_GPU_HEADERS}/gz_miniz-src" "${KYOEI_GPU_HEADERS}/gz_miniz-build")
file(READ "${KYOEI_CORE}/gpu/window.cpp" KYOEI_WINDOW)
file(READ "${KYOEI_CORE}/gpu/pad.cpp" KYOEI_PAD)
kyoei_replace_checked(KYOEI_PAD
  "{SDL_GAMEPAD_BUTTON_SOUTH,0x4000},{SDL_GAMEPAD_BUTTON_EAST,0x2000}"
  "{SDL_GAMEPAD_BUTTON_SOUTH,0x2000},{SDL_GAMEPAD_BUTTON_EAST,0x4000}")
set(KYOEI_PAD "#include \"kyoei_mouse.h\"\n#include \"kyoei_photo_controls.h\"\n${KYOEI_PAD}")
set(KYOEI_PAD "#include \"kyoei_pc_controls.h\"\n#include \"kyoei_input_mode.h\"\n${KYOEI_PAD}")
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
# Nintendo layout: the right face button (A) confirms, the bottom one (B) goes back.
kyoei_replace_checked(KYOEI_PAD "        buttons |= physical_buttons;"
  "        if (kyoei_input_confirm_on_right(kyoei_input_current_family()))
            physical_buttons = (physical_buttons & ~0x6000u) | ((physical_buttons & 0x2000u) << 1) | ((physical_buttons & 0x4000u) >> 1);
        buttons |= physical_buttons;")
kyoei_replace_checked(KYOEI_PAD
  "    if (buttons & 0x100) data.analogButtons.l2 = 255;"
  "    KyoeiPhotoControls::Apply(data,buttons);\n    if (buttons & 0x100) data.analogButtons.l2 = 255;")
set(KYOEI_PAD "#include \"kyoei_title_exit.h\"
${KYOEI_PAD}")
kyoei_replace_checked(KYOEI_PAD "    if (gz_dialog_input(buttons)) {"
  "    const int kyoei_title_consumed=kyoei_title_exit_input(buttons,data.leftStick.y);
    if (kyoei_title_exit_take_quit()) { SDL_Event quit{}; quit.type=SDL_EVENT_QUIT; SDL_PushEvent(&quit); }
    if (kyoei_title_consumed || gz_dialog_input(buttons)) {")
kyoei_replace_checked(KYOEI_PAD
  "keyboard_axis(SDL_SCANCODE_A,SDL_SCANCODE_D,data.leftStick.x)"
  "(kyoei_prompt_pause_active()?data.leftStick.x:keyboard_axis(SDL_SCANCODE_A,SDL_SCANCODE_D,data.leftStick.x))")
# PC controls: the camera turns with the mouse or a gamepad, not with I/J/K/L (Davy, 23/10).
kyoei_replace_checked(KYOEI_PAD
  "    data.rightStick = {keyboard_axis(SDL_SCANCODE_J,SDL_SCANCODE_L,data.rightStick.x),
                       keyboard_axis(SDL_SCANCODE_I,SDL_SCANCODE_K,data.rightStick.y)};"
  "    // Kyoei: no keyboard camera; the mouse and the gamepad turn it.")
# Xbox View and Nintendo Minus are the touch pad button (map on/off), as the launcher lists.
kyoei_replace_checked(KYOEI_PAD
  "{SDL_GAMEPAD_BUTTON_TOUCHPAD,0x100000}};"
  "{SDL_GAMEPAD_BUTTON_TOUCHPAD,0x100000},{SDL_GAMEPAD_BUTTON_BACK,0x100000}};")
# Touch pad fingers reach the game (slide actions); see kyoei_touch.h.
set(KYOEI_PAD "#include \"kyoei_touch.h\"
${KYOEI_PAD}")
kyoei_replace_checked(KYOEI_PAD
  "    // Kyoei: no keyboard camera; the mouse and the gamepad turn it."
  "    // Kyoei: no keyboard camera; the mouse and the gamepad turn it.
    KyoeiTouch::Fill(device, data, buttons);")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_pad.cpp" CONTENT "${KYOEI_PAD}" @ONLY)
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
set(KYOEI_WINDOW "#include \"kyoei_overlay_gpu.h\"\n${KYOEI_WINDOW}")
kyoei_replace_checked(KYOEI_WINDOW
  "    SDL_Event event;"
  "    KyoeiOverlay::UpdateTextInput(window);\n    SDL_Event event;")
set(KYOEI_WINDOW "#include \"kyoei_input_mode.h\"\n${KYOEI_WINDOW}")
# The pad family picks the button art (kyoei_live_prompts.h) and, for Nintendo, the confirm button.
set(KYOEI_WINDOW "#include <SDL3/SDL.h>
#include \"kyoei_input_mode.h\"
static unsigned kyoei_pad_family(SDL_JoystickID id) {
    switch (SDL_GetGamepadTypeForID(id)) {
    case SDL_GAMEPAD_TYPE_PS3: case SDL_GAMEPAD_TYPE_PS4: case SDL_GAMEPAD_TYPE_PS5: return KYOEI_PAD_PLAYSTATION;
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO: case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT: case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:
        return KYOEI_PAD_NINTENDO;
    default: return KYOEI_PAD_XBOX;
    }
}
${KYOEI_WINDOW}")
kyoei_replace_checked(KYOEI_WINDOW
  "        gz_pad_event(event);"
  "        if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_TEXT_INPUT) kyoei_input_keyboard();\n        else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) { kyoei_input_controller_family(kyoei_pad_family(event.gbutton.which)); kyoei_input_controller_button(); }\n        else if (event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION) { kyoei_input_controller_family(kyoei_pad_family(event.gaxis.which)); kyoei_input_controller_axis(event.gaxis.axis,event.gaxis.value); }\n        else if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST || event.type == SDL_EVENT_GAMEPAD_REMOVED) kyoei_input_release();\n        if (KyoeiOverlay::HandleEvent(event)) continue;\n        gz_pad_event(event);")
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
  "        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F8 && std::getenv(\"KYOEI_INPUT_TEST\")) { if (!event.key.repeat) { SDL_Event controller{}; controller.type=SDL_EVENT_GAMEPAD_BUTTON_DOWN; controller.gbutton.button=SDL_GAMEPAD_BUTTON_SOUTH; SDL_PushEvent(&controller); } continue; }\n        if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_TEXT_INPUT)")
set(KYOEI_WINDOW "#include \"kyoei_prompt_state.h\"\n${KYOEI_WINDOW}")
kyoei_replace_checked(KYOEI_WINDOW
  "        KyoeiMouse::Event(window,event);"
  "        if (event.type==SDL_EVENT_KEY_DOWN && (event.key.key==SDLK_ESCAPE || event.key.key==SDLK_TAB || event.key.key==SDLK_SPACE || event.key.key==SDLK_BACKSPACE)) kyoei_prompt_reset();\n        KyoeiMouse::Event(window,event);")

set(KYOEI_WINDOW "#include \"kyoei_photo.h\"\n${KYOEI_WINDOW}")
kyoei_replace_checked(KYOEI_WINDOW
  "        KyoeiMouse::Event(window,event);"
  "        if (KyoeiPhoto::HandleEvent(window,event)) continue;\n        KyoeiMouse::Event(window,event);")
kyoei_replace_checked(KYOEI_WINDOW
  "    KyoeiOverlay::UpdateTextInput(window);"
  "    KyoeiOverlay::UpdateTextInput(window);\n    KyoeiPhoto::Update(window);")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kyoei_window.cpp" CONTENT "${KYOEI_WINDOW}" @ONLY)
find_program(KYOEI_GLSLC glslc HINTS "$ENV{VULKAN_SDK}/Bin" REQUIRED)
# GPU: the GoW3 PC core (gpu-gow3/, a bbport-derived renderer) with Kyoei's pad and window
# adaptations above. The Godzilla core supplies the C runtime and the prebuilt libraries.
set(KYOEI_GOW3_PAD "${CMAKE_CURRENT_BINARY_DIR}/kyoei_pad.cpp")
set(KYOEI_GOW3_WINDOW "${CMAKE_CURRENT_BINARY_DIR}/kyoei_window.cpp")
add_subdirectory(gpu-gow3)
set(KYOEI_GOW3_PREBUILT ${KYOEI_GPU_LIBRARIES})
list(FILTER KYOEI_GOW3_PREBUILT EXCLUDE REGEX "gzgpu\.lib$|sirit\.lib$")
target_link_libraries(kyoei PRIVATE gow3_core ${KYOEI_GOW3_PREBUILT}
  ${KYOEI_SYSTEM_LIBRARIES} synchronization pdh ntdll dbghelp "$ENV{VULKAN_SDK}/Lib/vulkan-1.lib")
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
get_target_property(KYOEI_OVERLAY_GPU_INCLUDES gow3_core INCLUDE_DIRECTORIES)
add_executable(test_kyoei_prepared_draw_pool src/test_kyoei_prepared_draw_pool.cpp)
target_compile_features(test_kyoei_prepared_draw_pool PRIVATE cxx_std_23)
target_include_directories(test_kyoei_prepared_draw_pool PRIVATE gpu-gow3)
add_executable(test_kyoei_mouse src/test_kyoei_mouse.cpp)
add_executable(test_kyoei_photo src/test_kyoei_photo.cpp)
target_include_directories(test_kyoei_photo PRIVATE ${KYOEI_OVERLAY_GPU_INCLUDES})
target_compile_features(test_kyoei_photo PRIVATE cxx_std_23)
target_compile_definitions(test_kyoei_photo PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS)
target_link_libraries(test_kyoei_photo PRIVATE "${KYOEI_CORE}/prebuilt/_deps/gz_sdl-build/SDL3-static.lib" ${KYOEI_SYSTEM_LIBRARIES})
add_executable(test_kyoei_live_prompts src/test_kyoei_live_prompts.cpp src/kyoei_prompt_state.c)
target_include_directories(test_kyoei_live_prompts PRIVATE src)
target_compile_features(test_kyoei_live_prompts PRIVATE cxx_std_23)
target_compile_options(test_kyoei_live_prompts PRIVATE -UNDEBUG)
target_compile_definitions(test_kyoei_live_prompts PRIVATE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(test_kyoei_live_prompts PRIVATE "${KYOEI_CORE}/prebuilt/_deps/gz_miniz-build/miniz.lib")

target_sources(test_kyoei_mouse PRIVATE src/kyoei_prompt_state.c)
target_include_directories(test_kyoei_mouse PRIVATE ${KYOEI_OVERLAY_GPU_INCLUDES})
target_compile_features(test_kyoei_mouse PRIVATE cxx_std_23)
target_compile_definitions(test_kyoei_mouse PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS)
target_link_libraries(test_kyoei_mouse PRIVATE "${KYOEI_CORE}/prebuilt/_deps/gz_sdl-build/SDL3-static.lib" ${KYOEI_SYSTEM_LIBRARIES})
add_executable(test_kyoei_log_filter src/test_kyoei_log_filter.c)
target_compile_definitions(test_kyoei_log_filter PRIVATE _CRT_SECURE_NO_WARNINGS)
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
