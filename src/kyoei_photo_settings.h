// Optional photographic adjustments, independent of guest simulation.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include "kyoei_photo_lights.h"
namespace KyoeiPhoto {
enum class Field {Lens,Roll,Speed,Exposure,Contrast,Saturation,Filter,Grain,Vignette,DofMode,FocusDistance,Aperture,LightSlot,LightControl,LightType,LightMode,LightColor,LightPower,LightX,LightY,LightZ,LightYaw,LightPitch,LightCone,Count};
enum class Look {Original,Mono,Sepia,Cool,Warm,Kaiju1954,Tokusatsu,KaijuNight,AnimeKaiju,Count};
struct Grade {float exposure=0,contrast=1,saturation=1;int filter=0;float grain=0,vignette=0;};
inline std::atomic<int> selected{0},filter{0};
inline std::atomic<float> exposure{0},contrast{1},saturation{1},camera_speed{1};
inline std::atomic<bool> experimental_dof{false};
inline std::atomic<bool> experimental_lights{false};
struct LightSettings {std::atomic<int> mode{0},color{0},type{0};std::atomic<float> power{1},x{0},y{0},z{0},yaw{0},pitch{-15},cone{45};};
inline LightSettings lights[KYOEI_PHOTO_LIGHT_COUNT];
inline std::atomic<int> light_slot{0};
inline std::atomic<bool> light_control{false};
inline LightSettings& CurrentLight(){return lights[light_slot.load()];}
inline std::atomic<int> dof_mode{0};
inline std::atomic<float> focus_distance{5},aperture{2.8f};
inline std::atomic<float> grain{0},vignette{0};
inline const char* FieldName(Field field) {
    constexpr const char* names[]={"Distancia focal da lente","Rotacao","Velocidade","Exposicao","Contraste","Saturacao","Filtro","Granulacao","Vinheta","Desfoque","Distancia de foco","Abertura","Luz selecionada","Controle","Tipo de luz","Luz ligada","Cor","Intensidade","Posicao X","Altura","Posicao Z","Direcao horizontal","Inclinacao","Abertura do facho"};
    return names[std::clamp(int(field),0,int(Field::Count)-1)];
}
inline const char* FilterName(int value) {
    constexpr const char* names[]={"Original","Preto e branco","Sepia","Frio","Quente","Kaiju 1954","Tokusatsu retro","Kaiju noturno","Kaiju anime"};
    return names[std::clamp(value,0,int(Look::Count)-1)];
}
enum class Group {Camera,Image,Focus,Lights,Count};
inline constexpr int group_begin[]={0,int(Field::Exposure),int(Field::DofMode),int(Field::LightSlot)};
inline constexpr int group_end[]={int(Field::Exposure),int(Field::DofMode),int(Field::LightSlot),int(Field::Count)};
inline std::atomic<int> remembered_field[4]{{0},{int(Field::Exposure)},{int(Field::DofMode)},{int(Field::LightSlot)}};
inline int FieldGroup(int field){return field<int(Field::Exposure)?0:field<int(Field::DofMode)?1:field<int(Field::LightSlot)?2:3;}
inline const char* GroupName(int group){constexpr const char* names[]={"CAMERA","IMAGEM","FOCO","LUZES"};return names[std::clamp(group,0,3)];}
inline bool FieldEnabled(int field){
    if(field<0 || field>=int(Field::Count))return false;
    if(field<int(Field::DofMode))return true;
    if(field<int(Field::LightSlot))return experimental_dof.load();
    return experimental_lights.load() && (field<int(Field::LightYaw) || CurrentLight().type.load()==1);
}
inline int FieldCount(){int count=0;for(int i=0;i<int(Field::Count);++i)count+=FieldEnabled(i);return count;}
inline bool SetGroup(int group){
    if(group<0 || group>=4 || !FieldEnabled(group_begin[group]))return false;
    int field=remembered_field[group].load();
    if(field<group_begin[group] || field>=group_end[group] || !FieldEnabled(field))field=group_begin[group];
    selected=field;return true;
}
inline void SelectGroup(int direction){
    int group=FieldGroup(selected.load());
    do{group=(group+(direction<0?-1:1)+4)%4;}while(!SetGroup(group));
}
inline void SelectField(int direction) {
    const int group=FieldGroup(selected.load()),begin=group_begin[group],end=group_end[group];
    int next=selected.load();
    if(!FieldEnabled(begin)){SetGroup(0);return;}
    do{next=begin+(next-begin+(direction<0?-1:1)+(end-begin))%(end-begin);}while(!FieldEnabled(next));
    selected=next;remembered_field[group]=next;
}
inline const char* LightColorName(int color){constexpr const char* names[]={"Branca","Quente","Fria","Vermelha","Verde","Azul","Violeta"};return names[std::clamp(color,0,6)];}
inline void LightRgb(float rgb[3],int slot=-1){
    const auto& light=slot<0?CurrentLight():lights[slot];
    constexpr float colors[][3]={{1,1,1},{1,.65f,.3f},{.4f,.65f,1},{1,.025f,.025f},{.025f,1,.025f},{.025f,.025f,1},{.65f,.05f,1}};
    const auto& color=colors[std::clamp(light.color.load(),0,6)];
    for(int i=0;i<3;++i)rgb[i]=color[i]*light.power.load();
}
inline Grade GetGrade(){return {exposure.load(),contrast.load(),saturation.load(),filter.load(),grain.load(),vignette.load()};}
inline bool Neutral(Grade g){return g.exposure==0 && g.contrast==1 && g.saturation==1 && g.filter==0 && g.grain==0 && g.vignette==0;}
inline void ResetGrade(){exposure=0;contrast=1;saturation=1;filter=0;camera_speed=1;grain=0;vignette=0;dof_mode=0;focus_distance=5;aperture=2.8f;light_slot=0;light_control=false;for(auto& light:lights){light.mode=0;light.color=0;light.power=1;light.x=0;light.y=0;light.z=0;light.type=0;light.yaw=0;light.pitch=-15;light.cone=45;}}
inline void AdjustGrade(Field field,int direction) {
    if(field==Field::LightSlot)light_slot=(light_slot.load()+direction+KYOEI_PHOTO_LIGHT_COUNT)%KYOEI_PHOTO_LIGHT_COUNT;
    if(field==Field::LightControl)light_control=!light_control.load();
    auto& light=CurrentLight();
    if(field==Field::LightType)light.type=!light.type.load();
    if(field==Field::LightYaw)light.yaw=std::remainder(light.yaw.load()+direction*5.f,360.f);
    if(field==Field::LightPitch)light.pitch=std::clamp(light.pitch.load()+direction*5.f,-89.f,89.f);
    if(field==Field::LightCone)light.cone=std::clamp(light.cone.load()+direction*5.f,5.f,160.f);
    if(field==Field::LightMode)light.mode=!light.mode.load();
    if(field==Field::LightColor)light.color=(light.color.load()+direction+7)%7;
    if(field==Field::LightPower)light.power=std::clamp(light.power.load()+direction*.25f,.25f,16.f);
    if(field==Field::LightX)light.x=std::clamp(light.x.load()+direction*.25f,-50.f,50.f);
    if(field==Field::LightY)light.y=std::clamp(light.y.load()+direction*.25f,-50.f,50.f);
    if(field==Field::LightZ)light.z=std::clamp(light.z.load()+direction*.25f,-50.f,50.f);
    if(field==Field::DofMode)dof_mode=(dof_mode.load()+direction+3)%3;
    if(field==Field::FocusDistance){focus_distance=std::clamp(focus_distance.load()*std::pow(1.1f,float(direction)),.2f,1000.f);dof_mode=1;}
    if(field==Field::Aperture){aperture=std::clamp(aperture.load()+direction*.2f,1.f,32.f);dof_mode=1;}
    if(field==Field::Exposure)exposure=std::clamp(exposure.load()+direction*.1f,-3.f,3.f);
    if(field==Field::Contrast)contrast=std::clamp(contrast.load()+direction*.05f,0.f,2.f);
    if(field==Field::Saturation)saturation=std::clamp(saturation.load()+direction*.1f,0.f,2.f);
    if(field==Field::Speed)camera_speed=std::clamp(camera_speed.load()+direction*.25f,.25f,5.f);
    if(field==Field::Grain)grain=std::clamp(grain.load()+direction*.05f,0.f,1.f);
    if(field==Field::Vignette)vignette=std::clamp(vignette.load()+direction*.05f,0.f,1.f);
    if(field==Field::Filter) {
        const int next=(filter.load()+direction+int(Look::Count))%int(Look::Count);filter=next;
        constexpr float grains[]={0,0,0,0,0,.35f,.18f,.10f,0};
        constexpr float vignettes[]={0,0,0,0,0,.30f,.20f,.35f,.10f};
        grain=grains[next];vignette=vignettes[next];
    }
}
inline float FocalLength(float vertical_fov) {return 17.5f/std::tan(vertical_fov*.00872664625997f);}
inline float FovFromFocal(float mm){return 114.591559026f*std::atan(17.5f/mm);}
inline float Decode(float x){return x<=.04045f?x/12.92f:std::pow((x+.055f)/1.055f,2.4f);}
inline float Encode(float x){x=std::clamp(x,0.f,1.f);return x<=.0031308f?x*12.92f:1.055f*std::pow(x,1.f/2.4f)-.055f;}
inline void GradeLinear(float& r,float& g,float& b,Grade grade) {
    const float gain=std::exp2(grade.exposure);
    const auto curve=[&](float x){return grade.contrast==0?.18f:.18f*std::pow(std::max(x*gain,0.f)/.18f,grade.contrast);};
    r=curve(r);g=curve(g);b=curve(b);
    const float l=r*.2126f+g*.7152f+b*.0722f;
    r=l+(r-l)*grade.saturation;g=l+(g-l)*grade.saturation;b=l+(b-l)*grade.saturation;
    if(grade.filter==1)r=g=b=l;
    else if(grade.filter==2){const float a=r,c=g,d=b;r=.393f*a+.769f*c+.189f*d;g=.349f*a+.686f*c+.168f*d;b=.272f*a+.534f*c+.131f*d;}
    else if(grade.filter==3){r*=.90f;b*=1.10f;}
    else if(grade.filter==4){r*=1.10f;b*=.90f;}
    else if(grade.filter==int(Look::Kaiju1954))r=g=b=.006f+.18f*std::pow(std::max(l,0.f)/.18f,1.18f);
    else if(grade.filter==int(Look::Tokusatsu)) {
        r=.015f+(l+(r-l)*1.12f)*1.08f;g=.010f+(l+(g-l)*1.12f)*1.02f;b=.015f+(l+(b-l)*1.12f)*.83f;
    } else if(grade.filter==int(Look::KaijuNight)) {
        const float shadow=1-std::clamp(l*2.f,0.f,1.f);
        r*=1.03f-.25f*shadow;g*=.95f+.15f*shadow;b*=1.15f+.15f*shadow;
    } else if(grade.filter==int(Look::AnimeKaiju)) {
        const float shadow=1-std::clamp(l*2.f,0.f,1.f);
        r=(l+(r-l)*1.16f)*(1.06f-.10f*shadow);
        g=(l+(g-l)*1.16f)*(1.02f+.02f*shadow);
        b=(l+(b-l)*1.16f)*(.97f+.13f*shadow);
    }
}
inline uint32_t FilmHash(uint32_t x,uint32_t y) {
    uint32_t h=x*UINT32_C(0x9e3779b9)^y*UINT32_C(0x85ebca6b)^UINT32_C(0x1954);
    h^=h>>16;h*=UINT32_C(0x7feb352d);h^=h>>15;h*=UINT32_C(0x846ca68b);return h^(h>>16);
}
inline void FilmTexture(float& r,float& g,float& b,unsigned x,unsigned y,unsigned width,unsigned height,Grade grade) {
    if(grade.grain==0 && grade.vignette==0)return;
    float noise=(float(FilmHash(x,y)&65535)/65535.f-.5f)*grade.grain*.16f;
    if(grade.filter==int(Look::Kaiju1954) && grade.grain>0) {
        if(FilmHash(x/2,37)%503==0 && (FilmHash(x/2,y/12)&3)!=0)noise+=grade.grain*.12f;
        if(FilmHash(x/3,y/3)%4093==0)noise-=grade.grain*.25f;
    }
    const float u=2*(x+.5f)/width-1,v=2*(y+.5f)/height-1;
    const float radius=std::min(1.f,(u*u+v*v)*.5f),shade=1-grade.vignette*.65f*radius*radius;
    r=std::clamp(r*shade+noise,0.f,1.f);g=std::clamp(g*shade+noise,0.f,1.f);b=std::clamp(b*shade+noise,0.f,1.f);
}
inline void GradePixels(uint8_t* pixels,size_t bytes,bool bgra,Grade grade,unsigned width=0) {
    if(Neutral(grade))return;
    if(!width)width=unsigned(bytes/4);
    if(!width || bytes%(size_t(width)*4))return;
    const unsigned height=unsigned(bytes/(size_t(width)*4));
    for(size_t i=0;i+3<bytes;i+=4) {
        auto* p=pixels+i;const int ri=bgra?2:0,bi=bgra?0:2;
        float r=Decode(p[ri]/255.f),g=Decode(p[1]/255.f),b=Decode(p[bi]/255.f);
        GradeLinear(r,g,b,grade);
        r=Encode(r);g=Encode(g);b=Encode(b);
        FilmTexture(r,g,b,unsigned(i/4%width),unsigned(i/4/width),width,height,grade);
        p[ri]=uint8_t(std::lround(r*255));p[1]=uint8_t(std::lround(g*255));p[bi]=uint8_t(std::lround(b*255));
    }
}
}
