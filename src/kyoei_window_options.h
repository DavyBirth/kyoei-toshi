#pragma once
#include <cstdlib>
namespace KyoeiWindowOptions {
struct Settings { int width,height; bool fullscreen; };
inline int Dimension(const char* variable,int fallback,int minimum,int maximum) {
    const char* text=std::getenv(variable);
    if (!text || !*text) return fallback;
    char* end=nullptr; const long value=std::strtol(text,&end,10);
    return end && !*end && value>=minimum && value<=maximum ? int(value) : fallback;
}
inline Settings Read(int width,int height) {
    const char* mode=std::getenv("KYOEI_FULLSCREEN");
    return {Dimension("KYOEI_WINDOW_WIDTH",width,640,8192),Dimension("KYOEI_WINDOW_HEIGHT",height,360,4320),mode && mode[0]=='1'};
}
}
