/* Photo preview: exact UnitSaveMark receiver filter, no save-state edits.
 * Installed before the loader TLS rewrite. SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_hud_trace.h"
#include "kyoei_photo_hud.h"
#include <windows.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef void (ABI *ModelDraw)(void *object);
static ModelDraw original_draw;
static unsigned char *base;
static int observer;
static _Atomic unsigned observations=0;
static uint64_t hash_bytes(const unsigned char *data,size_t size) {
    uint64_t hash=UINT64_C(14695981039346656037);
    for(size_t i=0;i<size;++i)hash=(hash^data[i])*UINT64_C(1099511628211);
    return hash;
}
static ABI void draw_model(void *object) {
    if(kyoei_photo_hud_hidden()) {
        uintptr_t table;memcpy(&table,object,sizeof(table));
        if(table==(uintptr_t)base+0x12d2060) {
            if(observer && atomic_load_explicit(&observations,memory_order_relaxed)<16 &&
               atomic_fetch_add_explicit(&observations,1,memory_order_relaxed)<16)
                gz_log("Photo save marker: suppressed visual UnitSaveMark object=%p table=%llx",object,(unsigned long long)table);
            return;
        }
    }
    original_draw(object);
}
void kyoei_photo_save_mark_configure(void *image,uint64_t size) {
    const char *option=getenv("KYOEI_PHOTO_SAVE_MARK");
    if(!option || strcmp(option,"1"))return;
    if(!image || size<0x12d2300 || original_draw)return;
    option=getenv("KYOEI_PHOTO_SAVE_MARK_OBSERVE");observer=option && !strcmp(option,"1");
    unsigned char *code=(unsigned char*)image+0x35b810;
    unsigned char checked[226];memcpy(checked,code,sizeof(checked));
    if(hash_bytes(checked,sizeof(checked))!=UINT64_C(0x996ccfb848565994) ||
       hash_bytes((unsigned char*)image+0xa22ab0,160)!=UINT64_C(0x39cf0cc63764e16e)) {
        gz_log("Photo save marker: model draw/type signature differs; original retained");return;
    }
    unsigned char *relay=NULL;
    for(unsigned i=0;i<16 && !relay;++i)
        relay=VirtualAlloc((unsigned char*)image+0x9000000+i*0x10000,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(!relay){gz_log("Photo save marker: no relay; disabled");return;}
    int64_t distance=(int64_t)(uintptr_t)(relay+32)-(int64_t)(uintptr_t)(code+5);DWORD previous;
    if(distance<INT32_MIN || distance>INT32_MAX){VirtualFree(relay,0,MEM_RELEASE);return;}
    size_t relay_size=kyoei_hud_trace_relay(relay,code+6,draw_model);
    if(!VirtualProtect(relay,4096,PAGE_EXECUTE_READ,&previous) ||
       !FlushInstructionCache(GetCurrentProcess(),relay,relay_size) ||
       !VirtualProtect(code,6,PAGE_EXECUTE_READWRITE,&previous)) {
        VirtualFree(relay,0,MEM_RELEASE);return;
    }
    original_draw=(ModelDraw)relay;base=image;
    int32_t relative=(int32_t)distance;code[0]=0xe9;memcpy(code+1,&relative,4);code[5]=0x90;
    DWORD ignored;
    if(!VirtualProtect(code,6,previous,&ignored) || !FlushInstructionCache(GetCurrentProcess(),code,6))
        gz_stop(21,"STOP: cannot finalize photo save marker hook protection/cache");
    gz_log("Photo save marker: exact UnitSaveMark draw filter installed at boot; normal save/update intact; preview unverified");
}
