// Exercise real modal implementation/ImGui without SDL, Vulkan, game or saves.
#include "kyoei_overlay.h"
#include "kyoei_input_mode.h"
#include "imgui.h"
#include "kyoei_photo_panel.h"
#include "imgui_internal.h"
#include <windows.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <future>
#include <thread>
#include <vector>
#include <string>
#define CHECK(x) do { if (!(x)) { std::fprintf(stderr,"line %d: %s\n",__LINE__,#x); std::exit(1); } } while(0)
static void wait_kind(int expected) {
    for (unsigned attempt=0;attempt<1000;++attempt) {
        if (kyoei_overlay_kind()==expected) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    CHECK(false);
}
static void check_draw(unsigned width,unsigned height,bool scroll_free=false) {
    for (int frame=0;frame<2;++frame) {
        auto *draw=KyoeiOverlayModel::Draw(width,height,1.f/60);
        CHECK(draw && draw->Valid && draw->TotalVtxCount>0);
        CHECK(draw->DisplaySize.x==float(width) && draw->DisplaySize.y==float(height));
        for (const auto *list:draw->CmdLists) for (const auto &command:list->CmdBuffer) {
            CHECK(std::isfinite(command.ClipRect.x) && command.ClipRect.x>=0);
            CHECK(command.ClipRect.y>=0 && command.ClipRect.z<=width && command.ClipRect.w<=height);
        }
    }
    auto *window=ImGui::FindWindowByName("##kyoei-modal");
    CHECK(window && window->Pos.x>=0 && window->Pos.y>=0);
    CHECK(window->Pos.x+window->Size.x<=width && window->Pos.y+window->Size.y<=height);
    if (scroll_free) CHECK(window->ScrollMax.y<1);
}
int main() {
    _set_error_mode(_OUT_TO_STDERR);
    _set_abort_behavior(0,_WRITE_ABORT_MSG|_CALL_REPORTFAULT);
    _putenv_s("KYOEI_IN_GAME_DIALOGS","1"); _putenv_s("KYOEI_LANGUAGE","en");
    KyoeiOverlayModel::InitializeGui(nullptr,true); KyoeiOverlayModel::SetReady(true);
    CHECK(KyoeiPhoto::CurrentCaptureNotice().message.empty());
    KyoeiPhoto::ReportCapture(false);CHECK(KyoeiPhoto::CurrentCaptureNotice().error);
    KyoeiPhoto::ReportCapture(true,"synthetic.png",1920,1080);
    CHECK(!KyoeiPhoto::CurrentCaptureNotice().error &&
        KyoeiPhoto::CurrentCaptureNotice().message.find("1920 x 1080")!=std::string::npos);
    for(bool lights:{false,true})for(bool prototype:{false,true})for(bool controller:{false,true})for(unsigned height:{720u,1080u})for(int group=0;group<4;++group) {
        KyoeiPhoto::experimental_lights=lights;
        KyoeiPhoto::experimental_dof=prototype;
        KyoeiPhoto::CurrentLight().type=1;KyoeiPhoto::light_control=lights;
        if(!KyoeiPhoto::SetGroup(group))continue;
        ImDrawData *photo=nullptr;
        // Exercise the exact photo panel within the real ImGui frame callback.
        for(int frame=0;frame<2;++frame) {
            if(controller)photo=KyoeiOverlayModel::Draw(height*16/9,height,1.f/60,[]{KyoeiPhoto::DrawCaptureNotice();KyoeiPhoto::DrawPanel(true,true);});
            else photo=KyoeiOverlayModel::Draw(height*16/9,height,1.f/60,[]{KyoeiPhoto::DrawCaptureNotice();KyoeiPhoto::DrawPanel(false,false);});
            CHECK(photo && photo->Valid);
            if(frame)CHECK(photo->TotalVtxCount>0); // auto-size windows settle after the first frame
        }
        auto *panel=ImGui::FindWindowByName("##photo-help");
        CHECK(panel && panel->Pos.x>=0 && panel->Pos.y>=0);
        CHECK(panel->Pos.x+panel->Size.x<=height*16/9 && panel->Pos.y+panel->Size.y<=height);
        auto *notice=ImGui::FindWindowByName("##photo-result");
        CHECK(notice && notice->Pos.x>=0 && notice->Pos.y>=0 &&
            notice->Pos.x+notice->Size.x<=height*16/9 && notice->Pos.y+notice->Size.y<=height);
    }
    // Guides render with no panel and stay inside the chosen photographic frame.
    for(unsigned height:{720u,1080u})for(int style=0;style<6;++style)for(int mode=1;mode<4;++mode){
        KyoeiPhoto::frame=style;KyoeiPhoto::guide=mode;
        auto* guide=KyoeiOverlayModel::Draw(height*16/9,height,1.f/60,[]{KyoeiPhoto::DrawCompositionGuide();});
        CHECK(guide && guide->Valid && guide->TotalVtxCount>0);
        const auto r=KyoeiPhoto::VisibleFrame(1920,1080,style);
        const float width=float(height*16/9);
        for(auto* list:guide->CmdLists)for(const auto& v:list->VtxBuffer){
            CHECK(v.pos.x>=r.x*width-3 && v.pos.x<=(r.x+r.w)*width+3);
            CHECK(v.pos.y>=r.y*height-3 && v.pos.y<=(r.y+r.h)*height+3);
        }
    }
    KyoeiPhoto::frame=0;KyoeiPhoto::guide=0;
    KyoeiPhoto::experimental_dof=false;
    KyoeiPhoto::experimental_lights=false;
    KyoeiPhoto::capture_notice_until=std::chrono::steady_clock::now()-std::chrono::seconds(1);
    CHECK(KyoeiPhoto::CurrentCaptureNotice().message.empty());
    const uint16_t initial[]={'K','e','n'};
    kyoei_overlay_input(0);
    CHECK(kyoei_overlay_begin_name(L"Synthetic name",initial,3,4));
    kyoei_input_controller_button(); check_draw(1280,720,true); check_draw(640,360,true);
    const auto controller_vertices=KyoeiOverlayModel::Draw(1280,720,1.f/60)->TotalVtxCount;
    kyoei_input_keyboard();check_draw(1280,720,true);
    const auto keyboard_vertices=KyoeiOverlayModel::Draw(1280,720,1.f/60)->TotalVtxCount;
    CHECK(keyboard_vertices<controller_vertices);
    kyoei_input_controller_button();check_draw(1280,720,true);
    CHECK(KyoeiOverlayModel::Draw(1280,720,1.f/60)->TotalVtxCount==controller_vertices);
    CHECK(kyoei_overlay_input(0x2000)); CHECK(kyoei_overlay_input(0x2000));
    kyoei_overlay_input(0); kyoei_overlay_input(0x2000);
    kyoei_overlay_input(0x1000); // triangle shifts
    kyoei_overlay_input(0x20); kyoei_overlay_input(0x2000); // B
    kyoei_input_keyboard(); CHECK(kyoei_overlay_text("\xc3\xa9"));
    kyoei_overlay_text("too long"); kyoei_overlay_text("\xed\xa0\x80");
    check_draw(1280,720,true);
    uint16_t result[7]={0,0,0,0,0,0xaaaa,0xbbbb}; unsigned status=99;
    CHECK(!kyoei_overlay_name_result(result,7,&status));
    kyoei_overlay_key(KYOEI_OVERLAY_KEY_ACCEPT);
    CHECK(kyoei_overlay_name_result(result,7,&status) && status==0);
    if (result[0]!='a' || result[1]!='a' || result[2]!='B' || result[3]!=0xe9 || result[4])
        std::fprintf(stderr,"synthetic name units: %04x %04x %04x %04x %04x\n",result[0],result[1],result[2],result[3],result[4]);
    CHECK(result[0]=='a' && result[1]=='a' && result[2]=='B' && result[3]==0xe9 && result[4]==0);
    CHECK(result[5]==0xaaaa && result[6]==0xbbbb);
    CHECK(kyoei_overlay_input(0x2000));
    kyoei_overlay_release_name(); CHECK(!kyoei_overlay_kind());
    CHECK(kyoei_overlay_needs_redraw()); kyoei_overlay_presented(kyoei_overlay_revision());
    CHECK(!kyoei_overlay_needs_redraw());
    CHECK(kyoei_overlay_begin_name(L"Abort",initial,3,4)); kyoei_overlay_abort_name();
    CHECK(kyoei_overlay_name_result(result,7,&status) && status==2);
    kyoei_overlay_release_name();
    CHECK(kyoei_overlay_begin_name(L"Cancel",initial,3,4));
    kyoei_overlay_input(0); kyoei_overlay_input(0x4000|0x2000);
    CHECK(kyoei_overlay_name_result(result,7,&status) && status==1);
    kyoei_overlay_release_name();
    std::vector<std::wstring> storage(1025);
    const uint16_t unicode_name[]={'A',0xd83d,0xde00,0xe9,'Z'};
    CHECK(kyoei_overlay_begin_name(L"Unicode editing",unicode_name,5,6));
    kyoei_overlay_key(KYOEI_OVERLAY_KEY_HOME);
    kyoei_overlay_key(KYOEI_OVERLAY_KEY_RIGHT);
    kyoei_overlay_key(KYOEI_OVERLAY_KEY_DELETE);
    kyoei_overlay_key(KYOEI_OVERLAY_KEY_END);
    CHECK(kyoei_overlay_text("!"));
    check_draw(640,360,true);
    kyoei_overlay_key(KYOEI_OVERLAY_KEY_ACCEPT);
    CHECK(kyoei_overlay_name_result(result,7,&status) && status==0);
    CHECK(result[0]=='A' && result[1]==0xe9 && result[2]=='Z' && result[3]=='!' && result[4]==0);
    kyoei_overlay_release_name();
    std::vector<const wchar_t *> labels;
    for (unsigned index=0;index<storage.size();++index) { storage[index]=L"Synthetic slot "+std::to_wstring(index); labels.push_back(storage[index].c_str()); }
    auto list=std::async(std::launch::async,[&]{return kyoei_overlay_select(L"Choose a synthetic slot",labels.data(),1025,1024);});
    wait_kind(KYOEI_OVERLAY_LIST); labels.clear(); storage.clear();
    check_draw(1280,720); kyoei_overlay_key(KYOEI_OVERLAY_KEY_UP);
    kyoei_overlay_key(KYOEI_OVERLAY_KEY_ACCEPT);
    CHECK(list.wait_for(std::chrono::seconds(1))==std::future_status::ready && list.get()==1023);
    const wchar_t *one[]={L"Only slot"};
    auto cancel=std::async(std::launch::async,[&]{return kyoei_overlay_select(L"Cancel list",one,1,0);});
    wait_kind(KYOEI_OVERLAY_LIST); kyoei_overlay_cancel_dialog();
    CHECK(cancel.wait_for(std::chrono::seconds(1))==std::future_status::ready && cancel.get()==-1);
    CHECK(kyoei_overlay_select(L"Invalid",nullptr,0,0)==INT_MIN);
    CHECK(kyoei_overlay_select(L"Invalid",one,1,2)==INT_MIN);
    for (unsigned type:{0u,1u,2u,7u,8u}) {
        kyoei_overlay_prepare_dialog();
        auto message=std::async(std::launch::async,[type]{return kyoei_overlay_message(L"Synthetic confirmation",type);});
        wait_kind(KYOEI_OVERLAY_MESSAGE); check_draw(640,360,true);
        kyoei_overlay_input(0); kyoei_overlay_input(0x2000);
        int expected=type==7?IDNO:type==8?IDCANCEL:IDOK;
        CHECK(message.wait_for(std::chrono::seconds(1))==std::future_status::ready && message.get()==expected);
    }
    auto progress=std::async(std::launch::async,[]{return kyoei_overlay_message(L"Synthetic progress",256);});
    wait_kind(KYOEI_OVERLAY_MESSAGE); kyoei_overlay_progress(150);
    check_draw(1280,720); kyoei_overlay_key(KYOEI_OVERLAY_KEY_ACCEPT);
    CHECK(progress.wait_for(std::chrono::milliseconds(1))==std::future_status::timeout);
    kyoei_overlay_key(KYOEI_OVERLAY_KEY_CANCEL);
    kyoei_overlay_input(0); kyoei_overlay_input(0x4000);
    CHECK(progress.wait_for(std::chrono::milliseconds(1))==std::future_status::timeout);
    CHECK(kyoei_overlay_kind()==KYOEI_OVERLAY_MESSAGE);
    KyoeiOverlayModel::SetReady(false);
    CHECK(progress.wait_for(std::chrono::seconds(1))==std::future_status::ready && progress.get()==0);
    CHECK(!kyoei_overlay_begin_name(L"Unavailable",initial,3,4));
    KyoeiOverlayModel::SetReady(true);
    kyoei_overlay_cancel_dialog();
    CHECK(kyoei_overlay_select(L"Cancelled before open",one,1,0)==-1);
    CHECK(kyoei_overlay_message(L"Cancelled before open",0)==0);
    kyoei_overlay_prepare_dialog();
    _putenv_s("KYOEI_IN_GAME_DIALOGS","0");
    CHECK(kyoei_overlay_select(L"Disabled",one,1,0)==INT_MIN);
    KyoeiOverlayModel::ShutdownGui();
    std::puts("OVERLAY PASS: real ImGui layout, UTF16/results, input edges, 1025 slots, ownership, cancellation and fallback");
}
