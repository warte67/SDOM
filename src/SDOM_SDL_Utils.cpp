/***  SDOM_SDL_Utils.cpp  ****************************
 *    ___ ___   ___  __  __   ___ ___  _     _   _ _   _ _                   
 *   / __|   \ / _ \|  \/  | / __|   \| |   | | | | |_(_) |___  __ _ __ _ __ 
 *   \__ \ |) | (_) | |\/| | \__ \ |) | |__ | |_| |  _| | (_-<_/ _| '_ \ '_ \
 *   |___/___/ \___/|_|  |_|_|___/___/|____|_\___/ \__|_|_/__(_)__| .__/ .__/
 *                        |___|           |___|                   |_|  |_|      
 *  
 * The SDOM_SDL_Utils class provides a collection of static utility functions to 
 * facilitate common SDL3 operations within the SDOM framework. It offers convenient 
 * methods for converting between SDL enumerations and their string representations, 
 * such as pixel formats, window flags, renderer presentation modes, and event types. 
 * Additionally, it includes helper functions for keycode-to-ASCII conversion and other 
 * SDL-specific tasks. By centralizing these utilities, SDL_Utils simplifies SDL 
 * integration, enhances code readability, and promotes consistency across the SDOM 
 * codebase.
 * 
 * 
 *   This software is provided 'as-is', without any express or implied
 *   warranty.  In no event will the authors be held liable for any damages
 *   arising from the use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose,
 *   including commercial applications, and to alter it and redistribute it
 *   freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not
 *       claim that you wrote the original software. If you use this software
 *       in a product, an acknowledgment in the product documentation would be
 *       appreciated but is not required.
 *   2. Altered source versions must be plainly marked as such, and must not be
 *       misrepresented as being the original software.
 *   3. This notice may not be removed or altered from any source distribution.
 *
 * Released under the ZLIB License.
 * Original Author: Jay Faries (warte67)
 *
 ******************/

 #include <SDOM/SDOM_SDL_Utils.hpp>

namespace SDOM
{

    void SDL_Utils::registerLuaBindings(sol::state_view lua)
    {
        // Register SDL_Color userdata so Lua can access r/g/b/a and getter helpers
        // if (!lua["SDL_Color"].valid()) {
        //     lua.new_usertype<SDL_Color>("SDL_Color",
        //         "r", &SDL_Color::r,
        //         "g", &SDL_Color::g,
        //         "b", &SDL_Color::b,
        //         "a", &SDL_Color::a,
        //         "getR", [](const SDL_Color& c) { return c.r; },
        //         "getG", [](const SDL_Color& c) { return c.g; },
        //         "getB", [](const SDL_Color& c) { return c.b; },
        //         "getA", [](const SDL_Color& c) { return c.a; }
        //     );
        // }
        if (!lua["SDL_Color"].valid()) {
            lua.new_usertype<SDL_Color>("SDL_Color",
                // factories: default and (r,g,b,a)
                sol::factories(
                    []() { return SDL_Color{255,255,255,255}; },
                    [](Uint8 r, Uint8 g, Uint8 b, Uint8 a) { return SDL_Color{r,g,b,a}; }
                ),
                "r", &SDL_Color::r,
                "g", &SDL_Color::g,
                "b", &SDL_Color::b,
                "a", &SDL_Color::a,
                "getR", [](const SDL_Color& c) { return c.r; },
                "getG", [](const SDL_Color& c) { return c.g; },
                "getB", [](const SDL_Color& c) { return c.b; },
                "getA", [](const SDL_Color& c) { return c.a; },
                // helper to build from a Lua table {r=,g=,b=,a=} or {r,g,b,a}
                "fromTable", [](const sol::table& t) {
                    SDL_Color c{255,255,255,255};
                    if (t["r"].valid()) c.r = static_cast<Uint8>(t["r"].get<int>());
                    else if (t[1].valid()) c.r = static_cast<Uint8>(t[1].get<int>());
                    if (t["g"].valid()) c.g = static_cast<Uint8>(t["g"].get<int>());
                    else if (t[2].valid()) c.g = static_cast<Uint8>(t[2].get<int>());
                    if (t["b"].valid()) c.b = static_cast<Uint8>(t["b"].get<int>());
                    else if (t[3].valid()) c.b = static_cast<Uint8>(t[3].get<int>());
                    if (t["a"].valid()) c.a = static_cast<Uint8>(t["a"].get<int>());
                    else if (t[4].valid()) c.a = static_cast<Uint8>(t[4].get<int>());
                    return c;
                }
            );
        }
                
        try 
        {
            // Create a helper table for SDL utilities
            sol::table t = lua.create_table();

            // String/enum conversions
            t.set_function("pixelFormatToString", [](SDL_PixelFormat f) { return SDL_Utils::pixelFormatToString(f); });
            t.set_function("pixelFormatFromString", [](const std::string& s) { return SDL_Utils::pixelFormatFromString(s); });

            t.set_function("windowFlagsToString", [](Uint64 f) { return SDL_Utils::windowFlagsToString(f); });
            t.set_function("windowFlagsFromString", [](const std::string& s) { return SDL_Utils::windowFlagsFromString(s); });

            t.set_function("rendererLogicalPresentationToString", [](Uint32 f) { return SDL_Utils::rendererLogicalPresentationToString(f); });
            t.set_function("rendererLogicalPresentationFromString", [](const std::string& s) { return SDL_Utils::rendererLogicalPresentationFromString(s); });

            t.set_function("eventTypeToString", [](SDL_EventType et) { return SDL_Utils::eventTypeToString(et); });
            t.set_function("eventTypeFromString", [](const std::string& s) { return SDL_Utils::eventTypeFromString(s); });

            t.set_function("keyToAscii", [](SDL_Keycode kc, SDL_Keymod km) { return SDL_Utils::keyToAscii(kc, km); });

            // Allow both SDL.Delay(6000) and SDL:Delay(6000) (method-style which passes the table as first arg)
            t.set_function("delay", [](sol::this_state, sol::variadic_args va) {
                Uint32 ms = 0;
                for (auto arg : va) {
                    if (arg.is<Uint32>()) { ms = arg.as<Uint32>(); break; }
                    if (arg.is<int>()) { ms = static_cast<Uint32>(arg.as<int>()); break; }
                    if (arg.is<double>()) { ms = static_cast<Uint32>(arg.as<double>()); break; }
                }
                SDL_Delay(ms);
            });

            // Convert an SDL_Event into a Lua table (returns table or nil on failure)
            t.set_function("eventToLuaTable", [lua](const SDL_Event& ev) -> sol::object {
                try {
                    sol::table tbl = SDL_Utils::eventToLuaTable(ev, lua);
                    return sol::object(lua, tbl);
                } catch (const sol::error& e) {
                    WARNING(std::string("SDL_Utils::eventToLuaTable -> sol::error: ") + e.what());
                } catch (const std::exception& e) {
                    WARNING(std::string("SDL_Utils::eventToLuaTable -> exception: ") + e.what());
                } catch (...) {
                    WARNING("SDL_Utils::eventToLuaTable -> unknown exception");
                }
                return sol::object(lua, sol::lua_nil);
            });

            // Allow colon-style calls by setting __index to self
            sol::table meta = lua.create_table();
            meta["__index"] = t;                 // Make colon calls work automatically
            t[sol::metatable_key] = meta;            

            // Expose under both names for convenience
            lua["SDL_Utils"] = t;
            lua["SDL"] = t; // shorter alias
        } catch (const sol::error& e) {
            WARNING(std::string("Failed to register SDL_Utils with Lua: ") + e.what());
        } catch (const std::exception& e) {
            WARNING(std::string("Failed to register SDL_Utils with Lua: ") + e.what());
        } catch (...) {
            WARNING("Failed to register SDL_Utils with Lua: unknown exception");
        }
    } // END SDL_Utils::registerLua()


    SDL_Color SDL_Utils::get__lua_color(const sol::table& t)
    {
        SDL_Color c{255,255,255,255};
        if (!t.valid()) return c;
        // keyed lookup first, then numeric array-style fallback
        c.r = static_cast<Uint8>(t["r"].get_or(t[1].get_or(static_cast<int>(c.r))));
        c.g = static_cast<Uint8>(t["g"].get_or(t[2].get_or(static_cast<int>(c.g))));
        c.b = static_cast<Uint8>(t["b"].get_or(t[3].get_or(static_cast<int>(c.b))));
        c.a = static_cast<Uint8>(t["a"].get_or(t[4].get_or(static_cast<int>(c.a))));
        return c;
    } // END SDL_Utils::get__lua_color()

    sol::table SDL_Utils::get_lua_color(sol::state_view lua, const SDL_Color& s)
    {
        sol::table t = lua.create_table();
        t["r"] = static_cast<int>(s.r);
        t["g"] = static_cast<int>(s.g);
        t["b"] = static_cast<int>(s.b);
        t["a"] = static_cast<int>(s.a);
        // also populate numeric indices for array-style access
        t[1] = static_cast<int>(s.r);
        t[2] = static_cast<int>(s.g);
        t[3] = static_cast<int>(s.b);
        t[4] = static_cast<int>(s.a);
        return t;
    } // END SDL_Utils::get_lua_color()

    sol::table SDL_Utils::get_lua_color(const SDL_Color& s)
    {
        // convenience: use global Lua state helper if available in project
        try {
            sol::state_view lua = getLua();
            return get_lua_color(lua, s);
        } catch(...) {
            // fallback: create a minimal invalid table via dummy state if getLua() not available
            // attempt to return an empty table attached to the global Lua state if possible
            // (this path should rarely be hit; callers can use the sol::state_view overload)
            sol::state_view lua = getLua();
            return get_lua_color(lua, s);
        }
    } // END SDL_Utils::get_lua_color()



    std::string SDL_Utils::pixelFormatToString(SDL_PixelFormat format)
    {
        switch (format) {
            case SDL_PIXELFORMAT_UNKNOWN: return "SDL_PIXELFORMAT_UNKNOWN";
            case SDL_PIXELFORMAT_INDEX1LSB: return "SDL_PIXELFORMAT_INDEX1LSB";
            case SDL_PIXELFORMAT_INDEX1MSB: return "SDL_PIXELFORMAT_INDEX1MSB";
            case SDL_PIXELFORMAT_INDEX2LSB: return "SDL_PIXELFORMAT_INDEX2LSB";
            case SDL_PIXELFORMAT_INDEX2MSB: return "SDL_PIXELFORMAT_INDEX2MSB";
            case SDL_PIXELFORMAT_INDEX4LSB: return "SDL_PIXELFORMAT_INDEX4LSB";
            case SDL_PIXELFORMAT_INDEX4MSB: return "SDL_PIXELFORMAT_INDEX4MSB";
            case SDL_PIXELFORMAT_INDEX8: return "SDL_PIXELFORMAT_INDEX8";
            case SDL_PIXELFORMAT_RGB332: return "SDL_PIXELFORMAT_RGB332";
            case SDL_PIXELFORMAT_XRGB4444: return "SDL_PIXELFORMAT_XRGB4444";
            case SDL_PIXELFORMAT_XBGR4444: return "SDL_PIXELFORMAT_XBGR4444";
            case SDL_PIXELFORMAT_XRGB1555: return "SDL_PIXELFORMAT_XRGB1555";
            case SDL_PIXELFORMAT_XBGR1555: return "SDL_PIXELFORMAT_XBGR1555";
            case SDL_PIXELFORMAT_ARGB4444: return "SDL_PIXELFORMAT_ARGB4444";
            case SDL_PIXELFORMAT_RGBA4444: return "SDL_PIXELFORMAT_RGBA4444";
            case SDL_PIXELFORMAT_ABGR4444: return "SDL_PIXELFORMAT_ABGR4444";
            case SDL_PIXELFORMAT_BGRA4444: return "SDL_PIXELFORMAT_BGRA4444";
            case SDL_PIXELFORMAT_ARGB1555: return "SDL_PIXELFORMAT_ARGB1555";
            case SDL_PIXELFORMAT_RGBA5551: return "SDL_PIXELFORMAT_RGBA5551";
            case SDL_PIXELFORMAT_ABGR1555: return "SDL_PIXELFORMAT_ABGR1555";
            case SDL_PIXELFORMAT_BGRA5551: return "SDL_PIXELFORMAT_BGRA5551";
            case SDL_PIXELFORMAT_RGB565: return "SDL_PIXELFORMAT_RGB565";
            case SDL_PIXELFORMAT_BGR565: return "SDL_PIXELFORMAT_BGR565";
            case SDL_PIXELFORMAT_RGB24: return "SDL_PIXELFORMAT_RGB24";
            case SDL_PIXELFORMAT_BGR24: return "SDL_PIXELFORMAT_BGR24";
            case SDL_PIXELFORMAT_XRGB8888: return "SDL_PIXELFORMAT_XRGB8888";
            case SDL_PIXELFORMAT_RGBX8888: return "SDL_PIXELFORMAT_RGBX8888";
            case SDL_PIXELFORMAT_XBGR8888: return "SDL_PIXELFORMAT_XBGR8888";
            case SDL_PIXELFORMAT_BGRX8888: return "SDL_PIXELFORMAT_BGRX8888";
            case SDL_PIXELFORMAT_ARGB8888: return "SDL_PIXELFORMAT_ARGB8888";
            case SDL_PIXELFORMAT_RGBA8888: return "SDL_PIXELFORMAT_RGBA8888";
            case SDL_PIXELFORMAT_ABGR8888: return "SDL_PIXELFORMAT_ABGR8888";
            case SDL_PIXELFORMAT_BGRA8888: return "SDL_PIXELFORMAT_BGRA8888";
            case SDL_PIXELFORMAT_XRGB2101010: return "SDL_PIXELFORMAT_XRGB2101010";
            case SDL_PIXELFORMAT_XBGR2101010: return "SDL_PIXELFORMAT_XBGR2101010";
            case SDL_PIXELFORMAT_ARGB2101010: return "SDL_PIXELFORMAT_ARGB2101010";
            case SDL_PIXELFORMAT_ABGR2101010: return "SDL_PIXELFORMAT_ABGR2101010";
            case SDL_PIXELFORMAT_RGB48: return "SDL_PIXELFORMAT_RGB48";
            case SDL_PIXELFORMAT_BGR48: return "SDL_PIXELFORMAT_BGR48";
            case SDL_PIXELFORMAT_RGBA64: return "SDL_PIXELFORMAT_RGBA64";
            case SDL_PIXELFORMAT_ARGB64: return "SDL_PIXELFORMAT_ARGB64";
            case SDL_PIXELFORMAT_BGRA64: return "SDL_PIXELFORMAT_BGRA64";
            case SDL_PIXELFORMAT_ABGR64: return "SDL_PIXELFORMAT_ABGR64";
            case SDL_PIXELFORMAT_RGB48_FLOAT: return "SDL_PIXELFORMAT_RGB48_FLOAT";
            case SDL_PIXELFORMAT_BGR48_FLOAT: return "SDL_PIXELFORMAT_BGR48_FLOAT";
            case SDL_PIXELFORMAT_RGBA64_FLOAT: return "SDL_PIXELFORMAT_RGBA64_FLOAT";
            case SDL_PIXELFORMAT_ARGB64_FLOAT: return "SDL_PIXELFORMAT_ARGB64_FLOAT";
            case SDL_PIXELFORMAT_BGRA64_FLOAT: return "SDL_PIXELFORMAT_BGRA64_FLOAT";
            case SDL_PIXELFORMAT_ABGR64_FLOAT: return "SDL_PIXELFORMAT_ABGR64_FLOAT";
            case SDL_PIXELFORMAT_RGB96_FLOAT: return "SDL_PIXELFORMAT_RGB96_FLOAT";
            case SDL_PIXELFORMAT_BGR96_FLOAT: return "SDL_PIXELFORMAT_BGR96_FLOAT";
            case SDL_PIXELFORMAT_RGBA128_FLOAT: return "SDL_PIXELFORMAT_RGBA128_FLOAT";
            case SDL_PIXELFORMAT_ARGB128_FLOAT: return "SDL_PIXELFORMAT_ARGB128_FLOAT";
            case SDL_PIXELFORMAT_BGRA128_FLOAT: return "SDL_PIXELFORMAT_BGRA128_FLOAT";
            case SDL_PIXELFORMAT_ABGR128_FLOAT: return "SDL_PIXELFORMAT_ABGR128_FLOAT";
            case SDL_PIXELFORMAT_YV12: return "SDL_PIXELFORMAT_YV12";
            case SDL_PIXELFORMAT_IYUV: return "SDL_PIXELFORMAT_IYUV";
            case SDL_PIXELFORMAT_YUY2: return "SDL_PIXELFORMAT_YUY2";
            case SDL_PIXELFORMAT_UYVY: return "SDL_PIXELFORMAT_UYVY";
            case SDL_PIXELFORMAT_YVYU: return "SDL_PIXELFORMAT_YVYU";
            case SDL_PIXELFORMAT_NV12: return "SDL_PIXELFORMAT_NV12";
            case SDL_PIXELFORMAT_NV21: return "SDL_PIXELFORMAT_NV21";
            case SDL_PIXELFORMAT_P010: return "SDL_PIXELFORMAT_P010";
            case SDL_PIXELFORMAT_EXTERNAL_OES: return "SDL_PIXELFORMAT_EXTERNAL_OES";
            case SDL_PIXELFORMAT_MJPG: return "SDL_PIXELFORMAT_MJPG";
            default: return "Unknown";
        }
    } // END SDL_Utils::pixelFormatToString()

    SDL_PixelFormat SDL_Utils::pixelFormatFromString(const std::string& str)
    {
        if (str == "SDL_PIXELFORMAT_UNKNOWN") return SDL_PIXELFORMAT_UNKNOWN;
        if (str == "SDL_PIXELFORMAT_INDEX1LSB") return SDL_PIXELFORMAT_INDEX1LSB;
        if (str == "SDL_PIXELFORMAT_INDEX1MSB") return SDL_PIXELFORMAT_INDEX1MSB;
        if (str == "SDL_PIXELFORMAT_INDEX2LSB") return SDL_PIXELFORMAT_INDEX2LSB;
        if (str == "SDL_PIXELFORMAT_INDEX2MSB") return SDL_PIXELFORMAT_INDEX2MSB;
        if (str == "SDL_PIXELFORMAT_INDEX4LSB") return SDL_PIXELFORMAT_INDEX4LSB;
        if (str == "SDL_PIXELFORMAT_INDEX4MSB") return SDL_PIXELFORMAT_INDEX4MSB;
        if (str == "SDL_PIXELFORMAT_INDEX8") return SDL_PIXELFORMAT_INDEX8;
        if (str == "SDL_PIXELFORMAT_RGB332") return SDL_PIXELFORMAT_RGB332;
        if (str == "SDL_PIXELFORMAT_XRGB4444") return SDL_PIXELFORMAT_XRGB4444;
        if (str == "SDL_PIXELFORMAT_XBGR4444") return SDL_PIXELFORMAT_XBGR4444;
        if (str == "SDL_PIXELFORMAT_XRGB1555") return SDL_PIXELFORMAT_XRGB1555;
        if (str == "SDL_PIXELFORMAT_XBGR1555") return SDL_PIXELFORMAT_XBGR1555;
        if (str == "SDL_PIXELFORMAT_ARGB4444") return SDL_PIXELFORMAT_ARGB4444;
        if (str == "SDL_PIXELFORMAT_RGBA4444") return SDL_PIXELFORMAT_RGBA4444;
        if (str == "SDL_PIXELFORMAT_ABGR4444") return SDL_PIXELFORMAT_ABGR4444;
        if (str == "SDL_PIXELFORMAT_BGRA4444") return SDL_PIXELFORMAT_BGRA4444;
        if (str == "SDL_PIXELFORMAT_ARGB1555") return SDL_PIXELFORMAT_ARGB1555;
        if (str == "SDL_PIXELFORMAT_RGBA5551") return SDL_PIXELFORMAT_RGBA5551;
        if (str == "SDL_PIXELFORMAT_ABGR1555") return SDL_PIXELFORMAT_ABGR1555;
        if (str == "SDL_PIXELFORMAT_BGRA5551") return SDL_PIXELFORMAT_BGRA5551;
        if (str == "SDL_PIXELFORMAT_RGB565") return SDL_PIXELFORMAT_RGB565;
        if (str == "SDL_PIXELFORMAT_BGR565") return SDL_PIXELFORMAT_BGR565;
        if (str == "SDL_PIXELFORMAT_RGB24") return SDL_PIXELFORMAT_RGB24;
        if (str == "SDL_PIXELFORMAT_BGR24") return SDL_PIXELFORMAT_BGR24;
        if (str == "SDL_PIXELFORMAT_XRGB8888") return SDL_PIXELFORMAT_XRGB8888;
        if (str == "SDL_PIXELFORMAT_RGBX8888") return SDL_PIXELFORMAT_RGBX8888;
        if (str == "SDL_PIXELFORMAT_XBGR8888") return SDL_PIXELFORMAT_XBGR8888;
        if (str == "SDL_PIXELFORMAT_BGRX8888") return SDL_PIXELFORMAT_BGRX8888;
        if (str == "SDL_PIXELFORMAT_ARGB8888") return SDL_PIXELFORMAT_ARGB8888;
        if (str == "SDL_PIXELFORMAT_RGBA8888") return SDL_PIXELFORMAT_RGBA8888;
        if (str == "SDL_PIXELFORMAT_ABGR8888") return SDL_PIXELFORMAT_ABGR8888;
        if (str == "SDL_PIXELFORMAT_BGRA8888") return SDL_PIXELFORMAT_BGRA8888;
        if (str == "SDL_PIXELFORMAT_XRGB2101010") return SDL_PIXELFORMAT_XRGB2101010;
        if (str == "SDL_PIXELFORMAT_XBGR2101010") return SDL_PIXELFORMAT_XBGR2101010;
        if (str == "SDL_PIXELFORMAT_ARGB2101010") return SDL_PIXELFORMAT_ARGB2101010;
        if (str == "SDL_PIXELFORMAT_ABGR2101010") return SDL_PIXELFORMAT_ABGR2101010;
        if (str == "SDL_PIXELFORMAT_RGB48") return SDL_PIXELFORMAT_RGB48;
        if (str == "SDL_PIXELFORMAT_BGR48") return SDL_PIXELFORMAT_BGR48;
        if (str == "SDL_PIXELFORMAT_RGBA64") return SDL_PIXELFORMAT_RGBA64;
        if (str == "SDL_PIXELFORMAT_ARGB64") return SDL_PIXELFORMAT_ARGB64;
        if (str == "SDL_PIXELFORMAT_BGRA64") return SDL_PIXELFORMAT_BGRA64;
        if (str == "SDL_PIXELFORMAT_ABGR64") return SDL_PIXELFORMAT_ABGR64;
        if (str == "SDL_PIXELFORMAT_RGB48_FLOAT") return SDL_PIXELFORMAT_RGB48_FLOAT;
        if (str == "SDL_PIXELFORMAT_BGR48_FLOAT") return SDL_PIXELFORMAT_BGR48_FLOAT;
        if (str == "SDL_PIXELFORMAT_RGBA64_FLOAT") return SDL_PIXELFORMAT_RGBA64_FLOAT;
        if (str == "SDL_PIXELFORMAT_ARGB64_FLOAT") return SDL_PIXELFORMAT_ARGB64_FLOAT;
        if (str == "SDL_PIXELFORMAT_BGRA64_FLOAT") return SDL_PIXELFORMAT_BGRA64_FLOAT;
        if (str == "SDL_PIXELFORMAT_ABGR64_FLOAT") return SDL_PIXELFORMAT_ABGR64_FLOAT;
        if (str == "SDL_PIXELFORMAT_RGB96_FLOAT") return SDL_PIXELFORMAT_RGB96_FLOAT;
        if (str == "SDL_PIXELFORMAT_BGR96_FLOAT") return SDL_PIXELFORMAT_BGR96_FLOAT;
        if (str == "SDL_PIXELFORMAT_RGBA128_FLOAT") return SDL_PIXELFORMAT_RGBA128_FLOAT;
        if (str == "SDL_PIXELFORMAT_ARGB128_FLOAT") return SDL_PIXELFORMAT_ARGB128_FLOAT;
        if (str == "SDL_PIXELFORMAT_BGRA128_FLOAT") return SDL_PIXELFORMAT_BGRA128_FLOAT;
        if (str == "SDL_PIXELFORMAT_ABGR128_FLOAT") return SDL_PIXELFORMAT_ABGR128_FLOAT;
        if (str == "SDL_PIXELFORMAT_YV12") return SDL_PIXELFORMAT_YV12;
        if (str == "SDL_PIXELFORMAT_IYUV") return SDL_PIXELFORMAT_IYUV;
        if (str == "SDL_PIXELFORMAT_YUY2") return SDL_PIXELFORMAT_YUY2;
        if (str == "SDL_PIXELFORMAT_UYVY") return SDL_PIXELFORMAT_UYVY;
        if (str == "SDL_PIXELFORMAT_YVYU") return SDL_PIXELFORMAT_YVYU;
        if (str == "SDL_PIXELFORMAT_NV12") return SDL_PIXELFORMAT_NV12;
        if (str == "SDL_PIXELFORMAT_NV21") return SDL_PIXELFORMAT_NV21;
        if (str == "SDL_PIXELFORMAT_P010") return SDL_PIXELFORMAT_P010;
        if (str == "SDL_PIXELFORMAT_EXTERNAL_OES") return SDL_PIXELFORMAT_EXTERNAL_OES;
        if (str == "SDL_PIXELFORMAT_MJPG") return SDL_PIXELFORMAT_MJPG;
        return SDL_PIXELFORMAT_UNKNOWN; // Return unknown format if not found
    } // END SDL_Utils::pixelFormatFromString()

    std::string SDL_Utils::windowFlagsToString(Uint64 flags)
    {
        std::string result;
        struct FlagInfo { Uint64 value; const char* name; };
        static const FlagInfo flagInfos[] = {
            { SDL_WINDOW_FULLSCREEN, "SDL_WINDOW_FULLSCREEN" },
            { SDL_WINDOW_OPENGL, "SDL_WINDOW_OPENGL" },
            { SDL_WINDOW_OCCLUDED, "SDL_WINDOW_OCCLUDED" },
            { SDL_WINDOW_HIDDEN, "SDL_WINDOW_HIDDEN" },
            { SDL_WINDOW_BORDERLESS, "SDL_WINDOW_BORDERLESS" },
            { SDL_WINDOW_RESIZABLE, "SDL_WINDOW_RESIZABLE" },
            { SDL_WINDOW_MINIMIZED, "SDL_WINDOW_MINIMIZED" },
            { SDL_WINDOW_MAXIMIZED, "SDL_WINDOW_MAXIMIZED" },
            { SDL_WINDOW_MOUSE_GRABBED, "SDL_WINDOW_MOUSE_GRABBED" },
            { SDL_WINDOW_INPUT_FOCUS, "SDL_WINDOW_INPUT_FOCUS" },
            { SDL_WINDOW_MOUSE_FOCUS, "SDL_WINDOW_MOUSE_FOCUS" },
            { SDL_WINDOW_EXTERNAL, "SDL_WINDOW_EXTERNAL" },
            { SDL_WINDOW_MODAL, "SDL_WINDOW_MODAL" },
            { SDL_WINDOW_HIGH_PIXEL_DENSITY, "SDL_WINDOW_HIGH_PIXEL_DENSITY" },
            { SDL_WINDOW_MOUSE_CAPTURE, "SDL_WINDOW_MOUSE_CAPTURE" },
            { SDL_WINDOW_MOUSE_RELATIVE_MODE, "SDL_WINDOW_MOUSE_RELATIVE_MODE" },
            { SDL_WINDOW_ALWAYS_ON_TOP, "SDL_WINDOW_ALWAYS_ON_TOP" },
            { SDL_WINDOW_UTILITY, "SDL_WINDOW_UTILITY" },
            { SDL_WINDOW_TOOLTIP, "SDL_WINDOW_TOOLTIP" },
            { SDL_WINDOW_POPUP_MENU, "SDL_WINDOW_POPUP_MENU" },
            { SDL_WINDOW_KEYBOARD_GRABBED, "SDL_WINDOW_KEYBOARD_GRABBED" },
            { SDL_WINDOW_VULKAN, "SDL_WINDOW_VULKAN" },
            { SDL_WINDOW_METAL, "SDL_WINDOW_METAL" },
            { SDL_WINDOW_TRANSPARENT, "SDL_WINDOW_TRANSPARENT" },
            { SDL_WINDOW_NOT_FOCUSABLE, "SDL_WINDOW_NOT_FOCUSABLE" }
        };
        bool first = true;
        for (const auto& info : flagInfos) {
            if (flags & info.value) {
                if (!first) result += "|";
                result += info.name;
                first = false;
            }
        }
        if (result.empty()) result = "0";
        return result;
    }

    static std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }

    Uint64 SDL_Utils::windowFlagsFromString(const std::string& str)
    {
        struct FlagInfo { Uint64 value; const char* name; };
        static const FlagInfo flagInfos[] = {
            { SDL_WINDOW_FULLSCREEN, "SDL_WINDOW_FULLSCREEN" },
            { SDL_WINDOW_OPENGL, "SDL_WINDOW_OPENGL" },
            { SDL_WINDOW_OCCLUDED, "SDL_WINDOW_OCCLUDED" },
            { SDL_WINDOW_HIDDEN, "SDL_WINDOW_HIDDEN" },
            { SDL_WINDOW_BORDERLESS, "SDL_WINDOW_BORDERLESS" },
            { SDL_WINDOW_RESIZABLE, "SDL_WINDOW_RESIZABLE" },
            { SDL_WINDOW_MINIMIZED, "SDL_WINDOW_MINIMIZED" },
            { SDL_WINDOW_MAXIMIZED, "SDL_WINDOW_MAXIMIZED" },
            { SDL_WINDOW_MOUSE_GRABBED, "SDL_WINDOW_MOUSE_GRABBED" },
            { SDL_WINDOW_INPUT_FOCUS, "SDL_WINDOW_INPUT_FOCUS" },
            { SDL_WINDOW_MOUSE_FOCUS, "SDL_WINDOW_MOUSE_FOCUS" },
            { SDL_WINDOW_EXTERNAL, "SDL_WINDOW_EXTERNAL" },
            { SDL_WINDOW_MODAL, "SDL_WINDOW_MODAL" },
            { SDL_WINDOW_HIGH_PIXEL_DENSITY, "SDL_WINDOW_HIGH_PIXEL_DENSITY" },
            { SDL_WINDOW_MOUSE_CAPTURE, "SDL_WINDOW_MOUSE_CAPTURE" },
            { SDL_WINDOW_MOUSE_RELATIVE_MODE, "SDL_WINDOW_MOUSE_RELATIVE_MODE" },
            { SDL_WINDOW_ALWAYS_ON_TOP, "SDL_WINDOW_ALWAYS_ON_TOP" },
            { SDL_WINDOW_UTILITY, "SDL_WINDOW_UTILITY" },
            { SDL_WINDOW_TOOLTIP, "SDL_WINDOW_TOOLTIP" },
            { SDL_WINDOW_POPUP_MENU, "SDL_WINDOW_POPUP_MENU" },
            { SDL_WINDOW_KEYBOARD_GRABBED, "SDL_WINDOW_KEYBOARD_GRABBED" },
            { SDL_WINDOW_VULKAN, "SDL_WINDOW_VULKAN" },
            { SDL_WINDOW_METAL, "SDL_WINDOW_METAL" },
            { SDL_WINDOW_TRANSPARENT, "SDL_WINDOW_TRANSPARENT" },
            { SDL_WINDOW_NOT_FOCUSABLE, "SDL_WINDOW_NOT_FOCUSABLE" }
        };
        
        Uint64 flags = 0;
        if (str == "0" || str.empty()) return 0;
        size_t start = 0, end = 0;
        while (end != std::string::npos) {
            end = str.find('|', start);
            std::string token = str.substr(start, end - start);
            token = trim(token); // <-- Trim whitespace here
            for (const auto& info : flagInfos) {
                if (token == info.name) {
                    flags |= info.value;
                    break;
                }
            }
            start = (end == std::string::npos) ? end : end + 1;
        }
        return flags;        
    }

    std::string SDL_Utils::rendererLogicalPresentationToString(Uint32 flags)
    {
        switch (flags) {
            case SDL_LOGICAL_PRESENTATION_DISABLED: return "SDL_LOGICAL_PRESENTATION_DISABLED";
            case SDL_LOGICAL_PRESENTATION_STRETCH: return "SDL_LOGICAL_PRESENTATION_STRETCH";
            case SDL_LOGICAL_PRESENTATION_LETTERBOX: return "SDL_LOGICAL_PRESENTATION_LETTERBOX";
            case SDL_LOGICAL_PRESENTATION_OVERSCAN: return "SDL_LOGICAL_PRESENTATION_OVERSCAN";
            case SDL_LOGICAL_PRESENTATION_INTEGER_SCALE: return "SDL_LOGICAL_PRESENTATION_INTEGER_SCALE";
            default: return "Unknown";
        }
    }
    SDL_RendererLogicalPresentation SDL_Utils::rendererLogicalPresentationFromString(const std::string& str)
    {
        if (str == "SDL_LOGICAL_PRESENTATION_DISABLED") return SDL_LOGICAL_PRESENTATION_DISABLED;
        if (str == "SDL_LOGICAL_PRESENTATION_STRETCH") return SDL_LOGICAL_PRESENTATION_STRETCH;
        if (str == "SDL_LOGICAL_PRESENTATION_LETTERBOX") return SDL_LOGICAL_PRESENTATION_LETTERBOX;
        if (str == "SDL_LOGICAL_PRESENTATION_OVERSCAN") return SDL_LOGICAL_PRESENTATION_OVERSCAN;
        if (str == "SDL_LOGICAL_PRESENTATION_INTEGER_SCALE") return SDL_LOGICAL_PRESENTATION_INTEGER_SCALE;
        return SDL_LOGICAL_PRESENTATION_DISABLED; // Default/fallback
    }

    std::string SDL_Utils::eventTypeToString(SDL_EventType type)
    {
        switch (type) {
            case SDL_EVENT_FIRST: return "SDL_EVENT_FIRST"; // Unused (do not remove)
            case SDL_EVENT_QUIT: return "SDL_EVENT_QUIT"; // User-requested quit
            case SDL_EVENT_TERMINATING: return "SDL_EVENT_TERMINATING"; // The application is being terminated by the OS
            case SDL_EVENT_LOW_MEMORY: return "SDL_EVENT_LOW_MEMORY"; // The application is low on memory
            case SDL_EVENT_WILL_ENTER_BACKGROUND: return "SDL_EVENT_WILL_ENTER_BACKGROUND"; // The application is about to enter the background
            case SDL_EVENT_DID_ENTER_BACKGROUND: return "SDL_EVENT_DID_ENTER_BACKGROUND"; // The application did enter the background
            case SDL_EVENT_WILL_ENTER_FOREGROUND: return "SDL_EVENT_WILL_ENTER_FOREGROUND"; // The application is about to enter the foreground
            case SDL_EVENT_DID_ENTER_FOREGROUND: return "SDL_EVENT_DID_ENTER_FOREGROUND"; // The application is now interactive
            case SDL_EVENT_LOCALE_CHANGED: return "SDL_EVENT_LOCALE_CHANGED"; // The user's locale preferences have changed
            case SDL_EVENT_DISPLAY_ORIENTATION: return "SDL_EVENT_DISPLAY_ORIENTATION"; // Display orientation has changed
            case SDL_EVENT_DISPLAY_ADDED: return "SDL_EVENT_DISPLAY_ADDED"; // Display has been added to the system
            case SDL_EVENT_DISPLAY_REMOVED: return "SDL_EVENT_DISPLAY_REMOVED"; // Display has been removed from the system
            case SDL_EVENT_DISPLAY_MOVED: return "SDL_EVENT_DISPLAY_MOVED"; // Display has changed position
            case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED: return "SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED"; // Display has changed desktop mode
            case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED: return "SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED"; // Display has changed current mode
            case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED: return "SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED"; // Display has changed content scale
            case SDL_EVENT_WINDOW_SHOWN: return "SDL_EVENT_WINDOW_SHOWN"; // Window has been shown
            case SDL_EVENT_WINDOW_HIDDEN: return "SDL_EVENT_WINDOW_HIDDEN"; // Window has been hidden
            case SDL_EVENT_WINDOW_EXPOSED: return "SDL_EVENT_WINDOW_EXPOSED"; // Window has been exposed
            case SDL_EVENT_WINDOW_MOVED: return "SDL_EVENT_WINDOW_MOVED"; // Window has been moved
            case SDL_EVENT_WINDOW_RESIZED: return "SDL_EVENT_WINDOW_RESIZED"; // Window has been resized
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: return "SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED"; // The pixel size of the window has changed
            case SDL_EVENT_WINDOW_METAL_VIEW_RESIZED: return "SDL_EVENT_WINDOW_METAL_VIEW_RESIZED"; // The pixel size of a Metal view has changed
            case SDL_EVENT_WINDOW_MINIMIZED: return "SDL_EVENT_WINDOW_MINIMIZED"; // Window has been minimized
            case SDL_EVENT_WINDOW_MAXIMIZED: return "SDL_EVENT_WINDOW_MAXIMIZED"; // Window has been maximized
            case SDL_EVENT_WINDOW_RESTORED: return "SDL_EVENT_WINDOW_RESTORED"; // Window has been restored
            case SDL_EVENT_WINDOW_MOUSE_ENTER: return "SDL_EVENT_WINDOW_MOUSE_ENTER"; // Window has gained mouse focus
            case SDL_EVENT_WINDOW_MOUSE_LEAVE: return "SDL_EVENT_WINDOW_MOUSE_LEAVE"; // Window has lost mouse focus
            case SDL_EVENT_WINDOW_FOCUS_GAINED: return "SDL_EVENT_WINDOW_FOCUS_GAINED"; // Window has gained keyboard focus
            case SDL_EVENT_WINDOW_FOCUS_LOST: return "SDL_EVENT_WINDOW_FOCUS_LOST"; // Window has lost keyboard focus
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED: return "SDL_EVENT_WINDOW_CLOSE_REQUESTED"; // The window manager requests that the window be closed
            case SDL_EVENT_WINDOW_HIT_TEST: return "SDL_EVENT_WINDOW_HIT_TEST"; // Window had a hit test
            case SDL_EVENT_WINDOW_ICCPROF_CHANGED: return "SDL_EVENT_WINDOW_ICCPROF_CHANGED"; // The ICC profile of the window's display has changed
            case SDL_EVENT_WINDOW_DISPLAY_CHANGED: return "SDL_EVENT_WINDOW_DISPLAY_CHANGED"; // Window has been moved to display
            case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED: return "SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED"; // Window display scale has been changed
            case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED: return "SDL_EVENT_WINDOW_SAFE_AREA_CHANGED"; // The window safe area has been changed
            case SDL_EVENT_WINDOW_OCCLUDED: return "SDL_EVENT_WINDOW_OCCLUDED"; // The window has been occluded
            case SDL_EVENT_WINDOW_ENTER_FULLSCREEN: return "SDL_EVENT_WINDOW_ENTER_FULLSCREEN"; // The window has entered fullscreen mode
            case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN: return "SDL_EVENT_WINDOW_LEAVE_FULLSCREEN"; // The window has left fullscreen mode
            case SDL_EVENT_WINDOW_DESTROYED: return "SDL_EVENT_WINDOW_DESTROYED"; // The window with the associated ID is being or has been destroyed
            case SDL_EVENT_WINDOW_HDR_STATE_CHANGED: return "SDL_EVENT_WINDOW_HDR_STATE_CHANGED"; // Window HDR properties have changed
            case SDL_EVENT_KEY_DOWN: return "SDL_EVENT_KEY_DOWN"; // Key pressed
            case SDL_EVENT_KEY_UP: return "SDL_EVENT_KEY_UP"; // Key released
            case SDL_EVENT_TEXT_EDITING: return "SDL_EVENT_TEXT_EDITING"; // Keyboard text editing
            case SDL_EVENT_TEXT_INPUT: return "SDL_EVENT_TEXT_INPUT"; // Keyboard text input
            case SDL_EVENT_KEYMAP_CHANGED: return "SDL_EVENT_KEYMAP_CHANGED"; // Keymap changed
            case SDL_EVENT_KEYBOARD_ADDED: return "SDL_EVENT_KEYBOARD_ADDED"; // A new keyboard has been inserted
            case SDL_EVENT_KEYBOARD_REMOVED: return "SDL_EVENT_KEYBOARD_REMOVED"; // A keyboard has been removed
            case SDL_EVENT_MOUSE_MOTION: return "SDL_EVENT_MOUSE_MOTION"; // Mouse moved
            case SDL_EVENT_MOUSE_BUTTON_DOWN: return "SDL_EVENT_MOUSE_BUTTON_DOWN"; // Mouse button pressed
            case SDL_EVENT_MOUSE_BUTTON_UP: return "SDL_EVENT_MOUSE_BUTTON_UP"; // Mouse button released
            case SDL_EVENT_MOUSE_WHEEL: return "SDL_EVENT_MOUSE_WHEEL"; // Mouse wheel motion
            case SDL_EVENT_MOUSE_ADDED: return "SDL_EVENT_MOUSE_ADDED"; // A new mouse has been inserted
            case SDL_EVENT_MOUSE_REMOVED: return "SDL_EVENT_MOUSE_REMOVED"; // A mouse has been removed
            case SDL_EVENT_JOYSTICK_AXIS_MOTION: return "SDL_EVENT_JOYSTICK_AXIS_MOTION"; // Joystick axis motion
            case SDL_EVENT_JOYSTICK_BALL_MOTION: return "SDL_EVENT_JOYSTICK_BALL_MOTION"; // Joystick trackball motion
            case SDL_EVENT_JOYSTICK_HAT_MOTION: return "SDL_EVENT_JOYSTICK_HAT_MOTION"; // Joystick hat position change
            case SDL_EVENT_JOYSTICK_BUTTON_DOWN: return "SDL_EVENT_JOYSTICK_BUTTON_DOWN"; // Joystick button pressed
            case SDL_EVENT_JOYSTICK_BUTTON_UP: return "SDL_EVENT_JOYSTICK_BUTTON_UP"; // Joystick button released
            case SDL_EVENT_JOYSTICK_ADDED: return "SDL_EVENT_JOYSTICK_ADDED"; // A new joystick has been inserted
            case SDL_EVENT_JOYSTICK_REMOVED: return "SDL_EVENT_JOYSTICK_REMOVED"; // An opened joystick has been removed
            case SDL_EVENT_JOYSTICK_BATTERY_UPDATED: return "SDL_EVENT_JOYSTICK_BATTERY_UPDATED"; // Joystick battery level change
            case SDL_EVENT_JOYSTICK_UPDATE_COMPLETE: return "SDL_EVENT_JOYSTICK_UPDATE_COMPLETE"; // Joystick update has completed
            case SDL_EVENT_GAMEPAD_AXIS_MOTION: return "SDL_EVENT_GAMEPAD_AXIS_MOTION"; // Gamepad axis motion
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN: return "SDL_EVENT_GAMEPAD_BUTTON_DOWN"; // Gamepad button pressed
            case SDL_EVENT_GAMEPAD_BUTTON_UP: return "SDL_EVENT_GAMEPAD_BUTTON_UP"; // Gamepad button released
            case SDL_EVENT_GAMEPAD_ADDED: return "SDL_EVENT_GAMEPAD_ADDED"; // A new gamepad has been inserted
            case SDL_EVENT_GAMEPAD_REMOVED: return "SDL_EVENT_GAMEPAD_REMOVED"; // A gamepad has been removed
            case SDL_EVENT_GAMEPAD_REMAPPED: return "SDL_EVENT_GAMEPAD_REMAPPED"; // The gamepad mapping was updated
            case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN: return "SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN"; // Gamepad touchpad was touched
            case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION: return "SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION"; // Gamepad touchpad finger was moved
            case SDL_EVENT_GAMEPAD_TOUCHPAD_UP: return "SDL_EVENT_GAMEPAD_TOUCHPAD_UP"; // Gamepad touchpad finger was lifted
            case SDL_EVENT_GAMEPAD_SENSOR_UPDATE: return "SDL_EVENT_GAMEPAD_SENSOR_UPDATE"; // Gamepad sensor was updated
            case SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED: return "SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED"; // Gamepad Steam Controller handle was updated
            case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE: return "SDL_EVENT_GAMEPAD_UPDATE_COMPLETE"; // Gamepad update has completed
            case SDL_EVENT_FINGER_DOWN: return "SDL_EVENT_FINGER_DOWN"; // Finger down
            case SDL_EVENT_FINGER_UP: return "SDL_EVENT_FINGER_UP"; // Finger up
            case SDL_EVENT_FINGER_MOTION: return "SDL_EVENT_FINGER_MOTION"; // Finger motion
            case SDL_EVENT_CLIPBOARD_UPDATE: return "SDL_EVENT_CLIPBOARD_UPDATE"; // Clipboard updated
            case SDL_EVENT_DROP_FILE: return "SDL_EVENT_DROP_FILE"; // File dropped
            case SDL_EVENT_DROP_TEXT: return "SDL_EVENT_DROP_TEXT"; // Text dropped
            case SDL_EVENT_DROP_BEGIN: return "SDL_EVENT_DROP_BEGIN"; // Drag and drop began
            case SDL_EVENT_DROP_COMPLETE: return "SDL_EVENT_DROP_COMPLETE"; // Drag and drop completed
            case SDL_EVENT_DROP_POSITION: return "SDL_EVENT_DROP_POSITION"; // Drag and drop position
            case SDL_EVENT_AUDIO_DEVICE_ADDED: return "SDL_EVENT_AUDIO_DEVICE_ADDED"; // Audio device added
            case SDL_EVENT_AUDIO_DEVICE_REMOVED: return "SDL_EVENT_AUDIO_DEVICE_REMOVED"; // Audio device removed
            case SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED: return "SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED"; // Audio device format changed
            case SDL_EVENT_SENSOR_UPDATE: return "SDL_EVENT_SENSOR_UPDATE"; // Sensor updated
            case SDL_EVENT_PEN_PROXIMITY_IN: return "SDL_EVENT_PEN_PROXIMITY_IN"; // Pen proximity in
            case SDL_EVENT_PEN_PROXIMITY_OUT: return "SDL_EVENT_PEN_PROXIMITY_OUT"; // Pen proximity out
            case SDL_EVENT_PEN_DOWN: return "SDL_EVENT_PEN_DOWN"; // Pen down
            case SDL_EVENT_PEN_UP: return "SDL_EVENT_PEN_UP"; // Pen up
            case SDL_EVENT_PEN_BUTTON_DOWN: return "SDL_EVENT_PEN_BUTTON_DOWN"; // Pen button down
            case SDL_EVENT_PEN_BUTTON_UP: return "SDL_EVENT_PEN_BUTTON_UP"; // Pen button up
            case SDL_EVENT_PEN_MOTION: return "SDL_EVENT_PEN_MOTION"; // Pen motion
            case SDL_EVENT_PEN_AXIS: return "SDL_EVENT_PEN_AXIS"; // Pen axis changed
            case SDL_EVENT_CAMERA_DEVICE_ADDED: return "SDL_EVENT_CAMERA_DEVICE_ADDED"; // Camera device added
            case SDL_EVENT_CAMERA_DEVICE_REMOVED: return "SDL_EVENT_CAMERA_DEVICE_REMOVED"; // Camera device removed
            case SDL_EVENT_CAMERA_DEVICE_APPROVED: return "SDL_EVENT_CAMERA_DEVICE_APPROVED"; // Camera device approved
            case SDL_EVENT_CAMERA_DEVICE_DENIED: return "SDL_EVENT_CAMERA_DEVICE_DENIED"; // Camera device denied
            case SDL_EVENT_RENDER_TARGETS_RESET: return "SDL_EVENT_RENDER_TARGETS_RESET"; // Render targets reset
            case SDL_EVENT_RENDER_DEVICE_RESET: return "SDL_EVENT_RENDER_DEVICE_RESET"; // Render device reset
            case SDL_EVENT_RENDER_DEVICE_LOST: return "SDL_EVENT_RENDER_DEVICE_LOST"; // Render device lost
            case SDL_EVENT_SYSTEM_THEME_CHANGED: return "SDL_EVENT_SYSTEM_THEME_CHANGED"; // The system theme (dark or light) has changed
            case SDL_EVENT_PRIVATE0: return "SDL_EVENT_PRIVATE0"; // Reserved private event 0
            case SDL_EVENT_PRIVATE1: return "SDL_EVENT_PRIVATE1"; // Reserved private event 1
            case SDL_EVENT_PRIVATE2: return "SDL_EVENT_PRIVATE2"; // Reserved private event 2
            case SDL_EVENT_PRIVATE3: return "SDL_EVENT_PRIVATE3"; // Reserved private event 3
            case SDL_EVENT_POLL_SENTINEL: return "SDL_EVENT_POLL_SENTINEL"; // Signals the end of an event poll cycle
            case SDL_EVENT_USER: return "SDL_EVENT_USER"; // User-defined event
            case SDL_EVENT_LAST: return "SDL_EVENT_LAST"; // Last event
            default: return "SDL_EVENT_LAST";
        }
    } // END SDL_Utils::eventTypeToString()

    SDL_EventType SDL_Utils::eventTypeFromString(const std::string& str)
    {
        if (str == "SDL_EVENT_FIRST") return SDL_EVENT_FIRST;
        if (str == "SDL_EVENT_QUIT") return SDL_EVENT_QUIT;
        if (str == "SDL_EVENT_TERMINATING") return SDL_EVENT_TERMINATING;
        if (str == "SDL_EVENT_LOW_MEMORY") return SDL_EVENT_LOW_MEMORY;
        if (str == "SDL_EVENT_WILL_ENTER_BACKGROUND") return SDL_EVENT_WILL_ENTER_BACKGROUND;
        if (str == "SDL_EVENT_DID_ENTER_BACKGROUND") return SDL_EVENT_DID_ENTER_BACKGROUND;
        if (str == "SDL_EVENT_WILL_ENTER_FOREGROUND") return SDL_EVENT_WILL_ENTER_FOREGROUND;
        if (str == "SDL_EVENT_DID_ENTER_FOREGROUND") return SDL_EVENT_DID_ENTER_FOREGROUND;
        if (str == "SDL_EVENT_LOCALE_CHANGED") return SDL_EVENT_LOCALE_CHANGED;
        if (str == "SDL_EVENT_DISPLAY_ORIENTATION") return SDL_EVENT_DISPLAY_ORIENTATION;
        if (str == "SDL_EVENT_DISPLAY_ADDED") return SDL_EVENT_DISPLAY_ADDED;
        if (str == "SDL_EVENT_DISPLAY_REMOVED") return SDL_EVENT_DISPLAY_REMOVED;
        if (str == "SDL_EVENT_DISPLAY_MOVED") return SDL_EVENT_DISPLAY_MOVED;
        if (str == "SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED") return SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED;
        if (str == "SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED") return SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED;
        if (str == "SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED") return SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED;
        if (str == "SDL_EVENT_WINDOW_SHOWN") return SDL_EVENT_WINDOW_SHOWN;
        if (str == "SDL_EVENT_WINDOW_HIDDEN") return SDL_EVENT_WINDOW_HIDDEN;
        if (str == "SDL_EVENT_WINDOW_EXPOSED") return SDL_EVENT_WINDOW_EXPOSED;
        if (str == "SDL_EVENT_WINDOW_MOVED") return SDL_EVENT_WINDOW_MOVED;
        if (str == "SDL_EVENT_WINDOW_RESIZED") return SDL_EVENT_WINDOW_RESIZED;
        if (str == "SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED") return SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED;
        if (str == "SDL_EVENT_WINDOW_METAL_VIEW_RESIZED") return SDL_EVENT_WINDOW_METAL_VIEW_RESIZED;
        if (str == "SDL_EVENT_WINDOW_MINIMIZED") return SDL_EVENT_WINDOW_MINIMIZED;
        if (str == "SDL_EVENT_WINDOW_MAXIMIZED") return SDL_EVENT_WINDOW_MAXIMIZED;
        if (str == "SDL_EVENT_WINDOW_RESTORED") return SDL_EVENT_WINDOW_RESTORED;
        if (str == "SDL_EVENT_WINDOW_MOUSE_ENTER") return SDL_EVENT_WINDOW_MOUSE_ENTER;
        if (str == "SDL_EVENT_WINDOW_MOUSE_LEAVE") return SDL_EVENT_WINDOW_MOUSE_LEAVE;
        if (str == "SDL_EVENT_WINDOW_FOCUS_GAINED") return SDL_EVENT_WINDOW_FOCUS_GAINED;
        if (str == "SDL_EVENT_WINDOW_FOCUS_LOST") return SDL_EVENT_WINDOW_FOCUS_LOST;
        if (str == "SDL_EVENT_WINDOW_CLOSE_REQUESTED") return SDL_EVENT_WINDOW_CLOSE_REQUESTED;
        if (str == "SDL_EVENT_WINDOW_HIT_TEST") return SDL_EVENT_WINDOW_HIT_TEST;
        if (str == "SDL_EVENT_WINDOW_ICCPROF_CHANGED") return SDL_EVENT_WINDOW_ICCPROF_CHANGED;
        if (str == "SDL_EVENT_WINDOW_DISPLAY_CHANGED") return SDL_EVENT_WINDOW_DISPLAY_CHANGED;
        if (str == "SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED") return SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED;
        if (str == "SDL_EVENT_WINDOW_SAFE_AREA_CHANGED") return SDL_EVENT_WINDOW_SAFE_AREA_CHANGED;
        if (str == "SDL_EVENT_WINDOW_OCCLUDED") return SDL_EVENT_WINDOW_OCCLUDED;
        if (str == "SDL_EVENT_WINDOW_ENTER_FULLSCREEN") return SDL_EVENT_WINDOW_ENTER_FULLSCREEN;
        if (str == "SDL_EVENT_WINDOW_LEAVE_FULLSCREEN") return SDL_EVENT_WINDOW_LEAVE_FULLSCREEN;
        if (str == "SDL_EVENT_WINDOW_DESTROYED") return SDL_EVENT_WINDOW_DESTROYED;
        if (str == "SDL_EVENT_WINDOW_HDR_STATE_CHANGED") return SDL_EVENT_WINDOW_HDR_STATE_CHANGED;
        if (str == "SDL_EVENT_KEY_DOWN") return SDL_EVENT_KEY_DOWN;
        if (str == "SDL_EVENT_KEY_UP") return SDL_EVENT_KEY_UP;
        if (str == "SDL_EVENT_TEXT_EDITING") return SDL_EVENT_TEXT_EDITING;
        if (str == "SDL_EVENT_TEXT_INPUT") return SDL_EVENT_TEXT_INPUT;
        if (str == "SDL_EVENT_KEYMAP_CHANGED") return SDL_EVENT_KEYMAP_CHANGED;
        if (str == "SDL_EVENT_KEYBOARD_ADDED") return SDL_EVENT_KEYBOARD_ADDED;
        if (str == "SDL_EVENT_KEYBOARD_REMOVED") return SDL_EVENT_KEYBOARD_REMOVED;
        if (str == "SDL_EVENT_MOUSE_MOTION") return SDL_EVENT_MOUSE_MOTION;
        if (str == "SDL_EVENT_MOUSE_BUTTON_DOWN") return SDL_EVENT_MOUSE_BUTTON_DOWN;
        if (str == "SDL_EVENT_MOUSE_BUTTON_UP") return SDL_EVENT_MOUSE_BUTTON_UP;
        if (str == "SDL_EVENT_MOUSE_WHEEL") return SDL_EVENT_MOUSE_WHEEL;
        if (str == "SDL_EVENT_MOUSE_ADDED") return SDL_EVENT_MOUSE_ADDED;
        if (str == "SDL_EVENT_MOUSE_REMOVED") return SDL_EVENT_MOUSE_REMOVED;
        if (str == "SDL_EVENT_JOYSTICK_AXIS_MOTION") return SDL_EVENT_JOYSTICK_AXIS_MOTION;
        if (str == "SDL_EVENT_JOYSTICK_BALL_MOTION") return SDL_EVENT_JOYSTICK_BALL_MOTION;
        if (str == "SDL_EVENT_JOYSTICK_HAT_MOTION") return SDL_EVENT_JOYSTICK_HAT_MOTION;
        if (str == "SDL_EVENT_JOYSTICK_BUTTON_DOWN") return SDL_EVENT_JOYSTICK_BUTTON_DOWN;
        if (str == "SDL_EVENT_JOYSTICK_BUTTON_UP") return SDL_EVENT_JOYSTICK_BUTTON_UP;
        if (str == "SDL_EVENT_JOYSTICK_ADDED") return SDL_EVENT_JOYSTICK_ADDED;
        if (str == "SDL_EVENT_JOYSTICK_REMOVED") return SDL_EVENT_JOYSTICK_REMOVED;
        if (str == "SDL_EVENT_JOYSTICK_BATTERY_UPDATED") return SDL_EVENT_JOYSTICK_BATTERY_UPDATED;
        if (str == "SDL_EVENT_JOYSTICK_UPDATE_COMPLETE") return SDL_EVENT_JOYSTICK_UPDATE_COMPLETE;
        if (str == "SDL_EVENT_GAMEPAD_AXIS_MOTION") return SDL_EVENT_GAMEPAD_AXIS_MOTION;
        if (str == "SDL_EVENT_GAMEPAD_BUTTON_DOWN") return SDL_EVENT_GAMEPAD_BUTTON_DOWN;
        if (str == "SDL_EVENT_GAMEPAD_BUTTON_UP") return SDL_EVENT_GAMEPAD_BUTTON_UP;
        if (str == "SDL_EVENT_GAMEPAD_ADDED") return SDL_EVENT_GAMEPAD_ADDED;
        if (str == "SDL_EVENT_GAMEPAD_REMOVED") return SDL_EVENT_GAMEPAD_REMOVED;
        if (str == "SDL_EVENT_GAMEPAD_REMAPPED") return SDL_EVENT_GAMEPAD_REMAPPED;
        if (str == "SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN") return SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN;
        if (str == "SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION") return SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION;
        if (str == "SDL_EVENT_GAMEPAD_TOUCHPAD_UP") return SDL_EVENT_GAMEPAD_TOUCHPAD_UP;
        if (str == "SDL_EVENT_GAMEPAD_SENSOR_UPDATE") return SDL_EVENT_GAMEPAD_SENSOR_UPDATE;
        if (str == "SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED") return SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED;
        if (str == "SDL_EVENT_GAMEPAD_UPDATE_COMPLETE") return SDL_EVENT_GAMEPAD_UPDATE_COMPLETE;
        if (str == "SDL_EVENT_FINGER_DOWN") return SDL_EVENT_FINGER_DOWN;
        if (str == "SDL_EVENT_FINGER_UP") return SDL_EVENT_FINGER_UP;
        if (str == "SDL_EVENT_FINGER_MOTION") return SDL_EVENT_FINGER_MOTION;
        if (str == "SDL_EVENT_CLIPBOARD_UPDATE") return SDL_EVENT_CLIPBOARD_UPDATE;
        if (str == "SDL_EVENT_DROP_FILE") return SDL_EVENT_DROP_FILE;
        if (str == "SDL_EVENT_DROP_TEXT") return SDL_EVENT_DROP_TEXT;
        if (str == "SDL_EVENT_DROP_BEGIN") return SDL_EVENT_DROP_BEGIN;
        if (str == "SDL_EVENT_DROP_COMPLETE") return SDL_EVENT_DROP_COMPLETE;
        if (str == "SDL_EVENT_DROP_POSITION") return SDL_EVENT_DROP_POSITION;
        if (str == "SDL_EVENT_AUDIO_DEVICE_ADDED") return SDL_EVENT_AUDIO_DEVICE_ADDED;
        if (str == "SDL_EVENT_AUDIO_DEVICE_REMOVED") return SDL_EVENT_AUDIO_DEVICE_REMOVED;
        if (str == "SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED") return SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED;
        if (str == "SDL_EVENT_SENSOR_UPDATE") return SDL_EVENT_SENSOR_UPDATE;
        if (str == "SDL_EVENT_PEN_PROXIMITY_IN") return SDL_EVENT_PEN_PROXIMITY_IN;
        if (str == "SDL_EVENT_PEN_PROXIMITY_OUT") return SDL_EVENT_PEN_PROXIMITY_OUT;
        if (str == "SDL_EVENT_PEN_DOWN") return SDL_EVENT_PEN_DOWN;
        if (str == "SDL_EVENT_PEN_UP") return SDL_EVENT_PEN_UP;
        if (str == "SDL_EVENT_PEN_BUTTON_DOWN") return SDL_EVENT_PEN_BUTTON_DOWN;
        if (str == "SDL_EVENT_PEN_BUTTON_UP") return SDL_EVENT_PEN_BUTTON_UP;
        if (str == "SDL_EVENT_PEN_MOTION") return SDL_EVENT_PEN_MOTION;
        if (str == "SDL_EVENT_PEN_AXIS") return SDL_EVENT_PEN_AXIS;
        if (str == "SDL_EVENT_CAMERA_DEVICE_ADDED") return SDL_EVENT_CAMERA_DEVICE_ADDED;
        if (str == "SDL_EVENT_CAMERA_DEVICE_REMOVED") return SDL_EVENT_CAMERA_DEVICE_REMOVED;
        if (str == "SDL_EVENT_CAMERA_DEVICE_APPROVED") return SDL_EVENT_CAMERA_DEVICE_APPROVED;
        if (str == "SDL_EVENT_CAMERA_DEVICE_DENIED") return SDL_EVENT_CAMERA_DEVICE_DENIED;
        if (str == "SDL_EVENT_RENDER_TARGETS_RESET") return SDL_EVENT_RENDER_TARGETS_RESET;
        if (str == "SDL_EVENT_RENDER_DEVICE_RESET") return SDL_EVENT_RENDER_DEVICE_RESET;
        if (str == "SDL_EVENT_RENDER_DEVICE_LOST") return SDL_EVENT_RENDER_DEVICE_LOST;
        if (str == "SDL_EVENT_SYSTEM_THEME_CHANGED") return SDL_EVENT_SYSTEM_THEME_CHANGED;
        if (str == "SDL_EVENT_PRIVATE0") return SDL_EVENT_PRIVATE0;
        if (str == "SDL_EVENT_PRIVATE1") return SDL_EVENT_PRIVATE1;
        if (str == "SDL_EVENT_PRIVATE2") return SDL_EVENT_PRIVATE2;
        if (str == "SDL_EVENT_PRIVATE3") return SDL_EVENT_PRIVATE3;
        if (str == "SDL_EVENT_POLL_SENTINEL") return SDL_EVENT_POLL_SENTINEL;
        if (str == "SDL_EVENT_USER") return SDL_EVENT_USER;
        if (str == "SDL_EVENT_LAST") return SDL_EVENT_LAST;
        return SDL_EVENT_LAST; // Default/fallback
    } // END SDL_Utils::eventTypeFromString()

    int SDL_Utils::keyToAscii(SDL_Keycode keycode, SDL_Keymod keymod)
    {
        // std::vector<std::tuple<SDL_Keycode, char, char>> scancodeToAsciiMap =
        static const std::unordered_map<SDL_Keycode, std::pair<char, char>> scancodeToAsciiMap = 
        {
            { SDLK_UNKNOWN,             { ' ',        ' '    } },   // 0x0000000du // < '\r'
            { SDLK_RETURN,              { '\r',       '\r'   } },   // 0x0000001bu // < '\x1B'
            { SDLK_ESCAPE,              { '\x1B',     '\x1B' } },   // 0x00000008u // < '\b'
            { SDLK_BACKSPACE,           { '\b',       '\b'   } },   // 0x00000009u // < '\t'
            { SDLK_TAB,                 { '\t',       '\t'   } },   // 0x00000020u // < ' '
            { SDLK_SPACE,               { ' ',        ' '    } },   // 0x00000021u // < '!'
            { SDLK_EXCLAIM,             { '!',        '1'    } },   // 0x00000022u // < '"'
            { SDLK_DBLAPOSTROPHE,       { '"',        '\''   } },   // 0x00000023u // < '#'
            { SDLK_HASH,                { '#',        '3'    } },   // 0x00000024u // < '$'
            { SDLK_DOLLAR,              { '$',        '4'    } },   // 0x00000025u // < '%'
            { SDLK_PERCENT,             { '%',        '5'    } },   // 0x00000026u // < '&'
            { SDLK_AMPERSAND,           { '&',        '7'    } },   // 0x00000027u // < '\''
            { SDLK_APOSTROPHE,          { '\'',       '\"'   } },   // 0x00000028u // < '('
            { SDLK_LEFTPAREN,           { '(',        '9'    } },   // 0x00000029u // < ')'
            { SDLK_RIGHTPAREN,          { ')',        '0'    } },   // 0x0000002au // < '*'
            { SDLK_ASTERISK,            { '*',        '8'    } },   // 0x0000002bu // < '+'
            { SDLK_PLUS,                { '+',        '='    } },   // 0x0000002cu // < ','
            { SDLK_COMMA,               { ',',        '<'    } },   // 0x0000002du // < '-'
            { SDLK_MINUS,               { '-',        '_'    } },   // 0x0000002eu // < '.'
            { SDLK_PERIOD,              { '.',        '>'    } },   // 0x0000002fu // < '/'
            { SDLK_SLASH,               { '/',        '?'    } },   // 0x00000030u // < '0'
            { SDLK_0,                   { '0',        ')'    } },   // 0x00000031u // < '1'
            { SDLK_1,                   { '1',        '!'    } },   // 0x00000032u // < '2'
            { SDLK_2,                   { '2',        '@'    } },   // 0x00000033u // < '3'
            { SDLK_3,                   { '3',        '#'    } },   // 0x00000034u // < '4'
            { SDLK_4,                   { '4',        '$'    } },   // 0x00000035u // < '5'
            { SDLK_5,                   { '5',        '%'    } },   // 0x00000036u // < '6'
            { SDLK_6,                   { '6',        '^'    } },   // 0x00000037u // < '7'
            { SDLK_7,                   { '7',        '&'    } },   // 0x00000038u // < '8'
            { SDLK_8,                   { '8',        '*'    } },   // 0x00000039u // < '9'
            { SDLK_9,                   { '9',        '('    } },   // 0x0000003au // < ':'
            { SDLK_COLON,               { ':',        ';'    } },   // 0x0000003bu // < ';'
            { SDLK_SEMICOLON,           { ';',        ':'    } },   // 0x0000003cu // < '<'
            { SDLK_LESS,                { '<',        ','    } },   // 0x0000003du // < '='
            { SDLK_EQUALS,              { '=',        '+'    } },   // 0x0000003eu // < '>'
            { SDLK_GREATER,             { '>',        '.'    } },   // 0x0000003fu // < '?'
            { SDLK_QUESTION,            { '?',        '/'    } },   // 0x00000040u // < '@'
            { SDLK_AT,                  { '@',        '2'    } },   // 0x0000005bu // < '['
            { SDLK_LEFTBRACKET,         { '[',        '{'    } },   // 0x0000005cu // < '\\'
            { SDLK_RIGHTBRACKET,        { ']',        '}'    } },   // 0x0000005du // < ']'
            { SDLK_CARET,               { '^',        '6'    } },   // 0x0000005eu // < '^'
            { SDLK_UNDERSCORE,          { '_',        '-'    } },   // 0x0000005fu // < '_'
            { SDLK_GRAVE,               { '`',        '~'    } },   // 0x00000060u // < '`'
            { SDLK_A,                   { 'a',        'A'    } },   // 0x00000061u // < 'a'
            { SDLK_B,                   { 'b',        'B'    } },   // 0x00000062u // < 'b'
            { SDLK_C,                   { 'c',        'C'    } },   // 0x00000063u // < 'c'
            { SDLK_D,                   { 'd',        'D'    } },   // 0x00000064u // < 'd'
            { SDLK_E,                   { 'e',        'E'    } },   // 0x00000065u // < 'e'
            { SDLK_F,                   { 'f',        'F'    } },   // 0x00000066u // < 'f'
            { SDLK_G,                   { 'g',        'G'    } },   // 0x00000067u // < 'g'
            { SDLK_H,                   { 'h',        'H'    } },   // 0x00000068u // < 'h'
            { SDLK_I,                   { 'i',        'I'    } },   // 0x00000069u // < 'i'
            { SDLK_J,                   { 'j',        'J'    } },   // 0x0000006au // < 'j'
            { SDLK_K,                   { 'k',        'K'    } },   // 0x0000006bu // < 'k'
            { SDLK_L,                   { 'l',        'L'    } },   // 0x0000006cu // < 'l'
            { SDLK_M,                   { 'm',        'M'    } },   // 0x0000006du // < 'm'
            { SDLK_N,                   { 'n',        'N'    } },   // 0x0000006eu // < 'n'
            { SDLK_O,                   { 'o',        'O'    } },   // 0x0000006fu // < 'o'
            { SDLK_P,                   { 'p',        'P'    } },   // 0x00000070u // < 'p'
            { SDLK_Q,                   { 'q',        'Q'    } },   // 0x00000071u // < 'q'
            { SDLK_R,                   { 'r',        'R'    } },   // 0x00000072u // < 'r'
            { SDLK_S,                   { 's',        'S'    } },   // 0x00000073u // < 's'
            { SDLK_T,                   { 't',        'T'    } },   // 0x00000074u // < 't'
            { SDLK_U,                   { 'u',        'U'    } },   // 0x00000075u // < 'u'
            { SDLK_V,                   { 'v',        'V'    } },   // 0x00000076u // < 'v'
            { SDLK_W,                   { 'w',        'W'    } },   // 0x00000077u // < 'w'
            { SDLK_X,                   { 'x',        'X'    } },   // 0x00000078u // < 'x'
            { SDLK_Y,                   { 'y',        'Y'    } },   // 0x00000079u // < 'y'
            { SDLK_Z,                   { 'z',        'Z'    } },   // 0x0000007au // < 'z'
            { SDLK_LEFTBRACE,           { '{',        '['    } },   // 0x0000007bu // < '{'
            { SDLK_PIPE,                { '|',        '\\'   } },   // 0x0000007cu // < '|'
            { SDLK_RIGHTBRACE,          { '}',        ']'    } },   // 0x0000007du // < '}'
            { SDLK_TILDE,               { '~',        '~'    } },   // 0x0000007eu // < '~'
            { SDLK_DELETE,              { '\x7F',     '\x7F' } },   // 0x0000007fu // < '\x7F'
            { SDLK_PLUSMINUS,           { '\xB1',     '\xB1' } },   // 0x000000b1u // < '\xB1'
            { SDLK_CAPSLOCK,            { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CAPSLOCK),             SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CAPSLOCK)}},
            { SDLK_F1,                  { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F1),                   SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F1)}},
            { SDLK_F2,                  { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F2),                   SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F2)}},
            { SDLK_F3,                  { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F3),                   SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F3)}},
            { SDLK_F4,                  { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F4),                   SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F4)}},
            { SDLK_F5,                  { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F5),                   SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F5)}},
            { SDLK_F6,                  { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F6),                   SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F6)}},
            { SDLK_F7,                  { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F7),                   SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F7)}},
            { SDLK_F8,                  { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F8),                   SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F8)}},
            { SDLK_F9,                  { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F9),                   SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F9)}},
            { SDLK_F10,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F10),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F10)}},
            { SDLK_F11,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F11),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F11)}},
            { SDLK_F12,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F12),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F12)}},
            { SDLK_PRINTSCREEN,         { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PRINTSCREEN),          SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PRINTSCREEN)}},
            { SDLK_SCROLLLOCK,          { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SCROLLLOCK),           SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SCROLLLOCK)}},
            { SDLK_PAUSE,               { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAUSE),                SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAUSE)}},
            { SDLK_INSERT,              { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_INSERT),               SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_INSERT)}},
            { SDLK_HOME,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_HOME),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_HOME)}},
            { SDLK_PAGEUP,              { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAGEUP),               SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAGEUP)}},
            { SDLK_END,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_END),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_END)}},
            { SDLK_PAGEDOWN,            { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAGEDOWN),             SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAGEDOWN)}},
            { SDLK_RIGHT,               { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RIGHT),                SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RIGHT)}},
            { SDLK_LEFT,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LEFT),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LEFT)}},
            { SDLK_DOWN,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DOWN),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DOWN)}},
            { SDLK_UP,                  { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_UP),                   SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_UP)}},
            { SDLK_NUMLOCKCLEAR,        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_NUMLOCKCLEAR),         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_NUMLOCKCLEAR)}},
            { SDLK_KP_DIVIDE,           { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DIVIDE),            SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DIVIDE)}},
            { SDLK_KP_MULTIPLY,         { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MULTIPLY),          SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MULTIPLY)}},
            { SDLK_KP_MINUS,            { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MINUS),             SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MINUS)}},
            { SDLK_KP_PLUS,             { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PLUS),              SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PLUS)}},
            { SDLK_KP_ENTER,            { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_ENTER),             SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_ENTER)}},
            { SDLK_KP_1,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_1),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_1)}},
            { SDLK_KP_2,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_2),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_2)}},
            { SDLK_KP_3,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_3),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_3)}},
            { SDLK_KP_4,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_4),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_4)}},
            { SDLK_KP_5,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_5),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_5)}},
            { SDLK_KP_6,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_6),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_6)}},
            { SDLK_KP_7,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_7),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_7)}},
            { SDLK_KP_8,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_8),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_8)}},
            { SDLK_KP_9,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_9),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_9)}},
            { SDLK_KP_0,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_0),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_0)}},
            { SDLK_KP_PERIOD,           { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PERIOD),            SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PERIOD)}},
            { SDLK_APPLICATION,         { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APPLICATION),          SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APPLICATION)}},
            { SDLK_POWER,               { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_POWER),                SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_POWER)}},
            { SDLK_KP_EQUALS,           { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EQUALS),            SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EQUALS)}},
            { SDLK_F13,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F13),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F13)}},
            { SDLK_F14,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F14),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F14)}},
            { SDLK_F15,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F15),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F15)}},
            { SDLK_F16,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F16),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F16)}},
            { SDLK_F17,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F17),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F17)}},
            { SDLK_F18,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F18),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F18)}},
            { SDLK_F19,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F19),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F19)}},
            { SDLK_F20,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F20),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F20)}},
            { SDLK_F21,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F21),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F21)}},
            { SDLK_F22,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F22),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F22)}},
            { SDLK_F23,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F23),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F23)}},
            { SDLK_F24,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F24),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F24)}},
            { SDLK_EXECUTE,             { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_EXECUTE),              SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_EXECUTE)}},
            { SDLK_HELP,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_HELP),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_HELP)}},
            { SDLK_MENU,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MENU),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MENU)}},
            { SDLK_SELECT,              { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SELECT),               SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SELECT)}},
            { SDLK_STOP,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_STOP),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_STOP)}},
            { SDLK_AGAIN,               { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AGAIN),                SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AGAIN)}},
            { SDLK_UNDO,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_UNDO),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_UNDO)}},
            { SDLK_CUT,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CUT),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CUT)}},
            { SDLK_COPY,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_COPY),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_COPY)}},
            { SDLK_PASTE,               { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PASTE),                SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PASTE)}},
            { SDLK_FIND,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_FIND),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_FIND)}},
            { SDLK_MUTE,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MUTE),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MUTE)}},
            { SDLK_VOLUMEUP,            { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_VOLUMEUP),             SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_VOLUMEUP)}},
            { SDLK_VOLUMEDOWN,          { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_VOLUMEDOWN),           SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_VOLUMEDOWN)}},
            { SDLK_KP_COMMA,            { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_COMMA),             SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_COMMA)}},
            { SDLK_KP_EQUALSAS400,      { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EQUALSAS400),       SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EQUALSAS400)}},
            { SDLK_ALTERASE,            { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_ALTERASE),             SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_ALTERASE)}},
            { SDLK_SYSREQ,              { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SYSREQ),               SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SYSREQ)}},
            { SDLK_CANCEL,              { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CANCEL),               SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CANCEL)}},
            { SDLK_CLEAR,               { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CLEAR),                SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CLEAR)}},
            { SDLK_PRIOR,               { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PRIOR),                SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PRIOR)}},
            { SDLK_RETURN2,             { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RETURN2),              SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RETURN2)}},
            { SDLK_SEPARATOR,           { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SEPARATOR),            SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SEPARATOR)}},
            { SDLK_OUT,                 { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_OUT),                  SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_OUT)}},
            { SDLK_OPER,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_OPER),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_OPER)}},
            { SDLK_CLEARAGAIN,          { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CLEARAGAIN),           SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CLEARAGAIN)}},
            { SDLK_CRSEL,               { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CRSEL),                SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CRSEL)}},
            { SDLK_EXSEL,               { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_EXSEL),                SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_EXSEL)}},
            { SDLK_KP_00,               { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_00),                SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_00)}},
            { SDLK_KP_000,              { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_000),               SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_000)}},
            { SDLK_THOUSANDSSEPARATOR,  { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_THOUSANDSSEPARATOR),   SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_THOUSANDSSEPARATOR)}},
            { SDLK_DECIMALSEPARATOR,    { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DECIMALSEPARATOR),     SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DECIMALSEPARATOR)}},
            { SDLK_CURRENCYUNIT,        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CURRENCYUNIT),         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CURRENCYUNIT)}},
            { SDLK_CURRENCYSUBUNIT,     { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CURRENCYSUBUNIT),      SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CURRENCYSUBUNIT)}},
            { SDLK_KP_LEFTPAREN,        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTPAREN),         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTPAREN)}},
            { SDLK_KP_RIGHTPAREN,       { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTPAREN),        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTPAREN)}},
            { SDLK_KP_LEFTBRACE,        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTBRACE),         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTBRACE)}},
            { SDLK_KP_RIGHTBRACE,       { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTBRACE),        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTBRACE)}},
            { SDLK_KP_TAB,              { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_TAB),               SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_TAB)}},
            { SDLK_KP_BACKSPACE,        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BACKSPACE),         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BACKSPACE)}},
            { SDLK_KP_A,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_A),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_A)}},
            { SDLK_KP_B,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_B),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_B)}},
            { SDLK_KP_C,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_C),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_C)}},
            { SDLK_KP_D,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_D),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_D)}},
            { SDLK_KP_E,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_E),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_E)}},
            { SDLK_KP_F,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_F),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_F)}},
            { SDLK_KP_XOR,              { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_XOR),               SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_XOR)}},
            { SDLK_KP_POWER,            { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_POWER),             SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_POWER)}},
            { SDLK_KP_PERCENT,          { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PERCENT),           SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PERCENT)}},
            { SDLK_KP_LESS,             { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LESS),              SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LESS)}},
            { SDLK_KP_GREATER,          { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_GREATER),           SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_GREATER)}},
            { SDLK_KP_AMPERSAND,        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AMPERSAND),         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AMPERSAND)}},
            { SDLK_KP_DBLAMPERSAND,     { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLAMPERSAND),      SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLAMPERSAND)}},
            { SDLK_KP_VERTICALBAR,      { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_VERTICALBAR),       SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_VERTICALBAR)}},
            { SDLK_KP_DBLVERTICALBAR,   { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLVERTICALBAR),    SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLVERTICALBAR)}},
            { SDLK_KP_COLON,            { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_COLON),             SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_COLON)}},
            { SDLK_KP_HASH,             { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HASH),              SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HASH)}},
            { SDLK_KP_SPACE,            { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_SPACE),             SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_SPACE)}},
            { SDLK_KP_AT,               { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AT),                SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AT)}},
            { SDLK_KP_EXCLAM,           { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EXCLAM),            SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EXCLAM)}},
            { SDLK_KP_MEMSTORE,         { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSTORE),          SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSTORE)}},
            { SDLK_KP_MEMRECALL,        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMRECALL),         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMRECALL)}},
            { SDLK_KP_MEMCLEAR,         { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMCLEAR),          SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMCLEAR)}},
            { SDLK_KP_MEMADD,           { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMADD),            SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMADD)}},
            { SDLK_KP_MEMSUBTRACT,      { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSUBTRACT),       SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSUBTRACT)}},
            { SDLK_KP_MEMMULTIPLY,      { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMMULTIPLY),       SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMMULTIPLY)}},
            { SDLK_KP_MEMDIVIDE,        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMDIVIDE),         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMDIVIDE)}},
            { SDLK_KP_PLUSMINUS,        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PLUSMINUS),         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PLUSMINUS)}},
            { SDLK_KP_CLEAR,            { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEAR),             SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEAR)}},
            { SDLK_KP_CLEARENTRY,       { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEARENTRY),        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEARENTRY)}},
            { SDLK_KP_BINARY,           { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BINARY),            SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BINARY)}},
            { SDLK_KP_OCTAL,            { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_OCTAL),             SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_OCTAL)}},
            { SDLK_KP_DECIMAL,          { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DECIMAL),           SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DECIMAL)}},
            { SDLK_KP_HEXADECIMAL,      { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HEXADECIMAL),       SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HEXADECIMAL)}},
            { SDLK_LCTRL,               { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LCTRL),                SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LCTRL)}},
            { SDLK_LSHIFT,              { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LSHIFT),               SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LSHIFT)}},
            { SDLK_LALT,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LALT),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LALT)}},
            { SDLK_LGUI,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LGUI),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LGUI)}},
            { SDLK_RCTRL,               { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RCTRL),                SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RCTRL)}},
            { SDLK_RSHIFT,              { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RSHIFT),               SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RSHIFT)}},
            { SDLK_RALT,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RALT),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RALT)}},
            { SDLK_RGUI,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RGUI),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RGUI)}},
            { SDLK_MODE,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MODE),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MODE)}},
            { SDLK_SLEEP,               { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SLEEP),                SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SLEEP)}},
            { SDLK_WAKE,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_WAKE),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_WAKE)}},
            { SDLK_CHANNEL_INCREMENT,   { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CHANNEL_INCREMENT),    SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CHANNEL_INCREMENT)}},
            { SDLK_CHANNEL_DECREMENT,   { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CHANNEL_DECREMENT),    SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CHANNEL_DECREMENT)}},
            { SDLK_MEDIA_PLAY,          { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_PLAY),           SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_PLAY)}},
            { SDLK_MEDIA_PAUSE,         { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_PAUSE),          SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_PAUSE)}},
            { SDLK_MEDIA_RECORD,        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_RECORD),         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_RECORD)}},
            { SDLK_MEDIA_FAST_FORWARD,  { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_FAST_FORWARD),   SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_FAST_FORWARD)}},
            { SDLK_MEDIA_REWIND,        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_REWIND),         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_REWIND)}},
            { SDLK_MEDIA_NEXT_TRACK,    { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_NEXT_TRACK),     SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_NEXT_TRACK)}},
            { SDLK_MEDIA_PREVIOUS_TRACK, { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_PREVIOUS_TRACK), SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_PREVIOUS_TRACK)}},
            { SDLK_MEDIA_STOP,          { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_STOP),           SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_STOP)}},
            { SDLK_MEDIA_EJECT,         { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_EJECT),          SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_EJECT)}},
            { SDLK_MEDIA_PLAY_PAUSE,    { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_PLAY_PAUSE),     SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_PLAY_PAUSE)}},
            { SDLK_MEDIA_SELECT,        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_SELECT),         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_SELECT)}},
            { SDLK_AC_NEW,              { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_NEW),               SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_NEW)}},
            { SDLK_AC_OPEN,             { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_OPEN),              SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_OPEN)}},
            { SDLK_AC_CLOSE,            { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_CLOSE),             SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_CLOSE)}},
            { SDLK_AC_EXIT,             { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_EXIT),              SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_EXIT)}},
            { SDLK_AC_SAVE,             { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_SAVE),              SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_SAVE)}},
            { SDLK_AC_PRINT,            { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_PRINT),             SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_PRINT)}},
            { SDLK_AC_PROPERTIES,       { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_PROPERTIES),        SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_PROPERTIES)}},
            { SDLK_AC_SEARCH,           { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_SEARCH),            SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_SEARCH)}},
            { SDLK_AC_HOME,             { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_HOME),              SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_HOME)}},
            { SDLK_AC_BACK,             { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BACK),              SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BACK)}},
            { SDLK_AC_FORWARD,          { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_FORWARD),           SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_FORWARD)}},
            { SDLK_AC_STOP,             { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_STOP),              SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_STOP)}},
            { SDLK_AC_REFRESH,          { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_REFRESH),           SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_REFRESH)}},
            { SDLK_AC_BOOKMARKS,        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BOOKMARKS),         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BOOKMARKS)}},
            { SDLK_SOFTLEFT,            { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SOFTLEFT),             SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SOFTLEFT)}},
            { SDLK_SOFTRIGHT,           { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SOFTRIGHT),            SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SOFTRIGHT)}},
            { SDLK_CALL,                { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CALL),                 SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CALL)}},
            { SDLK_ENDCALL,             { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_ENDCALL),              SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_ENDCALL)}},
            { SDLK_LEFT_TAB,            { ' ', ' '}},  // 0x20000001u < Extended key Left Tab
            { SDLK_LEVEL5_SHIFT,        { ' ', ' '}},  // 0x20000002u < Extended key Level 5 Shift
            { SDLK_MULTI_KEY_COMPOSE,   { ' ', ' '}},  // 0x20000003u < Extended key Multi-key Compose
            { SDLK_LMETA,               { ' ', ' '}},  // 0x20000004u < Extended key Left Meta
            { SDLK_RMETA,               { ' ', ' '}},  // 0x20000005u < Extended key Right Meta
            { SDLK_LHYPER,              { ' ', ' '}},  // 0x20000006u < Extended key Left Hyper
            { SDLK_RHYPER,              { ' ', ' '}}   // 0x20000007u < Extended key Right Hyper
        };

        auto it = scancodeToAsciiMap.find(keycode);
        if (it != scancodeToAsciiMap.end()) 
        {
            char normalChar = it->second.first;
            char shiftedChar = it->second.second;
            bool isShiftPressed = keymod & SDL_KMOD_SHIFT;
            bool isCapsLockActive = keymod & SDL_KMOD_CAPS;
            // Determine the ASCII character to return
            if (isShiftPressed) 
            {
                return shiftedChar;
            } else if (isCapsLockActive && std::isalpha(normalChar)) 
            {
                return std::toupper(normalChar);
            } 
            else 
            {
                return normalChar;
            }
        }
        // Return 0 if the keycode is not found
        return 0;
    } // END SDL_Utils::SDLKeycodeToAscii()

    sol::table SDL_Utils::eventToLuaTable(const SDL_Event& se, sol::state_view lua)
    {
    sol::table t = lua.create_table();
    SDL_EventType etype = static_cast<SDL_EventType>(se.type);
    t["type"] = SDL_Utils::eventTypeToString(etype);
    // include common timestamp if available
    try { t["timestamp"] = se.common.timestamp; } catch(...) {}

        switch (etype) {
            // -- Application Events -- //
            case SDL_EVENT_TERMINATING:
            case SDL_EVENT_LOW_MEMORY:
            case SDL_EVENT_WILL_ENTER_BACKGROUND:
            case SDL_EVENT_DID_ENTER_BACKGROUND:
            case SDL_EVENT_WILL_ENTER_FOREGROUND:
            case SDL_EVENT_DID_ENTER_FOREGROUND:
            case SDL_EVENT_LOCALE_CHANGED:
            case SDL_EVENT_SYSTEM_THEME_CHANGED: {
                sol::table app = lua.create_table();
                try { app["timestamp"] = se.common.timestamp; } catch(...) {}
                // Optionally, add a name for the event:
                app["event"] = SDL_Utils::eventTypeToString(etype);
                t["app"] = app;
                break;
            }      
            case SDL_EVENT_QUIT: {
                sol::table q = lua.create_table();
                try { q["timestamp"] = se.quit.timestamp; } catch(...) {}
                t["quit"] = q;
                break;
            }

            // -- Mouse Events -- //
            case SDL_EVENT_MOUSE_MOTION: {
                sol::table m = lua.create_table();
                try { m["timestamp"] = se.motion.timestamp; } catch(...) {}
                try { m["windowID"] = se.motion.windowID; } catch(...) {}
                try { m["which"] = se.motion.which; } catch(...) {}
                try { m["state"] = static_cast<Uint32>(se.motion.state); } catch(...) {}
                m["x"] = se.motion.x;
                m["y"] = se.motion.y;
                m["xrel"] = se.motion.xrel;
                m["yrel"] = se.motion.yrel;
                t["motion"] = m;
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                sol::table b = lua.create_table();
                try { b["timestamp"] = se.button.timestamp; } catch(...) {}
                try { b["windowID"] = se.button.windowID; } catch(...) {}
                try { b["which"] = se.button.which; } catch(...) {}
                try { b["down"] = se.button.down; } catch(...) {}
                b["button"] = se.button.button;
                b["clicks"] = se.button.clicks;
                b["x"] = se.button.x;
                b["y"] = se.button.y;
                t["button"] = b;
                break;
            }
            case SDL_EVENT_MOUSE_WHEEL: {
                sol::table w = lua.create_table();
                try { w["timestamp"] = se.wheel.timestamp; } catch(...) {}
                try { w["windowID"] = se.wheel.windowID; } catch(...) {}
                try { w["which"] = se.wheel.which; } catch(...) {}
                w["x"] = se.wheel.x;
                w["y"] = se.wheel.y;
                try { w["direction"] = static_cast<int>(se.wheel.direction); } catch(...) {}
                w["mouse_x"] = se.wheel.mouse_x;
                w["mouse_y"] = se.wheel.mouse_y;
                try { w["integer_x"] = se.wheel.integer_x; } catch(...) {}
                try { w["integer_y"] = se.wheel.integer_y; } catch(...) {}
                t["wheel"] = w;
                break;
            }
            case SDL_EVENT_MOUSE_ADDED:
            case SDL_EVENT_MOUSE_REMOVED: {
                sol::table md = lua.create_table();
                try { md["timestamp"] = se.mdevice.timestamp; } catch(...) {}
                try { md["which"] = se.mdevice.which; } catch(...) {}
                t["mdevice"] = md;
                break;
            }            

            // -- Keyboard Events -- //
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP: {
                sol::table k = lua.create_table();
                try { k["timestamp"] = se.key.timestamp; } catch(...) {}
                try { k["windowID"] = se.key.windowID; } catch(...) {}
                try { k["which"] = se.key.which; } catch(...) {}
                k["scancode"] = se.key.scancode;
                k["keycode"] = se.key.key;
                try { k["mod"] = se.key.mod; } catch(...) {}
                try { k["raw"] = se.key.raw; } catch(...) {}
                try { k["down"] = se.key.down; } catch(...) {}
                try { k["repeat"] = se.key.repeat; } catch(...) {}
                t["key"] = k;
                break;
            }
            case SDL_EVENT_TEXT_EDITING: {
                sol::table edit = lua.create_table();
                try { edit["timestamp"] = se.edit.timestamp; } catch(...) {}
                try { edit["windowID"] = se.edit.windowID; } catch(...) {}
                try { edit["text"] = std::string(se.edit.text ? se.edit.text : ""); } catch(...) { edit["text"] = std::string(); }
                try { edit["start"] = se.edit.start; } catch(...) {}
                try { edit["length"] = se.edit.length; } catch(...) {}
                t["edit"] = edit;
                break;
            }
            // case SDL_EVENT_TEXT_EDITING_CANDIDATES: {
            //     sol::table cand = lua.create_table();
            //     try { cand["timestamp"] = se.edit_cands.timestamp; } catch(...) {}
            //     try { cand["windowID"] = se.edit_cands.windowID; } catch(...) {}
            //     try { cand["num_candidates"] = se.edit_cands.num_candidates; } catch(...) {}
            //     try { cand["selected_candidate"] = se.edit_cands.selected_candidate; } catch(...) {}
            //     try { cand["horizontal"] = se.edit_cands.horizontal; } catch(...) {}
            //     // candidates: array of strings
            //     try {
            //         if (se.edit_cands.candidates && se.edit_cands.num_candidates > 0) {
            //             sol::table arr = lua.create_table();
            //             for (int i = 0; i < se.edit_cands.num_candidates; ++i) {
            //                 arr[i+1] = std::string(se.edit_cands.candidates[i] ? se.edit_cands.candidates[i] : "");
            //             }
            //             cand["candidates"] = arr;
            //         }
            //     } catch(...) {}
            //     t["candidates"] = cand;
            //     break;
            // }
            // case SDL_EVENT_KEYMAP_CHANGED: {
            //     sol::table km = lua.create_table();
            //     try { km["timestamp"] = se.keymap.timestamp; } catch(...) {}
            //     t["keymap"] = km;
            //     break;
            // }
            case SDL_EVENT_KEYBOARD_ADDED:
            case SDL_EVENT_KEYBOARD_REMOVED: {
                sol::table kd = lua.create_table();
                try { kd["timestamp"] = se.kdevice.timestamp; } catch(...) {}
                try { kd["which"] = se.kdevice.which; } catch(...) {}
                t["kdevice"] = kd;
                break;
            }
            case SDL_EVENT_TEXT_INPUT: {
                sol::table ti = lua.create_table();
                try { ti["timestamp"] = se.text.timestamp; } catch(...) {}
                try { ti["windowID"] = se.text.windowID; } catch(...) {}
                try {
                    ti["text"] = std::string(se.text.text);
                } catch (...) {
                    // fallback: empty string
                    ti["text"] = std::string();
                }
                t["text"] = ti;
                break;
            }

            // -- Drop Events -- //
            case SDL_EVENT_DROP_BEGIN:
            case SDL_EVENT_DROP_FILE:
            case SDL_EVENT_DROP_TEXT:
            case SDL_EVENT_DROP_COMPLETE:
            case SDL_EVENT_DROP_POSITION: {
                sol::table d = lua.create_table();
                try { d["timestamp"] = se.drop.timestamp; } catch(...) {}
                try { d["windowID"] = se.drop.windowID; } catch(...) {}
                try { d["x"] = se.drop.x; } catch(...) {}
                try { d["y"] = se.drop.y; } catch(...) {}
                try { d["source"] = std::string(se.drop.source ? se.drop.source : ""); } catch(...) { d["source"] = std::string(); }
                try { d["data"] = std::string(se.drop.data ? se.drop.data : ""); } catch(...) { d["data"] = std::string(); }
                t["drop"] = d;
                break;
            }

            // -- Clipboard Events -- //
            case SDL_EVENT_CLIPBOARD_UPDATE: {
                sol::table cb = lua.create_table();
                try { cb["timestamp"] = se.clipboard.timestamp; } catch(...) {}
                try { cb["owner"] = se.clipboard.owner; } catch(...) {}
                int num = 0;
                try { num = static_cast<int>(se.clipboard.num_mime_types); cb["num_mime_types"] = num; } catch(...) { cb["num_mime_types"] = 0; }
                try {
                    if (se.clipboard.mime_types && num > 0) {
                        sol::table mt = lua.create_table();
                        for (int i = 0; i < num; ++i) {
                            const char* s = se.clipboard.mime_types[i];
                            mt[i+1] = std::string(s ? s : "");
                        }
                        cb["mime_types"] = mt;
                    }
                } catch(...) {
                    // ignore mime_types issues
                }
                t["clipboard"] = cb;
                break;
            }

            // -- Window Events -- //
            case SDL_EVENT_WINDOW_SHOWN:
            case SDL_EVENT_WINDOW_HIDDEN:
            case SDL_EVENT_WINDOW_EXPOSED:
            case SDL_EVENT_WINDOW_MOVED:
            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            case SDL_EVENT_WINDOW_METAL_VIEW_RESIZED:
            case SDL_EVENT_WINDOW_MINIMIZED:
            case SDL_EVENT_WINDOW_MAXIMIZED:
            case SDL_EVENT_WINDOW_RESTORED:
            case SDL_EVENT_WINDOW_MOUSE_ENTER:
            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
            case SDL_EVENT_WINDOW_FOCUS_LOST:
            case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
            case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            case SDL_EVENT_WINDOW_HIT_TEST:
            case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
            case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
            case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
            case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
            case SDL_EVENT_WINDOW_OCCLUDED:
            case SDL_EVENT_WINDOW_HDR_STATE_CHANGED:
            case SDL_EVENT_WINDOW_DESTROYED: {
                sol::table w = lua.create_table();
                // window events generally contain timestamp, windowID and two integer data fields
                try { w["timestamp"] = se.window.timestamp; } catch (...) {}
                try { w["windowID"] = se.window.windowID; } catch (...) {}
                try { w["data1"] = se.window.data1; } catch (...) {}
                try { w["data2"] = se.window.data2; } catch (...) {}
                t["window"] = w;
                break;
            }
            case SDL_EVENT_DISPLAY_ADDED:
            case SDL_EVENT_DISPLAY_REMOVED:
            case SDL_EVENT_DISPLAY_MOVED:
            case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED:
            case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
            case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED:
            case SDL_EVENT_DISPLAY_ORIENTATION: {
                sol::table dv = lua.create_table();
                try { dv["timestamp"] = se.display.timestamp; } catch (...) {}
                try { dv["displayID"] = se.display.displayID; } catch (...) {}
                try { dv["data1"] = se.display.data1; } catch (...) {}
                try { dv["data2"] = se.display.data2; } catch (...) {}
                t["display"] = dv;
                break;
            }

            // -- Gamepad Events -- //
            case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
                sol::table ca = lua.create_table();
                try { ca["timestamp"] = se.gaxis.timestamp; } catch (...) {}
                try { ca["which"] = se.gaxis.which; } catch (...) {}
                try { ca["axis"] = se.gaxis.axis; } catch (...) {}
                try { ca["value"] = se.gaxis.value; } catch (...) {}
                t["gaxis"] = ca;
                break;
            }
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            case SDL_EVENT_GAMEPAD_BUTTON_UP: {
                sol::table cb = lua.create_table();
                try { cb["timestamp"] = se.gbutton.timestamp; } catch (...) {}
                try { cb["which"] = se.gbutton.which; } catch (...) {}
                try { cb["button"] = se.gbutton.button; } catch (...) {}
                try { cb["down"] = se.gbutton.down; } catch (...) {}
                t["gbutton"] = cb;
                break;
            }
            case SDL_EVENT_GAMEPAD_ADDED:
            case SDL_EVENT_GAMEPAD_REMOVED:
            case SDL_EVENT_GAMEPAD_REMAPPED:
            case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
            case SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED: {
                sol::table gd = lua.create_table();
                try { gd["timestamp"] = se.gdevice.timestamp; } catch(...) {}
                try { gd["which"] = se.gdevice.which; } catch(...) {}
                t["gdevice"] = gd;
                break;
            }
            case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
            case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
            case SDL_EVENT_GAMEPAD_TOUCHPAD_UP: {
                sol::table gt = lua.create_table();
                try { gt["timestamp"] = se.gtouchpad.timestamp; } catch(...) {}
                try { gt["which"] = se.gtouchpad.which; } catch(...) {}
                try { gt["touchpad"] = se.gtouchpad.touchpad; } catch(...) {}
                try { gt["finger"] = se.gtouchpad.finger; } catch(...) {}
                try { gt["x"] = se.gtouchpad.x; } catch(...) {}
                try { gt["y"] = se.gtouchpad.y; } catch(...) {}
                try { gt["pressure"] = se.gtouchpad.pressure; } catch(...) {}
                t["gtouchpad"] = gt;
                break;
            }
            case SDL_EVENT_GAMEPAD_SENSOR_UPDATE: {
                sol::table gs = lua.create_table();
                try { gs["timestamp"] = se.gsensor.timestamp; } catch(...) {}
                try { gs["which"] = se.gsensor.which; } catch(...) {}
                try { gs["sensor"] = se.gsensor.sensor; } catch(...) {}
                try {
                    sol::table arr = lua.create_table();
                    arr[1] = se.gsensor.data[0];
                    arr[2] = se.gsensor.data[1];
                    arr[3] = se.gsensor.data[2];
                    gs["data"] = arr;
                } catch(...) {}
                try { gs["sensor_timestamp"] = se.gsensor.sensor_timestamp; } catch(...) {}
                t["gsensor"] = gs;
                break;
            }

            // -- Joystick Events -- //
            case SDL_EVENT_JOYSTICK_AXIS_MOTION: {
                sol::table ja = lua.create_table();
                try { ja["timestamp"] = se.jaxis.timestamp; } catch (...) {}
                try { ja["which"] = se.jaxis.which; } catch (...) {}
                try { ja["axis"] = se.jaxis.axis; } catch (...) {}
                try { ja["value"] = se.jaxis.value; } catch (...) {}
                t["jaxis"] = ja;
                break;
            }
            case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
            case SDL_EVENT_JOYSTICK_BUTTON_UP: {
                sol::table jb = lua.create_table();
                try { jb["timestamp"] = se.jbutton.timestamp; } catch (...) {}
                try { jb["which"] = se.jbutton.which; } catch (...) {}
                try { jb["button"] = se.jbutton.button; } catch (...) {}
                try { jb["down"] = se.jbutton.down; } catch (...) {}
                t["jbutton"] = jb;
                break;
            }
            case SDL_EVENT_JOYSTICK_ADDED:
            case SDL_EVENT_JOYSTICK_REMOVED:
            case SDL_EVENT_JOYSTICK_UPDATE_COMPLETE: {
                sol::table jd = lua.create_table();
                try { jd["timestamp"] = se.jdevice.timestamp; } catch(...) {}
                try { jd["which"] = se.jdevice.which; } catch(...) {}
                t["jdevice"] = jd;
                break;
            }
            case SDL_EVENT_JOYSTICK_BALL_MOTION: {
                sol::table jb = lua.create_table();
                try { jb["timestamp"] = se.jball.timestamp; } catch(...) {}
                try { jb["which"] = se.jball.which; } catch(...) {}
                try { jb["ball"] = se.jball.ball; } catch(...) {}
                try { jb["xrel"] = se.jball.xrel; } catch(...) {}
                try { jb["yrel"] = se.jball.yrel; } catch(...) {}
                t["jball"] = jb;
                break;
            }
            case SDL_EVENT_JOYSTICK_HAT_MOTION: {
                sol::table jh = lua.create_table();
                try { jh["timestamp"] = se.jhat.timestamp; } catch(...) {}
                try { jh["which"] = se.jhat.which; } catch(...) {}
                try { jh["hat"] = se.jhat.hat; } catch(...) {}
                try { jh["value"] = se.jhat.value; } catch(...) {}
                t["jhat"] = jh;
                break;
            }
            case SDL_EVENT_JOYSTICK_BATTERY_UPDATED: {
                sol::table jbat = lua.create_table();
                try { jbat["timestamp"] = se.jbattery.timestamp; } catch(...) {}
                try { jbat["which"] = se.jbattery.which; } catch(...) {}
                try { jbat["state"] = se.jbattery.state; } catch(...) {}
                try { jbat["percent"] = se.jbattery.percent; } catch(...) {}
                t["jbattery"] = jbat;
                break;
            }

            // -- Touch Events -- //
            case SDL_EVENT_FINGER_DOWN:
            case SDL_EVENT_FINGER_UP:
            case SDL_EVENT_FINGER_MOTION: {
                sol::table f = lua.create_table();
                try { f["timestamp"] = se.tfinger.timestamp; } catch (...) {}
                try { f["touchID"] = se.tfinger.touchID; } catch (...) {}
                try { f["fingerID"] = se.tfinger.fingerID; } catch (...) {}
                try { f["x"] = se.tfinger.x; } catch (...) {}
                try { f["y"] = se.tfinger.y; } catch (...) {}
                try { f["dx"] = se.tfinger.dx; } catch (...) {}
                try { f["dy"] = se.tfinger.dy; } catch (...) {}
                try { f["pressure"] = se.tfinger.pressure; } catch (...) {}
                try { f["windowID"] = se.tfinger.windowID; } catch (...) {}
                t["tfinger"] = f;
                break;
            }

            // -- Pen Events -- //
            case SDL_EVENT_PEN_PROXIMITY_IN:
            case SDL_EVENT_PEN_PROXIMITY_OUT: {
                sol::table pp = lua.create_table();
                try { pp["timestamp"] = se.pproximity.timestamp; } catch(...) {}
                try { pp["windowID"] = se.pproximity.windowID; } catch(...) {}
                try { pp["which"] = se.pproximity.which; } catch(...) {}
                t["pproximity"] = pp;
                break;
            }
            case SDL_EVENT_PEN_DOWN:
            case SDL_EVENT_PEN_UP: {
                sol::table pt = lua.create_table();
                try { pt["timestamp"] = se.ptouch.timestamp; } catch(...) {}
                try { pt["windowID"] = se.ptouch.windowID; } catch(...) {}
                try { pt["which"] = se.ptouch.which; } catch(...) {}
                try { pt["pen_state"] = se.ptouch.pen_state; } catch(...) {}
                try { pt["x"] = se.ptouch.x; } catch(...) {}
                try { pt["y"] = se.ptouch.y; } catch(...) {}
                try { pt["eraser"] = se.ptouch.eraser; } catch(...) {}
                try { pt["down"] = se.ptouch.down; } catch(...) {}
                t["ptouch"] = pt;
                break;
            }
            case SDL_EVENT_PEN_MOTION: {
                sol::table pm = lua.create_table();
                try { pm["timestamp"] = se.pmotion.timestamp; } catch(...) {}
                try { pm["windowID"] = se.pmotion.windowID; } catch(...) {}
                try { pm["which"] = se.pmotion.which; } catch(...) {}
                try { pm["pen_state"] = se.pmotion.pen_state; } catch(...) {}
                try { pm["x"] = se.pmotion.x; } catch(...) {}
                try { pm["y"] = se.pmotion.y; } catch(...) {}
                t["pmotion"] = pm;
                break;
            }
            case SDL_EVENT_PEN_BUTTON_DOWN:
            case SDL_EVENT_PEN_BUTTON_UP: {
                sol::table pb = lua.create_table();
                try { pb["timestamp"] = se.pbutton.timestamp; } catch(...) {}
                try { pb["windowID"] = se.pbutton.windowID; } catch(...) {}
                try { pb["which"] = se.pbutton.which; } catch(...) {}
                try { pb["pen_state"] = se.pbutton.pen_state; } catch(...) {}
                try { pb["x"] = se.pbutton.x; } catch(...) {}
                try { pb["y"] = se.pbutton.y; } catch(...) {}
                try { pb["button"] = se.pbutton.button; } catch(...) {}
                try { pb["down"] = se.pbutton.down; } catch(...) {}
                t["pbutton"] = pb;
                break;
            }
            case SDL_EVENT_PEN_AXIS: {
                sol::table pa = lua.create_table();
                try { pa["timestamp"] = se.paxis.timestamp; } catch(...) {}
                try { pa["windowID"] = se.paxis.windowID; } catch(...) {}
                try { pa["which"] = se.paxis.which; } catch(...) {}
                try { pa["pen_state"] = se.paxis.pen_state; } catch(...) {}
                try { pa["x"] = se.paxis.x; } catch(...) {}
                try { pa["y"] = se.paxis.y; } catch(...) {}
                try { pa["axis"] = se.paxis.axis; } catch(...) {}
                try { pa["value"] = se.paxis.value; } catch(...) {}
                t["paxis"] = pa;
                break;
            }

            // -- Audio Hotplug / Device Events -- //
            case SDL_EVENT_AUDIO_DEVICE_ADDED:
            case SDL_EVENT_AUDIO_DEVICE_REMOVED:
            case SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED: {
                sol::table ad = lua.create_table();
                try { ad["timestamp"] = se.adevice.timestamp; } catch(...) {}
                try { ad["which"] = se.adevice.which; } catch(...) {}
                try { ad["recording"] = se.adevice.recording; } catch(...) {}
                t["adevice"] = ad;
                break;
            }

            // -- Camera Device Events -- //
            case SDL_EVENT_CAMERA_DEVICE_ADDED:
            case SDL_EVENT_CAMERA_DEVICE_REMOVED:
            case SDL_EVENT_CAMERA_DEVICE_APPROVED:
            case SDL_EVENT_CAMERA_DEVICE_DENIED: {
                sol::table cd = lua.create_table();
                try { cd["timestamp"] = se.cdevice.timestamp; } catch(...) {}
                try { cd["which"] = se.cdevice.which; } catch(...) {}
                t["cdevice"] = cd;
                break;
            }

            // -- Sensor Events -- //
            case SDL_EVENT_SENSOR_UPDATE: {
                sol::table s = lua.create_table();
                try { s["timestamp"] = se.sensor.timestamp; } catch(...) {}
                try { s["which"] = se.sensor.which; } catch(...) {}
                try {
                    sol::table arr = lua.create_table();
                    for (int i = 0; i < 6; ++i) arr[i+1] = se.sensor.data[i];
                    s["data"] = arr;
                } catch(...) {}
                try { s["sensor_timestamp"] = se.sensor.sensor_timestamp; } catch(...) {}
                t["sensor"] = s;
                break;
            }

            // -- Render Events -- //
            case SDL_EVENT_RENDER_TARGETS_RESET:
            case SDL_EVENT_RENDER_DEVICE_RESET:
            case SDL_EVENT_RENDER_DEVICE_LOST: {
                sol::table rr = lua.create_table();
                try { rr["timestamp"] = se.render.timestamp; } catch(...) {}
                try { rr["windowID"] = se.render.windowID; } catch(...) {}
                t["render"] = rr;
                break;
            }

            // -- User Events -- //
            case SDL_EVENT_USER: {
                sol::table ue = lua.create_table();
                try { ue["timestamp"] = se.user.timestamp; } catch(...) {}
                try { ue["windowID"] = se.user.windowID; } catch(...) {}
                try { ue["code"] = se.user.code; } catch(...) {}
                try {
                    // expose user data pointers as integer addresses
                    uintptr_t d1 = reinterpret_cast<uintptr_t>(se.user.data1);
                    ue["data1"] = static_cast<unsigned long long>(d1);
                } catch(...) {}
                try {
                    uintptr_t d2 = reinterpret_cast<uintptr_t>(se.user.data2);
                    ue["data2"] = static_cast<unsigned long long>(d2);
                } catch(...) {}
                t["user"] = ue;
                break;
            }
            // Add more cases for other SDL_Event types as needed
            default:
                // Optionally dump raw bytes or minimal info
                break;
        }
        return t;
    } // END SDL_Utils::eventToLuaTable()

    // SDL_FRect SDL_Utils::tableToFRect(const sol::table& t)
    // {
    //     SDL_FRect r{0,0,0,0};
    //     if (!t.valid()) return r;
    //     // keyed lookup first, then array-style
    //     r.x = t["x"].get_or(t[1].get_or(0.0f));
    //     r.y = t["y"].get_or(t[2].get_or(0.0f));
    //     r.w = t["w"].get_or(t[3].get_or(0.0f));
    //     r.h = t["h"].get_or(t[4].get_or(0.0f));
    //     r.w = t["width"].get_or(r.w);   // allow 'width'
    //     r.h = t["height"].get_or(r.h);  // allow 'height'
    //     return r;
    // }

    SDL_FRect SDL_Utils::tableToFRect(const sol::table& t)
    {
        SDL_FRect r{0,0,0,0};
        if (!t.valid()) return r;

        // If table uses left/top/right/bottom (or aliases l/t/r/b), prefer that form.
        bool has_ltrb = t["left"].valid() || t["l"].valid() ||
                        t["top"].valid()  || t["t"].valid() ||
                        t["right"].valid()|| t["r"].valid() ||
                        t["bottom"].valid()|| t["b"].valid();

        if (has_ltrb) 
        {
            // left / l / fallback to numeric[1]
            r.x = t["left"].get_or(t["l"].get_or(t[1].get_or(0.0f)));
            // top / t / fallback to numeric[2]
            r.y = t["top"].get_or(t["t"].get_or(t[2].get_or(0.0f)));

            // width: prefer explicit w/width, else try numeric[3]
            float w = t["w"].get_or(t["width"].get_or(t[3].get_or(0.0f)));
            // right / r / fallback to numeric[3]
            bool has_right = t["right"].valid() || t["r"].valid();
            float right = t["right"].get_or(t["r"].get_or(t[3].get_or(0.0f)));

            // height: prefer explicit h/height, else try numeric[4]
            float h = t["h"].get_or(t["height"].get_or(t[4].get_or(0.0f)));
            // bottom / b / fallback to numeric[4]
            bool has_bottom = t["bottom"].valid() || t["b"].valid();
            float bottom = t["bottom"].get_or(t["b"].get_or(t[4].get_or(0.0f)));

            // compute final width/height using right/bottom if provided, otherwise use w/h
            if (has_right) r.w = right - r.x;
            else r.w = w;
            if (has_bottom) r.h = bottom - r.y;
            else r.h = h;

            return r;
        }

        // keyed lookup first (x,y,w,h or width/height), then array-style
        r.x = t["x"].get_or(t[1].get_or(0.0f));
        r.y = t["y"].get_or(t[2].get_or(0.0f));
        r.w = t["w"].get_or(t["width"].get_or(t[3].get_or(0.0f)));
        r.h = t["h"].get_or(t["height"].get_or(t[4].get_or(0.0f)));

        return r;
    }


    SDL_Color SDL_Utils::colorFromSol(const sol::object& o)
    {
        SDL_Color c{255,255,255,255};
        if (!o.valid() || o == sol::lua_nil) return c;
        if (o.is<SDL_Color>()) return o.as<SDL_Color>();
        if (o.is<sol::table>()) {
            sol::table t = o.as<sol::table>();
            c.r = static_cast<Uint8>(t["r"].get_or(t[1].get_or(255)));
            c.g = static_cast<Uint8>(t["g"].get_or(t[2].get_or(255)));
            c.b = static_cast<Uint8>(t["b"].get_or(t[3].get_or(255)));
            c.a = static_cast<Uint8>(t["a"].get_or(t[4].get_or(255)));
            return c;
        }
        if (o.is<int>()) { int v = o.as<int>(); c.r = c.g = c.b = static_cast<Uint8>(v); return c; }
        return c;
    }


    SDL_ScaleMode SDL_Utils::scaleModeFromSol(const sol::object& o)
    {
        if (!o.valid() || o == sol::lua_nil) return SDL_SCALEMODE_NEAREST;
        if (o.is<std::string>()) {
            std::string s = o.as<std::string>();
            if (s == "linear" || s == "SDL_SCALEMODE_LINEAR") return SDL_SCALEMODE_LINEAR;
            return SDL_SCALEMODE_NEAREST;
        }
        if (o.is<int>()) {
            int v = o.as<int>();
            return static_cast<SDL_ScaleMode>(v);
        }
        return SDL_SCALEMODE_NEAREST;
    }



} // namespace SDOM
