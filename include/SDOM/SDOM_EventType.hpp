/***  SDOM_EventType.hpp  ****************************
 *   ___ ___   ___  __  __   ___             _  _____                 _              
 *  / __|   \ / _ \|  \/  | | __|_ _____ _ _| ||_   _|  _ _ __  ___  | |_  _ __ _ __ 
 *  \__ \ |) | (_) | |\/| | | _|\ V / -_) ' \  _|| || || | '_ \/ -_)_| ' \| '_ \ '_ \
 *  |___/___/ \___/|_|  |_|_|___|\_/\___|_||_\__||_| \_, | .__/\___(_)_||_| .__/ .__/
 *                       |___|                       |__/|_|              |_|  |_|   
 *  
 * The SDOM_EventType class defines and manages the various types of events that can 
 * be dispatched and handled within the SDOM framework. It provides a flexible system 
 * for registering both predefined and custom event types, each with configurable 
 * propagation properties such as capturing, bubbling, target-only, and global scope. 
 * EventType instances are used to categorize and identify events throughout the event 
 * system, enabling robust event-driven programming and fine-grained control over event 
 * flow. By supporting both standard UI events (mouse, keyboard, window, etc.) and 
 * user-defined events, the EventType class forms the foundation for extensible and 
 * scalable event handling in SDOM-based applications.
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
 * Wayland Behavior Status (SDL3/Wayland guidance)
 *
 *   ✅ Works normally (verified):
 *     - EventType::EnterFullscreen
 *     - EventType::LeaveFullscreen
 *
 *   🔄 Should work normally (assume OK on Wayland):
 *     - EventType::Show
 *     - EventType::Hide
 *     - EventType::Resize
 *
 *   ⚠️ Wayland‑gated (compositor controlled, may be ignored):
 *     - EventType::Move
 *     - EventType::FocusLost
 *     - EventType::FocusGained
 *
 * Notes:
 * - On Wayland, programmatic raise/focus and window repositioning are requests.
 *   Compositors may ignore them; tests should treat these as informational.
 * - Fullscreen entry/exit and resize/show/hide typically work as expected.
 *
 ******************/

#ifndef SDOM_EVENTTYPE_HPP_
#define SDOM_EVENTTYPE_HPP_

#include <unordered_set>
#include <unordered_map>
#include <atomic>
#include <cstdint>


// Allow optional Lua bindings: forward-declare `sol::state_view` so callers
// that include this header don't need to pull in the Sol2 headers unless
// they require Lua functionality.
namespace sol { class state_view; }


namespace SDOM
{
    class EventType
    {
        // ---- 🧪 Test Legend --------------------------------------------------------------------
        //
        //   🔄 In Progress     -- Test or feature is under active development or debugging.
        //   🗓️ Planned         -- Concept defined but not yet implemented in code.
        //   ❓ Missing System  -- Dependent subsystem not yet implemented.
        //   🚫 Untestable      -- Cannot be directly tested by this harness (e.g., Quit, None).
        //   ⚠️ Failing         -- Test implemented but currently failing or unstable.
        //   ✅ Fully Verified  -- Passed all test modes; stable behavior.
        //
        // ---- 🧪 Test Modes ---------------------------------------------------------------------
        //
        //   🧩 Synthetic Test Mode -- (Phase I)   Verified via synthetic (queued or manual) events.
        //   🧠 Lifetime Test Mode  -- (Phase II)  Event lifetime depends on compositor /
        //                                       desktop environment (may vary by platform).
        //   📜 Lua Test Mode       -- (Phase III) Verified Lua bindings via Lua test harness script.
        //
        // ----------------------------------------------------------------------------------------


    public:

        // 🚫 Untestable ---------------------------------------------------------------
        static EventType None;              // 🚫
        static EventType Quit;              // 🚫

        // 🔄 Application Lifecycle ----------------------------------------------------
        static EventType Added;             // 🧩🧠 EventType_test{3}, Event_test{7}
        static EventType Removed;           // 🧩🧠 EventType_test{3}, Event_test{7}
        static EventType AddedToStage;      // 🧩🧠 EventType_test{3}, Event_test{7}
        static EventType RemovedFromStage;  // 🧩🧠 EventType_test{3}, Event_test{7}
        static EventType StageOpened;       // 🧩🧠 EventType_test{4}
        static EventType StageClosed;       // 🧩🧠 EventType_test{4}

        // 🔄 Input --------------------------------------------------------------------
        // Keyboard
        static EventType KeyDown;           // 🧩 EventType_test{1}, Event_test{3}  📜 Lua: EventType_UnitTests.lua test{2}
        static EventType KeyUp;             // 🧩 EventType_test{1}, Event_test{3}  📜 Lua: EventType_UnitTests.lua test{2}
        static EventType TextInput;         // ❓ IME/TextInput system not yet implemented
        // Mouse
        static EventType MouseButtonDown;   // 🧩 Event_test{4, 8}                  📜 Lua: EventType_UnitTests.lua test{3}
        static EventType MouseButtonUp;     // 🧩 Event_test{4, 8}                  📜 Lua: EventType_UnitTests.lua test{3}
        static EventType MouseWheel;        // 🧩 Event_test{4, 8}                  📜 Lua: EventType_UnitTests.lua test{3}
        static EventType MouseMove;         // 🧩 Event_test{4, 8}                  📜 Lua: EventType_UnitTests.lua test{1,3}
        static EventType MouseClick;        // 🧩 Event_test{4, 8}                  📜 Lua: EventType_UnitTests.lua test{4}
        static EventType MouseDoubleClick;  // 🧩 Event_test{4, 8}                  📜 Lua: EventType_UnitTests.lua test{4}
        static EventType MouseEnter;        // 🧩🧠 EventType_test{5}               📜 Lua: EventType_UnitTests.lua test{5}
        static EventType MouseLeave;        // 🧩🧠 EventType_test{5}               📜 Lua: EventType_UnitTests.lua test{5}

        // 🔄 Window / Focus -----------------------------------------------------------
        static EventType FocusGained;       // Do not use yet. Only applies to the not yet implemented Window objects.
        static EventType FocusLost;         // Do not use yet. Only applies to the not yet implemented Window objects.
        static EventType Resize;            // 🧩🧠 Event_test{5}, EventType_test{5}   📜 Lua: EventType_UnitTests.lua test{6}
        static EventType Move;              // 🧩🧠 Event_test{5}, EventType_test{5}   📜 Lua: EventType_UnitTests.lua test{6}
        static EventType Show;              // 🧩🧠 Event_test{5}, EventType_test{5}   📜 Lua: EventType_UnitTests.lua test{6}
        static EventType Hide;              // 🧩🧠 Event_test{5}, EventType_test{5}   📜 Lua: EventType_UnitTests.lua test{6}
        static EventType EnterFullscreen;   // 🧩🧠 Event_test{5}, EventType_test{5}   📜 Lua: EventType_UnitTests.lua test{6}
        static EventType LeaveFullscreen;   // 🧩🧠 Event_test{5}, EventType_test{5}   📜 Lua: EventType_UnitTests.lua test{6}

        // 🔄 UI / State ---------------------------------------------------------------
        static EventType ValueChanged;      // 🧩 Event_test{6}
        static EventType StateChanged;      // 🧩 Event_test{6}
        static EventType SelectionChanged;  // 🧩 Event_test{6}
        static EventType Enabled;           // 🧩 Event_test{6}
        static EventType Disabled;          // 🧩 Event_test{6}
        static EventType Visible;           // 🧩 Event_test{6}
        static EventType Hidden;            // 🧩 Event_test{6}

        // 🔄 Drag & Drop --------------------------------------------------------------
        static EventType Drag;              // 🧩 Event_test{4, 8}
        static EventType Dragging;          // 🧩 Event_test{4, 8}
        static EventType Drop;              // 🧩 Event_test{4, 8}

        // ❓ Timer (no Timer system yet) ----------------------------------------------
        static EventType TimerStart;        
        static EventType TimerStop;         
        static EventType TimerPause;        
        static EventType TimerTick;         
        static EventType TimerCycleComplete;
        static EventType TimerComplete;     

        // ❓ Clipboard (clipboard sub-system not yet implemented) ---------------------
        static EventType ClipboardCopy;     
        static EventType ClipboardPaste;    

        // ✅ Listener-Only ------------------------------------------------------------
        //   Fully verified through Lua callback system (listener_callbacks.lua).
        //   Each event is registered via stage:addEventListener using EventType constants
        //   and confirmed to dispatch correctly within SDOM’s runtime loop.
        //
        //   • OnInit / OnQuit – lifecycle setup and teardown confirmed.
        //   • OnEvent – SDL and user input routed correctly to Lua.
        //   • OnUpdate / OnRender / OnPreRender – verified per-frame ordering and stability.
        //
        static EventType OnInit;            // 🧩📜 Verified in listener_callbacks.lua:on_init
        static EventType OnQuit;            // 🧩📜 Verified in listener_callbacks.lua:on_quit
        static EventType OnEvent;           // 🧩📜 Verified in listener_callbacks.lua:on_event
        static EventType OnUpdate;          // 🧩📜 Verified in listener_callbacks.lua:on_update
        static EventType OnRender;          // 🧩📜 Verified in listener_callbacks.lua:on_render
        static EventType OnPreRender;       // 🧩📜 Verified in listener_callbacks.lua:on_prerender


        // 🔄 Frame / Misc -------------------------------------------------------------
        static EventType EnterFrame;        // 🧩 Event_test{3}
        static EventType SDL_Event;         // 🧩 Event_test{3}
        static EventType User;              // 🗓️ Planned
               
        explicit EventType(const std::string& name, const std::string& doc = std::string())
            : name(name), doc_(doc), category_("Uncategorized"), captures_(true), bubbles_(true), targetOnly_(false), global_(false)
        { registerEventType(name, this); getOrAssignId(); }

        // New overload that accepts an explicit category string
        explicit EventType(const std::string& name, const std::string& category, const std::string& doc = std::string())
            : name(name), doc_(doc), category_(category), captures_(true), bubbles_(true), targetOnly_(false), global_(false)
        { registerEventType(name, this); getOrAssignId(); }

        explicit EventType(const std::string& name, bool captures, bool bubbles, bool targetOnly, bool global, const std::string& doc = std::string()) 
            : name(name), doc_(doc), category_("Uncategorized"), captures_(captures), bubbles_(bubbles), targetOnly_(targetOnly), global_(global) 
        { 
            registerEventType(name, this); 
            getOrAssignId();
        }

        // New overload that accepts category + propagation flags
        explicit EventType(const std::string& name, const std::string& category, bool captures, bool bubbles, bool targetOnly, bool global, const std::string& doc = std::string()) 
            : name(name), doc_(doc), category_(category), captures_(captures), bubbles_(bubbles), targetOnly_(targetOnly), global_(global) 
        { 
            registerEventType(name, this); 
            getOrAssignId();
        }

        const std::string& getName() const { return name; }

        bool operator==(const EventType& other) const { return name == other.name; }
        bool operator!=(const EventType& other) const { return name != other.name; }
        bool operator<(const EventType& other) const { return name < other.name; }

        static void registerEventType(const std::string& name, EventType* ptr) {
            if (registry.find(name) == registry.end()) {
                registry.insert({name, ptr});
                registry_order.push_back(ptr);
            }
        }

        static bool isRegistered(const std::string& name) {
            return registry.find(name) != registry.end();
        }

        // Return the internal registry mapping event name -> EventType*
        static const std::unordered_map<std::string, EventType*>& getRegistry() { return registry; }

        // // Ensure all predefined EventType static instances are inserted into
        // // the registry. Calling this from startup code avoids static
        // // initialization order issues on some platforms.
        // // NOTE: Each EventType constructor already registers itself via
        // // registerEventType(name, this). This helper is therefore
        // // usually redundant, but is retained here to force initialization
        // ///ODR-use of the predefined statics in cases where static
        // // initialization order across translation units could lead to an
        // // empty registry. It is kept for backward-compatibility and safety.
        // static void registerAll();

        // Register EventType usertype/table in a Lua state so scripts can
        // access EventType constants and query properties.
        static void registerLua(sol::state_view lua);

        // -- Getters -- //
        bool getCaptures() const;
        bool getBubbles() const;
        bool getTargetOnly() const;
        bool getGlobal() const;

        // Metering policy getters
        bool isCritical() const;                 // flush-before-enqueue, never metered
        bool isMeterEnabled() const;             // true to coalesce events of this type
        uint16_t getMeterIntervalMs() const;     // per-type meter interval override
        enum class CoalesceStrategy { None, Last, Sum, Count };
        enum class CoalesceKey { Global, ByTarget };
        CoalesceStrategy getCoalesceStrategy() const;
        CoalesceKey getCoalesceKey() const;

        // -- Setters -- //
        EventType& setCaptures(bool captures);
        EventType& setBubbles(bool bubbles);
        EventType& setTargetOnly(bool targetOnly);
        EventType& setGlobal(bool global);

        // Metering policy setters (builder-style for static defaults)
        EventType& setCritical(bool critical);
        EventType& setMeterEnabled(bool enabled);
        EventType& setMeterIntervalMs(uint16_t ms);
        EventType& setCoalesceStrategy(CoalesceStrategy s);
        EventType& setCoalesceKey(CoalesceKey k);

        // -- Public Lua support -- //   
                
        // -- Static Utilities for Lua and C++ introspection -- //
        static std::vector<EventType*> getAll();
        static EventType* fromName(const std::string& name);
        static std::string toString(const EventType& et);
        static size_t count();

        // Numeric id support for C API interop
        using IdType = uint32_t;
        IdType getOrAssignId();
        void setId(IdType id);
        static EventType* fromId(IdType id);
        // Optional documentation string for this EventType (used by generators)
        std::string getDoc() const;
        void setDoc(const std::string& s);
        // Optional category string used by generators to group related EventTypes
        std::string getCategory() const { return category_; }
        void setCategory(const std::string& c) { category_ = c; }

    private:
        std::string name;
        std::string doc_;
        std::string category_ = "Uncategorized";
        static inline std::unordered_map<std::string, EventType*> registry;
        static inline std::vector<EventType*> registry_order; // preserves insertion/definition order

        // id registry + allocator for stable numeric ids
        inline static std::unordered_map<IdType, EventType*> idRegistry;
        inline static std::atomic<IdType> next_event_type_id{1};
        IdType id_ = 0;

        bool captures_ = true;
        bool bubbles_ = true;
        bool targetOnly_ = false;
        bool global_ = false;

        // Metering policy (defaults: not critical, not metered)
        bool critical_ = false;
        bool meter_enabled_ = false;
        uint16_t meter_interval_ms_ = 0; // 0 => use SDOM_EVENT_METER_MS_DEFAULT
        CoalesceStrategy coalesce_strategy_ = CoalesceStrategy::None;
        CoalesceKey coalesce_key_ = CoalesceKey::Global;

    };

} // namespace SDOM

#endif // SDOM_EVENTTYPE_HPP_
