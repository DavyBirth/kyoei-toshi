// SPDX-License-Identifier: GPL-2.0-or-later
#version 450
layout(set=0,binding=0) uniform sampler2D image;
layout(push_constant) uniform Grade {float exposure;float contrast;float saturation;int preset;int encoded;} grade;
layout(location=0) in vec2 uv;
layout(location=0) out vec4 color;
void main(){
    vec4 source=texture(image,uv);
    vec3 input_color=source.rgb;
    if(grade.encoded!=0)input_color=mix(input_color/12.92,pow((input_color+.055)/1.055,vec3(2.4)),greaterThan(input_color,vec3(.04045)));
    vec3 c=grade.contrast==0?vec3(.18):.18*pow(max(input_color*exp2(grade.exposure),vec3(0))/.18,vec3(grade.contrast));
    float l=dot(c,vec3(.2126,.7152,.0722));
    c=mix(vec3(l),c,grade.saturation);
    if(grade.preset==1)c=vec3(l);
    else if(grade.preset==2)c=vec3(dot(c,vec3(.393,.769,.189)),dot(c,vec3(.349,.686,.168)),dot(c,vec3(.272,.534,.131)));
    else if(grade.preset==3)c*=vec3(.9,1,1.1);
    else if(grade.preset==4)c*=vec3(1.1,1,.9);
    c=clamp(c,0,1);
    if(grade.encoded!=0)c=mix(c*12.92,1.055*pow(c,vec3(1.0/2.4))-.055,greaterThan(c,vec3(.0031308)));
    color=vec4(c,source.a);
}
