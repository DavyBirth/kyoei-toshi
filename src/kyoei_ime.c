/* Windows text entry for the PS4 IME dialog. SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "gpu.h"
#include <windows.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <wchar.h>
#include "kyoei_input_mode.h"
#include "kyoei_name_model.h"
#include "kyoei_overlay.h"
#include "kyoei_ime.h"

typedef KyoeiImeParam ImeParam;
_Static_assert(sizeof(ImeParam) == 96, "IME parameter ABI");
_Static_assert(offsetof(ImeParam, buffer) == 40, "IME buffer ABI");
_Static_assert(sizeof(wchar_t) == 2, "Windows UTF16");
static volatile LONG status, abort_requested;
static HANDLE worker;
static HWND panel, edit;
static ImeParam param;
static wchar_t initial[2049], title[257], accepted[2049];
static uint32_t end_status;
static uint32_t previous_buttons;
#define IME_CONFIRM (WM_APP + 41)
static int done;
static int using_overlay;
static SRWLOCK result_lock=SRWLOCK_INIT;

static void finish(HWND window, uint32_t result) {
    if (result == 0) {
        GetWindowTextW(edit, accepted, (int)param.max_length + 1);
        /* Filters run on the guest caller in GetResult, not the UI thread. */
    }
    end_status = result; done = 1; DestroyWindow(window);
}
static LRESULT CALLBACK ime_proc(HWND window, UINT message, WPARAM w, LPARAM l) {
    (void)l;
    switch (message) {
    case WM_COMMAND:
        if (LOWORD(w) == IDOK) { finish(window, 0); return 0; }
        if (LOWORD(w) == IDCANCEL) { finish(window, 1); return 0; }
        break;
    case IME_CONFIRM: finish(window, (uint32_t)w); return 0;
    case WM_CLOSE: finish(window, 1); return 0;
    case WM_TIMER:
        if (InterlockedCompareExchange(&abort_requested, 0, 0)) finish(window, 2);
        return 0;
    case WM_DESTROY: done = 1; return 0;
    }
    return DefWindowProcW(window, message, w, l);
}
static DWORD WINAPI show_ime(void *unused) {
    (void)unused;
    HWND parent = (HWND)gz_gpu_window_handle();
    WNDCLASSW cls = {0}; cls.lpfnWndProc = ime_proc;
    cls.hInstance = GetModuleHandleW(NULL); cls.lpszClassName = L"KyoeiTextEntry";
    cls.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(32512));
    cls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&cls);
    RECT r = {0}; GetWindowRect(parent, &r);
    HWND window = CreateWindowExW(WS_EX_DLGMODALFRAME, cls.lpszClassName,
        title[0] ? title : L"Nome do personagem", WS_CAPTION | WS_SYSMENU | WS_POPUP,
        r.left + 200, r.top + 170, 560, 180, parent, NULL, cls.hInstance, NULL);
    if (!window) { end_status = 2; InterlockedExchange(&status, 2); return 0; }
    edit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", initial,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
        20, 20, 510, 32, window, (HMENU)100, cls.hInstance, NULL);
    HWND ok = CreateWindowW(L"BUTTON", L"Confirmar", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
        270, 75, 120, 32, window, (HMENU)IDOK, cls.hInstance, NULL);
    HWND cancel = CreateWindowW(L"BUTTON", L"Cancelar", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        410, 75, 120, 32, window, (HMENU)IDCANCEL, cls.hInstance, NULL);
    if (!edit || !ok || !cancel) { DestroyWindow(window); end_status = 2; InterlockedExchange(&status, 2); return 0; }
    SendMessageW(edit, EM_SETLIMITTEXT, param.max_length, 0);
    HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    SendMessageW(edit, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessageW(ok, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessageW(cancel, WM_SETFONT, (WPARAM)font, TRUE);
    InterlockedExchangePointer((void *volatile *)&panel, window);
    SetTimer(window, 1, 50, NULL); ShowWindow(window, SW_SHOW);
    SetForegroundWindow(window); SetFocus(edit); SendMessageW(edit, EM_SETSEL, 0, -1);
    MSG message;
    while (!done && GetMessageW(&message, NULL, 0, 0) > 0) {
        if (message.message == WM_KEYDOWN || message.message == WM_CHAR) kyoei_input_keyboard();
        if (message.message == WM_KEYDOWN && message.wParam == VK_RETURN) { finish(window, 0); continue; }
        if (message.message == WM_KEYDOWN && message.wParam == VK_ESCAPE) { finish(window, 1); continue; }
        if (!IsDialogMessageW(window, &message)) { TranslateMessage(&message); DispatchMessageW(&message); }
    }
    InterlockedExchangePointer((void *volatile *)&panel, NULL);
    gz_log("Kyoei IME: entry finished, result=%u", end_status);
    InterlockedExchange(&status, 2);
    return 0;
}
/* Consume gamepad presses while the text window is active. */
int kyoei_ime_input(uint32_t buttons) {
    uint32_t pressed = buttons & ~previous_buttons; previous_buttons = buttons;
    if (InterlockedCompareExchange(&status, 0, 0) != 1) return 0;
    HWND window = (HWND)InterlockedCompareExchangePointer((void *volatile *)&panel, NULL, NULL);
    if (window && (pressed & (8 | 0x2000))) PostMessageW(window, IME_CONFIRM, 0, 0);
    else if (window && (pressed & 0x4000)) PostMessageW(window, IME_CONFIRM, 1, 0);
    return 1;
}
static ABI int32_t ime_init(const ImeParam *p, const void *extended) {
    if (!p) return (int32_t)0x80bc0031;
    if (InterlockedCompareExchange(&status, 0, 0)) return (int32_t)0x80bc0001;
    if (extended) return (int32_t)0x80bc001c;
    if (p->user != 1) return (int32_t)0x80bc0010;
    if (!p->buffer) return (int32_t)0x80bc0017;
    if (!p->max_length || p->max_length > 2048) return (int32_t)0x80bc0016;
    gz_log("Kyoei IME: requested type=%u option=0x%x filter=%p", p->type, p->option, (void *)p->filter);
    if (p->type || p->option) return (int32_t)0x80bc0015;
    param = *p;
    size_t n = 0;
    while (n < p->max_length && p->buffer[n]) { initial[n] = p->buffer[n]; ++n; }
    if (p->buffer[n]) return (int32_t)0x80bc0006;
    initial[n] = 0; title[0] = 0;
    if (!kyoei_name_valid((const uint16_t *)initial,n)) return (int32_t)0x80bc0006;
    if (p->title) { n = 0; while (n < 256 && p->title[n]) { title[n] = p->title[n]; ++n; } title[n] = 0; }
    done = 0; end_status = 2; InterlockedExchange(&abort_requested, 0);
    using_overlay=kyoei_overlay_begin_name(title,(const uint16_t *)initial,wcslen(initial),p->max_length);
    InterlockedExchange(&status, 1);
    if (using_overlay) {
        worker=NULL;
        gz_log("Kyoei IME: in-game entry opened, max_length=%u",p->max_length);
        return 0;
    }
    worker = CreateThread(NULL, 0, show_ime, NULL, 0, NULL);
    if (!worker) { InterlockedExchange(&status, 0); return (int32_t)0x80bc0003; }
    gz_log("Kyoei IME: native text entry opened, max_length=%u", p->max_length);
    return 0;
}
static ABI int32_t ime_status(void) {
    AcquireSRWLockExclusive(&result_lock);
    if (using_overlay && InterlockedCompareExchange(&status,0,0)==1 &&
        kyoei_overlay_name_result((uint16_t *)accepted,2049,&end_status)) InterlockedExchange(&status,2);
    int32_t value=InterlockedCompareExchange(&status,0,0);
    ReleaseSRWLockExclusive(&result_lock);
    return value;
}
static ABI int32_t ime_result(uint32_t *result) {
    if (!result) return (int32_t)0x80bc0031;
    if (ime_status() != 2) return (int32_t)0x80bc0106;
    if (end_status == 0) {
        uint32_t n = (uint32_t)wcslen(accepted);
        if (n > param.max_length || !kyoei_name_valid((const uint16_t *)accepted,n)) return (int32_t)0x80bc0006;
        if (param.filter) {
            uint16_t filtered[2049] = {0}; uint32_t length = param.max_length;
            int32_t error = param.filter(filtered, &length, (uint16_t *)accepted, n);
            if (error) return error;
            if (length > param.max_length) return (int32_t)0x80bc0006;
            if (!kyoei_name_valid(filtered,length)) return (int32_t)0x80bc0006;
            memcpy(param.buffer, filtered, length * 2); param.buffer[length] = 0;
        } else memcpy(param.buffer, accepted, (n + 1) * 2);
    }
    memset(result, 0, 16); result[0] = end_status; return 0;
}
static ABI int32_t ime_abort(void) {
    if (ime_status() != 1) return (int32_t)0x80bc0105;
    if (using_overlay) { kyoei_overlay_abort_name(); return 0; }
    InterlockedExchange(&abort_requested, 1); return 0;
}
static ABI int32_t ime_term(void) {
    if (ime_status() != 2) return (int32_t)0x80bc0106;
    if (worker) { WaitForSingleObject(worker, INFINITE); CloseHandle(worker); worker = NULL; }
    if (using_overlay) { kyoei_overlay_release_name(); using_overlay=0; }
    memset(&param, 0, sizeof(param)); InterlockedExchange(&status, 0); return 0;
}
uintptr_t kyoei_ime_resolve(const char *symbol) {
    if (!strcmp(symbol, "sceImeDialogInit")) return (uintptr_t)ime_init;
    if (!strcmp(symbol, "sceImeDialogGetStatus")) return (uintptr_t)ime_status;
    if (!strcmp(symbol, "sceImeDialogGetResult")) return (uintptr_t)ime_result;
    if (!strcmp(symbol, "sceImeDialogAbort")) return (uintptr_t)ime_abort;
    if (!strcmp(symbol, "sceImeDialogTerm")) return (uintptr_t)ime_term;
    return 0;
}
