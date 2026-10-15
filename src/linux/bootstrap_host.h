/* POSIX host memory for the first Linux loader stage. No guest runtime yet.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#define MAX_PATH 4096
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef int LONG;
typedef pthread_mutex_t CRITICAL_SECTION;
typedef struct { size_t dwPageSize; } SYSTEM_INFO;
static void InitializeCriticalSection(CRITICAL_SECTION *p) { if(pthread_mutex_init(p,NULL))abort(); }
static void EnterCriticalSection(CRITICAL_SECTION *p) { if(pthread_mutex_lock(p))abort(); }
static void LeaveCriticalSection(CRITICAL_SECTION *p) { if(pthread_mutex_unlock(p))abort(); }
static void GetSystemInfo(SYSTEM_INFO *p) { p->dwPageSize=(size_t)sysconf(_SC_PAGESIZE); }
#define GetCurrentProcess() 0
#define TerminateProcess(p, code) _Exit(code)
#define FlushInstructionCache(p, address, size) __builtin___clear_cache((char*)(address),(char*)(address)+(size))
#define MEM_RESERVE 1
#define MEM_COMMIT 2
#define MEM_RELEASE 4
#define PAGE_NOACCESS PROT_NONE
#define PAGE_READONLY PROT_READ
#define PAGE_READWRITE (PROT_READ|PROT_WRITE)
#define PAGE_EXECUTE_READ (PROT_READ|PROT_EXEC)
#define PAGE_EXECUTE_READWRITE (PROT_READ|PROT_WRITE|PROT_EXEC)
static struct {void *address;size_t size;} allocations[8];
static void *VirtualAlloc(void *address,size_t size,unsigned allocation,int protection) {
    (void)allocation;
    unsigned slot=0;while(slot<8&&allocations[slot].address)++slot;
    if(slot==8)return NULL;
    int flags=MAP_PRIVATE|MAP_ANONYMOUS;
    if(address)flags|=MAP_FIXED_NOREPLACE;
    void *p=mmap(address,size,protection,flags,-1,0);
    if(p==MAP_FAILED)return NULL;
    if(address&&p!=address){munmap(p,size);return NULL;}
    allocations[slot].address=p;allocations[slot].size=size;return p;
}
static int VirtualFree(void *address,size_t size,unsigned allocation) {
    (void)size;(void)allocation;
    for(unsigned i=0;i<8;++i)if(allocations[i].address==address){
        if(munmap(address,allocations[i].size))return 0;
        allocations[i].address=NULL;return 1;
    }
    return 0;
}
static int VirtualProtect(void *address,size_t size,DWORD protection,DWORD *old) {
    *old=0;return !mprotect(address,size,(int)protection);
}
