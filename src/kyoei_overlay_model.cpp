// SPDX-License-Identifier: GPL-2.0-or-later
// Modal contracts are Kyoei-specific. Render-thread ownership follows the public
// GoW3 overlay reference; no game addresses, cheats or settings code copied.
#include "kyoei_overlay.h"
#include "kyoei_name_model.h"
#include "kyoei_pc_names.h"
#include "kyoei_list_model.h"
#include "kyoei_input_mode.h"
#include "kyoei_title_exit.h"
#include "kyoei_ui_art.h"
#include "kyoei_ui_draw.h"
#include "kyoei_ui_text.h"
#include <cfloat>
#include <cmath>
extern "C" void gz_log(const char *, ...);
#include <unordered_map>
#include "imgui.h"
#include "imgui_internal.h"
#include <cstdio>
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
    if (language && !std::strcmp(language,"ja") && text) {
        const std::pair<const wchar_t *,const char *> known[]={
            {L"Escolha onde salvar","セーブする場所を選んでください"},
            {L"Escolha o save para carregar","ロードするデータを選んでください"},
            {L"Escolha o save para excluir","削除するデータを選んでください"},
            {L"Criar novo save","新規作成"},
            {L"Substituir este save?","このデータに上書きしますか？"},
            {L"Salvar neste slot?","ここにセーブしますか？"},
            {L"Carregar este save?","このデータをロードしますか？"},
            {L"Excluir este save?","このデータを削除しますか？"}};
        for (const auto &[source,target]:known) if (!std::wcscmp(text,source)) return target;
    }
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
    if (!kind || done || !pressed || (kind==KYOEI_OVERLAY_MESSAGE && !message_choices)) return;
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
    if (done || (kind==KYOEI_OVERLAY_MESSAGE && !message_choices)) return;
    if (key==KYOEI_OVERLAY_KEY_CANCEL) { finish_locked(1); return; }
    if (key==KYOEI_OVERLAY_KEY_ACCEPT) {
        if ((kind!=KYOEI_OVERLAY_MESSAGE || message_choices) &&
            (kind!=KYOEI_OVERLAY_NAME || kyoei_name_can_accept(&name))) finish_locked(0);
        return;
    }
    if (kind==KYOEI_OVERLAY_NAME) {
        if (key==KYOEI_OVERLAY_KEY_LEFT) kyoei_name_cursor(&name,0);
        if (key==KYOEI_OVERLAY_KEY_RIGHT) kyoei_name_cursor(&name,1);
        if (key==KYOEI_OVERLAY_KEY_BACKSPACE) kyoei_name_backspace(&name);
        if (key==KYOEI_OVERLAY_KEY_DELETE) kyoei_name_delete(&name);
        if (key==KYOEI_OVERLAY_KEY_HOME) kyoei_name_boundary(&name,0);
        if (key==KYOEI_OVERLAY_KEY_END) kyoei_name_boundary(&name,1);
        if (key==KYOEI_OVERLAY_KEY_SELECT_ALL) name.select_all=name.length!=0;
    } else {
        uint32_t pressed=key==KYOEI_OVERLAY_KEY_UP || key==KYOEI_OVERLAY_KEY_LEFT?0x10:
                         key==KYOEI_OVERLAY_KEY_DOWN || key==KYOEI_OVERLAY_KEY_RIGHT?0x40:0;
        apply_pressed_locked(pressed);
    }
    ++revision;
}
bool available_locked() { return ready && enabled() && kind==KYOEI_OVERLAY_NONE; }
// The modals are drawn like the game's Option screen (tools/make_ui_art.py pieces): title bar,
// selection marker, row lines, description frame and a prompt bar with the pad's icons.
// Mouse hit boxes are invisible ImGui buttons over the same rows.
using KyoeiUiText::T;
using namespace KyoeiUiDraw;
const ImU32 text_color=IM_COL32(232,232,228,255), dim_color=IM_COL32(150,158,158,255);
bool hit(const HudSpace &hud,const char *id,float x0,float y0,float x1,float y1) {
    ImGui::SetCursorScreenPos(hud.P(x0,y0));
    return ImGui::InvisibleButton(id,ImVec2((x1-x0)*hud.unit,(y1-y0)*hud.unit));
}
bool hovered_now() { return ImGui::IsItemHovered() && (ImGui::GetIO().MouseDelta.x || ImGui::GetIO().MouseDelta.y); }
void backdrop(ImDrawList *draw,const HudSpace &hud,const char *title) {
    draw->AddRectFilled(ImVec2(0,0),ImGui::GetIO().DisplaySize,IM_COL32(4,7,9,170));
    const ImVec2 bar=KyoeiUiArt::Size("option_title");
    KyoeiUiArt::Draw(draw,"option_title",hud.P(0,54),hud.P(bar.x,54+bar.y));
    HudText(draw,hud,236,90,34,title);
}
struct Prompt { const char *pad, *key, *label; };
void prompt_bar(ImDrawList *draw,const HudSpace &hud,bool controller,std::initializer_list<Prompt> prompts) {
    draw->AddRectFilled(hud.P(0,962),hud.P(1920,1080),IM_COL32(6,10,12,165));
    KyoeiUiArt::Draw(draw,"pause_line",hud.P(0,960),hud.P(1920,964));
    float x=60;
    for (const auto &prompt:prompts) {
        const char *button=controller?prompt.pad:prompt.key;
        if (!button) continue;
        const std::string piece=controller?std::string(KyoeiUiArt::Pad(button)):std::string("key_")+button;
        // LB stands for the L1/R1 pair (cursor left/right).
        const std::string pair=controller && !std::strcmp(button,"LB")?std::string(KyoeiUiArt::Pad("RB")):std::string();
        x=pair.empty()?HudPrompt(draw,hud,x,1030,{piece.c_str()},prompt.label):HudPrompt(draw,hud,x,1030,{piece.c_str(),pair.c_str()},prompt.label);
    }
}
// One option row: marker when chosen, label, the native line under it. True on click.
bool option_row(ImDrawList *draw,const HudSpace &hud,int id,float x0,float x1,float y,const char *label,bool chosen,bool &hover,float align=0) {
    char name[16]; std::snprintf(name,sizeof name,"##row%d",id);
    const bool clicked=hit(hud,name,x0-10,y-38,x1,y+14);
    hover=hovered_now();
    if (chosen) KyoeiUiArt::Draw(draw,"option_point",hud.P(x0-10,y-30),hud.P(align?x1:x0+(x1-x0)*.45f,y+2));
    HudText(draw,hud,align?(x0+x1)/2:x0+8,y,31,label,chosen?text_color:dim_color,align);
    KyoeiUiArt::Draw(draw,"option_line",hud.P(x0,y+8),hud.P(x1,y+18));
    return clicked;
}
void list_screen(ImDrawList *draw,const HudSpace &hud,bool controller) {
    backdrop(draw,hud,caption.c_str());
    const float x0=560,x1=1360,top=230,step=58; const unsigned rows=11;
    const unsigned count=(unsigned)items.size();
    unsigned first=selected>=rows/2?selected-rows/2:0;
    if (count<=rows) first=0; else if (first>count-rows) first=count-rows;
    draw->AddRectFilled(hud.P(x0-40,top-60),hud.P(x1+40,top+std::min(rows,count)*step+10),IM_COL32(6,10,12,140),6*hud.unit);
    for (unsigned row=0;row<rows && first+row<count;++row) {
        const unsigned index=first+row; bool hover=false;
        if (option_row(draw,hud,(int)index,x0,x1,top+row*step,items[index].c_str(),index==selected,hover)) {
            if (selected==index) finish_locked(0); else selected=index;
            ++revision;
        } else if (hover && selected!=index) { selected=index; ++revision; }
    }
    if (first) HudText(draw,hud,(x0+x1)/2,top-24,26,"\xE2\x96\xB2",dim_color,.5f);
    if (first+rows<count) HudText(draw,hud,(x0+x1)/2,top+rows*step-4,26,"\xE2\x96\xBC",dim_color,.5f);
    if (const float wheel=ImGui::GetIO().MouseWheel) {
        const int next=std::clamp((int)selected+(wheel>0?-1:1),0,(int)count-1);
        if ((unsigned)next!=selected) { selected=(unsigned)next; ++revision; }
    }
    prompt_bar(draw,hud,controller,{{"A","Enter",T("Confirm","Confirmar","決定")},
                                    {"B","Esc",T("Back","Voltar","戻る")},
                                    {"DPAD_UD","arrows",T("Select","Selecionar","選択")}});
}
void message_screen(ImDrawList *draw,const HudSpace &hud,bool controller) {
    draw->AddRectFilled(ImVec2(0,0),ImGui::GetIO().DisplaySize,IM_COL32(4,7,9,150));
    const float x0=520,x1=1400,wrap=x1-x0-60;
    const float text_height=KyoeiUiArt::Font()->CalcTextSizeA(30*hud.unit,FLT_MAX,wrap*hud.unit,caption.c_str()).y/hud.unit;
    const float body=std::max(38.f,text_height+8);
    const float choices=message_choices?message_choices*58.f:70.f;
    const float height=60+body+40+choices, top=540-height/2;
    draw->AddRectFilled(hud.P(x0-30,top-20),hud.P(x1+30,top+height+20),IM_COL32(6,10,12,215),6*hud.unit);
    KyoeiUiArt::Draw(draw,"option_bar",hud.P(x0-30,top-20),hud.P(x1+30,top-16));
    KyoeiUiArt::Draw(draw,"option_frame",hud.P(x0-10,top+10),hud.P(x1+10,top+40+body));
    HudText(draw,hud,x0+20,top+52,30,caption.c_str(),text_color,0,wrap);
    const float y=top+60+body+58;
    if (!message_choices) {
        const float w=x1-x0-120;
        draw->AddRectFilled(hud.P(x0+60,y-24),hud.P(x0+60+w,y-16),IM_COL32(40,48,50,255));
        KyoeiUiArt::Draw(draw,"option_bar",hud.P(x0+60,y-24),hud.P(x0+60+w*float(progress)/100.f,y-16));
        HudText(draw,hud,(x0+x1)/2,y+26,26,T("Please wait...","Aguarde...","しばらくお待ちください"),dim_color,.5f);
        return;
    }
    for (unsigned choice=0;choice<message_choices;++choice) {
        const char *label=message_choices==1?"OK":message_type==1 || message_type==7?
            (choice?T("No","Não","いいえ"):T("Yes","Sim","はい")):(choice?T("Cancel","Cancelar","キャンセル"):"OK");
        bool hover=false;
        if (option_row(draw,hud,(int)choice,x0+220,x1-220,y+choice*58.f,label,choice==selected,hover,.5f)) { selected=choice; finish_locked(0); }
        else if (hover && selected!=choice) { selected=choice; ++revision; }
    }
    prompt_bar(draw,hud,controller,{{"A","Enter",T("Confirm","Confirmar","決定")},
                                    {"B","Esc",T("Back","Voltar","戻る")}});
}
void name_screen(ImDrawList *draw,const HudSpace &hud,bool controller) {
    backdrop(draw,hud,T("Character Name","Nome do personagem","名前入力"));
    const float x0=460,x1=1460,top=200;
    draw->AddRectFilled(hud.P(x0-40,top-40),hud.P(x1+40,controller?720:520),IM_COL32(6,10,12,150),6*hud.unit);
    if (!caption.empty()) HudText(draw,hud,x0,top,28,caption.c_str(),dim_color,0,x1-x0);
    // Name field in the native frame; the text scrolls to keep the cursor visible.
    const float fy=top+40, fh=86;
    KyoeiUiArt::Draw(draw,"option_frame",hud.P(x0,fy),hud.P(x1,fy+fh));
    const auto value=utf8((const wchar_t *)name.text);
    std::wstring prefix((const wchar_t *)name.text,name.cursor);
    const float cursor=HudTextWidth(hud,40,utf8(prefix.c_str()).c_str());
    const float shift=std::max(0.f,cursor-(x1-x0-60));
    draw->PushClipRect(hud.P(x0+20,fy),hud.P(x1-20,fy+fh),true);
    if (name.select_all) draw->AddRectFilled(hud.P(x0+28-shift,fy+20),hud.P(x0+32-shift+HudTextWidth(hud,40,value.c_str()),fy+68),IM_COL32(30,110,110,200));
    HudText(draw,hud,x0+30-shift,fy+58,40,value.c_str());
    if (!name.select_all && std::fmod(ImGui::GetTime(),1.0)<.6)
        draw->AddLine(hud.P(x0+32-shift+cursor,fy+22),hud.P(x0+32-shift+cursor,fy+66),text_color,2*hud.unit);
    draw->PopClipRect();
    char counter[64];
    if (kyoei_name_is_roman_default(&name)) std::snprintf(counter,sizeof counter,"%s",T("Default name","Nome padrão","デフォルトの名前"));
    else std::snprintf(counter,sizeof counter,"%u / %u",(unsigned)name.length,(unsigned)(name.extended?name.limit:name.native_limit));
    HudText(draw,hud,x1,fy+fh+36,24,counter,dim_color,1);
    if (!kyoei_name_can_accept(&name)) {
        char limit[128]; std::snprintf(limit,sizeof limit,T("Up to %u characters, or keep the default name.",
            "Até %u caracteres, ou mantenha o nome padrão.","%u文字以内で入力するか、デフォルトの名前のままにしてください。"),(unsigned)name.native_limit);
        HudText(draw,hud,x0,fy+fh+36,24,limit,IM_COL32(240,150,120,255));
    }
    if (!controller) {
        HudText(draw,hud,x0,fy+fh+90,26,T("Type your name.","Digite seu nome.","名前を入力してください。"),dim_color);
        prompt_bar(draw,hud,false,{{nullptr,"Enter",T("Confirm","Confirmar","決定")},{nullptr,"Esc",T("Cancel","Cancelar","キャンセル")},
            {nullptr,"Backspace",T("Erase","Apagar","削除")},{nullptr,"arrows",T("Cursor","Cursor","カーソル")}});
        return;
    }
    const float ky=fy+fh+80, kw=(x1-x0)/10, kh=72;
    for (unsigned key=0;key<40;++key) {
        const float kx=x0+(key%10)*kw, kt=ky+(key/10)*kh;
        std::string label;
        if (key<26) label=char((name.upper?'A':'a')+key);
        else if (key<36) label=char('0'+key-26);
        else label=key==36?T("Space","Espaço","スペース"):key==37?T("Shift","Maiúsc.","大/小"):key==38?T("Erase","Apagar","削除"):T("Done","Pronto","決定");
        char id[16]; std::snprintf(id,sizeof id,"##key%u",key);
        if (hit(hud,id,kx+4,kt+4,kx+kw-4,kt+kh-4)) { name.key=key; kyoei_name_activate(&name); ++revision; if (name.result) finish_locked(0); }
        else if (hovered_now() && name.key!=key) { name.key=key; ++revision; }
        const bool chosen=key==name.key;
        draw->AddRectFilled(hud.P(kx+4,kt+4),hud.P(kx+kw-4,kt+kh-4),chosen?IM_COL32(24,92,92,230):IM_COL32(16,24,28,200),4*hud.unit);
        if (chosen) KyoeiUiArt::Draw(draw,"option_bar",hud.P(kx+4,kt+kh-10),hud.P(kx+kw-4,kt+kh-6));
        HudText(draw,hud,kx+kw/2,kt+kh/2+12,label.size()>1?24:32,label.c_str(),chosen?text_color:dim_color,.5f);
    }
    prompt_bar(draw,hud,true,{{"A",nullptr,T("Enter","Inserir","入力")},{"B",nullptr,T("Cancel","Cancelar","キャンセル")},
        {"X",nullptr,T("Erase","Apagar","削除")},{"Y",nullptr,T("Shift","Maiúsc.","大/小")},
        {"LB",nullptr,T("Cursor","Cursor","カーソル")},{"MENU",nullptr,T("Done","Pronto","決定")}});
}
}

extern "C" int kyoei_overlay_begin_name(const wchar_t *title,const uint16_t *text,size_t length,size_t limit) {
    std::scoped_lock lock{mutex};
    if (!available_locked() || !kyoei_name_begin(&name,text,length,limit)) return 0;
    const char *language=std::getenv("KYOEI_LANGUAGE");
    if (limit==4 && language && !std::strcmp(language,"en")) kyoei_name_show_roman(&name);
    if(limit==4 && language && !std::strcmp(language,"en") && kyoei_pc_names_enabled()) {
        uint16_t expanded[25];
        if(kyoei_pc_name_lookup(text,length,expanded)) {
            size_t n=0;while(expanded[n])++n;
            memcpy(name.text,expanded,(n+1)*2);name.length=name.cursor=n;name.select_all=1;
        }
        name.extended=1;name.limit=24;
    }
    caption=ui_text(title); done=false; end_status=2; kind=KYOEI_OVERLAY_NAME; ++revision; return 1;
}
extern "C" int kyoei_overlay_name_result(uint16_t *text,size_t capacity,unsigned *result) {
    std::scoped_lock lock{mutex};
    if (kind!=KYOEI_OVERLAY_NAME || !done || !result || !text || capacity<=kyoei_name_stored_length(&name)) return 0;
    *result=end_status;
    if (!end_status && !kyoei_name_export(&name,text,capacity)) return 0;
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
ImFont *hud_font;
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
    // The PC screens in the game's style use a heavier face, close to the game's labels
    // (Segoe UI Semibold when installed), with the same Japanese fallback.
    hud_font=nullptr;
    if (FILE *probe=std::fopen("C:/Windows/Fonts/seguisb.ttf","rb")) {
        std::fclose(probe);
        hud_font=io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/seguisb.ttf",20);
        if (hud_font && font_directory && *font_directory) {
            ImFontConfig config; config.MergeMode=true;
            io.Fonts->AddFontFromFileTTF((std::string(font_directory)+"/NotoSansCJK-Regular.ttc").c_str(),20,&config);
        }
    }
    if (headless) { io.Fonts->Build(); io.Fonts->SetTexID(ImTextureID(1)); }
    scale=1;
}
void SetReady(bool value) {
    std::scoped_lock lock{mutex}; ready=value;
    if (!value) finish_locked(2);
}
void release_title_art();
void release_ui_art();
void ShutdownGui() {
    std::scoped_lock lock{mutex}; ready=false; finish_locked(2);
    release_title_art(); release_ui_art();
    ImGui::DestroyContext();
}
// The PC title item is drawn from art made in the native item style
// (tools/make_title_exit_art.py): a sharp layer plus a blurred focus layer,
// placed and faded from the game's own menu nodes (HUD space 1920x1080).
constexpr float TITLE_ART_DX=0, TITLE_ART_DY=0; // calibrated against the native items

// Both layers live in the font atlas (custom rects) so they upload through the
// same path as the glyphs; a separate user texture upload faulted in the driver.
ImFontAtlasRectId title_art[2]={ImFontAtlasRectId_Invalid,ImFontAtlasRectId_Invalid};
bool title_art_loaded;
int title_art_width, title_art_height;
void load_title_art() {
    title_art_loaded=true;
    const char *path=std::getenv("KYOEI_TITLE_EXIT_ART");
    if (!path || !*path) return;
    FILE *file=std::fopen(path,"rb");
    if (!file) return;
    unsigned char head[16]; uint32_t w=0,h=0;
    bool ok=std::fread(head,1,16,file)==16 && !std::memcmp(head,"KYART1\0\0",8);
    if (ok) { std::memcpy(&w,head+8,4); std::memcpy(&h,head+12,4); ok=w && h && w<=1024 && h<=256; }
    std::vector<unsigned char> pixels(ok?(size_t)w*h*8:0);
    ok=ok && std::fread(pixels.data(),1,pixels.size(),file)==pixels.size();
    std::fclose(file);
    if (!ok) return;
    ImFontAtlas *atlas=ImGui::GetIO().Fonts;
    atlas->TexPixelsUseColors=true;
    for (int layer=0; layer<2; ++layer) {
        ImFontAtlasRect r;
        title_art[layer]=atlas->AddCustomRect((int)w,(int)h,&r);
        if (title_art[layer]==ImFontAtlasRectId_Invalid || atlas->TexData->Format!=ImTextureFormat_RGBA32) return;
        for (uint32_t y=0; y<h; ++y)
            std::memcpy(atlas->TexData->GetPixelsAt(r.x,r.y+(int)y),pixels.data()+((size_t)layer*h+y)*w*4,(size_t)w*4);
        ImFontAtlasTextureBlockQueueUpload(atlas,atlas->TexData,r.x,r.y,r.w,r.h);
    }
    title_art_width=(int)w; title_art_height=(int)h;
}
void release_title_art() {
    title_art[0]=title_art[1]=ImFontAtlasRectId_Invalid;
    title_art_loaded=false; title_art_width=0;
}
// KYART2 (tools/make_ui_art.py): named pieces for the PC screens, loaded once into the atlas.
struct UiPiece { ImFontAtlasRectId id; int w, h; };
std::unordered_map<std::string,UiPiece> ui_art;
bool ui_art_loaded;
void load_ui_art() {
    ui_art_loaded=true;
    const char *path=std::getenv("KYOEI_UI_ART");
    if (!path || !*path) return;
    FILE *file=std::fopen(path,"rb");
    if (!file) { gz_log("Kyoei UI art: missing %s",path); return; }
    unsigned char head[12]; uint32_t count=0;
    bool ok=std::fread(head,1,12,file)==12 && !std::memcmp(head,"KYART2\0\0",8);
    if (ok) { std::memcpy(&count,head+8,4); ok=count && count<=256; }
    ImFontAtlas *atlas=ImGui::GetIO().Fonts;
    atlas->TexPixelsUseColors=true;
    std::vector<unsigned char> pixels;
    for (uint32_t i=0; ok && i<count; ++i) {
        char name[25]={}; uint32_t size[2]={};
        ok=std::fread(name,1,24,file)==24 && std::fread(size,4,2,file)==2 && size[0] && size[1] &&
           size[0]<=1024 && size[1]<=512;
        if (!ok) break;
        pixels.resize((size_t)size[0]*size[1]*4);
        ok=std::fread(pixels.data(),1,pixels.size(),file)==pixels.size();
        ImFontAtlasRect r;
        ImFontAtlasRectId id=ok?atlas->AddCustomRect((int)size[0],(int)size[1],&r):ImFontAtlasRectId_Invalid;
        if (id==ImFontAtlasRectId_Invalid || atlas->TexData->Format!=ImTextureFormat_RGBA32) { ok=false; break; }
        for (uint32_t y=0; y<size[1]; ++y)
            std::memcpy(atlas->TexData->GetPixelsAt(r.x,r.y+(int)y),pixels.data()+(size_t)y*size[0]*4,(size_t)size[0]*4);
        ImFontAtlasTextureBlockQueueUpload(atlas,atlas->TexData,r.x,r.y,r.w,r.h);
        ui_art[name]={id,(int)size[0],(int)size[1]};
    }
    std::fclose(file);
    gz_log("Kyoei UI art: %zu pieces%s",ui_art.size(),ok?"":" (file damaged; the rest is skipped)");
}
void release_ui_art() { ui_art.clear(); ui_art_loaded=false; }
void title_exit(unsigned width,unsigned height) {
    KyoeiTitleItem item;
    if (!title_art_width || !kyoei_title_exit_item(&item)) return;
    // 16:9 HUD letterboxed into the swapchain, as the game composes it.
    float unit=std::min(float(width)/1920.f,float(height)/1080.f);
    float left=(float(width)-1920*unit)/2, top=(float(height)-1080*unit)/2;
    ImVec2 at(left+(item.x+TITLE_ART_DX)*unit, top+(item.y+TITLE_ART_DY)*unit);
    ImVec2 end(at.x+title_art_width*unit, at.y+title_art_height*unit);
    auto *draw=ImGui::GetBackgroundDrawList();
    int alpha=(int)(std::clamp(item.alpha,0.f,1.f)*255);
    int glow=(int)(std::clamp(item.focus,0.f,1.f)*alpha);
    ImFontAtlas *atlas=ImGui::GetIO().Fonts;
    ImFontAtlasRect normal,focus;
    if (!atlas->GetCustomRect(title_art[0],&normal) || !atlas->GetCustomRect(title_art[1],&focus)) return;
    if (glow>0) draw->AddImage(atlas->TexRef,at,end,focus.uv0,focus.uv1,IM_COL32(255,255,255,glow));
    draw->AddImage(atlas->TexRef,at,end,normal.uv0,normal.uv1,IM_COL32(255,255,255,alpha));
}
}
namespace KyoeiOverlayModel {
ImDrawData *Draw(unsigned width,unsigned height,float seconds,void (*extra)()) {
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
    if (!title_art_loaded) load_title_art();
    if (!ui_art_loaded) load_ui_art();
    ImGui::NewFrame();
    if (kind && !done) {
        bool controller=kyoei_input_current_mode()==KYOEI_INPUT_CONTROLLER;
        // A transparent full-screen window only carries the mouse hit boxes.
        ImGui::SetNextWindowPos(ImVec2(0,0)); ImGui::SetNextWindowSize(ImVec2(float(width),float(height)));
        ImGui::Begin("##kyoei-modal",nullptr,ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings|
                     ImGuiWindowFlags_NoBackground|ImGuiWindowFlags_NoScrollWithMouse);
        auto *draw=ImGui::GetBackgroundDrawList(); const HudSpace hud=Hud();
        if (kind==KYOEI_OVERLAY_NAME) name_screen(draw,hud,controller);
        else if (kind==KYOEI_OVERLAY_LIST) list_screen(draw,hud,controller);
        else message_screen(draw,hud,controller);
        ImGui::End();
    } else title_exit(width,height);
    if(extra)extra();
    ImGui::Render(); return ImGui::GetDrawData();
}
}
namespace KyoeiUiArt {
bool Draw(ImDrawList *draw,const char *name,ImVec2 min,ImVec2 max,ImU32 tint) {
    const auto it=KyoeiOverlayModel::ui_art.find(name);
    if (it==KyoeiOverlayModel::ui_art.end()) return false;
    ImFontAtlas *atlas=ImGui::GetIO().Fonts; ImFontAtlasRect r;
    if (!atlas->GetCustomRect(it->second.id,&r)) return false;
    draw->AddImage(atlas->TexRef,min,max,r.uv0,r.uv1,tint);
    return true;
}
ImVec2 Size(const char *name) {
    const auto it=KyoeiOverlayModel::ui_art.find(name);
    return it==KyoeiOverlayModel::ui_art.end()?ImVec2(0,0):ImVec2((float)it->second.w,(float)it->second.h);
}
ImFont *Font() { return KyoeiOverlayModel::hud_font?KyoeiOverlayModel::hud_font:ImGui::GetFont(); }
const char *Pad(const char *button) {
    static thread_local std::string name;
    const unsigned family=kyoei_input_current_family();
    name=std::string(family==KYOEI_PAD_XBOX?"xbox_":family==KYOEI_PAD_NINTENDO?"nintendo_":"ps_")+button;
    return name.c_str();
}
}
