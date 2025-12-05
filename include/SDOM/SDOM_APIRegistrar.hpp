#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <SDOM/SDOM_DataRegistry.hpp>

namespace SDOM::API {

// Base helper for API registration modules (CoreAPI, IDisplayObjectAPI, etc.).
// Keeps common helper utilities in one place so individual registrars stay small.
class IAPIRegistrar {
public:
    virtual ~IAPIRegistrar() = default;
    virtual void registerBindings() = 0;

    using CallArg = SDOM::CAPI::CallArg;
    using CallResult = SDOM::CAPI::CallResult;

    static CallResult makeBoolResult(bool value) {
        return CallResult::FromBool(value);
    }

    static CallResult makeStringResult(const std::string& value) {
        return CallResult::FromString(value);
    }

    static const char* argAsCString(const CallArg& arg) {
        if (arg.kind == CallArg::Kind::CString) {
            return arg.s.c_str();
        }
        if (arg.kind == CallArg::Kind::Ptr) {
            return static_cast<const char*>(arg.v.p);
        }
        return nullptr;
    }

    template <typename T>
    static T* argAsPtr(const CallArg& arg) {
        if (arg.kind == CallArg::Kind::Ptr) {
            return static_cast<T*>(arg.v.p);
        }
        return nullptr;
    }

    template <typename T>
    static T* argAsPtr(const std::vector<CallArg>& args, std::size_t index) {
        if (index >= args.size()) {
            return nullptr;
        }
        return argAsPtr<T>(args[index]);
    }

    static const char* argAsCString(const std::vector<CallArg>& args, std::size_t index) {
        if (index >= args.size()) {
            return nullptr;
        }
        return argAsCString(args[index]);
    }
};

} // namespace SDOM::API
