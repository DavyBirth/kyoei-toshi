"""Exercise the actual adapted save dialog with a controlled UI worker."""
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[1]
source = (root / 'build/kyoei_services.c').read_text()
start = source.index('/* SaveDataDialog layouts')
end = source.index('/* ---- PlayGo:', start)
prelude = r'''
#include <windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "gz.h"
#include "dialog_ui.h"
#define DLG_SAVE 1
#define DIALOG_NOT_INITIALIZED ((int32_t)0x80B80003)
static int dialog_state[5], dialog_initialized[5];
static volatile LONG save_running, msg_running, error_running;
typedef struct { uint64_t size; char reserved[36]; uint32_t magic; } MsgBase;
typedef struct { wchar_t text[4096]; unsigned type; } MsgWork;
static int dialog_init(int n) { dialog_initialized[n]=1; dialog_state[n]=1; return 0; }
static int dialog_term(int n) { dialog_initialized[n]=0; dialog_state[n]=0; return 0; }
void gz_log(const char *fmt, ...) { (void)fmt; }
void gz_dialog_prepare(void) {}
void gz_dialog_cancel(void) {}
void gz_dialog_progress(unsigned n) { (void)n; }
static int dialog_answer=IDOK;
int gz_dialog_show(const wchar_t *text, unsigned type) { (void)text; (void)type; return dialog_answer; }
static int choice, saw_new;
int kyoei_dialog_select(const wchar_t *title, const wchar_t *const *labels, unsigned n, unsigned initial) {
 (void)title; (void)initial;
 saw_new = !wcscmp(labels[n-1], L"Criar novo save");
 return choice;
}
'''
test = r'''
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d failed\n",__LINE__); return 1; } } while(0)
int main(void) {
 SaveParam p={0}; p.size=sizeof(p); p.base.size=sizeof(MsgBase); p.mode=1; p.display=1;
 KyoeiNewItem item={0}; KyoeiSaveItems items={0}; items.new_item=&item; p.items=&items;
 char destination[40]; SaveResult result={0}; result.directory=destination;
 p.user_data=(void*)0x1234; result.param=(void*)0x5678;
 CHECK(!save_init()); choice=0; memset(destination,0x5a,sizeof(destination));
 CHECK(!save_open(&p)); CHECK(WaitForSingleObject(save_worker,2000)==WAIT_OBJECT_0);
 CHECK(save_status()==3); CHECK(!save_result(&result));
 CHECK(saw_new && result.mode==1 && result.result==0 && result.button==0);
 CHECK(destination[0]==0 && destination[32]==0x5a);
 CHECK(result.directory==destination && result.param==(void*)0x5678 && result.user_data==(void*)0x1234);
 CHECK(!save_term());
 char names[2][32]={"SAVEDATA-000","SAVEDATA-001"}; items.names=names; items.count=2;
 CHECK(!save_init()); choice=1; CHECK(!save_open(&p));
 CHECK(WaitForSingleObject(save_worker,2000)==WAIT_OBJECT_0); CHECK(!save_result(&result));
 CHECK(!strcmp(destination,"SAVEDATA-001")); CHECK(!save_term());
 CHECK(!save_init()); choice=-1; CHECK(!save_open(&p));
 CHECK(WaitForSingleObject(save_worker,2000)==WAIT_OBJECT_0); CHECK(!save_result(&result));
 CHECK(result.result==1 && result.button==0 && destination[0]==0); CHECK(!save_term());
 CHECK(!save_init()); items.count=1025; CHECK(save_open(&p)==(int32_t)0x80B8000A); CHECK(!save_term());
 uint32_t system[12]={2}; p.mode=3; p.system=system;
 CHECK(!save_init()); dialog_answer=IDNO; CHECK(!save_open(&p));
 CHECK(WaitForSingleObject(save_worker,2000)==WAIT_OBJECT_0); CHECK(!save_result(&result));
 CHECK(result.result==0 && result.button==2); CHECK(!save_term());
 CHECK(!save_init()); system[0]=99; CHECK(save_open(&p)==(int32_t)0x80B8000F); CHECK(!save_term());
 puts("SAVE LIST PASS: new/existing slots, cancellation, caller pointers, guarded result and invalid count");
 return 0;
}
'''
build = root / 'build/test-save-list'
build.mkdir(exist_ok=True)
(build/'test.c').write_text(prelude + source[start:end] + test)
subprocess.run(['clang', '-O2', '-D_CRT_SECURE_NO_WARNINGS', '-I'+str(root/'src'),
                '-I'+str(root/'third_party/godzilla-core/src'), str(build/'test.c'),
                '-o',str(build/'test.exe')],check=True)
subprocess.run([str(build/'test.exe')],check=True)
