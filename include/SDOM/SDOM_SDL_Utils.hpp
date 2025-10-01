/***  SDOM_SDL_Utils.hpp  ****************************
 *   ___ ___   ___  __  __   ___ ___  _     _   _ _   _ _      _              
 *  / __|   \ / _ \|  \/  | / __|   \| |   | | | | |_(_) |___ | |_  _ __ _ __ 
 *  \__ \ |) | (_) | |\/| | \__ \ |) | |__ | |_| |  _| | (_-<_| ' \| '_ \ '_ \
 *  |___/___/ \___/|_|  |_|_|___/___/|____|_\___/ \__|_|_/__(_)_||_| .__/ .__/
 *                       |___|           |___|                     |_|  |_|     
 *  
 * The SDOM_SDL_Utils class provides a collection of static utility functions to 
 * facilitate common SDL3 operations within the SDOM framework. It offers convenient 
 * methods for converting between SDL enumerations and their string representations, 
 * such as pixel formats, window flags, renderer presentation modes, and event types. 
 * Additionally, it includes helper functions for keycode-to-ASCII conversion and other 
 * SDL-specific tasks. By centralizing these utilities, SDL_Utils simplifies SDL 
 * integration, enhances code readability, and promotes consistency across the SDOM codebase.
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

#ifndef __SDOM_SDL_UTILS_HPP__
#define __SDOM_SDL_UTILS_HPP__

#include <SDOM/SDOM.hpp>

namespace SDOM
{

    class SDL_Utils
    {
    public:
        SDL_Utils() = default;
        ~SDL_Utils() = default;

        // Utility functions for SDL operations
        static std::string pixelFormatToString(SDL_PixelFormat format);
        static SDL_PixelFormat pixelFormatFromString(const std::string& str);

        static std::string windowFlagsToString(Uint64 flags);
        static Uint64 windowFlagsFromString(const std::string& str);

        static std::string rendererLogicalPresentationToString(Uint32 flags);
        static SDL_RendererLogicalPresentation rendererLogicalPresentationFromString(const std::string& str);

        static std::string eventTypeToString(SDL_EventType type);
        static SDL_EventType eventTypeFromString(const std::string& str);

        static int keyToAscii(SDL_Keycode keycode, SDL_Keymod keymod);

        static sol::table eventToLuaTable(const SDL_Event& event, sol::state_view lua);
        static void registerLua(sol::state_view lua);


    };

} // namespace SDOM

#endif // __SDOM_SDL_UTILS_HPP__