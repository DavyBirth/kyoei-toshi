/* Save/load selection and viewport without a presentation dependency. */
#ifndef KYOEI_LIST_MODEL_H
#define KYOEI_LIST_MODEL_H
#include <stdint.h>
typedef struct { unsigned first, rows; } KyoeiListView;
static inline unsigned kyoei_list_next(unsigned count,unsigned selected,uint32_t pressed) {
    if (!count || selected>=count) return 0;
    if (pressed&(0x10|0x80)) return selected?selected-1:count-1;
    if (pressed&(0x20|0x40)) return selected+1==count?0:selected+1;
    return selected;
}
static inline KyoeiListView kyoei_list_view(unsigned count,unsigned selected,int height) {
    KyoeiListView view={0};
    if (!count || selected>=count || height<=160) return view;
    unsigned visible=(unsigned)(height-160)/30;
    if (!visible) return view;
    view.first=selected>=visible?selected-visible+1:0;
    view.rows=count-view.first;
    if (view.rows>visible) view.rows=visible;
    return view;
}
#endif
