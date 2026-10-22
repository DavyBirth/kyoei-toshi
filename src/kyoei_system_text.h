/* System dialog texts (save data, errors, progress) in the launcher's language:
 * KYOEI_LANGUAGE ja = Japanese, pt* = Portuguese, anything else = English.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef KYOEI_SYSTEM_TEXT_H
#define KYOEI_SYSTEM_TEXT_H
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
enum {
    KYOEI_TEXT_ERROR, KYOEI_TEXT_SAVE_ERROR, KYOEI_TEXT_NODATA, KYOEI_TEXT_ASK_SAVE,
    KYOEI_TEXT_ASK_LOAD, KYOEI_TEXT_ASK_DELETE, KYOEI_TEXT_ASK_OVERWRITE, KYOEI_TEXT_NOSPACE,
    KYOEI_TEXT_CORRUPTED, KYOEI_TEXT_SAVED, KYOEI_TEXT_LOADED, KYOEI_TEXT_DELETED,
    KYOEI_TEXT_RESTORING, KYOEI_TEXT_SAVING, KYOEI_TEXT_LOADING, KYOEI_TEXT_DELETING,
    KYOEI_TEXT_WAIT, KYOEI_TEXT_HINT, KYOEI_TEXT_YES, KYOEI_TEXT_NO, KYOEI_TEXT_CANCEL,
    KYOEI_TEXT_COUNT
};
static inline int kyoei_text_language(void) {
    const char *language = getenv("KYOEI_LANGUAGE");
    if (language && !strcmp(language, "ja")) return 1;
    if (language && !strncmp(language, "pt", 2)) return 2;
    return 0;
}
/* Format strings take the error code as their only argument. */
static inline const wchar_t *kyoei_system_text(int id) {
    static const wchar_t *const table[KYOEI_TEXT_COUNT][3] = {
        {L"An error occurred.\n\nCode: 0x%08X", L"エラーが発生しました。\n\nエラーコード: 0x%08X", L"Ocorreu um erro.\n\nCódigo: 0x%08X"},
        {L"A saved data error occurred.\n\nCode: 0x%08X", L"セーブデータのエラーが発生しました。\n\nエラーコード: 0x%08X", L"Erro nos dados salvos.\n\nCódigo: 0x%08X"},
        {L"There is no saved data.", L"セーブデータがありません。", L"Não há dados salvos."},
        {L"Save to this slot?", L"このセーブデータにセーブしますか？", L"Salvar neste slot?"},
        {L"Load this saved data?", L"このセーブデータをロードしますか？", L"Carregar este save?"},
        {L"Delete this saved data?", L"このセーブデータを削除しますか？", L"Excluir este save?"},
        {L"Overwrite this saved data?", L"このセーブデータに上書きしますか？", L"Substituir este save?"},
        {L"There is not enough free space to save.", L"空き容量が不足しています。", L"Não há espaço livre suficiente."},
        {L"The saved data is corrupted.", L"セーブデータが壊れています。", L"Os dados salvos estão corrompidos."},
        {L"Saving completed.", L"セーブが完了しました。", L"Dados salvos."},
        {L"Loading completed.", L"ロードが完了しました。", L"Dados carregados."},
        {L"Deletion completed.", L"削除が完了しました。", L"Dados excluídos."},
        {L"Restoring saved data...", L"セーブデータを復元しています…", L"Restaurando dados salvos..."},
        {L"Saving...", L"セーブしています…", L"Salvando..."},
        {L"Loading...", L"ロードしています…", L"Carregando..."},
        {L"Deleting...", L"削除しています…", L"Excluindo..."},
        {L"Please wait...", L"しばらくお待ちください…", L"Aguarde..."},
        {L"X / Enter: confirm     Arrows: choose     Circle / Esc: back", L"×/Enter: 決定     矢印: 選択     ○/Esc: 戻る", L"X / Enter: confirmar     Setas: escolher     Círculo / Esc: voltar"},
        {L"Yes", L"はい", L"Sim"},
        {L"No", L"いいえ", L"Não"},
        {L"Cancel", L"キャンセル", L"Cancelar"},
    };
    return id >= 0 && id < KYOEI_TEXT_COUNT ? table[id][kyoei_text_language()] : L"";
}
#endif
