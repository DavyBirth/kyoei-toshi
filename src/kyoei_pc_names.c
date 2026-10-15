/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "kyoei_pc_names.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#include <windows.h>
#endif
typedef struct {uint16_t token[5],name[25];} Record;
static Record records[512];static uint32_t count;
static int loaded,valid;
static atomic_flag lock=ATOMIC_FLAG_INIT;
static void acquire(void){while(atomic_flag_test_and_set_explicit(&lock,memory_order_acquire)){} }
static void release(void){atomic_flag_clear_explicit(&lock,memory_order_release);}
int kyoei_pc_names_enabled(void){const char *p=getenv("KYOEI_NAME_STORE");return p&&*p;}
static void load(void){
    if(loaded)return;loaded=1;
    const char *path=getenv("KYOEI_NAME_STORE");if(!path||!*path)return;
    FILE *f=fopen(path,"rb");if(!f){valid=errno==ENOENT;return;}
    char magic[8];uint32_t n;
    if(fread(magic,1,8,f)==8&&!memcmp(magic,"KYNAMES1",8)&&fread(&n,4,1,f)==1&&n<=512&&
       fread(records,sizeof(Record),n,f)==n&&fgetc(f)==EOF){
        valid=1;count=n;
        for(uint32_t i=0;i<n;++i)if(records[i].token[4]||records[i].name[24]||!records[i].name[0])valid=0;
    }
    fclose(f);
}
int kyoei_pc_name_lookup(const uint16_t *token,size_t length,uint16_t output[25]){
    if(!token||length!=4||!output)return 0;
    acquire();load();int found=0;
    if(valid)for(uint32_t i=0;i<count;++i)if(!memcmp(token,records[i].token,8)){
        memcpy(output,records[i].name,50);found=1;break;
    }
    release();return found;
}
int kyoei_pc_name_store(const uint16_t *name,size_t length,uint16_t token[5]){
    if(!name||!length||length>24||!token||name[length])return 0;
    acquire();load();if(!valid){release();return 0;}
    for(uint32_t i=0;i<count;++i)if(!memcmp(name,records[i].name,(length+1)*2)){
        memcpy(token,records[i].token,10);release();return 1;
    }
    if(count==512){release();return 0;}
    Record record={0};memcpy(record.name,name,(length+1)*2);
    record.token[0]='Z';record.token[1]='A'+count/(26*26);record.token[2]='A'+count/26%26;record.token[3]='A'+count%26;
    records[count]=record;uint32_t next=count+1;
    const char *path=getenv("KYOEI_NAME_STORE");char temporary[4096];
    int size=snprintf(temporary,sizeof(temporary),"%s.tmp",path);
    if(size<0||(size_t)size>=sizeof(temporary)){release();return 0;}
    FILE *f=fopen(temporary,"wb");int ok=0;
    if(f){ok=fwrite("KYNAMES1",1,8,f)==8&&fwrite(&next,4,1,f)==1&&fwrite(records,sizeof(Record),next,f)==next;
        if(fflush(f))ok=0;if(fclose(f))ok=0;}
#ifdef _WIN32
    if(ok)ok=MoveFileExA(temporary,path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
#else
    if(ok)ok=rename(temporary,path)==0;
#endif
    if(ok){count=next;memcpy(token,record.token,10);}else remove(temporary);
    release();return ok;
}
