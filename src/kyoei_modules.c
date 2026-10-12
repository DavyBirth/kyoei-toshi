/* Offline-linked native modules, initialized when requested by the game.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_modules.h"
#include <windows.h>
#include <bcrypt.h>
#include <stdio.h>
#include <string.h>

typedef struct { char nid[32]; uintptr_t address; } Symbol;
typedef struct { uint64_t address; uint32_t size, protection; } ModuleSegment;
typedef struct {
    uint64_t size; char name[256]; int32_t id; uint32_t tls_index;
    uint64_t tls_address; uint32_t tls_initial_size, tls_size, tls_offset, tls_align;
    uint64_t init, fini, reserved1, reserved2, eh_header, eh_frame;
    uint32_t eh_header_size, eh_frame_size; ModuleSegment segments[4]; uint32_t segment_count;
} ModuleInfo;
_Static_assert(sizeof(ModuleInfo)==424 && offsetof(ModuleInfo,segments)==352, "module info ABI");
typedef struct { char path[256]; uint64_t base; uintptr_t init, fini; int started; Symbol symbols[128]; unsigned count; ModuleInfo info; } Module;
static Module modules[8];
static unsigned count;
static SRWLOCK lock = SRWLOCK_INIT;

void kyoei_modules_configure(const char *out, void *image, uint64_t size) {
    char path[1024], line[512];
    snprintf(path, sizeof(path), "%s/deferred.tsv", out);
    FILE *file = fopen(path, "r");
    if (!file) gz_stop(21, "STOP: cannot read deferred native modules");
    Module *current = NULL;
    while (fgets(line, sizeof(line), file)) {
        unsigned long long base, init, address;
        if (line[0] == 'M') {
            if (count == 8) gz_stop(21, "STOP: too many deferred modules");
            current = &modules[count++];
            if (sscanf(line, "M\t%255[^\t]\t%llx\t%llx", current->path, &base, &init) != 3 || base >= size || init >= size)
                gz_stop(21, "STOP: invalid deferred module");
            current->base = base; current->init = (uintptr_t)image + init;
            current->info.size = sizeof(ModuleInfo); current->info.id = 0x4000 + count - 1;
            current->info.init = current->init;
            const char *name = strrchr(current->path, '/'); name = name ? name+1 : current->path;
            snprintf(current->info.name, sizeof(current->info.name), "%s", name);
            char *extension = strrchr(current->info.name, '.'); if (extension) *extension=0;
        } else if (line[0] == 'F') {
            if (!current || sscanf(line, "F\t%llx", &address) != 1 || address >= size)
                gz_stop(21, "STOP: invalid deferred finalizer");
            current->fini = (uintptr_t)image + address;
            current->info.fini = current->fini;
        } else if (line[0] == 'S') {
            unsigned long long bytes, protection;
            if (!current || current->info.segment_count >= 4 ||
                sscanf(line,"S\t%llx\t%llx\t%llx",&address,&bytes,&protection)!=3 ||
                address >= size || !bytes || bytes > size-address || bytes > UINT32_MAX || protection > 7)
                gz_stop(21,"STOP: invalid module segment");
            ModuleSegment *segment = &current->info.segments[current->info.segment_count++];
            segment->address = (uintptr_t)image + address; segment->size = (uint32_t)bytes; segment->protection = (uint32_t)protection;
        } else if (line[0] == 'H') {
            unsigned long long header_size, frame, frame_size;
            if (!current || sscanf(line,"H\t%llx\t%llx\t%llx\t%llx",&address,&header_size,&frame,&frame_size)!=4 ||
                address >= size || header_size > size-address || frame >= size || frame_size > size-frame ||
                header_size > UINT32_MAX || frame_size > UINT32_MAX)
                gz_stop(21,"STOP: invalid module unwind metadata");
            current->info.eh_header=(uintptr_t)image+address; current->info.eh_header_size=(uint32_t)header_size;
            current->info.eh_frame=(uintptr_t)image+frame; current->info.eh_frame_size=(uint32_t)frame_size;
        } else if (line[0] == 'E') {
            if (!current || current->count == 128) gz_stop(21, "STOP: invalid deferred exports");
            Symbol *entry = &current->symbols[current->count++];
            if (sscanf(line, "E\t%31[^\t]\t%llx", entry->nid, &address) != 2 || address >= size)
                gz_stop(21, "STOP: invalid deferred symbol");
            entry->address = (uintptr_t)image + address;
        } else gz_stop(21, "STOP: invalid deferred table record");
    }
    fclose(file);
}
int kyoei_module_deferred(uint64_t base) {
    for (unsigned i = 0; i < count; ++i) if (modules[i].base == base) return 1;
    return 0;
}

static ABI int32_t load_module(const char *path, uint64_t args, const void *argp, uint32_t flags, const void *options, int32_t *result) {
    if (!path) return (int32_t)0x8002000e;
    if (flags || options) return (int32_t)0x80020016;
    for (unsigned i = 0; i < count; ++i) {
        Module *module = &modules[i];
        if (strcmp(path, module->path)) continue;
        AcquireSRWLockExclusive(&lock);
        int value = 0;
        if (!module->started) {
            typedef int (ABI *Init)(uint64_t, const void *, const void *);
            value = ((Init)module->init)(args, argp, options);
            if (!value) module->started = 1;
            gz_log("Kyoei module: %s native initializer returned %d", path, value);
        }
        ReleaseSRWLockExclusive(&lock);
        if (result) *result = value;
        return value ? value : (int32_t)(0x4000+i);
    }
    gz_stop(21, "STOP: native module not prepared: %s", path);
}

static ABI int32_t stop_module(int32_t handle, uint64_t args, const void *argp,
                              uint32_t flags, const void *options, int32_t *result) {
    unsigned index = (unsigned)handle - 0x4000u;
    if (flags || options) return (int32_t)0x80020016;
    AcquireSRWLockExclusive(&lock);
    if (index >= count || !modules[index].started) {
        ReleaseSRWLockExclusive(&lock); return (int32_t)0x80020003;
    }
    Module *module = &modules[index];
    if (!module->fini) {
        ReleaseSRWLockExclusive(&lock);
        gz_stop(21, "STOP: module finalizer metadata missing: %s", module->path);
    }
    typedef int32_t (ABI *Fini)(uint64_t, const void *, const void *);
    int32_t value = ((Fini)module->fini)(args, argp, options);
    if (!value) module->started = 0;
    if (result) *result = value;
    gz_log("Kyoei module: %s native finalizer returned %d; fixed linked image retained", module->path, value);
    ReleaseSRWLockExclusive(&lock);
    return value;
}
static ABI int32_t info_from_address(uint64_t address, int32_t flags, ModuleInfo *info) {
    if (!info) return (int32_t)0x8002000e;
    if (flags < 0 || flags >= 3) { memset(info,0,sizeof(*info)); return (int32_t)0x80020016; }
    /* Metadata is immutable after startup. Do not take the lifecycle lock:
     * libc queries it inside the finalizer while stop_module owns that lock. */
    for (unsigned i=0;i<count;++i) for (unsigned j=0;j<modules[i].info.segment_count;++j) {
        ModuleSegment *segment=&modules[i].info.segments[j];
        if (address >= segment->address && address-segment->address < segment->size) {
            *info=modules[i].info; return 0;
        }
    }
    return (int32_t)0x80020003;
}
static void symbol_nid(const char *name, char output[12]) {
    static const unsigned char salt[] = {0x51,0x8d,0x64,0xa6,0x35,0xde,0xd8,0xc1,0xe6,0xb0,0x39,0xb1,0xc3,0xe5,0x52,0x30};
    static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
    BCRYPT_ALG_HANDLE algorithm = NULL;
    BCRYPT_HASH_HANDLE hash = NULL;
    unsigned char digest[20], reversed[8];
    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA1_ALGORITHM, NULL, 0) ||
        BCryptCreateHash(algorithm, &hash, NULL, 0, NULL, 0, 0) ||
        BCryptHashData(hash, (PUCHAR)name, (ULONG)strlen(name), 0) ||
        BCryptHashData(hash, (PUCHAR)salt, sizeof(salt), 0) ||
        BCryptFinishHash(hash, digest, sizeof(digest), 0)) gz_stop(21, "STOP: cannot hash native symbol");
    BCryptDestroyHash(hash); BCryptCloseAlgorithmProvider(algorithm, 0);
    for (int i = 0; i < 8; ++i) reversed[i] = digest[7-i];
    unsigned bits = 0, value = 0, position = 0;
    for (int i = 0; i < 8; ++i) {
        value = (value << 8) | reversed[i]; bits += 8;
        while (bits >= 6) { bits -= 6; output[position++] = alphabet[(value >> bits)&63]; }
    }
    if (bits) output[position++] = alphabet[(value << (6-bits))&63];
    output[position] = 0;
}
/* PS4 libc passes ModuleInfoEx here (verified at libc+0x711d5),
 * rather than FreeBSD's dl_phdr_info. Match executable ELF segments. */
static ABI int32_t elf_match_address(const ModuleInfo *info, uint64_t address) {
    if (!info || info->segment_count > 4) return 0;
    for (unsigned i=0;i<info->segment_count;++i) {
        const ModuleSegment *segment=&info->segments[i];
        if ((segment->protection & 1) && segment->size > sizeof(void *) &&
            address >= segment->address && address-segment->address < segment->size-sizeof(void *))
            return 1;
    }
    return 0;
}
static ABI int32_t module_symbol(int32_t handle, const char *name, void **address) {
    if (!name || !address) return (int32_t)0x8002000e;
    unsigned index = (unsigned)handle - 0x4000u;
    if (index >= count || !modules[index].started) return (int32_t)0x80020003;
    char nid[12]; symbol_nid(name, nid);
    for (unsigned i = 0; i < modules[index].count; ++i) {
        Symbol *symbol = &modules[index].symbols[i];
        if (!strcmp(symbol->nid, nid) || !strcmp(symbol->nid, name)) {
            *address = (void *)symbol->address;
            gz_log("Kyoei dlsym: %s -> %p", name, *address);
            return 0;
        }
    }
    *address = NULL;
    return (int32_t)0x80020003;
}
uintptr_t kyoei_modules_resolve(const char *symbol) {
    if (!strcmp(symbol, "sceKernelLoadStartModule")) return (uintptr_t)load_module;
    if (!strcmp(symbol, "sceKernelStopUnloadModule")) return (uintptr_t)stop_module;
    if (!strcmp(symbol, "sceKernelGetModuleInfoFromAddr")) return (uintptr_t)info_from_address;
    if (!strcmp(symbol, "__elf_phdr_match_addr")) return (uintptr_t)elf_match_address;
    if (!strcmp(symbol, "sceKernelDlsym")) return (uintptr_t)module_symbol;
    return 0;
}
