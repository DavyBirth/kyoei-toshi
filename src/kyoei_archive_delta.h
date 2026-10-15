// Read-time PSARC delta (tools/psarc_delta.py): the guest sees
// new TOC + base body + appended blocks; the base file is never modified.
// Included into rt_file.c; all access goes through the file layer below.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
static int64_t read_at(HANDLE h, void *buffer, uint64_t size, int64_t offset);
typedef struct KyoeiDelta {
    uint64_t base_size, base_toc, new_toc, tail, virtual_size;
    unsigned char *table, *appended;
} KyoeiDelta;
static KyoeiDelta kyoei_delta;
static int kyoei_delta_state; /* 0 unchecked, 1 active, -1 unavailable */

static int kyoei_delta_target(const char *guest) {
    return guest && (!strcmp(guest,"/app0/DAT/common.psarc") || !strcmp(guest,"/hostapp/DAT/common.psarc"));
}
static uint64_t kyoei_delta_hash(const unsigned char *bytes, size_t count) {
    uint64_t hash=UINT64_C(14695981039346656037);
    for (size_t i=0;i<count;++i) hash=(hash^bytes[i])*UINT64_C(1099511628211);
    return hash;
}
/* Loads and validates the delta against the opened base once. Any mismatch
 * keeps the original archive, so a stale delta can never corrupt reads. */
static const KyoeiDelta *kyoei_delta_for(const char *guest, HANDLE base) {
    if (!kyoei_delta_target(guest)) return NULL;
    if (kyoei_delta_state) return kyoei_delta_state>0 ? &kyoei_delta : NULL;
    kyoei_delta_state=-1;
    const char *path=getenv("KYOEI_ARCHIVE_DELTA");
    wchar_t wide[1024];
    if (!path || !*path || gz_widen(path,wide,1024)) return NULL;
    HANDLE h=CreateFileW(wide,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (h==INVALID_HANDLE_VALUE) { gz_log("Kyoei delta: missing %s; original archive kept",path); return NULL; }
    LARGE_INTEGER s; unsigned char head[48]; uint64_t f[6]; LARGE_INTEGER base_size;
    unsigned char *data=NULL, *header=NULL;
    int ok=GetFileSizeEx(h,&s) && s.QuadPart>=48 && s.QuadPart<256*1024*1024 &&
        read_at(h,head,48,0)==48 && !memcmp(head,"KYDELTA1",8) && GetFileSizeEx(base,&base_size);
    if (ok) {
        memcpy(f,head+8,40);
        ok=(uint64_t)base_size.QuadPart==f[0] && f[1]>=32 && f[1]<=f[0] && f[2]>=32 &&
           48+f[2]+f[3]==(uint64_t)s.QuadPart && f[1]<128*1024*1024;
    }
    if (ok) ok=(data=malloc(f[2]+f[3]))!=NULL && (header=malloc(f[1]))!=NULL &&
        read_at(h,data,f[2]+f[3],48)==(int64_t)(f[2]+f[3]) && read_at(base,header,f[1],0)==(int64_t)f[1] &&
        kyoei_delta_hash(header,f[1])==f[4];
    CloseHandle(h); free(header);
    if (!ok) { free(data); gz_log("Kyoei delta: %s does not match this archive; original kept",path); return NULL; }
    kyoei_delta=(KyoeiDelta){.base_size=f[0],.base_toc=f[1],.new_toc=f[2],.tail=f[3],
        .virtual_size=f[2]+(f[0]-f[1])+f[3],.table=data,.appended=data+f[2]};
    kyoei_delta_state=1;
    gz_log("Kyoei delta: %s applied (%llu TOC + %llu appended bytes)",path,
        (unsigned long long)f[2],(unsigned long long)f[3]);
    return &kyoei_delta;
}
static int64_t kyoei_delta_read(const KyoeiDelta *d, HANDLE h, void *buffer, uint64_t size, int64_t offset) {
    if (!d) return read_at(h,buffer,size,offset);
    if (offset<0) return -E_INVAL;
    uint64_t at=(uint64_t)offset, done=0, body=d->base_size-d->base_toc;
    while (done<size && at<d->virtual_size) {
        uint64_t want=size-done; unsigned char *out=(unsigned char *)buffer+done; int64_t n;
        if (at<d->new_toc) {
            n=(int64_t)(want<d->new_toc-at?want:d->new_toc-at);
#ifdef GZ_GPU
            gz_gpu_prepare_write(out,(size_t)n);
#endif
            memcpy(out,d->table+at,(size_t)n);
        } else if (at<d->new_toc+body) {
            uint64_t left=d->new_toc+body-at;
            n=read_at(h,out,want<left?want:left,(int64_t)(d->base_toc+at-d->new_toc));
            if (n<=0) return done?(int64_t)done:n;
        } else {
            uint64_t from=at-d->new_toc-body;
            n=(int64_t)(want<d->tail-from?want:d->tail-from);
#ifdef GZ_GPU
            gz_gpu_prepare_write(out,(size_t)n);
#endif
            memcpy(out,d->appended+from,(size_t)n);
        }
        done+=(uint64_t)n; at+=(uint64_t)n;
    }
    return (int64_t)done;
}
/* stat() may precede open(): load the delta from a temporary base handle. */
static void kyoei_delta_stat(const char *guest, const wchar_t *wide, GuestStat *out) {
    if (!kyoei_delta_target(guest)) return;
    if (!kyoei_delta_state) {
        HANDLE h=CreateFileW(wide,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
        if (h!=INVALID_HANDLE_VALUE) { kyoei_delta_for(guest,h); CloseHandle(h); }
    }
    if (kyoei_delta_state>0) { out->size=(int64_t)kyoei_delta.virtual_size; out->blocks=(out->size+511)/512; }
}
