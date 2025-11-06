// SDOM_IButtonObject.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IButtonObject.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_IDataObject.hpp>

namespace SDOM {

    namespace {
        template <typename F>
        inline void bind_both_impl(sol::table& handleTbl,
                                   sol::optional<sol::usertype<DisplayHandle>>& maybeUT,
                                   const char* name,
                                   F&& fn)
        {
            sol::object cur = handleTbl.raw_get_or(name, sol::lua_nil);
            if (!cur.valid() || cur == sol::lua_nil) {
                handleTbl.set_function(name, std::forward<F>(fn));
            }
            if (maybeUT) {
                try { (*maybeUT)[name] = fn; } catch(...) {}
            }
        }
    }

    void IButtonObject::registerLuaBindings(sol::state_view lua)
    {
        // If already registered, do nothing
        if (lua["_SDOM_IButtonObject_registered"].valid() && lua["_SDOM_IButtonObject_registered"].get_or(false))
            return;


        // // --- ButtonState table (enum + helpers) ---
        // sol::table BS = lua["ButtonState"];
        // if (!BS.valid()) BS = lua.create_table();

        // // Populate canonical names and all aliases
        // for (const auto& p : button_state_pairs) {
        //     BS[p.second] = p.first; // lower-case name -> int
        // }

        // BS.set_function("toString", [](int v) {
        //     auto it = button_state_to_string.find(v);
        //     if (it == button_state_to_string.end()) return std::string();
        //     return it->second; // already lowered
        // });

        // BS.set_function("fromString", [lua](const std::string& s) -> sol::object {
        //     auto opt = button_state_from_name(s);
        //     if (!opt) return sol::make_object(lua, sol::lua_nil);
        //     return sol::make_object(lua, static_cast<int>(*opt));
        // });

        // lua["ButtonState"] = BS;

        // // Keep this binder minimal and safe for now: only publish ButtonState.
        // // We do not touch the DisplayHandle usertype/table here to avoid
        // // order-of-registration pitfalls during early Core initialization.
        // lua["_SDOM_IButtonObject_registered"] = true;
    }

} // namespace SDOM
