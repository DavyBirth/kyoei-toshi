// SPDX-License-Identifier: GPL-2.0-or-later
#include "kyoei_photo_settings.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <vector>
int main(){
    using namespace KyoeiPhoto;
    std::array<uint8_t,8> pixels{12,128,240,77,50,90,130,255},original=pixels;
    GradePixels(pixels.data(),pixels.size(),false,{});assert(pixels==original);
    uint8_t rgba[]={40,100,180,71},bgra[]={180,100,40,71};
    Grade grade{.7f,1.2f,.5f,2};
    GradePixels(rgba,4,false,grade);GradePixels(bgra,4,true,grade);
    assert(rgba[0]==bgra[2] && rgba[1]==bgra[1] && rgba[2]==bgra[0] && rgba[3]==71 && bgra[3]==71);
    uint8_t gray[]={128,128,128,9};GradePixels(gray,4,false,{1,1,1,0});
    assert(gray[0]>=175 && gray[0]<=176 && gray[0]==gray[1] && gray[1]==gray[2] && gray[3]==9);
    uint8_t mono[]={30,140,210,88};GradePixels(mono,4,false,{0,1,1,1});
    assert(mono[0]==mono[1] && mono[1]==mono[2] && mono[3]==88);
    uint8_t shadows[]={0,15,30,17};GradePixels(shadows,4,false,{0,1.2f,1,0});
    assert(shadows[0]==0 && shadows[1]>0 && shadows[2]>shadows[1] && shadows[3]==17);
    for(int preset=0;preset<int(Look::Count);++preset)for(float ev:{-3.f,0.f,3.f}) {
        uint8_t p[]={0,128,255,123};GradePixels(p,4,false,{ev,2,2,preset});assert(p[3]==123);
    }
    for(float fov:{15.f,40.f,90.f,110.f})assert(std::abs(FovFromFocal(FocalLength(fov))-fov)<.001f);
    for(int i=0;i<200;++i){AdjustGrade(Field::Exposure,1);AdjustGrade(Field::Contrast,-1);AdjustGrade(Field::Speed,1);}
    assert(exposure==3 && contrast==0 && camera_speed==5);
    ResetGrade();assert(Neutral(GetGrade()) && camera_speed==1);
    for(int i=0;i<int(Look::Kaiju1954);++i)AdjustGrade(Field::Filter,1);
    assert(filter==int(Look::Kaiju1954) && grain>0 && vignette>0);
    std::vector<uint8_t> film(64*64*4,128);
    for(size_t i=3;i<film.size();i+=4)film[i]=73;
    auto repeated=film,clean=film;
    Grade film_grade=GetGrade();GradePixels(film.data(),film.size(),false,film_grade,64);
    GradePixels(repeated.data(),repeated.size(),false,film_grade,64);assert(film==repeated);
    film_grade.grain=film_grade.vignette=0;GradePixels(clean.data(),clean.size(),false,film_grade,64);
    assert(film!=clean);
    for(size_t i=0;i<film.size();i+=4)assert(film[i]==film[i+1] && film[i+1]==film[i+2] && film[i+3]==73);
    AdjustGrade(Field::Grain,1);assert(grain>.35f && filter==int(Look::Kaiju1954));
    ResetGrade();assert(Neutral(GetGrade()));
    selected=0;SelectField(-1);assert(selected==int(Field::Speed));SelectField(1);assert(selected==0);
    for(int i=0;i<200;++i){AdjustGrade(Field::FocusDistance,-1);AdjustGrade(Field::Aperture,1);}
    assert(dof_mode==1 && focus_distance==.2f && aperture==32);
    AdjustGrade(Field::DofMode,1);assert(dof_mode==2);
    ResetGrade();assert(dof_mode==0 && focus_distance==5 && aperture==2.8f);
    experimental_dof=true;SetGroup(2);SelectField(-1);assert(selected==int(Field::Aperture));SetGroup(0);
    experimental_dof=false;selected=0;
    for(bool dof:{false,true})for(bool lights_enabled:{false,true}) {
        experimental_dof=dof;experimental_lights=lights_enabled;
        for(int group=0;group<4;++group){
            if(!SetGroup(group))continue;
            const int first=selected.load();int count=0;
            for(int field=group_begin[group];field<group_end[group];++field)count+=FieldEnabled(field);
            for(int i=0;i<count;++i){SelectField(1);assert(FieldEnabled(selected.load()) && FieldGroup(selected.load())==group);}
            assert(selected==first);
            for(int i=0;i<count;++i)SelectField(-1);
            assert(selected==first);
        }
        SetGroup(0);SelectGroup(1);assert(FieldGroup(selected.load())==1);
        SelectGroup(1);assert(FieldGroup(selected.load())==(dof?2:lights_enabled?3:0));
    }
    SetGroup(1);selected=int(Field::Exposure);SelectField(1);SetGroup(0);SetGroup(1);assert(selected==int(Field::Contrast));
    experimental_lights=false;experimental_dof=false;
    AdjustGrade(Field::LightMode,1);assert(CurrentLight().mode==1);
    AdjustGrade(Field::LightColor,-1);assert(CurrentLight().color==6);
    for(int i=0;i<300;++i){AdjustGrade(Field::LightPower,1);AdjustGrade(Field::LightX,1);AdjustGrade(Field::LightY,-1);}
    assert(CurrentLight().power==16 && CurrentLight().x==50 && CurrentLight().y==-50);
    float rgb[3];LightRgb(rgb);assert(rgb[0]>rgb[1] && rgb[2]==16);
    ResetGrade();assert(CurrentLight().mode==0 && CurrentLight().color==0 && CurrentLight().power==1 && CurrentLight().x==0 && CurrentLight().y==0 && CurrentLight().z==0);
    AdjustGrade(Field::LightMode,1);AdjustGrade(Field::LightColor,1);
    AdjustGrade(Field::LightSlot,1);assert(light_slot==1 && CurrentLight().mode==0 && CurrentLight().color==0);
    AdjustGrade(Field::LightMode,1);AdjustGrade(Field::LightX,1);
    AdjustGrade(Field::LightSlot,1);assert(light_slot==2 && CurrentLight().mode==0 && CurrentLight().x==0);
    AdjustGrade(Field::LightMode,1);AdjustGrade(Field::LightPower,1);
    AdjustGrade(Field::LightSlot,1);assert(light_slot==0 && CurrentLight().mode==1 && CurrentLight().color==1 && CurrentLight().x==0);
    assert(lights[1].mode==1 && lights[1].x==.25f && lights[2].mode==1 && lights[2].power==1.25f);
    AdjustGrade(Field::LightType,1);assert(CurrentLight().type==1);
    AdjustGrade(Field::LightControl,1);assert(light_control);
    for(int i=0;i<300;++i){AdjustGrade(Field::LightPitch,1);AdjustGrade(Field::LightCone,-1);AdjustGrade(Field::LightYaw,1);}
    assert(CurrentLight().pitch==89 && CurrentLight().cone==5 && std::abs(CurrentLight().yaw.load())<=180);
    ResetGrade();for(const auto& light:lights)assert(light.mode==0 && light.color==0 && light.power==1 && light.x==0 && light.y==0 && light.z==0);
    assert(!light_control && CurrentLight().type==0 && CurrentLight().pitch==-15 && CurrentLight().cone==45);
    // Smooth anime palette must retain nearby tones instead of posterizing them.
    float ar=.10f,ag=.10f,ab=.10f,br=.101f,bg=.101f,bb=.101f;
    Grade anime;anime.filter=int(Look::AnimeKaiju);GradeLinear(ar,ag,ab,anime);GradeLinear(br,bg,bb,anime);
    assert(br>ar && bg>ag && bb>ab && br-ar<.002f);
    puts("PHOTO SETTINGS PASS: neutral bytes, linear EV, color order, alpha, filters, lens roundtrip, limits/reset");
}
