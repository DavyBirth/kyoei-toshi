/* Local list extension for the embedded dialog, on its own UI thread. */
#pragma once
#include <windows.h>
#include "dialog_ui.h"
#include "kyoei_list_model.h"
#include "kyoei_overlay.h"
static _Thread_local const wchar_t *const *kyoei_list_labels;
static _Thread_local unsigned kyoei_list_count, kyoei_list_selected;
static void kyoei_list_move(HWND window, uint32_t pressed) {
    kyoei_list_selected=kyoei_list_next(kyoei_list_count,kyoei_list_selected,pressed);
    InvalidateRect(window, NULL, FALSE);
}
static void kyoei_list_draw(HDC dc, const RECT *r) {
    KyoeiListView view=kyoei_list_view(kyoei_list_count,kyoei_list_selected,r->bottom);
    unsigned start=view.first;
    for (unsigned i = start; i < start+view.rows; ++i) {
        RECT row = {24, 55 + (int)(i - start) * 30, r->right - 24, 84 + (int)(i - start) * 30};
        if (i == kyoei_list_selected) {
            HBRUSH brush = CreateSolidBrush(RGB(30, 110, 165));
            FillRect(dc, &row, brush); DeleteObject(brush);
        }
        DrawTextW(dc, kyoei_list_labels[i], -1, &row, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
    }
}
int kyoei_dialog_select(const wchar_t *title, const wchar_t *const *labels,
                        unsigned count, unsigned initial) {
    if (!count || !labels || initial >= count) return -1;
    int overlay=kyoei_overlay_select(title,labels,count,initial);
    if (overlay!=INT_MIN) return overlay;
    kyoei_list_labels = labels; kyoei_list_count = count; kyoei_list_selected = initial;
    int answer = gz_dialog_show(title, 512);
    kyoei_list_labels = NULL;
    return answer == IDOK ? (int)kyoei_list_selected : -1;
}
