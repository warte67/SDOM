/***  SDOM_Event.hpp  ****************************
 *    ___ ___   ___  __  __   ___             _     _              
 *   / __|   \ / _ \|  \/  | | __|_ _____ _ _| |_  | |_  _ __ _ __ 
 *   \__ \ |) | (_) | |\/| | | _|\ V / -_) ' \  _|_| ' \| '_ \ '_ \
 *   |___/___/ \___/|_|  |_|_|___|\_/\___|_||_\__(_)_||_| .__/ .__/
 *                        |___|                         |_|  |_|   
 *  
 * The SDOM_Event class encapsulates the event system within the SDOM framework, 
 * providing a unified interface for handling user input and system events such 
 * as keyboard, mouse, and custom application events. It supports event propagation 
 * phases (capture, target, bubbling), payload data for custom event information, 
 * and thread-safe access to event properties. The class integrates closely with 
 * SDL3’s event system, allowing seamless translation between SDL events and SDOM’s 
 * object-oriented event model. SDOM_Event is designed for flexibility and 
 * extensibility, enabling robust event-driven programming and interactive GUI 
 * development within SDOM-based applications.
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

#ifndef __SDOM_EVENT_HPP__
#define __SDOM_EVENT_HPP__

#include <SDL3/SDL.h>
#include <SDOM/SDOM_IDataObject.hpp>
#include <SDOM/SDOM_EventType.hpp>
#include <mutex>

namespace SDOM
{
    class IDisplayObject;

    class Event : public IDataObject
    {
        friend class EventManager;
        
    public:

        // using Json = nlohmann::json;

        enum class Phase {
            Capture,
            Target,
            Bubbling
        };    

        Event(EventType type = EventType("None"), ResourceHandle target = nullptr, float fElapsedTime = 0.0f);

        virtual ~Event() = default;    

        // virtual methods
        virtual bool onInit() override;
        virtual void onQuit() override;        
        
        // Default accessors 
        EventType getType() const;
        std::string getTypeName() const;

        std::string getPhaseString() const;

        Phase getPhase() const;
        Event& setPhase(Phase phase);

        ResourceHandle getTarget() const;
        Event& setTarget(ResourceHandle newTarget);

        ResourceHandle getCurrentTarget() const;
        Event& setCurrentTarget(ResourceHandle newCurrentTarget);

        ResourceHandle getRelatedTarget() const;
        Event& setRelatedTarget(ResourceHandle newRelatedTarget);

        bool isPropagationStopped() const;
        Event& stopPropagation();

        bool isDefaultBehaviorDisabled() const;
        Event& setDisableDefaultBehavior(bool disable);

        bool getUseCapture() const;
        Event& setUseCapture(bool useCapture);

        float getElapsedTime() const;
        Event& setElapsedTime(float elapsedTime);

        SDL_Event getSDL_Event() const;
        Event& setSDL_Event(const SDL_Event& sdlEvent);

        // Payload accessors
        const nlohmann::json& getPayload() const {
            std::lock_guard<std::mutex> lock(eventMutex_); 
            return payload; 
        }
        Event& setPayload(const nlohmann::json& data) {
            std::lock_guard<std::mutex> lock(eventMutex_); 
            payload = data; 
            return *this; 
        }
        // Convenience for setting a key/value in the payload
        template<typename T>
        Event& setPayloadValue(const std::string& key, const T& value) 
        {
            std::lock_guard<std::mutex> lock(eventMutex_); 
            payload[key] = value; 
            return *this; 
        }
        template<typename T>
        T getPayloadValue(const std::string& key) const 
        {
            std::lock_guard<std::mutex> lock(eventMutex_); 
            return payload.value(key, T{}); 
        }


        // Mouse event accessors
        float getMouseX() const;
        Event& setMouseX(float x);

        float getMouseY() const;
        Event& setMouseY(float y);

        float getWheelX() const;
        Event& setWheelX(float x);

        float getWheelY() const;
        Event& setWheelY(float y);

        float getDragOffsetX() const;
        Event& setDragOffsetX(float offsetX);

        float getDragOffsetY() const;
        Event& setDragOffsetY(float offsetY);

        int getClickCount() const;
        Event& setClickCount(int count);

        uint8_t getButton() const;
        Event& setButton(uint8_t btn);

        // keyboard event accessors

        SDL_Scancode getScanCode() const;
        Event& setScanCode(SDL_Scancode scancode);

        SDL_Keycode getKeycode() const;
        Event& setKeycode(SDL_Keycode keycode);

        Uint16 getKeymod() const;
        Event& setKeymod(Uint16 keymod);

        int getAsciiCode() const;
        Event& setAsciiCode(int asciiCode);        


    protected:
        EventType type;                                 // Type of the event, e.g., KeyDown, MouseClick, etc.
        ResourceHandle target = nullptr;                // Target of the event, usually the object that triggered it    
        ResourceHandle currentTarget = nullptr;         // Current target during event propagation
        ResourceHandle relatedTarget = nullptr;         // For events that involve a related target (e.g., drag and drop)
        Phase currentPhase;                             // Current phase of the event propagation
        SDL_Event sdlEvent;                             // underlying SDL event
        mutable bool propagationStopped = false;        // Indicates if event propagation is stopped
        mutable bool disableDefaultBehavior = false;    // Indicates if default behavior is disabled
        mutable bool useCapture = false;                // Indicates if the event is in the capture phase
        float fElapsedTime = 0.0f;                      // Time elapsed since the last frame

        Json payload;                                   // Generic event-specific data

        // Mouse Event Properties: (Not yet defined as proper JSON properties)
        float mouse_x;              // mouse x-coordinate
        float mouse_y;              // mouse y-coordinate
        float wheelX = 0.0f;        // Horizontal wheel movement
        float wheelY = 0.0f;        // Vertical wheel movement
        float dragOffsetX = 0.0f;   // horizontal drag offset
        float dragOffsetY = 0.0f;   // vertical drag offset
        int clickCount = 0;         // Number of clicks for double-click detection
        uint8_t button = 0;         // Mouse button that triggered the event (SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT, etc.)

        // Keyboard Event Properties: (Not yet defined as proper JSON properties)
        SDL_Scancode scancode_;     // The physical key pressed
        SDL_Keycode keycode_;       // The virtual key pressed
        Uint16 keymod_;             // Modifier keys (e.g., Shift, Ctrl, Alt)
        int asciiCode_;             // ASCII code of the key pressed
        mutable std::mutex eventMutex_;

        // Prevent copying
        // Event(const Event&) = delete;               // Deleted copy constructor
        // Event& operator=(const Event&) = delete;    // Deleted copy assignment operator
    public:
        // Copy constructor
        Event(const Event& other)
            : type(other.type),
            target(other.target),
            currentTarget(other.currentTarget),
            relatedTarget(other.relatedTarget),
            currentPhase(other.currentPhase),
            sdlEvent(other.sdlEvent),
            propagationStopped(other.propagationStopped),
            disableDefaultBehavior(other.disableDefaultBehavior),
            useCapture(other.useCapture),
            fElapsedTime(other.fElapsedTime),
            payload(other.payload),
            mouse_x(other.mouse_x),
            mouse_y(other.mouse_y),
            wheelX(other.wheelX),
            wheelY(other.wheelY),
            dragOffsetX(other.dragOffsetX),
            dragOffsetY(other.dragOffsetY),
            clickCount(other.clickCount),
            button(other.button),
            scancode_(other.scancode_),
            keycode_(other.keycode_),
            keymod_(other.keymod_),
            asciiCode_(other.asciiCode_) {
            // Note: `eventMutex_` is initialized as a new mutex
        }

        // Copy assignment operator
        Event& operator=(const Event& other) {
            if (this != &other) {
                std::lock_guard<std::mutex> lock(eventMutex_);
                type = other.type;
                target = other.target;
                currentTarget = other.currentTarget;
                relatedTarget = other.relatedTarget;
                currentPhase = other.currentPhase;
                sdlEvent = other.sdlEvent;
                propagationStopped = other.propagationStopped;
                disableDefaultBehavior = other.disableDefaultBehavior;
                useCapture = other.useCapture;
                fElapsedTime = other.fElapsedTime;
                payload = other.payload;
                mouse_x = other.mouse_x;
                mouse_y = other.mouse_y;
                wheelX = other.wheelX;
                wheelY = other.wheelY;
                dragOffsetX = other.dragOffsetX;
                dragOffsetY = other.dragOffsetY;
                clickCount = other.clickCount;
                button = other.button;
                scancode_ = other.scancode_;
                keycode_ = other.keycode_;
                keymod_ = other.keymod_;
                asciiCode_ = other.asciiCode_;
            }
            return *this;
        }

    };

} // namespace SDOM

#endif // __SDOM_EVENT_HPP__