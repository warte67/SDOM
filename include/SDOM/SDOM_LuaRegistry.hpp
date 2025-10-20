// SDOM_LuaRegistry.hpp - lightweight registry for per-type Lua bindings
#pragma once

#include <sol/sol.hpp>
#include <unordered_map>
#include <string>
#include <mutex>

namespace SDOM {

struct LuaBindingRegistry {
    using Fn = sol::protected_function;
    std::mutex m;
    std::unordered_map<std::string, std::unordered_map<std::string, Fn>> map;

    void register_fn(const std::string& type, const std::string& name, Fn fn) {
        std::lock_guard<std::mutex> lk(m);
        map[type][name] = std::move(fn);
        try {
            if (type == "Group" && name == "getFontSize") {
                std::cout << "[TRACE_REG] LuaBindingRegistry::register_fn stored Group.getFontSize; registry size for Group=" << map[type].size();
                try { std::cout << " fn_lua_state=" << (void*)map[type][name].lua_state(); } catch(...) {}
                std::cout << std::endl;
            }
            if (type == "Label" && name == "getFontSize") {
                std::cout << "[TRACE_REG] LuaBindingRegistry::register_fn stored Label.getFontSize; registry size for Label=" << map[type].size();
                try { std::cout << " fn_lua_state=" << (void*)map[type][name].lua_state(); } catch(...) {}
                std::cout << std::endl;
            }
        } catch(...) {}
    }

    bool has(const std::string& type, const std::string& name) {
        std::lock_guard<std::mutex> lk(m);
        auto it = map.find(type);
        return it != map.end() && it->second.find(name) != it->second.end();
    }

    Fn get(const std::string& type, const std::string& name) {
        std::lock_guard<std::mutex> lk(m);
        Fn &fn = map.at(type).at(name);
        // Focused trace: report the stored protected_function object address and its lua_State
        try {
            std::cout << "[TRACE_REG] LuaBindingRegistry::get returning " << type << "." << name
                      << " registry_fn_addr=" << (void*)&fn
                      << " fn_lua_state=" << (void*)fn.lua_state()
                      << std::endl;
        } catch(...) {}
        return fn;
    }

    void dump() {
        std::lock_guard<std::mutex> lk(m);
        for (auto &p : map) {
            const auto &t = p.first; const auto &m2 = p.second;
            std::cout << "LuaRegistry[" << t << "]: ";
            for (const auto &kv : m2) std::cout << kv.first << " ";
            std::cout << std::endl;
        }
    }
};

LuaBindingRegistry& getLuaBindingRegistry();

} // namespace SDOM
