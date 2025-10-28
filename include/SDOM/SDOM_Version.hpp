// SDOM_Version.hpp.in
#pragma once

#define SDOM_VERSION_MAJOR 0
#define SDOM_VERSION_MINOR 1
#define SDOM_VERSION_PATCH 11
#define SDOM_VERSION_CODENAME "pre-alpha"
#define SDOM_VERSION_BUILD "2025-10-28_15:13:42_b530f0cc"

#include <string>
#include <cstdio>
#include <sol/sol.hpp>

namespace SDOM {

class Version {
public:
    Version(sol::state_view lua) { registerLuaBindings(lua); }
    ~Version() = default;

    std::string toString() const {
        char buf[64];
        snprintf(buf, sizeof(buf), "%d.%d.%d",
                 SDOM_VERSION_MAJOR, SDOM_VERSION_MINOR, SDOM_VERSION_PATCH);
        return std::string(buf);
    }

    std::string fullString() const {
        char buf[128];
        snprintf(buf, sizeof(buf), "v%d.%d.%d (%s, %s)",
                 SDOM_VERSION_MAJOR, SDOM_VERSION_MINOR, SDOM_VERSION_PATCH,
                 SDOM_VERSION_CODENAME, SDOM_VERSION_BUILD);
        return std::string(buf);
    }

    int getMajor() const { return SDOM_VERSION_MAJOR; }
    int getMinor() const { return SDOM_VERSION_MINOR; }
    int getPatch() const { return SDOM_VERSION_PATCH; }
    std::string getCodename() const { return SDOM_VERSION_CODENAME; }
    std::string getLastBuild() const { return SDOM_VERSION_BUILD; }

    void registerLuaBindings(sol::state_view lua) {
        sol::table t = lua.create_table_with(
            "major", getMajor(),
            "minor", getMinor(),
            "patch", getPatch(),
            "codename", getCodename(),
            "last_build", getLastBuild()
        );

        t.set_function("toString", [&]() { return toString(); });
        t.set_function("fullString", [&]() { return fullString(); });

        // Metatable allows colon (:) syntax too
        sol::table meta = lua.create_table();
        meta["__index"] = t;
        t[sol::metatable_key] = meta;

        lua["Version"] = t;
    }
};

} // namespace SDOM
