/* Included in the adapted services unit after SaveParam/SaveResult. */
#pragma once
typedef struct { const char *title; void *icon; uint64_t icon_size; char reserved[32]; } KyoeiNewItem;
typedef struct {
    int32_t user, pad; const void *title; const char (*names)[32];
    uint32_t count, pad2; const KyoeiNewItem *new_item;
    uint32_t focus, pad3; const char *focus_name; uint32_t style; char reserved[32];
} KyoeiSaveItems;
_Static_assert(sizeof(KyoeiSaveItems) == 96, "PS4 save list ABI");
typedef struct { wchar_t **labels; char (*names)[32]; unsigned count, initial; uint32_t display; } KyoeiSaveList;
static char kyoei_save_selected[32];
int kyoei_dialog_select(const wchar_t *, const wchar_t *const *, unsigned, unsigned);
static void kyoei_save_list_free(KyoeiSaveList *list) {
    if (!list) return;
    if (list->labels) for (unsigned i = 0; i < list->count; ++i) free(list->labels[i]);
    free(list->labels); free(list->names); free(list);
}
static KyoeiSaveList *kyoei_save_list_copy(const SaveParam *p) {
    const KyoeiSaveItems *items = p->items;
    if (!items || items->count > 1024 || (items->count && !items->names) ||
        p->display < 1 || p->display > 3) return NULL;
    unsigned extra = p->display == 1 && items->new_item != NULL;
    unsigned count = items->count + extra;
    if (!count) return NULL;
    KyoeiSaveList *list = calloc(1, sizeof(*list));
    if (!list) return NULL;
    list->count = count; list->display = p->display;
    list->labels = calloc(count, sizeof(*list->labels));
    list->names = calloc(count, sizeof(*list->names));
    if (!list->labels || !list->names) goto failed;
    for (unsigned i = 0; i < count; ++i) {
        list->labels[i] = calloc(256, sizeof(wchar_t));
        if (!list->labels[i]) goto failed;
        if (i < items->count) {
            if (!memchr(items->names[i], 0, 32)) goto failed;
            memcpy(list->names[i], items->names[i], 32);
            if (!MultiByteToWideChar(CP_UTF8, 0, list->names[i], -1, list->labels[i], 256)) goto failed;
        } else wcscpy(list->labels[i], L"Criar novo save");
    }
    if (items->focus == 1 || items->focus == 3) list->initial = count - 1;
    else if (items->focus == 6 && items->focus_name)
        for (unsigned i = 0; i < items->count; ++i)
            if (!strncmp(list->names[i], items->focus_name, 32)) list->initial = i;
    return list;
failed:
    kyoei_save_list_free(list); return NULL;
}
static DWORD WINAPI kyoei_save_list_show(void *arg) {
    KyoeiSaveList *list = arg;
    const wchar_t *title = list->display == 1 ? L"Escolha onde salvar" :
        list->display == 2 ? L"Escolha o save para carregar" : L"Escolha o save para excluir";
    int selected = kyoei_dialog_select(title, (const wchar_t *const *)list->labels, list->count, list->initial);
    /* LIST returns an invalid button id even when a directory was chosen. */
    save_button = 0; save_result_code = selected >= 0 ? 0 : 1;
    memset(kyoei_save_selected, 0, sizeof(kyoei_save_selected));
    if (selected >= 0) memcpy(kyoei_save_selected, list->names[selected], 32);
    gz_log("Kyoei save list: selected=%d directory='%s' result=%u", selected, kyoei_save_selected, save_result_code);
    kyoei_save_list_free(list);
    InterlockedExchange(&save_running, 0);
    return 0;
}
