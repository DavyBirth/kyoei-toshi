/* Offline-linked native modules, initialized when requested by the game.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_modules.h"
#include <windows.h>
#include <bcrypt.h>
#include <stdio.h>
#include <string.h>

typedef struct { char nid[32]; uintptr_t address; } Symbol;
typedef struct { char path[256]; uint64_t base; uintptr_t init, fini; int started; Symbol symbols[128]; unsigned count; } Module;
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
        } else if (line[0] == 'F') {
            if (!current || sscanf(line, "F\t%llx", &address) != 1 || address >= size)
                gz_stop(21, "STOP: invalid deferred finalizer");
            current->fini = (uintptr_t)image + address;
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
    if (!strcmp(symbol, "sceKernelDlsym")) return (uintptr_t)module_symbol;
    return 0;
}
