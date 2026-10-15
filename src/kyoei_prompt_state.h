/* Rendered pause-page prompts supply context for keyboard page navigation.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
void kyoei_prompt_pause_seen(void);
void kyoei_prompt_status_seen(void);
void kyoei_prompt_reset(void);
void kyoei_prompt_presented(void);
int kyoei_prompt_pause_active(void);
int kyoei_prompt_pause_root(void);
#ifdef __cplusplus
}
#endif
