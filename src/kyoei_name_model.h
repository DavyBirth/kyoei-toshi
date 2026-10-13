/* Headless UTF-16 name editor and controller keyboard. SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef KYOEI_NAME_MODEL_H
#define KYOEI_NAME_MODEL_H
#include <stdint.h>
#include <stddef.h>
#include <string.h>

enum { KYOEI_NAME_EDITING, KYOEI_NAME_ACCEPTED, KYOEI_NAME_CANCELLED };
typedef struct {
    uint16_t text[2049];
    size_t length, cursor, limit;
    unsigned key, upper, result, select_all;
} KyoeiNameModel;

/* PS4 limits count UTF-16 units; never split a surrogate pair. */
static inline int kyoei_name_valid(const uint16_t *text, size_t length) {
    for (size_t i=0; i<length; ++i) {
        uint16_t c=text[i];
        if (!c) return 0;
        if (c>=0xd800 && c<=0xdbff) {
            if (++i==length || text[i]<0xdc00 || text[i]>0xdfff) return 0;
        } else if (c>=0xdc00 && c<=0xdfff) return 0;
    }
    return 1;
}
static inline int kyoei_name_begin(KyoeiNameModel *m, const uint16_t *text,
                                   size_t length, size_t limit) {
    if (!m || !text || !limit || limit>2048 || length>limit || !kyoei_name_valid(text,length)) return 0;
    memset(m,0,sizeof(*m)); memcpy(m->text,text,length*sizeof(*text));
    m->length=m->cursor=length; m->limit=limit; m->select_all=length!=0;
    return 1;
}
static inline int kyoei_name_insert(KyoeiNameModel *m, const uint16_t *text, size_t length) {
    if (!m || m->result || !text || !length || !kyoei_name_valid(text,length)) return 0;
    size_t used=m->select_all?0:m->length;
    if (length>m->limit-used) return 0;
    if (m->select_all) { m->length=m->cursor=0; m->text[0]=0; m->select_all=0; }
    memmove(m->text+m->cursor+length,m->text+m->cursor,(m->length-m->cursor+1)*sizeof(*text));
    memcpy(m->text+m->cursor,text,length*sizeof(*text));
    m->length+=length; m->cursor+=length; return 1;
}
static inline void kyoei_name_cursor(KyoeiNameModel *m, int right) {
    if (!m || m->result) return;
    if (m->select_all) { m->select_all=0; m->cursor=right?m->length:0; return; }
    if (right && m->cursor<m->length) {
        uint16_t c=m->text[m->cursor++];
        if (c>=0xd800 && c<=0xdbff) ++m->cursor;
    } else if (!right && m->cursor) {
        uint16_t c=m->text[--m->cursor];
        if (c>=0xdc00 && c<=0xdfff) --m->cursor;
    }
}
static inline void kyoei_name_backspace(KyoeiNameModel *m) {
    if (!m || m->result) return;
    if (m->select_all) { m->text[0]=0; m->cursor=m->length=0; m->select_all=0; return; }
    if (!m->cursor) return;
    size_t end=m->cursor; kyoei_name_cursor(m,0);
    memmove(m->text+m->cursor,m->text+end,(m->length-end+1)*sizeof(uint16_t));
    m->length-=end-m->cursor;
}
/* Four rows of ten keys: letters, digits, space, shift, erase, done. */
static inline void kyoei_name_move(KyoeiNameModel *m, int dx, int dy) {
    if (!m || m->result) return;
    int column=(int)(m->key%10), row=(int)(m->key/10);
    column=(column+(dx>0?1:dx<0?-1:0)+10)%10;
    row=(row+(dy>0?1:dy<0?-1:0)+4)%4;
    m->key=(unsigned)(row*10+column);
}
static inline void kyoei_name_activate(KyoeiNameModel *m) {
    if (!m || m->result) return;
    uint16_t c=0;
    if (m->key<26) c=(uint16_t)((m->upper?'A':'a')+m->key);
    else if (m->key<36) c=(uint16_t)('0'+m->key-26);
    else if (m->key==36) c=' ';
    else if (m->key==37) m->upper=!m->upper;
    else if (m->key==38) kyoei_name_backspace(m);
    else if (m->key==39) m->result=KYOEI_NAME_ACCEPTED;
    if (c) kyoei_name_insert(m,&c,1);
}
static inline void kyoei_name_buttons(KyoeiNameModel *m, uint32_t pressed) {
    if (!m || m->result) return;
    /* Cancel takes priority over simultaneous confirm. */
    if (pressed&0x4000) { m->result=KYOEI_NAME_CANCELLED; return; }
    if (pressed&8) { m->result=KYOEI_NAME_ACCEPTED; return; }
    if (pressed&0x10) kyoei_name_move(m,0,-1);
    else if (pressed&0x40) kyoei_name_move(m,0,1);
    if (pressed&0x80) kyoei_name_move(m,-1,0);
    else if (pressed&0x20) kyoei_name_move(m,1,0);
    if (pressed&0x1000) m->upper=!m->upper;
    if (pressed&0x8000) kyoei_name_backspace(m);
    if (pressed&0x400) kyoei_name_cursor(m,0);
    if (pressed&0x800) kyoei_name_cursor(m,1);
    if (pressed&0x2000) kyoei_name_activate(m);
}
#endif
