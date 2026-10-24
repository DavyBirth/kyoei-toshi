// Language of the PC screens: the game's (KYOEI_LANGUAGE: en, pt-BR, ja). SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <cstdlib>
#include <cstring>
namespace KyoeiUiText {
enum Language { English, Portuguese, Japanese };
inline Language Current() {
    static const Language language = [] {
        const char* value = std::getenv("KYOEI_LANGUAGE");
        if (value && !std::strncmp(value, "pt", 2)) return Portuguese;
        if (value && !std::strcmp(value, "ja")) return Japanese;
        return English;
    }();
    return language;
}
/// The text for the current language.
inline const char* T(const char* en, const char* pt, const char* ja) {
    const Language language = Current();
    return language == Portuguese ? pt : language == Japanese ? ja : en;
}
}
