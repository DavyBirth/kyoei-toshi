/* Opt-in observation of the verified CUSA03562 command-marker consumer.
 * Original routine runs unchanged through a SysV trampoline; no text/layout
 * edits, and no per-frame logging. SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_hud_trace.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef uintptr_t (ABI *GuestMarker)(void *, const void *);
static GuestMarker original_marker;
static volatile LONG reports;

static int readable(const void *pointer, size_t size) {
    MEMORY_BASIC_INFORMATION info;
    if (!pointer || !VirtualQuery(pointer, &info, sizeof(info)) || info.State!=MEM_COMMIT ||
        (info.Protect&(PAGE_NOACCESS|PAGE_GUARD))) return 0;
    uintptr_t offset=(uintptr_t)pointer-(uintptr_t)info.BaseAddress;
    return offset<=info.RegionSize && size<=info.RegionSize-offset;
}
static void *field_pointer(const void *object, size_t offset) {
    void *value=NULL;
    if (!object) return NULL;
    if (readable((const char *)object+offset,sizeof(value)))
        memcpy(&value,(const char *)object+offset,sizeof(value));
    return value;
}
static unsigned string_characters(const void *object) {
    const unsigned char *text=field_pointer(object,0);
    unsigned count=0;
    for (size_t offset=0; offset<4096; ++offset) {
        if (!readable(text ? text+offset : NULL,1)) return UINT_MAX;
        unsigned char value=text[offset];
        if (!value) return count;
        if ((value&0xc0)!=0x80) ++count;
    }
    return UINT_MAX;
}
static float float_field(const void *object,size_t offset) {
    float value=0;
    if (object && readable((const char *)object+offset,sizeof(value)))
        memcpy(&value,(const char *)object+offset,sizeof(value));
    return value;
}
static ABI uintptr_t trace_marker(void *object,const void *text) {
    uintptr_t result=original_marker(object,text);
    if (InterlockedIncrement(&reports)<=24 && readable(object,0x280)) {
        void *plate=field_pointer(object,0x238);
        void *label=field_pointer(object,0x278);
        gz_log("Kyoei HUD trace: input_chars=%u shown_chars=%u plate_scale_x=%.4f plate_x=%.3f text_x=%.3f",
            string_characters(text),label?string_characters((char *)label+0x110):UINT_MAX,
            float_field(plate,0x80),float_field(plate,0x60),float_field(label,0x60));
        // Raw technical fields for identifying font/size. Never log source prose.
        if (label && readable((char *)label+0x98,0x78)) {
            unsigned fields[30]; memcpy(fields,(char *)label+0x98,sizeof(fields));
            for (unsigned i=0;i<30;i+=6)
                gz_log("Kyoei HUD fields: offset=0x%x %08x %08x %08x %08x %08x %08x",
                    0x98+i*4,fields[i],fields[i+1],fields[i+2],fields[i+3],fields[i+4],fields[i+5]);
        }
    }
    return result;
}

size_t kyoei_hud_trace_relay(unsigned char *relay,const void *resume,const void *callback) {
    // The verified six-byte prologue has no RIP-relative operands.
    const unsigned char prologue[]={0x55,0x48,0x89,0xe5,0x41,0x57};
    const unsigned char jump[]={0xff,0x25,0,0,0,0};
    memcpy(relay,prologue,6); memcpy(relay+6,jump,6);
    uintptr_t address=(uintptr_t)resume; memcpy(relay+12,&address,8);
    memcpy(relay+32,jump,6); address=(uintptr_t)callback; memcpy(relay+38,&address,8);
    return 46;
}

void kyoei_hud_trace_configure(void *image,uint64_t size) {
    const char *enabled=getenv("KYOEI_HUD_TRACE");
    if (!enabled || strcmp(enabled,"1")) return;
    const size_t entry=0xa6d9a0,length=0x1da;
    if (!image || size<entry+length) { gz_log("Kyoei HUD trace: image too small; skipped"); return; }
    unsigned char *code=(unsigned char *)image+entry;
    uint64_t hash=UINT64_C(14695981039346656037);
    for (size_t i=0;i<length;++i) hash=(hash^code[i])*UINT64_C(1099511628211);
    if (hash!=UINT64_C(0x28c6cdeef5969e0d)) { gz_log("Kyoei HUD trace: routine signature differs; skipped"); return; }
    unsigned char *relay=NULL;
    for (unsigned i=0;i<16 && !relay;++i)
        relay=VirtualAlloc((unsigned char *)image+0x5000000+i*0x10000,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if (!relay) { gz_log("Kyoei HUD trace: no relay allocation; skipped"); return; }
    kyoei_hud_trace_relay(relay,code+6,trace_marker);
    int64_t distance=(int64_t)(uintptr_t)(relay+32)-(int64_t)(uintptr_t)(code+5);
    DWORD prior;
    if (distance<INT32_MIN || distance>INT32_MAX || !VirtualProtect(relay,4096,PAGE_EXECUTE_READ,&prior)) {
        VirtualFree(relay,0,MEM_RELEASE); gz_log("Kyoei HUD trace: relay unavailable; skipped"); return;
    }
    if (!VirtualProtect(code,6,PAGE_EXECUTE_READWRITE,&prior)) {
        VirtualFree(relay,0,MEM_RELEASE); gz_log("Kyoei HUD trace: code protection unavailable; skipped"); return;
    }
    original_marker=(GuestMarker)relay;
    int32_t relative=(int32_t)distance;
    code[0]=0xe9; memcpy(code+1,&relative,4); code[5]=0x90;
    DWORD ignored;
    if (!VirtualProtect(code,6,prior,&ignored)) gz_stop(21,"STOP: cannot restore HUD trace code protection");
    FlushInstructionCache(GetCurrentProcess(),relay,46);
    FlushInstructionCache(GetCurrentProcess(),code,6);
    gz_log("Kyoei HUD trace: command marker verified; original behavior retained; first 24 updates only");
}
