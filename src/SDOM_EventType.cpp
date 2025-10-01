/***  SDOM_EventType.cpp  ****************************
 *    ___ ___   ___  __  __   ___             _  _____                              
 *   / __|   \ / _ \|  \/  | | __|_ _____ _ _| ||_   _|  _ _ __  ___   __ _ __ _ __ 
 *   \__ \ |) | (_) | |\/| | | _|\ V / -_) ' \  _|| || || | '_ \/ -_)_/ _| '_ \ '_ \
 *   |___/___/ \___/|_|  |_|_|___|\_/\___|_||_\__||_| \_, | .__/\___(_)__| .__/ .__/
 *                        |___|                       |__/|_|            |_|  |_|    
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
 ******************/
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_EventType.hpp>

namespace SDOM
{
    // Predefined event types (Name, Captures, Bubbles, TargetOnly, Global)
    EventType EventType::None("None", false, false, false, false);
    EventType EventType::SDL_Event("SDL_Event", false, false, false, true);
    EventType EventType::Quit("Quit", false, false, false, true);
    EventType EventType::EnterFrame("EnterFrame", false, false, false, false); // should only fire event listeners at target
    // Mouse event types (Name, Captures, Bubbles, TargetOnly, Global)
    EventType EventType::MouseButtonUp("MouseButtonUp", true, true, false, false);
    EventType EventType::MouseButtonDown("MouseButtonDown", true, true, false, false);
    EventType EventType::MouseWheel("MouseWheel", true, true, false, false);
    EventType EventType::MouseMove("MouseMove", false, false, true, false);
    EventType EventType::MouseClick("MouseClick", true, true, false, false);
    EventType EventType::MouseDoubleClick("MouseDoubleClick", true, true, false, false);    
    EventType EventType::MouseEnter("MouseEnter", false, false, true, false);
    EventType EventType::MouseLeave("MouseLeave", false, false, true, false);
    // Stage event types (Name, Captures, Bubbles, TargetOnly, Global)
    EventType EventType::StageClosed("StageClosed", false, false, false, true);
    // Keyboard event types (Name, Captures, Bubbles, TargetOnly, Global)
    EventType EventType::KeyDown("KeyDown", true, true, false, false);
    EventType EventType::KeyUp("KeyUp", true, true, false, false);
    // Timer event types (Name, Captures, Bubbles, TargetOnly, Global)
    EventType EventType::Timer("Timer", false, false, false, false);
    EventType EventType::Tick("Tick", false, false, false, false);
    EventType EventType::Timeout("Timeout", false, false, false, false);
    // Window event types (Name, Captures, Bubbles, TargetOnly, Global)
    EventType EventType::FocusGained("FocusGained", false, false, true, false);
    EventType EventType::FocusLost("FocusLost", false, false, true, false);
    EventType EventType::Resize("Resize", true, true, false, false);
    EventType EventType::Move("Move", true, true, false, false);
    EventType EventType::Show("Show", false, false, true, false);
    EventType EventType::Hide("Hide", false, false, true, false);
    EventType EventType::EnterFullscreen("EnterFullscreen", true, true, false, false);
    EventType EventType::LeaveFullscreen("LeaveFullscreen", true, true, false, false);
    // General UI event types (Name, Captures, Bubbles, TargetOnly, Global)
    EventType EventType::ValueChanged("ValueChanged", true, true, false, false);
    EventType EventType::StateChanged("StateChanged", true, true, false, false);
    EventType EventType::SelectionChanged("SelectionChanged", true, true, false, false);
    EventType EventType::Enabled("Enabled", false, false, true, false);
    EventType EventType::Disabled("Disabled", false, false, true, false);
    EventType EventType::Visible("Visible", true, true, false, false);
    EventType EventType::Hidden("Hidden", true, true, false, false);
    // Drag & Drop event types (Name, Captures, Bubbles, TargetOnly, Global)
    EventType EventType::Drag("Drag", true, true, false, false);
    EventType EventType::Dragging("Dragging", true, true, false, false);
    EventType EventType::Drop("Drop", true, true, false, false);
    // Clipboard event types (Name, Captures, Bubbles, TargetOnly, Global)
    EventType EventType::ClipboardCopy("ClipboardCopy", true, true, false, false);
    EventType EventType::ClipboardPaste("ClipboardPaste", true, true, false, false);

    // Application lifecycle event types
    EventType EventType::Added("Added", true, true, false, false);
    EventType EventType::Removed("Removed", true, true, false, false);
    EventType EventType::AddedToStage("AddedToStage", true, true, false, false);
    EventType EventType::RemovedFromStage("RemovedFromStage", true, true, false, false);
    // Event Listener Only Events
    EventType EventType::OnInit("OnInit", true, true, false, false);
    EventType EventType::OnQuit("OnQuit", false, false, false, true); // global only
    EventType EventType::OnEvent("OnEvent", true, true, false, false);
    EventType EventType::OnUpdate("OnUpdate", true, true, false, false);
    EventType EventType::OnRender("OnRender", true, true, false, false);
    
    // Pre-render hook: listeners-only, non-capturing/non-bubbling
    EventType EventType::OnPreRender("OnPreRender", false, false, false, false);

    // Custom User event types (Name, Captures, Bubbles, TargetOnly, Global)
    EventType EventType::User("User", true, true, false, false);



    // // Ensure registry is populated with the predefined static instances.
    // // NOTE: Constructors for each static EventType already call
    // // registerEventType(...). This function is therefore typically
    // // redundant, but remains to force ODR-use of the statics and avoid
    // // static-initialization-order issues. Kept for backward-compatibility.
    // void EventType::registerAll()
    // {
    //     // Using addresses of the statics to register them in the map
    //     registerEventType(None.getName(), &None);
    //     registerEventType(SDL_Event.getName(), &SDL_Event);
    //     registerEventType(Quit.getName(), &Quit);
    //     registerEventType(EnterFrame.getName(), &EnterFrame);

    //     registerEventType(MouseButtonUp.getName(), &MouseButtonUp);
    //     registerEventType(MouseButtonDown.getName(), &MouseButtonDown);
    //     registerEventType(MouseWheel.getName(), &MouseWheel);
    //     registerEventType(MouseMove.getName(), &MouseMove);
    //     registerEventType(MouseClick.getName(), &MouseClick);
    //     registerEventType(MouseDoubleClick.getName(), &MouseDoubleClick);
    //     registerEventType(MouseEnter.getName(), &MouseEnter);
    //     registerEventType(MouseLeave.getName(), &MouseLeave);

    //     registerEventType(StageClosed.getName(), &StageClosed);

    //     registerEventType(KeyDown.getName(), &KeyDown);
    //     registerEventType(KeyUp.getName(), &KeyUp);

    //     registerEventType(Timer.getName(), &Timer);
    //     registerEventType(Tick.getName(), &Tick);
    //     registerEventType(Timeout.getName(), &Timeout);

    //     registerEventType(FocusGained.getName(), &FocusGained);
    //     registerEventType(FocusLost.getName(), &FocusLost);
    //     registerEventType(Resize.getName(), &Resize);
    //     registerEventType(Move.getName(), &Move);
    //     registerEventType(Show.getName(), &Show);
    //     registerEventType(Hide.getName(), &Hide);
    //     registerEventType(EnterFullscreen.getName(), &EnterFullscreen);
    //     registerEventType(LeaveFullscreen.getName(), &LeaveFullscreen);

    //     registerEventType(ValueChanged.getName(), &ValueChanged);
    //     registerEventType(StateChanged.getName(), &StateChanged);
    //     registerEventType(SelectionChanged.getName(), &SelectionChanged);
    //     registerEventType(Enabled.getName(), &Enabled);
    //     registerEventType(Disabled.getName(), &Disabled);
    //     registerEventType(Visible.getName(), &Visible);
    //     registerEventType(Hidden.getName(), &Hidden);

    //     registerEventType(Drag.getName(), &Drag);
    //     registerEventType(Dragging.getName(), &Dragging);
    //     registerEventType(Drop.getName(), &Drop);

    //     registerEventType(ClipboardCopy.getName(), &ClipboardCopy);
    //     registerEventType(ClipboardPaste.getName(), &ClipboardPaste);

    //     registerEventType(Added.getName(), &Added);
    //     registerEventType(Removed.getName(), &Removed);
    //     registerEventType(AddedToStage.getName(), &AddedToStage);
    //     registerEventType(RemovedFromStage.getName(), &RemovedFromStage);

    //     registerEventType(OnInit.getName(), &OnInit);
    //     registerEventType(OnQuit.getName(), &OnQuit);
    //     registerEventType(OnEvent.getName(), &OnEvent);
    //     registerEventType(OnUpdate.getName(), &OnUpdate);
    //     registerEventType(OnRender.getName(), &OnRender);

    //     registerEventType(User.getName(), &User);
    // }

    void EventType::registerLua(sol::state_view lua)
    {
        try {
            // create a simple usertype so EventType values are usable as userdata
            if (!lua["EventType"].valid()) {
                lua.new_usertype<EventType>("EventType", sol::no_constructor,
                    "getName", &EventType::getName,
                    "getCaptures", &EventType::getCaptures,
                    "getBubbles", &EventType::getBubbles,
                    "getTargetOnly", &EventType::getTargetOnly,
                    "getGlobal", &EventType::getGlobal
                );
            }
            // Populate a table with references to the static EventType instances
            sol::table etbl = lua.create_table();
            const auto& reg = EventType::getRegistry();
            for (const auto& kv : reg) {
                const std::string& name = kv.first;
                EventType* ptr = kv.second;
                if (ptr) {
                    etbl[name] = sol::make_object(lua, std::ref(*ptr));
                }
            }
            lua["EventType"] = etbl;
        } catch (...) {
            // non-fatal
        }
    }

    // getters
    bool EventType::getCaptures() const 
    { 
        return captures_; 
    }
    bool EventType::getBubbles() const     
    { 
        return bubbles_; 
    }
    bool EventType::getTargetOnly() const 
    { 
        return targetOnly_; 
    }
    bool EventType::getGlobal() const 
    { 
        return global_; 
    }

    // setters
    EventType& EventType::setCaptures(bool captures) 
    { 
        captures_ = captures; 
        return *this; 
    }
    EventType& EventType::setBubbles(bool bubbles) 
    { 
        bubbles_ = bubbles; 
        return *this; 
    }
    EventType& EventType::setTargetOnly(bool targetOnly) 
    { 
        targetOnly_ = targetOnly; 
        return *this; 
    }
    EventType& EventType::setGlobal(bool global) 
    { 
        global_ = global; 
        return *this; 
    }

} // namespace SDOM