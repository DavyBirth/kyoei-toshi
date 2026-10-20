// Capture result appears only in the host UI, after the PNG was written.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <chrono>
#include <mutex>
#include <string>
namespace KyoeiPhoto {
struct CaptureNotice {std::string message;bool error{};};
inline std::mutex capture_notice_mutex;
inline CaptureNotice capture_notice;
inline std::chrono::steady_clock::time_point capture_notice_until;
inline void ReportCapture(bool success,const std::string& name={},unsigned width=0,unsigned height=0){
    std::scoped_lock lock{capture_notice_mutex};
    capture_notice={success?"Foto salva: photos/"+name+" ("+std::to_string(width)+" x "+std::to_string(height)+")":
        "Nao foi possivel salvar a foto.",!success};
    capture_notice_until=std::chrono::steady_clock::now()+std::chrono::seconds(5);
}
inline CaptureNotice CurrentCaptureNotice(){
    std::scoped_lock lock{capture_notice_mutex};
    return std::chrono::steady_clock::now()<capture_notice_until?capture_notice:CaptureNotice{};
}
}
