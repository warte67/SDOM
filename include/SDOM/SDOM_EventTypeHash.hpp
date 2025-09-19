/***  SDOM_EventTypeHash.hpp  ****************************
 *   ___ ___   ___  __  __   ___             _  _____               _  _         _      _              
 *  / __|   \ / _ \|  \/  | | __|_ _____ _ _| ||_   _|  _ _ __  ___| || |__ _ __| |_   | |_  _ __ _ __ 
 *  \__ \ |) | (_) | |\/| | | _|\ V / -_) ' \  _|| || || | '_ \/ -_) __ / _` (_-< ' \ _| ' \| '_ \ '_ \
 *  |___/___/ \___/|_|  |_|_|___|\_/\___|_||_\__||_| \_, | .__/\___|_||_\__,_/__/_||_(_)_||_| .__/ .__/
 *                       |___|                       |__/|_|                                |_|  |_|     
 *  
 * The SDOM_EventTypeHash struct provides a custom hash functor for the EventType class, 
 * enabling efficient use of EventType objects as keys in unordered associative containers such 
 * as std::unordered_map and std::unordered_set. By hashing the event type’s name, this functor 
 * ensures that event types can be quickly and uniquely identified within hash-based data 
 * structures. This is essential for fast event lookup, registration, and dispatching within 
 * the SDOM event system, supporting scalable and performant event-driven programming.
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

#ifndef SDOM_EVENT_TYPE_HASH_HPP
#define SDOM_EVENT_TYPE_HASH_HPP

#include "SDOM2/SDOM_Event.hpp"
#include "SDOM2/SDOM_EventType.hpp"

namespace SDOM {

    class Event;
    class EventType;

    // Custom hash functor for SDOM::EventType to be used in unordered containers
    struct EventTypeHash {
        std::size_t operator()(const EventType& eventType) const noexcept {
            return std::hash<std::string>()(eventType.getName());
        }
    };

} // namespace SDOM

#endif // SDOM_EVENT_TYPE_HASH_HPP