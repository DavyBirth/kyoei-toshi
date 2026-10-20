// Optional photographic adjustments, independent of guest simulation.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
namespace KyoeiPhoto {
enum class Field {Lens,Roll,Speed,Exposure,Contrast,Saturation,Filter,Count};
struct Grade {float exposure=0,contrast=1,saturation=1;int filter=0;};
inline std::atomic<int> selected{0},filter{0};
inline std::atomic<float> exposure{0},contrast{1},saturation{1},camera_speed{1};
inline const char* FieldName(Field field) {
    constexpr const char* names[]={"Lente","Rotacao","Velocidade","Exposicao","Contraste","Saturacao","Filtro"};
    return names[std::clamp(int(field),0,int(Field::Count)-1)];
}
inline const char* FilterName(int value) {
    constexpr const char* names[]={"Original","Preto e branco","Sepia","Frio","Quente"};
    return names[std::clamp(value,0,4)];
}
inline void SelectField(int direction) {selected.store((selected.load()+direction+int(Field::Count))%int(Field::Count));}
inline Grade GetGrade(){return {exposure.load(),contrast.load(),saturation.load(),filter.load()};}
inline bool Neutral(Grade g){return g.exposure==0 && g.contrast==1 && g.saturation==1 && g.filter==0;}
inline void ResetGrade(){exposure=0;contrast=1;saturation=1;filter=0;camera_speed=1;}
inline void AdjustGrade(Field field,int direction) {
    if(field==Field::Exposure)exposure=std::clamp(exposure.load()+direction*.1f,-3.f,3.f);
    if(field==Field::Contrast)contrast=std::clamp(contrast.load()+direction*.05f,0.f,2.f);
    if(field==Field::Saturation)saturation=std::clamp(saturation.load()+direction*.1f,0.f,2.f);
    if(field==Field::Speed)camera_speed=std::clamp(camera_speed.load()+direction*.25f,.25f,5.f);
    if(field==Field::Filter)filter=(filter.load()+direction+5)%5;
}
inline float FocalLength(float vertical_fov) {return 12.f/std::tan(vertical_fov*.00872664625997f);}
inline float FovFromFocal(float mm){return 114.591559026f*std::atan(12.f/mm);}
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
}
inline void GradePixels(uint8_t* pixels,size_t bytes,bool bgra,Grade grade) {
    if(Neutral(grade))return;
    for(size_t i=0;i+3<bytes;i+=4) {
        auto* p=pixels+i;const int ri=bgra?2:0,bi=bgra?0:2;
        float r=Decode(p[ri]/255.f),g=Decode(p[1]/255.f),b=Decode(p[bi]/255.f);
        GradeLinear(r,g,b,grade);
        p[ri]=uint8_t(std::lround(Encode(r)*255));p[1]=uint8_t(std::lround(Encode(g)*255));p[bi]=uint8_t(std::lround(Encode(b)*255));
    }
}
}
