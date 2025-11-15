#include <SDOM/CAPI/SDOM_CAPI_Common.h>
#include <SDOM/CAPI/sdom_capi_objects_generated.h>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <cstring>

extern "C" {
const char* SDOM_EventType__SDL_Event_getcaptures(const SDOM_EventType__SDL_Event* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::SDL_Event.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__SDL_Event_setcaptures(SDOM_EventType__SDL_Event* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::SDL_Event.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__SDL_Event_getbubbles(const SDOM_EventType__SDL_Event* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::SDL_Event.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__SDL_Event_setbubbles(SDOM_EventType__SDL_Event* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::SDL_Event.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__SDL_Event_gettarget_only(const SDOM_EventType__SDL_Event* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::SDL_Event.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__SDL_Event_settarget_only(SDOM_EventType__SDL_Event* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::SDL_Event.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__SDL_Event_getglobal(const SDOM_EventType__SDL_Event* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::SDL_Event.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__SDL_Event_setglobal(SDOM_EventType__SDL_Event* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::SDL_Event.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnRender_getcaptures(const SDOM_EventType__OnRender* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnRender.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnRender_setcaptures(SDOM_EventType__OnRender* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnRender.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnRender_getbubbles(const SDOM_EventType__OnRender* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnRender.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnRender_setbubbles(SDOM_EventType__OnRender* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnRender.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnRender_gettarget_only(const SDOM_EventType__OnRender* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnRender.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnRender_settarget_only(SDOM_EventType__OnRender* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnRender.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnRender_getglobal(const SDOM_EventType__OnRender* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnRender.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnRender_setglobal(SDOM_EventType__OnRender* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnRender.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnUpdate_getcaptures(const SDOM_EventType__OnUpdate* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnUpdate.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnUpdate_setcaptures(SDOM_EventType__OnUpdate* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnUpdate.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnUpdate_getbubbles(const SDOM_EventType__OnUpdate* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnUpdate.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnUpdate_setbubbles(SDOM_EventType__OnUpdate* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnUpdate.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnUpdate_gettarget_only(const SDOM_EventType__OnUpdate* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnUpdate.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnUpdate_settarget_only(SDOM_EventType__OnUpdate* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnUpdate.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnUpdate_getglobal(const SDOM_EventType__OnUpdate* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnUpdate.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnUpdate_setglobal(SDOM_EventType__OnUpdate* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnUpdate.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnQuit_getcaptures(const SDOM_EventType__OnQuit* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnQuit.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnQuit_setcaptures(SDOM_EventType__OnQuit* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnQuit.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnQuit_getbubbles(const SDOM_EventType__OnQuit* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnQuit.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnQuit_setbubbles(SDOM_EventType__OnQuit* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnQuit.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnQuit_gettarget_only(const SDOM_EventType__OnQuit* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnQuit.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnQuit_settarget_only(SDOM_EventType__OnQuit* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnQuit.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnQuit_getglobal(const SDOM_EventType__OnQuit* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnQuit.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnQuit_setglobal(SDOM_EventType__OnQuit* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnQuit.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__ClipboardPaste_getcaptures(const SDOM_EventType__ClipboardPaste* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::ClipboardPaste.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__ClipboardPaste_setcaptures(SDOM_EventType__ClipboardPaste* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::ClipboardPaste.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__ClipboardPaste_getbubbles(const SDOM_EventType__ClipboardPaste* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::ClipboardPaste.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__ClipboardPaste_setbubbles(SDOM_EventType__ClipboardPaste* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::ClipboardPaste.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__ClipboardPaste_gettarget_only(const SDOM_EventType__ClipboardPaste* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::ClipboardPaste.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__ClipboardPaste_settarget_only(SDOM_EventType__ClipboardPaste* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::ClipboardPaste.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__ClipboardPaste_getglobal(const SDOM_EventType__ClipboardPaste* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::ClipboardPaste.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__ClipboardPaste_setglobal(SDOM_EventType__ClipboardPaste* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::ClipboardPaste.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerComplete_getcaptures(const SDOM_EventType__TimerComplete* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerComplete.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerComplete_setcaptures(SDOM_EventType__TimerComplete* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerComplete.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerComplete_getbubbles(const SDOM_EventType__TimerComplete* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerComplete.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerComplete_setbubbles(SDOM_EventType__TimerComplete* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerComplete.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerComplete_gettarget_only(const SDOM_EventType__TimerComplete* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerComplete.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerComplete_settarget_only(SDOM_EventType__TimerComplete* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerComplete.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerComplete_getglobal(const SDOM_EventType__TimerComplete* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerComplete.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerComplete_setglobal(SDOM_EventType__TimerComplete* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerComplete.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerCycleComplete_getcaptures(const SDOM_EventType__TimerCycleComplete* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerCycleComplete.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerCycleComplete_setcaptures(SDOM_EventType__TimerCycleComplete* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerCycleComplete.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerCycleComplete_getbubbles(const SDOM_EventType__TimerCycleComplete* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerCycleComplete.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerCycleComplete_setbubbles(SDOM_EventType__TimerCycleComplete* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerCycleComplete.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerCycleComplete_gettarget_only(const SDOM_EventType__TimerCycleComplete* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerCycleComplete.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerCycleComplete_settarget_only(SDOM_EventType__TimerCycleComplete* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerCycleComplete.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerCycleComplete_getglobal(const SDOM_EventType__TimerCycleComplete* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerCycleComplete.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerCycleComplete_setglobal(SDOM_EventType__TimerCycleComplete* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerCycleComplete.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerStop_getcaptures(const SDOM_EventType__TimerStop* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerStop.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerStop_setcaptures(SDOM_EventType__TimerStop* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerStop.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerStop_getbubbles(const SDOM_EventType__TimerStop* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerStop.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerStop_setbubbles(SDOM_EventType__TimerStop* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerStop.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerStop_gettarget_only(const SDOM_EventType__TimerStop* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerStop.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerStop_settarget_only(SDOM_EventType__TimerStop* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerStop.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerStop_getglobal(const SDOM_EventType__TimerStop* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerStop.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerStop_setglobal(SDOM_EventType__TimerStop* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerStop.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__Drop_getcaptures(const SDOM_EventType__Drop* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Drop.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Drop_setcaptures(SDOM_EventType__Drop* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Drop.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__Drop_getbubbles(const SDOM_EventType__Drop* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Drop.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Drop_setbubbles(SDOM_EventType__Drop* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Drop.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__Drop_gettarget_only(const SDOM_EventType__Drop* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Drop.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Drop_settarget_only(SDOM_EventType__Drop* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Drop.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__Drop_getglobal(const SDOM_EventType__Drop* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Drop.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Drop_setglobal(SDOM_EventType__Drop* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Drop.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__Dragging_getcaptures(const SDOM_EventType__Dragging* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Dragging.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Dragging_setcaptures(SDOM_EventType__Dragging* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Dragging.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__Dragging_getbubbles(const SDOM_EventType__Dragging* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Dragging.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Dragging_setbubbles(SDOM_EventType__Dragging* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Dragging.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__Dragging_gettarget_only(const SDOM_EventType__Dragging* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Dragging.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Dragging_settarget_only(SDOM_EventType__Dragging* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Dragging.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__Dragging_getglobal(const SDOM_EventType__Dragging* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Dragging.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Dragging_setglobal(SDOM_EventType__Dragging* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Dragging.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__Hidden_getcaptures(const SDOM_EventType__Hidden* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Hidden.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Hidden_setcaptures(SDOM_EventType__Hidden* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Hidden.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__Hidden_getbubbles(const SDOM_EventType__Hidden* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Hidden.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Hidden_setbubbles(SDOM_EventType__Hidden* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Hidden.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__Hidden_gettarget_only(const SDOM_EventType__Hidden* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Hidden.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Hidden_settarget_only(SDOM_EventType__Hidden* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Hidden.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__Hidden_getglobal(const SDOM_EventType__Hidden* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Hidden.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Hidden_setglobal(SDOM_EventType__Hidden* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Hidden.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__EnterFrame_getcaptures(const SDOM_EventType__EnterFrame* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::EnterFrame.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__EnterFrame_setcaptures(SDOM_EventType__EnterFrame* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::EnterFrame.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__EnterFrame_getbubbles(const SDOM_EventType__EnterFrame* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::EnterFrame.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__EnterFrame_setbubbles(SDOM_EventType__EnterFrame* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::EnterFrame.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__EnterFrame_gettarget_only(const SDOM_EventType__EnterFrame* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::EnterFrame.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__EnterFrame_settarget_only(SDOM_EventType__EnterFrame* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::EnterFrame.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__EnterFrame_getglobal(const SDOM_EventType__EnterFrame* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::EnterFrame.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__EnterFrame_setglobal(SDOM_EventType__EnterFrame* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::EnterFrame.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__Disabled_getcaptures(const SDOM_EventType__Disabled* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Disabled.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Disabled_setcaptures(SDOM_EventType__Disabled* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Disabled.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__Disabled_getbubbles(const SDOM_EventType__Disabled* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Disabled.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Disabled_setbubbles(SDOM_EventType__Disabled* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Disabled.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__Disabled_gettarget_only(const SDOM_EventType__Disabled* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Disabled.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Disabled_settarget_only(SDOM_EventType__Disabled* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Disabled.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__Disabled_getglobal(const SDOM_EventType__Disabled* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Disabled.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Disabled_setglobal(SDOM_EventType__Disabled* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Disabled.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__Drag_getcaptures(const SDOM_EventType__Drag* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Drag.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Drag_setcaptures(SDOM_EventType__Drag* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Drag.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__Drag_getbubbles(const SDOM_EventType__Drag* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Drag.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Drag_setbubbles(SDOM_EventType__Drag* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Drag.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__Drag_gettarget_only(const SDOM_EventType__Drag* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Drag.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Drag_settarget_only(SDOM_EventType__Drag* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Drag.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__Drag_getglobal(const SDOM_EventType__Drag* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Drag.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Drag_setglobal(SDOM_EventType__Drag* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Drag.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__SelectionChanged_getcaptures(const SDOM_EventType__SelectionChanged* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::SelectionChanged.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__SelectionChanged_setcaptures(SDOM_EventType__SelectionChanged* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::SelectionChanged.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__SelectionChanged_getbubbles(const SDOM_EventType__SelectionChanged* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::SelectionChanged.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__SelectionChanged_setbubbles(SDOM_EventType__SelectionChanged* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::SelectionChanged.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__SelectionChanged_gettarget_only(const SDOM_EventType__SelectionChanged* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::SelectionChanged.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__SelectionChanged_settarget_only(SDOM_EventType__SelectionChanged* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::SelectionChanged.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__SelectionChanged_getglobal(const SDOM_EventType__SelectionChanged* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::SelectionChanged.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__SelectionChanged_setglobal(SDOM_EventType__SelectionChanged* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::SelectionChanged.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseButtonUp_getcaptures(const SDOM_EventType__MouseButtonUp* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseButtonUp.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseButtonUp_setcaptures(SDOM_EventType__MouseButtonUp* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseButtonUp.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseButtonUp_getbubbles(const SDOM_EventType__MouseButtonUp* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseButtonUp.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseButtonUp_setbubbles(SDOM_EventType__MouseButtonUp* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseButtonUp.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseButtonUp_gettarget_only(const SDOM_EventType__MouseButtonUp* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseButtonUp.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseButtonUp_settarget_only(SDOM_EventType__MouseButtonUp* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseButtonUp.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseButtonUp_getglobal(const SDOM_EventType__MouseButtonUp* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseButtonUp.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseButtonUp_setglobal(SDOM_EventType__MouseButtonUp* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseButtonUp.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseButtonDown_getcaptures(const SDOM_EventType__MouseButtonDown* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseButtonDown.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseButtonDown_setcaptures(SDOM_EventType__MouseButtonDown* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseButtonDown.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseButtonDown_getbubbles(const SDOM_EventType__MouseButtonDown* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseButtonDown.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseButtonDown_setbubbles(SDOM_EventType__MouseButtonDown* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseButtonDown.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseButtonDown_gettarget_only(const SDOM_EventType__MouseButtonDown* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseButtonDown.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseButtonDown_settarget_only(SDOM_EventType__MouseButtonDown* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseButtonDown.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseButtonDown_getglobal(const SDOM_EventType__MouseButtonDown* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseButtonDown.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseButtonDown_setglobal(SDOM_EventType__MouseButtonDown* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseButtonDown.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__Visible_getcaptures(const SDOM_EventType__Visible* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Visible.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Visible_setcaptures(SDOM_EventType__Visible* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Visible.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__Visible_getbubbles(const SDOM_EventType__Visible* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Visible.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Visible_setbubbles(SDOM_EventType__Visible* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Visible.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__Visible_gettarget_only(const SDOM_EventType__Visible* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Visible.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Visible_settarget_only(SDOM_EventType__Visible* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Visible.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__Visible_getglobal(const SDOM_EventType__Visible* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Visible.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Visible_setglobal(SDOM_EventType__Visible* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Visible.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__FocusLost_getcaptures(const SDOM_EventType__FocusLost* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::FocusLost.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__FocusLost_setcaptures(SDOM_EventType__FocusLost* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::FocusLost.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__FocusLost_getbubbles(const SDOM_EventType__FocusLost* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::FocusLost.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__FocusLost_setbubbles(SDOM_EventType__FocusLost* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::FocusLost.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__FocusLost_gettarget_only(const SDOM_EventType__FocusLost* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::FocusLost.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__FocusLost_settarget_only(SDOM_EventType__FocusLost* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::FocusLost.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__FocusLost_getglobal(const SDOM_EventType__FocusLost* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::FocusLost.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__FocusLost_setglobal(SDOM_EventType__FocusLost* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::FocusLost.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__KeyUp_getcaptures(const SDOM_EventType__KeyUp* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::KeyUp.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__KeyUp_setcaptures(SDOM_EventType__KeyUp* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::KeyUp.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__KeyUp_getbubbles(const SDOM_EventType__KeyUp* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::KeyUp.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__KeyUp_setbubbles(SDOM_EventType__KeyUp* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::KeyUp.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__KeyUp_gettarget_only(const SDOM_EventType__KeyUp* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::KeyUp.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__KeyUp_settarget_only(SDOM_EventType__KeyUp* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::KeyUp.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__KeyUp_getglobal(const SDOM_EventType__KeyUp* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::KeyUp.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__KeyUp_setglobal(SDOM_EventType__KeyUp* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::KeyUp.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__AddedToStage_getcaptures(const SDOM_EventType__AddedToStage* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::AddedToStage.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__AddedToStage_setcaptures(SDOM_EventType__AddedToStage* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::AddedToStage.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__AddedToStage_getbubbles(const SDOM_EventType__AddedToStage* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::AddedToStage.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__AddedToStage_setbubbles(SDOM_EventType__AddedToStage* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::AddedToStage.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__AddedToStage_gettarget_only(const SDOM_EventType__AddedToStage* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::AddedToStage.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__AddedToStage_settarget_only(SDOM_EventType__AddedToStage* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::AddedToStage.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__AddedToStage_getglobal(const SDOM_EventType__AddedToStage* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::AddedToStage.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__AddedToStage_setglobal(SDOM_EventType__AddedToStage* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::AddedToStage.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__RemovedFromStage_getcaptures(const SDOM_EventType__RemovedFromStage* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::RemovedFromStage.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__RemovedFromStage_setcaptures(SDOM_EventType__RemovedFromStage* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::RemovedFromStage.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__RemovedFromStage_getbubbles(const SDOM_EventType__RemovedFromStage* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::RemovedFromStage.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__RemovedFromStage_setbubbles(SDOM_EventType__RemovedFromStage* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::RemovedFromStage.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__RemovedFromStage_gettarget_only(const SDOM_EventType__RemovedFromStage* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::RemovedFromStage.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__RemovedFromStage_settarget_only(SDOM_EventType__RemovedFromStage* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::RemovedFromStage.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__RemovedFromStage_getglobal(const SDOM_EventType__RemovedFromStage* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::RemovedFromStage.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__RemovedFromStage_setglobal(SDOM_EventType__RemovedFromStage* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::RemovedFromStage.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnEvent_getcaptures(const SDOM_EventType__OnEvent* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnEvent.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnEvent_setcaptures(SDOM_EventType__OnEvent* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnEvent.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnEvent_getbubbles(const SDOM_EventType__OnEvent* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnEvent.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnEvent_setbubbles(SDOM_EventType__OnEvent* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnEvent.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnEvent_gettarget_only(const SDOM_EventType__OnEvent* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnEvent.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnEvent_settarget_only(SDOM_EventType__OnEvent* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnEvent.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnEvent_getglobal(const SDOM_EventType__OnEvent* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnEvent.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnEvent_setglobal(SDOM_EventType__OnEvent* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnEvent.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerTick_getcaptures(const SDOM_EventType__TimerTick* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerTick.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerTick_setcaptures(SDOM_EventType__TimerTick* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerTick.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerTick_getbubbles(const SDOM_EventType__TimerTick* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerTick.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerTick_setbubbles(SDOM_EventType__TimerTick* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerTick.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerTick_gettarget_only(const SDOM_EventType__TimerTick* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerTick.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerTick_settarget_only(SDOM_EventType__TimerTick* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerTick.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerTick_getglobal(const SDOM_EventType__TimerTick* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerTick.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerTick_setglobal(SDOM_EventType__TimerTick* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerTick.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__KeyDown_getcaptures(const SDOM_EventType__KeyDown* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::KeyDown.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__KeyDown_setcaptures(SDOM_EventType__KeyDown* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::KeyDown.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__KeyDown_getbubbles(const SDOM_EventType__KeyDown* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::KeyDown.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__KeyDown_setbubbles(SDOM_EventType__KeyDown* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::KeyDown.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__KeyDown_gettarget_only(const SDOM_EventType__KeyDown* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::KeyDown.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__KeyDown_settarget_only(SDOM_EventType__KeyDown* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::KeyDown.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__KeyDown_getglobal(const SDOM_EventType__KeyDown* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::KeyDown.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__KeyDown_setglobal(SDOM_EventType__KeyDown* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::KeyDown.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__StageClosed_getcaptures(const SDOM_EventType__StageClosed* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::StageClosed.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__StageClosed_setcaptures(SDOM_EventType__StageClosed* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::StageClosed.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__StageClosed_getbubbles(const SDOM_EventType__StageClosed* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::StageClosed.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__StageClosed_setbubbles(SDOM_EventType__StageClosed* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::StageClosed.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__StageClosed_gettarget_only(const SDOM_EventType__StageClosed* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::StageClosed.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__StageClosed_settarget_only(SDOM_EventType__StageClosed* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::StageClosed.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__StageClosed_getglobal(const SDOM_EventType__StageClosed* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::StageClosed.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__StageClosed_setglobal(SDOM_EventType__StageClosed* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::StageClosed.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__ClipboardCopy_getcaptures(const SDOM_EventType__ClipboardCopy* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::ClipboardCopy.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__ClipboardCopy_setcaptures(SDOM_EventType__ClipboardCopy* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::ClipboardCopy.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__ClipboardCopy_getbubbles(const SDOM_EventType__ClipboardCopy* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::ClipboardCopy.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__ClipboardCopy_setbubbles(SDOM_EventType__ClipboardCopy* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::ClipboardCopy.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__ClipboardCopy_gettarget_only(const SDOM_EventType__ClipboardCopy* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::ClipboardCopy.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__ClipboardCopy_settarget_only(SDOM_EventType__ClipboardCopy* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::ClipboardCopy.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__ClipboardCopy_getglobal(const SDOM_EventType__ClipboardCopy* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::ClipboardCopy.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__ClipboardCopy_setglobal(SDOM_EventType__ClipboardCopy* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::ClipboardCopy.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__Added_getcaptures(const SDOM_EventType__Added* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Added.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Added_setcaptures(SDOM_EventType__Added* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Added.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__Added_getbubbles(const SDOM_EventType__Added* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Added.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Added_setbubbles(SDOM_EventType__Added* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Added.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__Added_gettarget_only(const SDOM_EventType__Added* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Added.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Added_settarget_only(SDOM_EventType__Added* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Added.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__Added_getglobal(const SDOM_EventType__Added* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Added.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Added_setglobal(SDOM_EventType__Added* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Added.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseMove_getcaptures(const SDOM_EventType__MouseMove* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseMove.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseMove_setcaptures(SDOM_EventType__MouseMove* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseMove.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseMove_getbubbles(const SDOM_EventType__MouseMove* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseMove.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseMove_setbubbles(SDOM_EventType__MouseMove* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseMove.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseMove_gettarget_only(const SDOM_EventType__MouseMove* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseMove.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseMove_settarget_only(SDOM_EventType__MouseMove* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseMove.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseMove_getglobal(const SDOM_EventType__MouseMove* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseMove.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseMove_setglobal(SDOM_EventType__MouseMove* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseMove.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__LeaveFullscreen_getcaptures(const SDOM_EventType__LeaveFullscreen* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::LeaveFullscreen.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__LeaveFullscreen_setcaptures(SDOM_EventType__LeaveFullscreen* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::LeaveFullscreen.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__LeaveFullscreen_getbubbles(const SDOM_EventType__LeaveFullscreen* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::LeaveFullscreen.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__LeaveFullscreen_setbubbles(SDOM_EventType__LeaveFullscreen* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::LeaveFullscreen.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__LeaveFullscreen_gettarget_only(const SDOM_EventType__LeaveFullscreen* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::LeaveFullscreen.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__LeaveFullscreen_settarget_only(SDOM_EventType__LeaveFullscreen* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::LeaveFullscreen.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__LeaveFullscreen_getglobal(const SDOM_EventType__LeaveFullscreen* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::LeaveFullscreen.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__LeaveFullscreen_setglobal(SDOM_EventType__LeaveFullscreen* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::LeaveFullscreen.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__Quit_getcaptures(const SDOM_EventType__Quit* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Quit.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Quit_setcaptures(SDOM_EventType__Quit* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Quit.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__Quit_getbubbles(const SDOM_EventType__Quit* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Quit.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Quit_setbubbles(SDOM_EventType__Quit* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Quit.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__Quit_gettarget_only(const SDOM_EventType__Quit* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Quit.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Quit_settarget_only(SDOM_EventType__Quit* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Quit.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__Quit_getglobal(const SDOM_EventType__Quit* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Quit.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Quit_setglobal(SDOM_EventType__Quit* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Quit.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__None_getcaptures(const SDOM_EventType__None* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::None.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__None_setcaptures(SDOM_EventType__None* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::None.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__None_getbubbles(const SDOM_EventType__None* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::None.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__None_setbubbles(SDOM_EventType__None* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::None.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__None_gettarget_only(const SDOM_EventType__None* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::None.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__None_settarget_only(SDOM_EventType__None* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::None.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__None_getglobal(const SDOM_EventType__None* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::None.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__None_setglobal(SDOM_EventType__None* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::None.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__StageOpened_getcaptures(const SDOM_EventType__StageOpened* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::StageOpened.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__StageOpened_setcaptures(SDOM_EventType__StageOpened* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::StageOpened.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__StageOpened_getbubbles(const SDOM_EventType__StageOpened* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::StageOpened.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__StageOpened_setbubbles(SDOM_EventType__StageOpened* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::StageOpened.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__StageOpened_gettarget_only(const SDOM_EventType__StageOpened* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::StageOpened.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__StageOpened_settarget_only(SDOM_EventType__StageOpened* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::StageOpened.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__StageOpened_getglobal(const SDOM_EventType__StageOpened* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::StageOpened.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__StageOpened_setglobal(SDOM_EventType__StageOpened* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::StageOpened.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseClick_getcaptures(const SDOM_EventType__MouseClick* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseClick.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseClick_setcaptures(SDOM_EventType__MouseClick* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseClick.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseClick_getbubbles(const SDOM_EventType__MouseClick* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseClick.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseClick_setbubbles(SDOM_EventType__MouseClick* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseClick.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseClick_gettarget_only(const SDOM_EventType__MouseClick* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseClick.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseClick_settarget_only(SDOM_EventType__MouseClick* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseClick.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseClick_getglobal(const SDOM_EventType__MouseClick* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseClick.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseClick_setglobal(SDOM_EventType__MouseClick* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseClick.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__User_getcaptures(const SDOM_EventType__User* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::User.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__User_setcaptures(SDOM_EventType__User* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::User.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__User_getbubbles(const SDOM_EventType__User* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::User.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__User_setbubbles(SDOM_EventType__User* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::User.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__User_gettarget_only(const SDOM_EventType__User* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::User.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__User_settarget_only(SDOM_EventType__User* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::User.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__User_getglobal(const SDOM_EventType__User* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::User.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__User_setglobal(SDOM_EventType__User* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::User.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnInit_getcaptures(const SDOM_EventType__OnInit* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnInit.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnInit_setcaptures(SDOM_EventType__OnInit* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnInit.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnInit_getbubbles(const SDOM_EventType__OnInit* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnInit.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnInit_setbubbles(SDOM_EventType__OnInit* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnInit.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnInit_gettarget_only(const SDOM_EventType__OnInit* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnInit.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnInit_settarget_only(SDOM_EventType__OnInit* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnInit.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnInit_getglobal(const SDOM_EventType__OnInit* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnInit.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnInit_setglobal(SDOM_EventType__OnInit* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnInit.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__Removed_getcaptures(const SDOM_EventType__Removed* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Removed.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Removed_setcaptures(SDOM_EventType__Removed* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Removed.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__Removed_getbubbles(const SDOM_EventType__Removed* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Removed.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Removed_setbubbles(SDOM_EventType__Removed* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Removed.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__Removed_gettarget_only(const SDOM_EventType__Removed* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Removed.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Removed_settarget_only(SDOM_EventType__Removed* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Removed.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__Removed_getglobal(const SDOM_EventType__Removed* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Removed.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Removed_setglobal(SDOM_EventType__Removed* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Removed.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__StateChanged_getcaptures(const SDOM_EventType__StateChanged* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::StateChanged.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__StateChanged_setcaptures(SDOM_EventType__StateChanged* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::StateChanged.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__StateChanged_getbubbles(const SDOM_EventType__StateChanged* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::StateChanged.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__StateChanged_setbubbles(SDOM_EventType__StateChanged* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::StateChanged.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__StateChanged_gettarget_only(const SDOM_EventType__StateChanged* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::StateChanged.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__StateChanged_settarget_only(SDOM_EventType__StateChanged* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::StateChanged.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__StateChanged_getglobal(const SDOM_EventType__StateChanged* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::StateChanged.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__StateChanged_setglobal(SDOM_EventType__StateChanged* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::StateChanged.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseDoubleClick_getcaptures(const SDOM_EventType__MouseDoubleClick* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseDoubleClick.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseDoubleClick_setcaptures(SDOM_EventType__MouseDoubleClick* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseDoubleClick.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseDoubleClick_getbubbles(const SDOM_EventType__MouseDoubleClick* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseDoubleClick.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseDoubleClick_setbubbles(SDOM_EventType__MouseDoubleClick* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseDoubleClick.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseDoubleClick_gettarget_only(const SDOM_EventType__MouseDoubleClick* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseDoubleClick.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseDoubleClick_settarget_only(SDOM_EventType__MouseDoubleClick* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseDoubleClick.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseDoubleClick_getglobal(const SDOM_EventType__MouseDoubleClick* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseDoubleClick.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseDoubleClick_setglobal(SDOM_EventType__MouseDoubleClick* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseDoubleClick.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseEnter_getcaptures(const SDOM_EventType__MouseEnter* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseEnter.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseEnter_setcaptures(SDOM_EventType__MouseEnter* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseEnter.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseEnter_getbubbles(const SDOM_EventType__MouseEnter* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseEnter.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseEnter_setbubbles(SDOM_EventType__MouseEnter* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseEnter.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseEnter_gettarget_only(const SDOM_EventType__MouseEnter* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseEnter.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseEnter_settarget_only(SDOM_EventType__MouseEnter* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseEnter.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseEnter_getglobal(const SDOM_EventType__MouseEnter* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseEnter.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseEnter_setglobal(SDOM_EventType__MouseEnter* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseEnter.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnPreRender_getcaptures(const SDOM_EventType__OnPreRender* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnPreRender.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnPreRender_setcaptures(SDOM_EventType__OnPreRender* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnPreRender.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnPreRender_getbubbles(const SDOM_EventType__OnPreRender* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnPreRender.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnPreRender_setbubbles(SDOM_EventType__OnPreRender* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnPreRender.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnPreRender_gettarget_only(const SDOM_EventType__OnPreRender* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnPreRender.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnPreRender_settarget_only(SDOM_EventType__OnPreRender* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnPreRender.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__OnPreRender_getglobal(const SDOM_EventType__OnPreRender* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::OnPreRender.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__OnPreRender_setglobal(SDOM_EventType__OnPreRender* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::OnPreRender.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__FocusGained_getcaptures(const SDOM_EventType__FocusGained* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::FocusGained.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__FocusGained_setcaptures(SDOM_EventType__FocusGained* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::FocusGained.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__FocusGained_getbubbles(const SDOM_EventType__FocusGained* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::FocusGained.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__FocusGained_setbubbles(SDOM_EventType__FocusGained* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::FocusGained.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__FocusGained_gettarget_only(const SDOM_EventType__FocusGained* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::FocusGained.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__FocusGained_settarget_only(SDOM_EventType__FocusGained* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::FocusGained.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__FocusGained_getglobal(const SDOM_EventType__FocusGained* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::FocusGained.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__FocusGained_setglobal(SDOM_EventType__FocusGained* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::FocusGained.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerPause_getcaptures(const SDOM_EventType__TimerPause* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerPause.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerPause_setcaptures(SDOM_EventType__TimerPause* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerPause.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerPause_getbubbles(const SDOM_EventType__TimerPause* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerPause.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerPause_setbubbles(SDOM_EventType__TimerPause* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerPause.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerPause_gettarget_only(const SDOM_EventType__TimerPause* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerPause.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerPause_settarget_only(SDOM_EventType__TimerPause* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerPause.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerPause_getglobal(const SDOM_EventType__TimerPause* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerPause.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerPause_setglobal(SDOM_EventType__TimerPause* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerPause.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerStart_getcaptures(const SDOM_EventType__TimerStart* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerStart.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerStart_setcaptures(SDOM_EventType__TimerStart* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerStart.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerStart_getbubbles(const SDOM_EventType__TimerStart* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerStart.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerStart_setbubbles(SDOM_EventType__TimerStart* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerStart.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerStart_gettarget_only(const SDOM_EventType__TimerStart* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerStart.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerStart_settarget_only(SDOM_EventType__TimerStart* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerStart.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__TimerStart_getglobal(const SDOM_EventType__TimerStart* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TimerStart.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TimerStart_setglobal(SDOM_EventType__TimerStart* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TimerStart.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__Hide_getcaptures(const SDOM_EventType__Hide* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Hide.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Hide_setcaptures(SDOM_EventType__Hide* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Hide.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__Hide_getbubbles(const SDOM_EventType__Hide* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Hide.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Hide_setbubbles(SDOM_EventType__Hide* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Hide.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__Hide_gettarget_only(const SDOM_EventType__Hide* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Hide.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Hide_settarget_only(SDOM_EventType__Hide* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Hide.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__Hide_getglobal(const SDOM_EventType__Hide* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Hide.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Hide_setglobal(SDOM_EventType__Hide* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Hide.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__TextInput_getcaptures(const SDOM_EventType__TextInput* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TextInput.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TextInput_setcaptures(SDOM_EventType__TextInput* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TextInput.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__TextInput_getbubbles(const SDOM_EventType__TextInput* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TextInput.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TextInput_setbubbles(SDOM_EventType__TextInput* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TextInput.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__TextInput_gettarget_only(const SDOM_EventType__TextInput* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TextInput.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TextInput_settarget_only(SDOM_EventType__TextInput* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TextInput.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__TextInput_getglobal(const SDOM_EventType__TextInput* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::TextInput.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__TextInput_setglobal(SDOM_EventType__TextInput* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::TextInput.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__Resize_getcaptures(const SDOM_EventType__Resize* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Resize.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Resize_setcaptures(SDOM_EventType__Resize* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Resize.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__Resize_getbubbles(const SDOM_EventType__Resize* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Resize.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Resize_setbubbles(SDOM_EventType__Resize* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Resize.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__Resize_gettarget_only(const SDOM_EventType__Resize* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Resize.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Resize_settarget_only(SDOM_EventType__Resize* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Resize.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__Resize_getglobal(const SDOM_EventType__Resize* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Resize.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Resize_setglobal(SDOM_EventType__Resize* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Resize.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__ValueChanged_getcaptures(const SDOM_EventType__ValueChanged* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::ValueChanged.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__ValueChanged_setcaptures(SDOM_EventType__ValueChanged* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::ValueChanged.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__ValueChanged_getbubbles(const SDOM_EventType__ValueChanged* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::ValueChanged.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__ValueChanged_setbubbles(SDOM_EventType__ValueChanged* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::ValueChanged.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__ValueChanged_gettarget_only(const SDOM_EventType__ValueChanged* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::ValueChanged.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__ValueChanged_settarget_only(SDOM_EventType__ValueChanged* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::ValueChanged.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__ValueChanged_getglobal(const SDOM_EventType__ValueChanged* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::ValueChanged.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__ValueChanged_setglobal(SDOM_EventType__ValueChanged* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::ValueChanged.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseLeave_getcaptures(const SDOM_EventType__MouseLeave* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseLeave.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseLeave_setcaptures(SDOM_EventType__MouseLeave* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseLeave.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseLeave_getbubbles(const SDOM_EventType__MouseLeave* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseLeave.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseLeave_setbubbles(SDOM_EventType__MouseLeave* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseLeave.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseLeave_gettarget_only(const SDOM_EventType__MouseLeave* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseLeave.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseLeave_settarget_only(SDOM_EventType__MouseLeave* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseLeave.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseLeave_getglobal(const SDOM_EventType__MouseLeave* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseLeave.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseLeave_setglobal(SDOM_EventType__MouseLeave* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseLeave.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__Move_getcaptures(const SDOM_EventType__Move* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Move.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Move_setcaptures(SDOM_EventType__Move* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Move.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__Move_getbubbles(const SDOM_EventType__Move* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Move.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Move_setbubbles(SDOM_EventType__Move* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Move.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__Move_gettarget_only(const SDOM_EventType__Move* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Move.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Move_settarget_only(SDOM_EventType__Move* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Move.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__Move_getglobal(const SDOM_EventType__Move* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Move.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Move_setglobal(SDOM_EventType__Move* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Move.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__Enabled_getcaptures(const SDOM_EventType__Enabled* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Enabled.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Enabled_setcaptures(SDOM_EventType__Enabled* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Enabled.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__Enabled_getbubbles(const SDOM_EventType__Enabled* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Enabled.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Enabled_setbubbles(SDOM_EventType__Enabled* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Enabled.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__Enabled_gettarget_only(const SDOM_EventType__Enabled* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Enabled.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Enabled_settarget_only(SDOM_EventType__Enabled* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Enabled.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__Enabled_getglobal(const SDOM_EventType__Enabled* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Enabled.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Enabled_setglobal(SDOM_EventType__Enabled* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Enabled.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseWheel_getcaptures(const SDOM_EventType__MouseWheel* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseWheel.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseWheel_setcaptures(SDOM_EventType__MouseWheel* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseWheel.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseWheel_getbubbles(const SDOM_EventType__MouseWheel* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseWheel.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseWheel_setbubbles(SDOM_EventType__MouseWheel* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseWheel.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseWheel_gettarget_only(const SDOM_EventType__MouseWheel* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseWheel.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseWheel_settarget_only(SDOM_EventType__MouseWheel* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseWheel.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__MouseWheel_getglobal(const SDOM_EventType__MouseWheel* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::MouseWheel.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__MouseWheel_setglobal(SDOM_EventType__MouseWheel* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::MouseWheel.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__EnterFullscreen_getcaptures(const SDOM_EventType__EnterFullscreen* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::EnterFullscreen.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__EnterFullscreen_setcaptures(SDOM_EventType__EnterFullscreen* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::EnterFullscreen.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__EnterFullscreen_getbubbles(const SDOM_EventType__EnterFullscreen* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::EnterFullscreen.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__EnterFullscreen_setbubbles(SDOM_EventType__EnterFullscreen* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::EnterFullscreen.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__EnterFullscreen_gettarget_only(const SDOM_EventType__EnterFullscreen* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::EnterFullscreen.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__EnterFullscreen_settarget_only(SDOM_EventType__EnterFullscreen* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::EnterFullscreen.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__EnterFullscreen_getglobal(const SDOM_EventType__EnterFullscreen* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::EnterFullscreen.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__EnterFullscreen_setglobal(SDOM_EventType__EnterFullscreen* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::EnterFullscreen.set.global", args);
    } catch(...) {}
}

const char* SDOM_EventType__Show_getcaptures(const SDOM_EventType__Show* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Show.get.captures", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Show_setcaptures(SDOM_EventType__Show* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Show.set.captures", args);
    } catch(...) {}
}

const char* SDOM_EventType__Show_getbubbles(const SDOM_EventType__Show* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Show.get.bubbles", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Show_setbubbles(SDOM_EventType__Show* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Show.set.bubbles", args);
    } catch(...) {}
}

const char* SDOM_EventType__Show_gettarget_only(const SDOM_EventType__Show* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Show.get.target_only", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Show_settarget_only(SDOM_EventType__Show* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Show.set.target_only", args);
    } catch(...) {}
}

const char* SDOM_EventType__Show_getglobal(const SDOM_EventType__Show* obj) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        SDOM::CAPI::CallResult r = SDOM::CAPI::invokeCallable("EventType::Show.get.global", args);
        if (r.kind == SDOM::CAPI::CallArg::Kind::CString) return SDOM_StrDup(r.s.c_str());
        if (r.kind == SDOM::CAPI::CallArg::Kind::Ptr && r.v.p) return (const char*)r.v.p;
        return nullptr;
    } catch(...) { return nullptr; }
}

void SDOM_EventType__Show_setglobal(SDOM_EventType__Show* obj, const char* v) {
    try {
        std::vector<SDOM::CAPI::CallArg> args;
        args.push_back(SDOM::CAPI::CallArg::makePtr((void*)obj));
        args.push_back(SDOM::CAPI::CallArg::makeCString(v));
        (void)SDOM::CAPI::invokeCallable("EventType::Show.set.global", args);
    } catch(...) {}
}

} // extern C
