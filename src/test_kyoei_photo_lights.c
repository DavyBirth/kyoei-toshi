/* Verify the real observer preserves outputs and the five-byte relay preserves flags. */
#include "kyoei_photo_lights.c"
#include <assert.h>
#include <stdio.h>
static int calls,hud=1,active=1;
static int pose_ready=1,created,updated;
static float camera_forward[3]={0,0,1};
int kyoei_photo_scene_light_move(float right,float up,float forward,float xyz[3]){
    (void)right;(void)up;if(!pose_ready)return 0;
    for(unsigned i=0;i<3;++i)xyz[i]=camera_forward[i]*forward;return 1;
}
int kyoei_photo_hud_hidden(void){return hud;}
int kyoei_photo_scene_active(void){return active;}
int kyoei_photo_scene_light_position(float xyz[3]){
    if(!pose_ready)return 0;xyz[0]=10;xyz[1]=20;xyz[2]=30;return 1;
}
void gz_log(const char *format,...){(void)format;}
void gz_stop(int code,const char *format,...){(void)format;exit(code);}
static void put_pointer(void *p,size_t at,void *v){memcpy((char*)p+at,&v,sizeof(v));}
static ABI uint32_t spy(void *sys,uint32_t types,uint32_t views,void **out,uint32_t cap){
    (void)sys;assert(types==4 && views==1 && cap==4);++calls;out[0]=(void*)0x1234;return 1;
}
static ABI void cache_spy(void *sys){(void)sys;++calls;}
static void *selection_system;
static unsigned char selected_point[0x270],selected_spot[0x290];
static unsigned char native_points[KYOEI_PHOTO_LIGHT_COUNT][0x270];
static unsigned char native_spots[KYOEI_PHOTO_LIGHT_COUNT][0x290];
static int created_spots,updated_spots;
#define native_point native_points[0]
static float expected_x=10,expected_y=20,expected_z=30,expected_red=8;
static ABI void *factory_spy(void){
    assert(created<KYOEI_PHOTO_LIGHT_COUNT);void *point=native_points[created++];memset(point,0,0x270);
    put_pointer(point,0,base+0x1254ed0);return point;
}
static ABI void update_spy(void *point){
    if(point==native_point){assert(scalar_at(point,0x80)==expected_x && scalar_at(point,0x84)==expected_y && scalar_at(point,0x88)==expected_z);assert(scalar_at(point,0x230)==expected_red);}
    else assert(point==native_points[1] || point==native_points[2]);float range=12;
    memcpy((char*)point+0x22c,&range,4);++updated;
}
static ABI void *spot_factory_spy(void){
    assert(created_spots<KYOEI_PHOTO_LIGHT_COUNT);void *point=native_spots[created_spots++];
    memset(point,0,0x290);put_pointer(point,0,base+0x1254fc8);return point;
}
static ABI void spot_update_spy(void *point){
    assert(point==native_spots[0]);
    const float dx=scalar_at(point,0x250),dy=scalar_at(point,0x254),dz=scalar_at(point,0x258);
    assert(fabsf(dx*dx+dy*dy+dz*dz-1)<.00001f && dy<0);
    assert(fabsf(scalar_at(point,0x260)-.78539816f)<.00001f && scalar_at(point,0x264)<scalar_at(point,0x260));
    float range=20;memcpy((char*)point+0x22c,&range,4);++updated_spots;
}
static ABI uint32_t selection_spy(void *sys,uint32_t types,uint32_t views,const void *origin,
                                  const void *camera,const void *filter,void **out,uint32_t capacity,uint32_t viewport){
    assert(sys==selection_system && types==0x0c && views==0x81);
    assert(origin==(void*)0x1110 && camera==(void*)0x2220 && filter==(void*)0x3330);
    assert(capacity==3 && viewport==7);++calls;
    out[0]=selected_point;out[1]=selected_spot;return 2;
}
int main(void){
    base=VirtualAlloc(NULL,0x1354430,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);assert(base);
    unsigned char system[0x68]={0};put_pointer(base,0x1354420,system);put_pointer(system,0,base+0x1259ba0);
    original_collect=spy;void *output[4]={0};
    assert(collect_lights(system,4,1,output,4)==1 && calls==1 && output[0]==(void*)0x1234);
    put_pointer(system,0,base+0x1254b48);
    assert(collect_lights(system,4,1,output,4)==1 && calls==2 && output[0]==(void*)0x1234);
    hud=0;assert(collect_lights(system,4,1,output,4)==1 && calls==3);
    active=0;assert(collect_lights(NULL,4,1,output,4)==1 && calls==4);
    original_cache_update=cache_spy;
    LightEntry entries[2]={{(void*)0x1234,2,1},{NULL,3,1}};
    put_pointer(system,0x58,entries);uint32_t n=2;memcpy(system+0x60,&n,4);memcpy(system+0x64,&n,4);
    unsigned char before[sizeof(system)];memcpy(before,system,sizeof(system));
    update_light_cache(system);assert(calls==5 && !memcmp(before,system,sizeof(system)));
    assert(entries[0].light==(void*)0x1234 && entries[1].light==NULL);
    uint32_t point_type=2,spot_type=3;
    memcpy(selected_point+0x224,&point_type,4);memcpy(selected_spot+0x224,&spot_type,4);
    original_select=selection_spy;selection_system=system;hud=active=1;
    void *selected[3]={NULL,NULL,(void*)0x4440};
    unsigned char point_before[sizeof(selected_point)],spot_before[sizeof(selected_spot)];
    memcpy(point_before,selected_point,sizeof(selected_point));memcpy(spot_before,selected_spot,sizeof(selected_spot));
    assert(select_lights(system,0x0c,0x81,(void*)0x1110,(void*)0x2220,(void*)0x3330,selected,3,7)==2);
    assert(calls==6 && selected[0]==selected_point && selected[1]==selected_spot && selected[2]==(void*)0x4440);
    assert(!memcmp(before,system,sizeof(system)) && !memcmp(point_before,selected_point,sizeof(selected_point)) && !memcmp(spot_before,selected_spot,sizeof(selected_spot)));
    hud=0;
    assert(select_lights(system,0x0c,0x81,(void*)0x1110,(void*)0x2220,(void*)0x3330,selected,3,7)==2 && calls==7);
    selection_system=NULL;
    assert(select_lights(NULL,0x0c,0x81,(void*)0x1110,(void*)0x2220,(void*)0x3330,selected,3,7)==2 && calls==8);
    source_proof=1;create_point=factory_spy;update_point=update_spy;
    hud=active=0;update_owned_point(system,0);assert(!created);
    hud=active=1;pose_ready=0;update_owned_point(system,0);assert(!created);
    pose_ready=1;update_owned_point(system,0);
    assert(created==1 && updated==1 && owned_sources[0].was_active && scalar_at(native_point,0x22c)==12);
    uint32_t enabled;memcpy(&enabled,native_point+0x220,4);assert(enabled==1);
    update_owned_point(system,0);assert(created==1 && updated==1);
    active=0;update_owned_point(system,0);memcpy(&enabled,native_point+0x220,4);assert(!enabled && !owned_sources[0].was_active);
    active=1;update_owned_point(system,0);assert(created==1 && updated==2 && owned_sources[0].was_active);
    hud=0;update_owned_point(system,0);memcpy(&enabled,native_point+0x220,4);assert(!enabled);
    source_controls=1;hud=1;expected_x=14;expected_y=22;expected_z=27;expected_red=.5f;
    kyoei_photo_lights_input(0,1,.5f,1,2,4,2,-3,0,0,-15,45);update_owned_point(system,0);
    assert(created==1 && updated==3);
    expected_x=15;kyoei_photo_lights_input(0,1,.5f,1,2,5,2,-3,0,0,-15,45);update_owned_point(system,0);
    assert(created==1 && updated==4);
    kyoei_photo_lights_input(0,1,NAN,1,2,0,0,0,0,0,-15,45);update_owned_point(system,0);assert(updated==4);
    kyoei_photo_lights_input(0,1,.5f,1,2,51,0,0,0,0,-15,45);update_owned_point(system,0);assert(updated==4);
    kyoei_photo_lights_input(0,0,1,1,1,0,0,0,0,0,-15,45);update_owned_point(system,0);
    memcpy(&enabled,native_point+0x220,4);assert(!enabled);
    assert(!memcmp(before,system,sizeof(system)) && !memcmp(point_before,selected_point,sizeof(selected_point)) && !memcmp(spot_before,selected_spot,sizeof(selected_spot)));
    kyoei_photo_lights_input(1,1,1,.1f,.1f,-2,0,0,0,0,-15,45);
    kyoei_photo_lights_input(2,1,.1f,.1f,2,2,1,0,0,0,-15,45);
    update_owned_point(system,1);update_owned_point(system,2);
    assert(created==3 && updated==6);
    assert(owned_sources[0].point!=owned_sources[1].point && owned_sources[1].point!=owned_sources[2].point);
    assert(scalar_at(native_points[1],0x80)==8 && scalar_at(native_points[2],0x80)==12);
    unsigned char second_before[0x270],third_before[0x270];
    memcpy(second_before,native_points[1],sizeof(second_before));memcpy(third_before,native_points[2],sizeof(third_before));
    expected_x=10;expected_y=20;expected_z=30;expected_red=1;
    kyoei_photo_lights_input(0,1,1,1,1,0,0,0,0,0,-15,45);update_owned_point(system,0);
    assert(created==3 && updated==7 && !memcmp(second_before,native_points[1],sizeof(second_before)) && !memcmp(third_before,native_points[2],sizeof(third_before)));
    kyoei_photo_lights_input(KYOEI_PHOTO_LIGHT_COUNT,1,1,1,1,0,0,0,0,0,-15,45);
    hud=0;for(unsigned i=0;i<KYOEI_PHOTO_LIGHT_COUNT;++i){update_owned_point(system,i);memcpy(&enabled,native_points[i]+0x220,4);assert(!enabled);}
    hud=1;for(unsigned i=0;i<KYOEI_PHOTO_LIGHT_COUNT;++i)update_owned_point(system,i);
    assert(created==3 && updated==10);
    create_spot=spot_factory_spy;update_spot=spot_update_spy;
    kyoei_photo_lights_input(1,1,1,.1f,.1f,-2,0,0,1,0,-15,45);update_owned_point(system,1);
    assert(created_spots==1 && updated_spots==1 && owned_sources[1].point==native_spots[0]);
    memcpy(&enabled,native_points[1]+0x220,4);assert(!enabled);
    kyoei_photo_lights_input(1,1,1,.1f,.1f,-2,0,0,0,0,-15,45);update_owned_point(system,1);
    assert(created==3 && updated==11 && owned_sources[1].point==native_points[1]);
    memcpy(&enabled,native_spots[0]+0x220,4);assert(!enabled);
    kyoei_photo_lights_input(1,0,1,.1f,.1f,-2,0,0,1,0,-15,45);update_owned_point(system,1);
    kyoei_photo_lights_input(1,1,1,.1f,.1f,-2,0,0,1,0,-15,45);update_owned_point(system,1);
    assert(created_spots==1 && updated_spots==2 && owned_sources[1].point==native_spots[0]);
    // A newly enabled spot follows the camera, then stays fixed as it turns.
    hud=0;update_owned_point(system,1);hud=1;
    camera_forward[0]=-1;camera_forward[2]=0;update_owned_point(system,1);
    assert(scalar_at(native_spots[0],0x250)<-.9f && fabsf(scalar_at(native_spots[0],0x258))<.00001f);
    camera_forward[0]=0;camera_forward[2]=1;
    kyoei_photo_lights_input(1,1,1,.1f,.1f,-2,0,0,1,90,-15,45);update_owned_point(system,1);
    assert(fabsf(scalar_at(native_spots[0],0x250))<.00001f && scalar_at(native_spots[0],0x258)>.9f);
    unsigned char replacement_system[0x68]={0};put_pointer(replacement_system,0,base+0x1254b48);
    put_pointer(base,0x1354420,replacement_system);hud=1;update_owned_point(replacement_system,0);
    assert(source_failed && created==3 && updated==11);
    source_proof=0;
    VirtualFree(base,0,MEM_RELEASE);
    unsigned char *memory=VirtualAlloc(NULL,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);assert(memory);
    const unsigned char function[]={0x31,0xc0,0x45,0x85,0xc0,0x74,0x06,0xb8,77,0,0,0,0xc3,0xb8,33,0,0,0,0xc3};
    memcpy(memory,function,sizeof(function));passthrough(memory+64,memory,5);
    const unsigned char prologue[]={0x55,0x48,0x89,0xe5,0x41,0x57,0xb8,42,0,0,0,0x41,0x5f,0x5d,0xc3};
    memcpy(memory+32,prologue,sizeof(prologue));passthrough(memory+128,memory+32,6);
    DWORD protection;assert(VirtualProtect(memory,4096,PAGE_EXECUTE_READ,&protection));
    assert(FlushInstructionCache(GetCurrentProcess(),memory,160));
    Collect trampoline=(Collect)(memory+64);
    assert(trampoline(NULL,0,0,NULL,0)==33);assert(trampoline(NULL,0,0,NULL,1)==77);
    trampoline=(Collect)(memory+128);assert(trampoline(NULL,0,0,NULL,0)==42);
    VirtualFree(memory,0,MEM_RELEASE);
    /* Exercise the real displaced selector prologue with nine SysV arguments:
     * the three stack arguments must survive the two relay jumps as well. */
    memory=VirtualAlloc(NULL,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);assert(memory);
    const unsigned char selector_body[]={0x55,0x48,0x89,0xe5,0x41,0x57,0x41,0x5f,0x5d};
    memcpy(memory,selector_body,sizeof(selector_body));absolute_jump(memory+sizeof(selector_body),selection_spy);
    passthrough(memory+64,memory,6);
    assert(VirtualProtect(memory,4096,PAGE_EXECUTE_READ,&protection));
    assert(FlushInstructionCache(GetCurrentProcess(),memory,96));
    SelectLights selection_relay=(SelectLights)(memory+64);
    assert(selection_relay(NULL,0x0c,0x81,(void*)0x1110,(void*)0x2220,(void*)0x3330,selected,3,7)==2 && calls==9);
    assert(selected[0]==selected_point && selected[1]==selected_spot && selected[2]==(void*)0x4440);
    VirtualFree(memory,0,MEM_RELEASE);
    puts("PHOTO LIGHT OBSERVER PASS: native outputs unchanged, selection forwards all nine arguments, invalid pointers ignored, relay preserves branch flags");
}
