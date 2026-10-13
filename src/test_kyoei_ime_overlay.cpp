// Invoke the real PS4 ABI adapter and guest filter, without game or native UI.
#include "kyoei_ime.h"
#include "kyoei_overlay.h"
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#define CHECK(x) do { if (!(x)) { std::fprintf(stderr,"line %d: %s\n",__LINE__,#x); std::exit(1); } } while(0)
extern "C" void gz_log(const char *,...) {}
extern "C" void *gz_gpu_window_handle() { return nullptr; }
static unsigned filter_calls,filter_mode;
static DWORD filter_thread;
static ABI int32_t guest_filter(uint16_t *output,uint32_t *length,const uint16_t *input,uint32_t count) {
    ++filter_calls; filter_thread=GetCurrentThreadId();
    CHECK(*length==4 && count==2 && input[0]=='h' && input[1]=='i');
    if (filter_mode==1) { *length=5; return 0; }
    if (filter_mode==2) { output[0]=0xd800; *length=1; return 0; }
    output[0]='H'; output[1]='I'; *length=2; return 0;
}
int main() {
    static_assert(sizeof(KyoeiImeParam)==96 && offsetof(KyoeiImeParam,buffer)==40);
    _set_error_mode(_OUT_TO_STDERR); _set_abort_behavior(0,_WRITE_ABORT_MSG|_CALL_REPORTFAULT);
    _putenv_s("KYOEI_IN_GAME_DIALOGS","1");
    KyoeiOverlayModel::InitializeGui(nullptr,true); KyoeiOverlayModel::SetReady(true);
    typedef int32_t (ABI *Init)(const KyoeiImeParam *,const void *);
    typedef int32_t (ABI *NoArgs)();
    typedef int32_t (ABI *Result)(uint32_t *);
    auto init=(Init)kyoei_ime_resolve("sceImeDialogInit");
    auto status=(NoArgs)kyoei_ime_resolve("sceImeDialogGetStatus");
    auto result=(Result)kyoei_ime_resolve("sceImeDialogGetResult");
    auto abort_entry=(NoArgs)kyoei_ime_resolve("sceImeDialogAbort");
    auto term=(NoArgs)kyoei_ime_resolve("sceImeDialogTerm");
    CHECK(init && status && result && abort_entry && term && !status());
    CHECK(init(nullptr,nullptr)==(int32_t)0x80bc0031);
    CHECK(!kyoei_ime_resolve("synthetic-unknown"));
    for (filter_mode=0;filter_mode<3;++filter_mode) {
        uint16_t buffer[7]={'K','e','n',0,0,0xaaaa,0xbbbb};
        KyoeiImeParam param{}; param.user=1; param.max_length=4; param.buffer=buffer; param.filter=guest_filter;
        filter_calls=0;
        CHECK(!init(&param,nullptr)); CHECK(status()==1);
        uint32_t info[5]={9,9,9,9,0xabcdef01};
        CHECK(result(info)==(int32_t)0x80bc0106 && !filter_calls);
        CHECK(init(&param,nullptr)==(int32_t)0x80bc0001);
        kyoei_overlay_text("hi"); kyoei_overlay_key(KYOEI_OVERLAY_KEY_ACCEPT);
        CHECK(status()==2 && !filter_calls && buffer[0]=='K');
        int32_t returned=result(info);
        CHECK(filter_calls==1 && filter_thread==GetCurrentThreadId());
        if (!filter_mode) {
            CHECK(!returned && buffer[0]=='H' && buffer[1]=='I' && buffer[2]==0);
            CHECK(info[0]==0 && info[1]==0 && info[2]==0 && info[3]==0);
        } else CHECK(returned==(int32_t)0x80bc0006 && buffer[0]=='K' && info[0]==9);
        CHECK(buffer[5]==0xaaaa && buffer[6]==0xbbbb && info[4]==0xabcdef01);
        CHECK(!term() && !status() && !kyoei_overlay_kind());
    }
    for (unsigned outcome:{1u,2u}) {
        uint16_t buffer[6]={'D','a','v','y',0,0xaaaa};
        KyoeiImeParam param{}; param.user=1; param.max_length=4; param.buffer=buffer;
        CHECK(!init(&param,nullptr));
        if (outcome==1) kyoei_overlay_key(KYOEI_OVERLAY_KEY_CANCEL); else CHECK(!abort_entry());
        CHECK(status()==2); uint32_t info[4]={0}; CHECK(!result(info) && info[0]==outcome);
        CHECK(buffer[0]=='D' && buffer[3]=='y' && buffer[4]==0 && buffer[5]==0xaaaa);
        CHECK(!term());
    }
    CHECK(term()==(int32_t)0x80bc0106);
    CHECK(result(nullptr)==(int32_t)0x80bc0031);
    uint16_t invalid[5]={0xd800,0};
    KyoeiImeParam bad{}; bad.user=1; bad.max_length=4; bad.buffer=invalid;
    CHECK(init(&bad,nullptr)==(int32_t)0x80bc0006 && !status());
    KyoeiOverlayModel::ShutdownGui();
    std::puts("IME OVERLAY ABI PASS: lifecycle, guest-thread filters, Unicode, guard words, cancel and abort");
}
