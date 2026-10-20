"""Check physical American buttons against actual generated guest navigation."""
from pathlib import Path
import subprocess
import re
root=Path(__file__).resolve().parents[1]
source=(root/'build/kyoei_dialog_ui.c').read_text(encoding='utf-8')
pad=(root/'build/kyoei_pad.cpp').read_text(encoding='utf-8')
south=int(re.search(r'\{SDL_GAMEPAD_BUTTON_SOUTH,(0x[0-9a-f]+)\}',pad)[1],16)
east=int(re.search(r'\{SDL_GAMEPAD_BUTTON_EAST,(0x[0-9a-f]+)\}',pad)[1],16)
assert south==0x2000 and east==0x4000
start=source.index('static void navigate(')
end=source.index('static LRESULT CALLBACK',start)
prelude=r'''
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
typedef struct {const wchar_t *text; unsigned type; int selected,choices,result,done;} Dialog;
static void choose(HWND window,Dialog *d,int result) {(void)window; d->result=result; d->done=1;}
static void kyoei_list_move(HWND window,uint32_t pressed) {(void)window; (void)pressed;}
'''
test=r'''
int main(void) {
 for (int choices=1;choices<=2;++choices) for (int selected=0;selected<choices;++selected) {
  Dialog d={0}; d.choices=choices; d.selected=selected;
  navigate(NULL,&d,PHYSICAL_CROSS); if (!d.done || d.result!=selected+1) return 1;
  d.done=0; navigate(NULL,&d,PHYSICAL_CIRCLE); if (!d.done || d.result!=(choices==2?2:0)) return 2;
 }
 Dialog list={0}; list.type=512; list.choices=2; navigate(NULL,&list,0x2000);
 if (!list.done || list.result!=1) return 3;
 list.done=0; navigate(NULL,&list,0x4000); if (!list.done || list.result!=2) return 4;
 puts("DIALOG BUTTONS PASS: physical cross confirms, circle cancels, one/two buttons and save list"); return 0;
}
'''
build=root/'build/test-dialog-buttons'
build.mkdir(exist_ok=True)
(build/'test.c').write_text(f'#define PHYSICAL_CROSS {south}\n#define PHYSICAL_CIRCLE {east}\n'+prelude+source[start:end]+test)
subprocess.run(['clang','-O2',str(build/'test.c'),'-luser32','-o',str(build/'test.exe')],check=True)
subprocess.run([str(build/'test.exe')],check=True)
assert 'VK_SPACE ? 0x2000 : w == VK_ESCAPE ? 0x4000' in source
assert 'X / Enter: confirmar' in source
