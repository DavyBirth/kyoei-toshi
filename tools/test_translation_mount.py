"""Use the real guest file backend with synthetic original/translated files."""
import os
from pathlib import Path
import subprocess
ROOT=Path(__file__).resolve().parents[1]
BUILD=ROOT/'build/test-translation-mount'
HARNESS=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gz.h"
void gz_log(const char *format,...) { (void)format; }
void gz_stop(int code,const char *format,...) { (void)format; exit(code); }
int32_t* gz_errno(void) { static int32_t error; return &error; }
uintptr_t gz_lookup(const GzExport* table,size_t count,const char* library,const char* symbol) {
 (void)library; for(size_t i=0;i<count;++i)if(!strcmp(table[i].symbol,symbol))return(uintptr_t)table[i].function;return 0;
}
#include "kyoei_files.c"
#define CHECK(x) do { if(!(x)){fprintf(stderr,"line %d failed\n",__LINE__);return 1;} } while(0)
int main(int argc,char** argv) {
 CHECK(argc==5); gz_file_configure(argv[1],argv[2]);
 for(int i=0;i<3;++i) {
   const char* paths[]={"/app0/DAT/common.psarc","/hostapp/DAT/common.psarc","DAT/common.psarc"};
   int fd=(int)do_open(paths[i],0,0); CHECK(fd>=3);char bytes[32]={0};
   CHECK(do_read(fd,bytes,sizeof(bytes))==(int64_t)strlen(argv[3]));CHECK(!strcmp(bytes,argv[3]));CHECK(!do_close(fd));
   CHECK(do_open(paths[i],1,0)==-E_ROFS);
 }
 int fd=(int)do_open("/app0/DAT/other.psarc",0,0);CHECK(fd>=3);
 char bytes[32]={0};CHECK(do_read(fd,bytes,sizeof(bytes))==5);CHECK(!strcmp(bytes,"OTHER"));CHECK(!do_close(fd));
 char host[1024];CHECK(!gz_file_translate("/app0/DAT/common.psarc-extra",host,sizeof(host)));
 CHECK(strstr(host,"dump\\DAT\\common.psarc-extra"));
 CHECK(gz_file_translate("/app0/../escape",host,sizeof(host))==E_ACCES);
 CHECK(!strcmp(gz_user_dir(),argv[2]));
 puts("TRANSLATION MOUNT PASS: selected archive, other original files, shared save root, read-only and path checks");return 0;
}
'''

def main():
    BUILD.mkdir(parents=True,exist_ok=True);dump=BUILD/'dump/DAT';dump.mkdir(parents=True,exist_ok=True)
    (dump/'common.psarc').write_text('ORIGINAL');(dump/'other.psarc').write_text('OTHER')
    translated=BUILD/'translated.psarc';translated.write_text('TRANSLATED')
    source=BUILD/'test.c';source.write_text(HARNESS,encoding='utf8');binary=BUILD/'test.exe'
    subprocess.run(['clang','-O2','-D_CRT_SECURE_NO_WARNINGS','-I'+str(ROOT/'src'),
                    '-I'+str(ROOT/'third_party/godzilla-core/src'),'-I'+str(ROOT/'build'),str(source),'-o',str(binary)],check=True)
    env=dict(os.environ,KYOEI_TRANSLATION_ARCHIVE=str(translated),GZ_SYSTEM_FONT='',GZ_SYSTEM_FONT_JP='')
    for language in ('ja','en','pt-BR'):
        expected='ORIGINAL' if language=='ja' else 'TRANSLATED'
        subprocess.run([str(binary),str(BUILD/'dump'),str(BUILD/'user'),expected,'unused'],env=dict(env,KYOEI_LANGUAGE=language),check=True)
    if (dump/'common.psarc').read_text()!='ORIGINAL':raise RuntimeError('Original modified')

if __name__=='__main__':main()
