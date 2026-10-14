"""Write, unmount and reopen a synthetic save using the actual folder backend."""
from pathlib import Path
import subprocess
import tempfile

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
static int fail_metadata_write;
static BOOL WINAPI test_write_file(HANDLE file,LPCVOID data,DWORD size,LPDWORD written,LPOVERLAPPED overlap) {
 if (fail_metadata_write) {
  DWORD partial=0; WriteFile(file,data,size>8?8:size,&partial,overlap);
  *written=partial; SetLastError(ERROR_DISK_FULL); return FALSE;
 }
 return WriteFile(file,data,size,written,overlap);
}
#define WriteFile test_write_file
#define Mount1 BackendMount1
#include "kyoei_savedata_backend.c"
#undef Mount1
#undef WriteFile
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
 CHECK(save_terminate()==ERR_BUSY && initialized && mounted[0]);
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
 MountPoint malformed={"/savedata0junk"}; CHECK(save_umount(&malformed)==ERR_NOT_FOUND);
 malformed=(MountPoint){"/savedata"}; CHECK(slot_of(&malformed)==-1);
 malformed=(MountPoint){"/savedata00"}; CHECK(slot_of(&malformed)==-1);
 memset(&malformed,'0',sizeof(malformed));memcpy(malformed.data,"/savedata",9);CHECK(slot_of(&malformed)==-1);
 CHECK(slot_of(&result.point)==slot && mounted[0]);
 Param changed=persisted;strcpy(changed.title,"Must not replace original");
 fail_metadata_write=1;
 CHECK(save_set_param(&result.point,0,&changed,sizeof(changed))==ERR_INTERNAL);
 fail_metadata_write=0;
 Param after={0};CHECK(!read_param(slots[slot].meta,&after) && !memcmp(&after,&persisted,sizeof(after)));
 snprintf(path,sizeof(path),"%s\\param.bin",slots[slot].meta); wchar_t wide[1200];
 CHECK(!gz_widen(path,wide,1200));
 HANDLE held=CreateFileW(wide,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
 CHECK(held!=INVALID_HANDLE_VALUE);
 CHECK(save_set_param(&result.point,0,&changed,sizeof(changed))==ERR_INTERNAL);
 CHECK(CloseHandle(held));
 CHECK(!read_param(slots[slot].meta,&after) && !memcmp(&after,&persisted,sizeof(after)));
 snprintf(path,sizeof(path),"%s\\param.bin.tmp-*",slots[slot].meta);CHECK(!gz_widen(path,wide,1200));
 WIN32_FIND_DATAW found;HANDLE find=FindFirstFileW(wide,&found);CHECK(find==INVALID_HANDLE_VALUE);
 CHECK(!save_set_param(&result.point,0,&changed,sizeof(changed)));
 CHECK(!read_param(slots[slot].meta,&after) && !strcmp(after.title,changed.title));
 unsigned char icon_bytes[16];memset(icon_bytes,0x4a,sizeof(icon_bytes));
 Icon icon={0};icon.buffer=icon_bytes;icon.buffer_size=sizeof(icon_bytes);icon.data_size=64;
 CHECK(!save_icon(&result.point,&icon));
 snprintf(path,sizeof(path),"%s\\icon0.png",slots[slot].meta);file=fopen(path,"rb");
 unsigned char icon_copy[17]={0};CHECK(file && fread(icon_copy,1,sizeof(icon_copy),file)==16);fclose(file);
 CHECK(!memcmp(icon_copy,icon_bytes,16));
 memset(icon_bytes,0x7b,16);fail_metadata_write=1;CHECK(save_icon(&result.point,&icon)==ERR_INTERNAL);fail_metadata_write=0;
 file=fopen(path,"rb");CHECK(file && fread(icon_copy,1,sizeof(icon_copy),file)==16);fclose(file);CHECK(icon_copy[0]==0x4a && icon_copy[15]==0x4a);
 CHECK(!save_umount(&result.point));input.mode=1;CHECK(!mount2(&input,&result));
 CHECK(save_icon(&result.point,&icon)==ERR_BAD_MOUNTED);
 CHECK(!save_umount(&result.point));input.mode=2;CHECK(!mount2(&input,&result));CHECK(!save_icon(&result.point,&icon));
 CHECK(!save_umount(&result.point));
 directory=(DirName){"FAILED-SAVE"};input.mode=34;fail_metadata_write=1;
 CHECK(mount2(&input,&result)==ERR_INTERNAL && !mounted[0]);fail_metadata_write=0;
 char base[600];root(1,NULL,base,sizeof(base));snprintf(path,sizeof(path),"%s\\FAILED-SAVE",base);
 CHECK(!folder_exists(path));snprintf(path,sizeof(path),"%s\\FAILED-SAVE.sce_sys",base);CHECK(!folder_exists(path));
 CHECK(!mount2(&input,&result) && result.status==1);CHECK(!save_umount(&result.point));
 directory=(DirName){"../escape"};
 CHECK(mount2(&input,&result)==ERR_PARAMETER); CHECK(!save_terminate());
 CHECK(save_clear_progress()==ERR_NOT_INITIALIZED);
 puts("SAVE STORAGE PASS: payload/metadata remount, invalid mount points, failed/short writes and replacement preserve old metadata, no stale temps");
 return 0;
}
'''
(build/'test.c').write_text(test)
subprocess.run(['clang','-O2','-D_CRT_SECURE_NO_WARNINGS','-I'+str(root/'src'),
                '-I'+str(root/'third_party/godzilla-core/src'),'-I'+str(root/'build'),
                str(build/'test.c'),'-o',str(build/'test.exe')],check=True)
with tempfile.TemporaryDirectory(prefix='fixture-',dir=build.resolve()) as fixture:
    if not Path(fixture).resolve().is_relative_to(build.resolve()):
        raise ValueError('Synthetic save fixture outside test build directory')
    subprocess.run([str(build/'test.exe'),fixture],check=True)
