/* Kyoei modal presentation API; guest ABI stays in the runtime adapters. */
/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef KYOEI_OVERLAY_H
#define KYOEI_OVERLAY_H
#include <stddef.h>
#include <stdint.h>
#include <wchar.h>
#include <limits.h>
#ifdef __cplusplus
extern "C" {
#endif
enum { KYOEI_OVERLAY_NONE, KYOEI_OVERLAY_NAME, KYOEI_OVERLAY_LIST, KYOEI_OVERLAY_MESSAGE };
enum { KYOEI_OVERLAY_KEY_LEFT, KYOEI_OVERLAY_KEY_RIGHT, KYOEI_OVERLAY_KEY_UP,
       KYOEI_OVERLAY_KEY_DOWN, KYOEI_OVERLAY_KEY_BACKSPACE, KYOEI_OVERLAY_KEY_SELECT_ALL,
       KYOEI_OVERLAY_KEY_ACCEPT, KYOEI_OVERLAY_KEY_CANCEL,
       KYOEI_OVERLAY_KEY_DELETE, KYOEI_OVERLAY_KEY_HOME, KYOEI_OVERLAY_KEY_END };
int kyoei_overlay_begin_name(const wchar_t *title,const uint16_t *text,size_t length,size_t limit);
int kyoei_overlay_name_result(uint16_t *text,size_t capacity,unsigned *result);
void kyoei_overlay_release_name(void);
void kyoei_overlay_abort_name(void);
/* INT_MIN means unavailable; the caller retains its existing native fallback. */
int kyoei_overlay_select(const wchar_t *title,const wchar_t *const *labels,unsigned count,unsigned initial);
int kyoei_overlay_message(const wchar_t *text,unsigned type);
void kyoei_overlay_cancel_dialog(void);
void kyoei_overlay_prepare_dialog(void);
void kyoei_overlay_progress(unsigned percent);
void kyoei_overlay_abort_all(void);
int kyoei_overlay_input(uint32_t buttons);
int kyoei_overlay_key(unsigned key);
int kyoei_overlay_text(const char *utf8);
void kyoei_overlay_mouse(int type,float x,float y,int button,int down);
int kyoei_overlay_kind(void);
int kyoei_overlay_wants_text(void);
int kyoei_overlay_needs_redraw(void);
uint64_t kyoei_overlay_revision(void);
void kyoei_overlay_presented(uint64_t revision);
#ifdef __cplusplus
}
struct ImDrawData;
namespace KyoeiOverlayModel {
void InitializeGui(const char *font_directory,bool headless);
void SetReady(bool ready);
void ShutdownGui();
ImDrawData *Draw(unsigned width,unsigned height,float seconds,void (*extra)()=nullptr);
}
#endif
#endif
