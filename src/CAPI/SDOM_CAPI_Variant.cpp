#include <SDOM/CAPI/SDOM_CAPI_Variant.h>
#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t SDOM_Type(const SDOM_Variant* v) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(v))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Type", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<uint8_t>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        const auto value = callResult.v.i < 0 ? std::int64_t{0} : callResult.v.i;
        return static_cast<uint8_t>(value);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        const auto value = callResult.v.d < 0.0 ? 0.0 : callResult.v.d;
        return static_cast<uint8_t>(value);
    }
    return static_cast<uint8_t>(0);
}

bool SDOM_IsNull(const SDOM_Variant* v) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(v))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_IsNull", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_IsBool(const SDOM_Variant* v) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(v))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_IsBool", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_IsInt(const SDOM_Variant* v) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(v))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_IsInt", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_IsFloat(const SDOM_Variant* v) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(v))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_IsFloat", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_IsString(const SDOM_Variant* v) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(v))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_IsString", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

SDOM_Variant SDOM_MakeNull(void) {
    // TODO: marshal return type 'SDOM_Variant'.
    (void)SDOM::CAPI::invokeCallable("SDOM_MakeNull", {});
    return {}; // placeholder
}

SDOM_Variant SDOM_MakeBool(bool b) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeBool(b));

    // TODO: marshal return type 'SDOM_Variant'.
    (void)SDOM::CAPI::invokeCallable("SDOM_MakeBool", args);
    return {}; // placeholder
}

SDOM_Variant SDOM_MakeInt(int64_t i) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(i)));

    // TODO: marshal return type 'SDOM_Variant'.
    (void)SDOM::CAPI::invokeCallable("SDOM_MakeInt", args);
    return {}; // placeholder
}

SDOM_Variant SDOM_MakeFloat(double f) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeDouble(static_cast<double>(f)));

    // TODO: marshal return type 'SDOM_Variant'.
    (void)SDOM::CAPI::invokeCallable("SDOM_MakeFloat", args);
    return {}; // placeholder
}

SDOM_Variant SDOM_MakeCString(const char* utf8) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makeCString(utf8));

    // TODO: marshal return type 'SDOM_Variant'.
    (void)SDOM::CAPI::invokeCallable("SDOM_MakeCString", args);
    return {}; // placeholder
}

bool SDOM_AsBool(const SDOM_Variant* v) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(v))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_AsBool", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

int64_t SDOM_AsInt(const SDOM_Variant* v) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(v))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_AsInt", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<int64_t>(callResult.v.i);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<int64_t>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        return static_cast<int64_t>(callResult.v.d);
    }
    return static_cast<int64_t>(0);
}

double SDOM_AsFloat(const SDOM_Variant* v) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(v))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_AsFloat", args);
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {
        return static_cast<double>(callResult.v.d);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {
        return static_cast<double>(callResult.v.u);
    }
    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {
        return static_cast<double>(callResult.v.i);
    }
    return static_cast<double>(0);
}

const char* SDOM_AsString(const SDOM_Variant* v) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(1);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(v))));

    static thread_local std::string s_result;
    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_AsString", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {
        s_result.clear();
        return s_result.c_str();
    }
    s_result = callResult.s;
    return s_result.c_str();
}

bool SDOM_Variant_GetPath(const SDOM_Variant* root, const char* path, SDOM_Variant* out_value) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(3);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(root))));
    args.push_back(SDOM::CAPI::CallArg::makeCString(path));
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(out_value)));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_GetPath", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_Variant_SetPath(SDOM_Variant* root, const char* path, const SDOM_Variant* value) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(3);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(root)));
    args.push_back(SDOM::CAPI::CallArg::makeCString(path));
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(value))));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_SetPath", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_Variant_PathExists(const SDOM_Variant* root, const char* path) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(root))));
    args.push_back(SDOM::CAPI::CallArg::makeCString(path));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_PathExists", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

bool SDOM_Variant_ErasePath(SDOM_Variant* root, const char* path) {
    std::vector<SDOM::CAPI::CallArg> args;
    args.reserve(2);
    args.push_back(SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(root)));
    args.push_back(SDOM::CAPI::CallArg::makeCString(path));

    const auto callResult = SDOM::CAPI::invokeCallable("SDOM_Variant_ErasePath", args);
    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {
        return false;
    }
    return callResult.v.b;
}

#ifdef __cplusplus
} // extern "C"
#endif
