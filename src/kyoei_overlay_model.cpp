// SPDX-License-Identifier: GPL-2.0-or-later
// Modal contracts are Kyoei-specific. Render-thread ownership follows the public
// GoW3 overlay reference; no game addresses, cheats or settings code copied.
#include "kyoei_overlay.h"
#include "kyoei_name_model.h"
#include "kyoei_list_model.h"
#include "kyoei_input_mode.h"
#include "imgui.h"
#include <windows.h>
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <cwchar>
#include <cstdlib>
#include <deque>
#include <mutex>
#include <string>
#include <vector>

namespace {
std::mutex mutex;
std::condition_variable completed;
std::atomic<int> kind{0};
bool ready{},done{},dialog_canceled{};
ImGuiStyle base_style;
unsigned end_status{},selected{},message_type{},message_choices{},progress{};
uint32_t previous_buttons{};
uint64_t revision{},presented{};
KyoeiNameModel name{};
std::string caption;
std::vector<std::string> items;
struct Mouse { int type,button,down; float x,y; };
std::deque<Mouse> mouse_events;
float scale=1;

bool enabled() {
    const char *value=std::getenv("KYOEI_IN_GAME_DIALOGS");
    return value && !std::strcmp(value,"1");
}
std::string utf8(const wchar_t *text) {
    if (!text) return {};
    int size=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,text,-1,nullptr,0,nullptr,nullptr);
    if (!size) return {};
    std::string value(size,'\0');
    WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,text,-1,value.data(),size,nullptr,nullptr);
    value.pop_back(); return value;
}
std::string ui_text(const wchar_t *text) {
    const char *language=std::getenv("KYOEI_LANGUAGE");
    if (language && !std::strcmp(language,"en") && text) {
        const std::pair<const wchar_t *,const char *> known[]={
            {L"Escolha onde salvar","Choose a save slot"},
            {L"Escolha o save para carregar","Choose a save to load"},
            {L"Escolha o save para excluir","Choose a save to delete"},
            {L"Criar novo save","Create a new save"},
            {L"Substituir este save?","Overwrite this save?"},
            {L"Salvar neste slot?","Save in this slot?"},
            {L"Carregar este save?","Load this save?"},
            {L"Excluir este save?","Delete this save?"}};
        for (const auto &[source,target]:known) if (!std::wcscmp(text,source)) return target;
    }
    return utf8(text);
}
void finish_locked(unsigned result) {
    if (!kind || done) return;
    done=true; end_status=result; ++revision;
    completed.notify_all();
}
void release_locked() {
    kind=KYOEI_OVERLAY_NONE; items.clear(); caption.clear(); mouse_events.clear();
    for (int button=0;button<5;++button) mouse_events.push_back({1,button,0,0,0});
    ++revision;
}
void apply_pressed_locked(uint32_t pressed) {
    if (!kind || done || !pressed) return;
    if (kind==KYOEI_OVERLAY_NAME) {
        kyoei_name_buttons(&name,pressed);
        if (name.result) finish_locked(name.result==KYOEI_NAME_ACCEPTED?0:1);
    } else {
        if (pressed&0x4000) { finish_locked(1); return; }
        if (kind==KYOEI_OVERLAY_LIST) selected=kyoei_list_next((unsigned)items.size(),selected,pressed);
        else if (message_choices && (pressed&(0x10|0x20|0x40|0x80))) selected=(selected+1)%message_choices;
        if ((pressed&(0x2000|8)) && (kind==KYOEI_OVERLAY_LIST || message_choices)) finish_locked(0);
    }
    ++revision;
}
void keyboard_locked(unsigned key) {
    if (done) return;
    if (key==KYOEI_OVERLAY_KEY_CANCEL) { finish_locked(1); return; }
    if (key==KYOEI_OVERLAY_KEY_ACCEPT) {
        if (kind!=KYOEI_OVERLAY_MESSAGE || message_choices) finish_locked(0);
        return;
    }
    if (kind==KYOEI_OVERLAY_NAME) {
        if (key==KYOEI_OVERLAY_KEY_LEFT) kyoei_name_cursor(&name,0);
        if (key==KYOEI_OVERLAY_KEY_RIGHT) kyoei_name_cursor(&name,1);
        if (key==KYOEI_OVERLAY_KEY_BACKSPACE) kyoei_name_backspace(&name);
        if (key==KYOEI_OVERLAY_KEY_SELECT_ALL) name.select_all=name.length!=0;
    } else {
        uint32_t pressed=key==KYOEI_OVERLAY_KEY_UP || key==KYOEI_OVERLAY_KEY_LEFT?0x10:
                         key==KYOEI_OVERLAY_KEY_DOWN || key==KYOEI_OVERLAY_KEY_RIGHT?0x40:0;
        apply_pressed_locked(pressed);
    }
    ++revision;
}
bool available_locked() { return ready && enabled() && kind==KYOEI_OVERLAY_NONE; }
void name_box(float width,bool controller) {
    ImGui::TextUnformatted("Character name");
    if (!caption.empty()) ImGui::TextWrapped("%s",caption.c_str());
    ImGui::Spacing();
    auto value=utf8((const wchar_t *)name.text);
    if (name.select_all) ImGui::PushStyleColor(ImGuiCol_FrameBg,ImVec4(.08f,.27f,.43f,1));
    ImGui::BeginChild("name-value",ImVec2(0,65*scale),ImGuiChildFlags_Borders,ImGuiWindowFlags_HorizontalScrollbar);
    // Horizontal scrolling keeps the actual cursor visible, without truncating names.
    std::wstring prefix((const wchar_t *)name.text,name.cursor);
    float cursor=ImGui::CalcTextSize(utf8(prefix.c_str()).c_str()).x;
    ImGui::TextUnformatted(value.c_str());
    auto origin=ImGui::GetItemRectMin();
    if (!name.select_all) ImGui::GetWindowDrawList()->AddLine(
        ImVec2(origin.x+cursor,origin.y),ImVec2(origin.x+cursor,origin.y+ImGui::GetTextLineHeight()),IM_COL32(245,245,245,255),2);
    float available=ImGui::GetContentRegionAvail().x;
    ImGui::SetScrollX(std::max(0.f,cursor-available+18*scale));
    ImGui::EndChild();
    if (name.select_all) ImGui::PopStyleColor();
    ImGui::TextDisabled("%u / %u",(unsigned)name.length,(unsigned)name.limit);
    if (controller) {
        const float gap=6*scale,key_width=(width-40*scale-gap*9)/10;
        for (unsigned key=0;key<40;++key) {
            if (key%10) ImGui::SameLine(0,gap);
            ImGui::PushID((int)key);
            std::string label;
            if (key<26) label=char((name.upper?'A':'a')+key);
            else if (key<36) label=char('0'+key-26);
            else label=key==36?"Space":key==37?"Shift":key==38?"Erase":"Done";
            const bool highlighted=key==name.key;
            if (highlighted) ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(.13f,.45f,.67f,1));
            if (ImGui::Button(label.c_str(),ImVec2(key_width,40*scale))) {
                name.key=key; kyoei_name_activate(&name); ++revision;
                if (name.result) finish_locked(0);
            }
            if (highlighted) ImGui::PopStyleColor();
            ImGui::PopID();
        }
        ImGui::TextWrapped("D-pad: choose a key   Circle: select   X: cancel");
        ImGui::TextWrapped("Square: erase   Triangle: shift   L1/R1: cursor   Options: done");
    } else ImGui::TextWrapped("Type your name. Arrow keys: cursor   Backspace: erase   Ctrl+A: select all");
    ImGui::Spacing();
    if (ImGui::Button("Confirm",ImVec2(135*scale,36*scale))) finish_locked(0);
    ImGui::SameLine();
    if (ImGui::Button("Cancel",ImVec2(135*scale,36*scale))) finish_locked(1);
    if (!controller) ImGui::TextDisabled("Enter: confirm   Esc: cancel");
}
void list_box(float height) {
    ImGui::TextWrapped("%s",caption.c_str());
    ImGui::Separator();
    ImGui::BeginChild("save-slots",ImVec2(0,height-155*scale),ImGuiChildFlags_Borders);
    ImGuiListClipper clipper; clipper.Begin((int)items.size(),40*scale);
    clipper.IncludeItemByIndex((int)selected);
    while (clipper.Step()) for (int index=clipper.DisplayStart;index<clipper.DisplayEnd;++index) {
        ImGui::PushID(index);
        if (ImGui::Selectable(items[index].c_str(),selected==(unsigned)index,0,ImVec2(0,36*scale))) {
            selected=(unsigned)index; ++revision;
        }
        if (selected==(unsigned)index) ImGui::SetScrollHereY(.5f);
        ImGui::PopID();
    }
    ImGui::EndChild();
    if (ImGui::Button("Confirm",ImVec2(135*scale,36*scale))) finish_locked(0);
    ImGui::SameLine();
    if (ImGui::Button("Cancel",ImVec2(135*scale,36*scale))) finish_locked(1);
    ImGui::TextDisabled("Circle / Enter: confirm   X / Esc: back");
}
void message_box() {
    ImGui::TextWrapped("%s",caption.c_str());
    ImGui::Spacing();
    if (!message_choices) {
        ImGui::ProgressBar(float(progress)/100.f,ImVec2(-1,28*scale));
        ImGui::TextDisabled("Please wait..."); return;
    }
    for (unsigned choice=0;choice<message_choices;++choice) {
        if (choice) ImGui::SameLine();
        const bool highlighted=choice==selected;
        if (highlighted) ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(.13f,.45f,.67f,1));
        const char *label=message_choices==1?"OK":
            message_type==1 || message_type==7?(choice?"No":"Yes"):(choice?"Cancel":"OK");
        if (ImGui::Button(label,ImVec2(135*scale,36*scale))) { selected=choice; finish_locked(0); }
        if (highlighted) ImGui::PopStyleColor();
    }
    ImGui::TextDisabled("Circle / Enter: confirm   X / Esc: back");
}
}

extern "C" int kyoei_overlay_begin_name(const wchar_t *title,const uint16_t *text,size_t length,size_t limit) {
    std::scoped_lock lock{mutex};
    if (!available_locked() || !kyoei_name_begin(&name,text,length,limit)) return 0;
    caption=ui_text(title); done=false; end_status=2; kind=KYOEI_OVERLAY_NAME; ++revision; return 1;
}
extern "C" int kyoei_overlay_name_result(uint16_t *text,size_t capacity,unsigned *result) {
    std::scoped_lock lock{mutex};
    if (kind!=KYOEI_OVERLAY_NAME || !done || !result || !text || capacity<=name.length) return 0;
    *result=end_status;
    if (!end_status) std::memcpy(text,name.text,(name.length+1)*sizeof(uint16_t));
    return 1;
}
extern "C" void kyoei_overlay_release_name() {
    std::scoped_lock lock{mutex}; if (kind==KYOEI_OVERLAY_NAME) release_locked();
}
extern "C" void kyoei_overlay_abort_name() {
    std::scoped_lock lock{mutex}; if (kind==KYOEI_OVERLAY_NAME) finish_locked(2);
}
extern "C" int kyoei_overlay_select(const wchar_t *title,const wchar_t *const *labels,unsigned count,unsigned initial) {
    if (!labels || !count || count>1025 || initial>=count) return INT_MIN;
    std::unique_lock lock{mutex}; if (!available_locked()) return INT_MIN;
    if (dialog_canceled) return -1;
    std::vector<std::string> copied; copied.reserve(count);
    for (unsigned i=0;i<count;++i) { if (!labels[i]) return INT_MIN; copied.push_back(ui_text(labels[i])); }
    caption=ui_text(title); items=std::move(copied); selected=initial; done=false;
    kind=KYOEI_OVERLAY_LIST; ++revision;
    completed.wait(lock,[]{return done;});
    int result=end_status?-1:(int)selected; release_locked(); return result;
}
extern "C" int kyoei_overlay_message(const wchar_t *text,unsigned type) {
    if (type>8 && type!=256) return INT_MIN;
    std::unique_lock lock{mutex}; if (!available_locked()) return INT_MIN;
    if (dialog_canceled) return 0;
    caption=ui_text(text); message_type=type; message_choices=type==256?0:type==0?1:2;
    selected=type==7 || type==8; done=false; kind=KYOEI_OVERLAY_MESSAGE; ++revision;
    completed.wait(lock,[]{return done;});
    int result=end_status?(message_choices==2?(type==1 || type==7?IDNO:IDCANCEL):0):
        selected==1?(type==1 || type==7?IDNO:IDCANCEL):IDOK;
    release_locked(); return result;
}
extern "C" void kyoei_overlay_cancel_dialog() {
    std::scoped_lock lock{mutex}; dialog_canceled=true;
    if (kind==KYOEI_OVERLAY_LIST || kind==KYOEI_OVERLAY_MESSAGE) finish_locked(1);
}
extern "C" void kyoei_overlay_prepare_dialog() {
    std::scoped_lock lock{mutex}; dialog_canceled=false; progress=0;
}
extern "C" void kyoei_overlay_progress(unsigned percent) {
    std::scoped_lock lock{mutex}; progress=std::min(percent,100u); if (kind==KYOEI_OVERLAY_MESSAGE) ++revision;
}
extern "C" void kyoei_overlay_abort_all() {
    std::scoped_lock lock{mutex}; finish_locked(2);
}
extern "C" int kyoei_overlay_input(uint32_t buttons) {
    std::scoped_lock lock{mutex}; uint32_t pressed=buttons&~previous_buttons; previous_buttons=buttons;
    if (!kind) return 0;
    apply_pressed_locked(pressed); return 1;
}
extern "C" int kyoei_overlay_key(unsigned key) {
    std::scoped_lock lock{mutex}; if (!kind) return 0; keyboard_locked(key); return 1;
}
extern "C" int kyoei_overlay_text(const char *text) {
    std::scoped_lock lock{mutex}; if (!kind) return 0;
    if (kind!=KYOEI_OVERLAY_NAME || done || !text) return 1;
    uint16_t value[2049];
    int length=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text,-1,(wchar_t *)value,2049);
    if (length>1 && kyoei_name_insert(&name,value,(size_t)length-1)) ++revision;
    return 1;
}
extern "C" void kyoei_overlay_mouse(int type,float x,float y,int button,int down) {
    std::scoped_lock lock{mutex};
    if (kind && !done && mouse_events.size()<128) mouse_events.push_back({type,button,down,x,y});
}
extern "C" int kyoei_overlay_kind() { return kind.load(); }
extern "C" int kyoei_overlay_wants_text() {
    std::scoped_lock lock{mutex}; return kind==KYOEI_OVERLAY_NAME && !done;
}
extern "C" int kyoei_overlay_needs_redraw() {
    std::scoped_lock lock{mutex}; return ready && (kind || revision!=presented);
}
extern "C" uint64_t kyoei_overlay_revision() { std::scoped_lock lock{mutex}; return revision; }
extern "C" void kyoei_overlay_presented(uint64_t value) { std::scoped_lock lock{mutex}; presented=value; }

namespace KyoeiOverlayModel {
void InitializeGui(const char *font_directory,bool headless) {
    std::scoped_lock lock{mutex};
    IMGUI_CHECKVERSION(); ImGui::CreateContext();
    auto &io=ImGui::GetIO(); io.IniFilename=nullptr; io.LogFilename=nullptr;
    io.BackendPlatformName="kyoei-modal";
    ImGui::StyleColorsDark();
    auto &style=ImGui::GetStyle(); style.WindowRounding=8; style.FrameRounding=5;
    style.WindowPadding=ImVec2(20,18); style.ItemSpacing=ImVec2(8,9);
    style.Colors[ImGuiCol_WindowBg]=ImVec4(.055f,.075f,.105f,.98f);
    style.Colors[ImGuiCol_Button]=ImVec4(.13f,.18f,.25f,1);
    base_style=style;
    if (font_directory && *font_directory) {
        std::string normal=std::string(font_directory)+"/NotoSans-Regular.ttf";
        std::string cjk=std::string(font_directory)+"/NotoSansCJK-Regular.ttc";
        if (ImGui::GetIO().Fonts->AddFontFromFileTTF(normal.c_str(),20)) {
            ImFontConfig config; config.MergeMode=true;
            io.Fonts->AddFontFromFileTTF(cjk.c_str(),20,&config);
        }
    }
    if (io.Fonts->Fonts.empty()) io.Fonts->AddFontDefault();
    if (headless) { io.Fonts->Build(); io.Fonts->SetTexID(ImTextureID(1)); }
    scale=1;
}
void SetReady(bool value) {
    std::scoped_lock lock{mutex}; ready=value;
    if (!value) finish_locked(2);
}
void ShutdownGui() {
    std::scoped_lock lock{mutex}; ready=false; finish_locked(2);
    ImGui::DestroyContext();
}
ImDrawData *Draw(unsigned width,unsigned height,float seconds) {
    std::scoped_lock lock{mutex};
    auto &io=ImGui::GetIO(); io.DisplaySize=ImVec2((float)width,(float)height);
    io.DeltaTime=seconds>0 && seconds<1?seconds:1.f/60;
    for (const auto &event:mouse_events) {
        if (event.type==0) io.AddMousePosEvent(event.x,event.y);
        else if (event.type==1) io.AddMouseButtonEvent(event.button,event.down!=0);
        else io.AddMouseWheelEvent(event.x,event.y);
    }
    mouse_events.clear();
    float next=std::clamp(float(height)/1080.f,.6f,2.f);
    auto &style=ImGui::GetStyle(); style=base_style; style.ScaleAllSizes(next);
    style.FontScaleMain=next; scale=next;
    ImGui::NewFrame();
    if (kind && !done) {
        bool controller=kyoei_input_current_mode()==KYOEI_INPUT_CONTROLLER;
        float panel_width=std::min(850*scale,float(width)-32*scale);
        float panel_height=std::min((kind==KYOEI_OVERLAY_NAME?(controller?600:440):kind==KYOEI_OVERLAY_LIST?540:340)*scale,float(height)-32*scale);
        ImGui::SetNextWindowPos(ImVec2(float(width)/2,float(height)/2),ImGuiCond_Always,ImVec2(.5f,.5f));
        ImGui::SetNextWindowSize(ImVec2(panel_width,panel_height));
        ImGui::Begin("##kyoei-modal",nullptr,ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings);
        if (kind==KYOEI_OVERLAY_NAME) name_box(panel_width,controller);
        else if (kind==KYOEI_OVERLAY_LIST) list_box(panel_height);
        else message_box();
        ImGui::End();
    }
    ImGui::Render(); return ImGui::GetDrawData();
}
}
