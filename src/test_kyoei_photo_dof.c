/* Verify native filter parameter ownership/restoration without game assets. */
#include "kyoei_photo_dof.c"
#include <assert.h>
#include <stdio.h>
static int hud=1,active=1,calls;
static unsigned char filter_data[0xd4],camera_data[0x290],view_data[0x20],camera_manager_data[0x50];
static float sampled_focus;
int kyoei_photo_hud_hidden(void){return hud;}
int kyoei_photo_scene_active(void){return active;}
void gz_log(const char *format,...){(void)format;}
void gz_stop(int code,const char *format,...){(void)format;exit(code);}
size_t kyoei_hud_trace_relay(unsigned char *r,const void *p,const void *c){(void)r;(void)p;(void)c;abort();}
static float getf(const void *p,size_t n){float v;memcpy(&v,(const char*)p+n,4);return v;}
static ABI void spy(void *filter){++calls;sampled_focus=getf(filter,0xc4);}
static void ptr(void *p,size_t n,void *value){memcpy((char*)p+n,&value,sizeof(value));}
int main(void){
    base=VirtualAlloc(NULL,0x13425b0,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);assert(base);
    ptr(base,0x13425a8,filter_data);ptr(base,0x1336200,view_data);
    ptr(base,0x1340800,camera_manager_data);ptr(camera_manager_data,0x48,view_data);ptr(view_data,0x18,camera_data);
    ptr(filter_data,0,base+0x127bb30);original_update=spy;
    put_f(camera_data,0x280,16.f/9);put_f(camera_data,0x288,.1f);put_f(camera_data,0x28c,1000);
    put_f(filter_data,0xb0,7);put_f(filter_data,0xc4,5);
    unsigned char original[36];memcpy(original,filter_data+0xb0,36);
    kyoei_photo_dof_input(1,2,1.4f,40);update_dof(filter_data);
    assert(kyoei_photo_dof_available() && calls==1 && sampled_focus==2);
    assert(getf(filter_data,0xc8)==1.4f && getf(filter_data,0xb0)==7);
    assert(getf(filter_data,0xb4)<2 && getf(filter_data,0xb8)>2);
    assert(getf(filter_data,0xbc)==.1f && getf(filter_data,0xc0)==1000);
    kyoei_photo_dof_input(2,2,1.4f,40);update_dof(filter_data);assert(filter_data[0xa0]==0);
    hud=0;update_dof(filter_data);assert(!saved.filter && !memcmp(original,filter_data+0xb0,36));
    hud=1;active=0;kyoei_photo_dof_input(1,1,1,50);update_dof(filter_data);assert(!saved.filter);
    active=1;update_dof(filter_data);assert(saved.filter);
    ptr(base,0x1336200,NULL);kyoei_photo_dof_input(0,5,4,40);update_dof(filter_data);assert(!saved.filter);
    ptr(filter_data,0,base+0x1252008);kyoei_photo_dof_input(1,3,2,40);update_dof(filter_data);assert(!saved.filter);
    VirtualFree(base,0,MEM_RELEASE);puts("PHOTO DOF PASS: projection, exposure, enable, exit and owner scope");return 0;
}
