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
    selected=0;SelectField(-1);assert(selected==int(Field::Count)-1);SelectField(1);assert(selected==0);
    puts("PHOTO SETTINGS PASS: neutral bytes, linear EV, color order, alpha, filters, lens roundtrip, limits/reset");
}
