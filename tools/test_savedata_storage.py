"""Write, unmount and reopen a synthetic save using the actual folder backend."""
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[1]
build = root/'build/test-save-storage'
build.mkdir(exist_ok=True)
test = r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gz.h"
static const char *user_root;
static char mounted[1024];
const char *gz_user_dir(void) { return user_root; }
void gz_log(const char *format,...) { (void)format; }
void gz_stop(int code,const char *format,...) { (void)format; exit(code); }
int gz_widen(const char *text,wchar_t *out,int count) { return MultiByteToWideChar(CP_UTF8,0,text,-1,out,count) ? 0 : -1; }
int gz_make_dirs(const char *path) {
 char copy[1024]; snprintf(copy,sizeof(copy),"%s",path);
 for (char *p=copy+3; *p; ++p) if (*p=='\\' || *p=='/') { char old=*p; *p=0; CreateDirectoryA(copy,NULL); *p=old; }
 return CreateDirectoryA(copy,NULL) || GetLastError()==ERROR_ALREADY_EXISTS ? 0 : -1;
}
int gz_file_mount(const char *guest,const char *host) { (void)guest; snprintf(mounted,sizeof(mounted),"%s",host); return 0; }
void gz_file_unmount(const char *guest) { (void)guest; mounted[0]=0; }
uintptr_t gz_lookup(const GzExport *table,size_t count,const char *library,const char *symbol) {
 for (size_t i=0;i<count;++i) if (!strcmp(table[i].symbol,symbol) && (!table[i].library || !strcmp(table[i].library,library))) return (uintptr_t)table[i].function;
 return 0;
}
#define Mount1 BackendMount1
#include "kyoei_savedata_backend.c"
#undef Mount1
#include "kyoei_savedata.c"
uintptr_t kyoei_shared_resolve(const char *library,const char *symbol,int data) { return data ? 0 : gz_savedata_resolve(library,symbol); }
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d failed\n",__LINE__); return 1; } } while(0)
int main(int argc,char **argv) {
 CHECK(argc==2); user_root=argv[1]; snprintf(title_id,sizeof(title_id),"CUSA03562");
 DirName directory={"TEST-SAVE"}; Mount2 input={0}; MountResult result;
 input.user=1; input.directory=&directory; input.blocks=96; input.mode=6;
 CHECK(mount2(&input,&result)==ERR_NOT_INITIALIZED);
 CHECK(!save_initialize(NULL));
 float progress=1; CHECK(!save_get_progress(&progress) && progress==0);
 CHECK(save_get_progress(NULL)==ERR_PARAMETER); CHECK(!save_clear_progress());
 input.mode=34; CHECK(!mount2(&input,&result));
 char path[1200]; snprintf(path,sizeof(path),"%s\\payload.bin",mounted);
 FILE *file=fopen(path,"wb"); CHECK(file && fwrite("roundtrip",1,9,file)==9); fclose(file);
 Param param={0}; strcpy(param.title,"Synthetic test"); param.user_param=123;
 CHECK(!save_set_param(&result.point,0,&param,sizeof(param)));
 CHECK(!save_umount(&result.point)); CHECK(!mounted[0]);
 CHECK(!save_terminate()); CHECK(!save_initialize(NULL)); input.mode=2;
 CHECK(!mount2(&input,&result) && !result.status);
 snprintf(path,sizeof(path),"%s\\payload.bin",mounted); char readback[10]={0};
 file=fopen(path,"rb"); CHECK(file && fread(readback,1,9,file)==9); fclose(file); CHECK(!strcmp(readback,"roundtrip"));
 int slot=slot_of(&result.point); Param persisted={0}; read_param(slots[slot].meta,&persisted);
 CHECK(!strcmp(persisted.title,"Synthetic test") && persisted.user_param==123);
 CHECK(!save_umount(&result.point)); directory=(DirName){"../escape"};
 CHECK(mount2(&input,&result)==ERR_PARAMETER); CHECK(!save_terminate());
 CHECK(save_clear_progress()==ERR_NOT_INITIALIZED);
 puts("SAVE STORAGE PASS: Mount2, real payload/metadata persisted across terminate/remount, invalid paths, backup progress");
 return 0;
}
'''
(build/'test.c').write_text(test)
subprocess.run(['clang','-O2','-D_CRT_SECURE_NO_WARNINGS','-I'+str(root/'src'),
                '-I'+str(root/'third_party/godzilla-core/src'),'-I'+str(root/'build'),
                str(build/'test.c'),'-o',str(build/'test.exe')],check=True)
subprocess.run([str(build/'test.exe'),str(build/'user')],check=True)
