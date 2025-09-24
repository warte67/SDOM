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
    // Custom User event types (Name, Captures, Bubbles, TargetOnly, Global)
    EventType EventType::User("User", true, true, false, false);

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