/* Opt-in observation of the verified CUSA03562 command-marker consumer.
 * Original routine runs unchanged through a SysV trampoline; no text/layout
 * edits, and no per-frame logging. SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_hud_trace.h"
#include "kyoei_hud_layout.h"
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef uintptr_t (ABI *GuestMarker)(void *, const void *);
static GuestMarker original_marker;
static volatile LONG reports;
static SRWLOCK trace_lock=SRWLOCK_INIT;
static uint64_t last_input_hash;
static void *last_label;
static int fit_enabled,trace_enabled;
static KyoeiHudMetrics fit_metrics;
typedef void (ABI *GuestAssign)(void *,const char *);
static GuestAssign assign_text;
typedef unsigned (ABI *GuestCount)(const char *);
static GuestCount count_text;

static uint64_t code_hash(const unsigned char *bytes,size_t count) {
    uint64_t hash=UINT64_C(14695981039346656037);
    for (size_t i=0;i<count;++i) hash=(hash^bytes[i])*UINT64_C(1099511628211);
    return hash;
}
static int load_metrics(const char *path) {
    if (!path) return 0;
    FILE *file=fopen(path,"rb");
    if (!file) return 0;
    unsigned char header[8];
    int okay=fread(header,1,8,file)==8 && !memcmp(header,"KHF1\x5f\0\0\0",8) &&
        fread(&fit_metrics,1,sizeof(fit_metrics),file)==sizeof(fit_metrics) && fgetc(file)==EOF;
    fclose(file);
    if (!okay) return 0;
    for (unsigned i=0;i<95;++i)
        if (!isfinite(fit_metrics.equivalent[i]) || fit_metrics.equivalent[i]<=0 || fit_metrics.equivalent[i]>4) return 0;
    return 1;
}

uint64_t gz_memory_clamp(uintptr_t address, uint64_t size);
static int readable(const void *pointer, size_t size) {
    /* Guest heap: the runtime's region table answers without a VirtualQuery syscall
     * (this ran several times per HUD marker per frame). A GPU-watched page then faults
     * into the runtime handler exactly as a guest read would. Guest stacks (the marker's
     * text object) are not in that table: VirtualQuery, remembering the last region. */
    if (!pointer || !size) return 0;
    if ((uintptr_t)pointer >= UINT64_C(0x1000000000) && gz_memory_clamp((uintptr_t)pointer, size) == size) return 1;
    static __declspec(thread) uintptr_t known_start, known_end;
    const uintptr_t start=(uintptr_t)pointer;
    if (start>=known_start && size<=known_end-start && start<known_end) return 1;
    MEMORY_BASIC_INFORMATION info;
    if (!VirtualQuery(pointer, &info, sizeof(info)) || info.State!=MEM_COMMIT ||
        (info.Protect&(PAGE_NOACCESS|PAGE_GUARD))) return 0;
    uintptr_t offset=start-(uintptr_t)info.BaseAddress;
    if (offset>info.RegionSize || size>info.RegionSize-offset) return 0;
    known_start=(uintptr_t)info.BaseAddress; known_end=known_start+info.RegionSize;
    return 1;
}
static void *field_pointer(const void *object, size_t offset) {
    void *value=NULL;
    if (!object) return NULL;
    if (readable((const char *)object+offset,sizeof(value)))
        memcpy(&value,(const char *)object+offset,sizeof(value));
    return value;
}
static unsigned string_characters(const void *object,uint64_t *hash) {
    const unsigned char *text=field_pointer(object,0);
    unsigned bytes=0;
    if (!object || !readable((const char *)object+8,sizeof(bytes))) return UINT_MAX;
    memcpy(&bytes,(const char *)object+8,sizeof(bytes));
    if (bytes>4096 || !readable(text,(size_t)bytes+1) || text[bytes]) return UINT_MAX;
    unsigned count=0;
    uint64_t digest=UINT64_C(14695981039346656037);
    for (size_t offset=0; offset<bytes; ++offset) {
        unsigned char value=text[offset];
        if (!value) return UINT_MAX;
        if ((value&0xc0)!=0x80) ++count;
        digest=(digest^value)*UINT64_C(1099511628211);
    }
    if (hash) *hash=digest;
    return count;
}
static float float_field(const void *object,size_t offset) {
    float value=0;
    if (object && readable((const char *)object+offset,sizeof(value)))
        memcpy(&value,(const char *)object+offset,sizeof(value));
    return value;
}
static ABI unsigned measure_map_text(const char *text) {
    unsigned characters=count_text(text);
    float equivalent;
    if (!fit_enabled || characters>115) return characters;
    MEMORY_BASIC_INFORMATION info;
    if (!text || !VirtualQuery(text,&info,sizeof(info)) || info.State!=MEM_COMMIT ||
        (info.Protect&(PAGE_NOACCESS|PAGE_GUARD))) return characters;
    size_t available=info.RegionSize-((uintptr_t)text-(uintptr_t)info.BaseAddress);
    if (available>116) available=116;
    const char *end=memchr(text,0,available);
    if (!end || !kyoei_hud_measure(&fit_metrics,text,(size_t)(end-text),&equivalent)) return characters;
    // Call sites convert this integer to float; round upward to retain margin.
    return (unsigned)ceilf(equivalent);
}
static int install_map_measure(unsigned char *image,uint64_t size,unsigned char *relay) {
    if (size<0xa5f4c2 || code_hash(image+0x31cd90,0x4d)!=UINT64_C(0xb21fac6ec7e95b52) ||
        code_hash(image+0xa5bfe8,0x45)!=UINT64_C(0x100e3132baf6ec4a) ||
        code_hash(image+0xa5f499,0x29)!=UINT64_C(0xcf8178d72f0daebd)) return 0;
    const size_t sites[]={0xa5bff0,0xa5f4a5};
    int32_t branches[2];DWORD previous[2];
    for (unsigned i=0;i<2;++i) {
        int64_t distance=(int64_t)(uintptr_t)(relay+64)-(int64_t)(uintptr_t)(image+sites[i]+5);
        if (distance<INT32_MIN || distance>INT32_MAX) return 0;
        branches[i]=(int32_t)distance;
    }
    if (!VirtualProtect(image+sites[0],5,PAGE_EXECUTE_READWRITE,&previous[0])) return 0;
    if (!VirtualProtect(image+sites[1],5,PAGE_EXECUTE_READWRITE,&previous[1])) {
        DWORD ignored;VirtualProtect(image+sites[0],5,previous[0],&ignored);return 0;
    }
    count_text=(GuestCount)(image+0x31cd90);
    for (unsigned i=0;i<2;++i) {
        image[sites[i]]=0xe8;memcpy(image+sites[i]+1,&branches[i],4);
        DWORD ignored;
        if (!VirtualProtect(image+sites[i],5,previous[i],&ignored)) gz_stop(21,"STOP: cannot restore map text code protection");
        FlushInstructionCache(GetCurrentProcess(),image+sites[i],5);
    }
    return 1;
}
static ABI uintptr_t trace_marker(void *object,const void *text) {
    uintptr_t result=original_marker(object,text);
    if (fit_enabled && readable(object,0x2cc) && readable(text,12)) {
        unsigned bytes; memcpy(&bytes,(const char *)text+8,4);
        const char *source=field_pointer(text,0);
        void *plate=field_pointer(object,0x238);
        void *label=field_pointer(object,0x278);
        unsigned mirrored; memcpy(&mirrored,(const char *)object+0x2c8,4);
        KyoeiHudFit fit;
        if (bytes<=115 && readable(source,(size_t)bytes+1) && readable(plate,0x84) &&
            readable(label,0x1a0) && kyoei_hud_fit(&fit_metrics,source,bytes,mirrored!=0,&fit)) {
            // The native setter owns allocation, inline storage and termination.
            // Never copy directly into the engine's fixed-size string object.
            assign_text((char *)label+0x110,source);
            memcpy((char *)plate+0x80,&fit.scale_x,4);
            memcpy((char *)plate+0x60,&fit.plate_x,4);
            memcpy((char *)label+0x60,&fit.text_x,4);
        }
    }
    if (!trace_enabled) return result;
    if (InterlockedCompareExchange(&reports,0,0)>=24 || !readable(object,0x280)) return result;
    uint64_t input_hash=0;
    unsigned input_count=string_characters(text,&input_hash);
    void *label=field_pointer(object,0x278);
    AcquireSRWLockExclusive(&trace_lock);
    if ((reports && input_hash==last_input_hash && label==last_label) || reports>=24) {
        ReleaseSRWLockExclusive(&trace_lock); return result;
    }
    last_input_hash=input_hash; last_label=label; InterlockedIncrement(&reports);
    {
        void *plate=field_pointer(object,0x238);
        unsigned font_index=UINT_MAX;
        if (label && readable((const char *)label+0x1a8,sizeof(font_index)))
            memcpy(&font_index,(const char *)label+0x1a8,sizeof(font_index));
        gz_log("Kyoei HUD trace: input_chars=%u shown_chars=%u plate_scale_x=%.4f plate_x=%.3f text_x=%.3f text_size_19c=%.3f font_index_1a8=%u",
            input_count,label?string_characters((char *)label+0x110,NULL):UINT_MAX,
            float_field(plate,0x80),float_field(plate,0x60),float_field(label,0x60),float_field(label,0x19c),font_index);
        // Raw technical fields for identifying font/size. Never log source prose.
        if (label && readable((char *)label+0x98,0x78)) {
            unsigned fields[30]; memcpy(fields,(char *)label+0x98,sizeof(fields));
            for (unsigned i=0;i<30;i+=6)
                gz_log("Kyoei HUD fields: offset=0x%x %08x %08x %08x %08x %08x %08x",
                    0x98+i*4,fields[i],fields[i+1],fields[i+2],fields[i+3],fields[i+4],fields[i+5]);
        }
        if (label && readable((char *)label+0x188,0x20)) {
            unsigned fields[8]; memcpy(fields,(char *)label+0x188,sizeof(fields));
            gz_log("Kyoei HUD font fields: offset=0x188 %08x %08x %08x %08x %08x %08x %08x %08x",
                fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6],fields[7]);
        }
    }
    ReleaseSRWLockExclusive(&trace_lock);
    return result;
}

#ifdef KYOEI_HUD_TRACE_TEST
void kyoei_hud_test_callbacks(GuestMarker original,GuestAssign assign,const KyoeiHudMetrics *metrics) {
    original_marker=original; assign_text=assign; fit_metrics=*metrics;
    fit_enabled=1; trace_enabled=0;
}
ABI uintptr_t kyoei_hud_test_marker(void *object,const void *text) { return trace_marker(object,text); }
unsigned kyoei_hud_test_map(GuestCount original,const char *text) { count_text=original;return measure_map_text(text); }
#endif

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
    fit_enabled=0; assign_text=NULL;
    const char *enabled=getenv("KYOEI_HUD_TRACE");
    trace_enabled=enabled && !strcmp(enabled,"1");
    const char *fit=getenv("KYOEI_HUD_FIT"),*language=getenv("KYOEI_LANGUAGE");
    int want_fit=fit && !strcmp(fit,"1") && language && !strcmp(language,"en");
    if (!trace_enabled && !want_fit) return;
    const size_t entry=0xa6d9a0,length=0x1da;
    if (!image || size<entry+length) { gz_log("Kyoei HUD trace: image too small; skipped"); return; }
    unsigned char *code=(unsigned char *)image+entry;
    uint64_t hash=code_hash(code,length);
    if (hash!=UINT64_C(0x28c6cdeef5969e0d)) { gz_log("Kyoei HUD trace: routine signature differs; skipped"); return; }
    if (want_fit) {
        if (size<0x31e152 || code_hash((unsigned char *)image+0x31e090,0xc2)!=UINT64_C(0xe8b3ee3e9ee96dfc) ||
            !load_metrics(getenv("KYOEI_HUD_METRICS"))) {
            gz_log("Kyoei HUD fit: native setter or metric cache unavailable; original layout retained");
            if (!trace_enabled) return;
        } else {
            fit_enabled=1; assign_text=(GuestAssign)((unsigned char *)image+0x31e090);
        }
    }
    unsigned char *relay=NULL;
    for (unsigned i=0;i<16 && !relay;++i)
        relay=VirtualAlloc((unsigned char *)image+0x5000000+i*0x10000,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if (!relay) { gz_log("Kyoei HUD trace: no relay allocation; skipped"); return; }
    kyoei_hud_trace_relay(relay,code+6,trace_marker);
    const unsigned char absolute_jump[]={0xff,0x25,0,0,0,0};
    memcpy(relay+64,absolute_jump,6);
    uintptr_t map_callback=(uintptr_t)measure_map_text;memcpy(relay+70,&map_callback,8);
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
    FlushInstructionCache(GetCurrentProcess(),relay,78);
    FlushInstructionCache(GetCurrentProcess(),code,6);
    gz_log("Kyoei HUD trace: command marker verified; fit_preview=%d; first 24 text changes only",fit_enabled);
    if (fit_enabled) gz_log("Kyoei HUD fit: English ASCII and verified UTF8 punctuation preview; full label, conservative native advances; bounded span; visual review pending");
    if (fit_enabled) gz_log("Kyoei map fit: extent/plate calls %s; original rotation and clamps retained",
        install_map_measure(image,size,relay)?"verified and adapted":"unavailable; original path retained");
}
