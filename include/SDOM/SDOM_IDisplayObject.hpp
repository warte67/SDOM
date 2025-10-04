/***  SDOM_IDisplayObject.hpp  ****************************
 *   ___ ___   ___  __  __   ___  _         _            ___  _     _        _     _              
 *  / __|   \ / _ \|  \/  | |   \(_)____ __| |__ _ _  _ / _ \| |__ (_)___ __| |_  | |_  _ __ _ __ 
 *  \__ \ |) | (_) | |\/| | | |) | (_-< '_ \ / _` | || | (_) | '_ \| / -_) _|  _|_| ' \| '_ \ '_ \
 *  |___/___/ \___/|_|  |_|_|___/|_/__/ .__/_\__,_|\_, |\___/|_.__// \___\__|\__(_)_||_| .__/ .__/
 *                       |___|        |_|          |__/          |__/                  |_|  |_|     
 *  
 * The SDOM_IDisplayObject class is the abstract base class for all visual and interactive 
 * elements in the SDOM framework. It defines the core interface and behavior for display 
 * objects, including rendering, event handling, parent-child hierarchy, anchoring, layout, 
 * and property management. IDisplayObject supports flexible positioning and sizing, z-ordering, 
 * visibility, and interactivity, as well as integration with the event system for robust user 
 * input and event-driven programming. Through its extensible design, it enables the creation of 
 * complex, nested GUI structures and serves as the foundation for all concrete display 
 * objects—such as panels, buttons, images, and labels—within SDOM-based applications.
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
/*
================================================================================
SDOM::IDisplayObject Contract

Intent:
    - IDisplayObject is the abstract base class for all visual and interactive elements in SDOM.
    - Defines the core interface and behavior for display objects, including rendering, event handling, parent-child hierarchy, anchoring, layout, and property management.
    - Enables creation of complex, nested GUI structures and serves as the foundation for all concrete display objects (panels, buttons, images, labels, etc.).

Requirements:
    1. Rendering
        - Must provide a method to render itself to a display surface.
        - Supports z-ordering and visibility toggling.

    2. Event Handling
        - Must handle user input and system events.
        - Supports event listener registration, removal, and event propagation (capture/bubble).

    3. Hierarchy Management
        - Supports parent-child relationships for nested GUI structures.
        - Provides methods to add, remove, and query children and parent.
        - Supports sorting and prioritizing children.

    4. Layout and Anchoring
        - Supports flexible positioning, sizing, and anchoring to edges.
        - Provides accessors and mutators for coordinates, dimensions, and anchor points.

    5. Property Management
        - Supports dynamic property registration and introspection (via IDataObject).
        - Properties are accessible for runtime modification and editor integration.

    6. Interactivity
        - Supports enabling/disabling, hiding/showing, and click/tab focus management.

    7. Extensibility
        - Designed for inheritance and composition.
        - Allows derived classes to extend or override core behavior.

    8. Type Identity and Verification
        - Must expose a public static type name (e.g., TypeName) for each concrete display object.
        - Must verify its type during construction (from InitStruct or Lua config) and report errors if mismatched.
        - Enables safe, consistent creation and identification of display object types across the SDOM framework.        

Non-Requirements:
    - Does not directly manage rendering resources (handled by other systems).
    - Does not implement concrete rendering or event logic (must be provided by derived classes).
    - Not responsible for serialization format (delegated to IDataObject).

Summary:
    IDisplayObject is a visual, interactive, and extensible interface for SDOM GUI elements, focused on:
        - Rendering and event handling
        - Hierarchical layout and anchoring
        - Dynamic property management
        - Interactivity and extensibility
================================================================================
*/


#ifndef __SDOM_IDISPLAY_OBJECT_HPP__
#define __SDOM_IDISPLAY_OBJECT_HPP__

#include <SDOM/SDOM.hpp>   
#include <SDOM/SDOM_IDataObject.hpp>
#include <iostream>

// // #include <SDOM/SDOM_Handle.hpp>
// #include <SDOM/SDOM_DomHandle.hpp>
// #include <SDOM/SDOM_ResHandle.hpp>

#include <SDOM/SDOM_EventTypeHash.hpp>



namespace SDOM
{

    class Event;
    class EventType;
    class EventTypeHash;
    class Stage;

/*
## Garbage Collection / Orphan Retention

Proposed approach: add an `OrphanRetentionPolicy` enum (or a smaller `retainOnOrphan` boolean for a quick
win) on `IDisplayObject` and make the Factory's orphan collector respect each object's policy. This supports 
editor/templates that should not be destroyed immediately and gives a clear migration path to more 
sophisticated GC.

Recommended enum (suggested values):
- `OrphanRetentionPolicy::AutoDestroy` — object is eligible for destruction immediately when orphaned. 
    (fast, default for ephemeral runtime objects)
- `OrphanRetentionPolicy::GracePeriod` — object is eligible only after a configurable grace period has 
    elapsed since it became orphaned; allows reparenting via DisplayObject within the grace window.
- `OrphanRetentionPolicy::RetainUntilManual` — object is never auto-destroyed; requires explicit Factory 
    or user code to destroy. (useful for editor templates and long-lived resources)
- `OrphanRetentionPolicy::RetainWhenReferenced` — object is retained while any DisplayObject or external 
    reference exists; destroyed only once all references are gone and policy conditions met. (optional 
    advanced mode)

Minimal API hints:
- in `IDisplayObject`: add `OrphanRetentionPolicy orphanPolicy = OrphanRetentionPolicy::GracePeriod;` 
    and `std::chrono::milliseconds orphanGrace{5000};`
- convenience setters/getters: `setOrphanPolicy(...)`, `getOrphanPolicy()`, `setOrphanGrace(...)`
- in the Factory: track an `optional<steady_clock::time_point> orphanedAt` per object and implement 
    `collectOrphans()` which checks policy+elapsed time and destroys accordingly.

Notes & test ideas:
- Reparent before grace expiry should clear `orphanedAt` and prevent destruction.
- `RetainUntilManual` objects should be visible in `Factory::listOrphanedObjects()` and only removed by 
    explicit destroy.
- Add unit tests for each policy (auto destroy, grace window reparenting, retained objects resisting 
    collection).

*/

    class IDisplayObject : public IDataObject
    {
        using SUPER = IDataObject;

        // --- Orphan Retention --- //
    public:
        enum class OrphanRetentionPolicy : int
        {
            RetainUntilManual,  // object is never auto-destroyed; requires explicit Factory or user code to destroy.
            AutoDestroy,        // object is eligible for destruction immediately when orphaned.
            GracePeriod         // allows reparenting via DisplayObject within the grace window.
        };
        OrphanRetentionPolicy getOrphanRetentionPolicy() const { return orphanPolicy_; }
        IDisplayObject& setOrphanRetentionPolicy(OrphanRetentionPolicy policy) { orphanPolicy_ = policy; return *this; }

    private: 
        std::chrono::steady_clock::time_point orphanedAt_ = std::chrono::steady_clock::now(); // Time when the object became orphaned
        OrphanRetentionPolicy orphanPolicy_ = OrphanRetentionPolicy::RetainUntilManual; // Default policy

    public:
        // --- Comparison Operators (for Sol2/Lua) --- //
        bool operator==(const IDisplayObject& other) const { return this == &other; }
        bool operator<(const IDisplayObject& other) const { return this < &other; }
        bool operator!=(const IDisplayObject& other) const { return !(*this == other); }
        bool operator>(const IDisplayObject& other) const { return other < *this; }
        bool operator<=(const IDisplayObject& other) const { return !(other < *this); }
        bool operator>=(const IDisplayObject& other) const { return !(*this < other); }

        static constexpr const char* TypeName = "IDisplayObject";

        // --- Construction & Initialization --- //
        struct InitStruct {
            std::string name = TypeName;
            std::string type = TypeName;
            float x = 0.0f, y = 0.0f, width = 0.0f, height = 0.0f;
            SDL_Color color = {255, 0, 255, 255};
            AnchorPoint anchorTop = AnchorPoint::TOP_LEFT, anchorLeft = AnchorPoint::TOP_LEFT;
            AnchorPoint anchorBottom = AnchorPoint::TOP_LEFT, anchorRight = AnchorPoint::TOP_LEFT;
            int z_order = 0, priority = 0;
            bool isClickable = true, isEnabled = true, isHidden = false;
            int tabPriority = 0;
            bool tabEnabled = true;
        };

        friend class Factory;

    protected:
        IDisplayObject(const InitStruct& init);
        IDisplayObject(const sol::table& config);
        IDisplayObject();

    public:
        virtual ~IDisplayObject();

        // --- Lifecycle & Core Virtuals --- //
        virtual bool onInit() override;
        virtual void onQuit() override {};
        virtual void onUpdate(float fElapsedTime) = 0;
        virtual void onEvent(const Event& event) = 0;
        virtual void onRender() = 0;
        virtual bool onUnitTest() { return true; }

        // --- Dirty/State Management --- //
        void cleanAll();
        bool getDirty() const { return bIsDirty_; }
        IDisplayObject& setDirty() { bIsDirty_ = true; return *this; }
        bool isDirty() const { return bIsDirty_; }

        // --- Debug/Utility --- //
        void printTree(int depth = 0, bool isLast = true, const std::vector<bool>& hasMoreSiblings = {}) const;

        // --- Event Handling --- //
        void addEventListener(EventType& type, std::function<void(Event&)> listener, bool useCapture = false, int priority = 0);
        void removeEventListener(EventType& type, std::function<void(Event&)> listener, bool useCapture = false);
        void triggerEventListeners(Event& event, bool useCapture);
        bool hasEventListeners(const EventType& type, bool useCapture) const;

        // --- Hierarchy Management --- //
        void addChild(DisplayObject child, bool useWorld = false, int worldX = 0, int worldY = 0);
        DisplayObject getChild(std::string name) const;
        bool removeChild(DisplayObject child);
        const std::vector<DisplayObject>& getChildren() const;
        DisplayObject getParent() const;
        IDisplayObject& setParent(const DisplayObject& parent);
        bool hasChild(DisplayObject child) const;

        // --- Type & Property Access --- //
        std::string getType() const { return type_; }
        IDisplayObject& setType(const std::string& newType) { type_ = newType; return *this; }
        Bounds getBounds() const { return { getLeft(), getTop(), getRight(), getBottom() }; }
        IDisplayObject& setBounds(const Bounds& b) { setLeft(b.left); setTop(b.top); setRight(b.right); setBottom(b.bottom); return *this; }  // **NEW**
        SDL_Color getColor() const { return color_; }
        IDisplayObject& setColor(const SDL_Color& color) { color_ = color; return *this; }

        // --- Priority & Z-Order --- //
        int getMaxPriority() const;
        int getMinPriority() const;
        int getPriority() const { return priority_; }
        IDisplayObject& setToHighestPriority();
        IDisplayObject& setToLowestPriority();
        IDisplayObject& sortChildrenByPriority();
        IDisplayObject& setPriority(int priority);
        std::vector<int> getChildrenPriorities() const;
        IDisplayObject& moveToTop();
        int getZOrder() const { return z_order_; }
        IDisplayObject& setZOrder(int z_order) { z_order_ = z_order; return *this; }

        // --- Focus & Interactivity --- //
        void setKeyboardFocus();
        bool isKeyboardFocused() const;
        bool isMouseHovered() const;
        bool isClickable() const { return isClickable_; }
        IDisplayObject& setClickable(bool clickable) { isClickable_ = clickable; setDirty(); return *this; }
        bool isEnabled() const { return isEnabled_; }
        IDisplayObject& setEnabled(bool enabled) { isEnabled_ = enabled; setDirty(); return *this; }
        bool isHidden() const { return isHidden_; }
        IDisplayObject& setHidden(bool hidden) { isHidden_ = hidden; setDirty(); return *this; }
        bool isVisible() const { return !isHidden_; }
        IDisplayObject& setVisible(bool visible) { isHidden_ = !visible; setDirty(); return *this; }

        // --- Tab Management --- //
        int getTabPriority() const;
        IDisplayObject& setTabPriority(int index);
        bool isTabEnabled() const;
        IDisplayObject& setTabEnabled(bool enabled);

        // --- Geometry & Layout --- //
        int getX() const;
        int getY() const;
        int getWidth() const;
        int getHeight() const;
        IDisplayObject& setX(int p_x);
        IDisplayObject& setY(int p_y);
        IDisplayObject& setWidth(int width);
        IDisplayObject& setHeight(int height);

        // --- Edge Anchors --- //
        AnchorPoint getAnchorTop() const { return anchorTop_; }
        AnchorPoint getAnchorLeft() const { return anchorLeft_; }
        AnchorPoint getAnchorBottom() const { return anchorBottom_; }
        AnchorPoint getAnchorRight() const { return anchorRight_; }
        void setAnchorTop(AnchorPoint ap);
        void setAnchorLeft(AnchorPoint ap);
        void setAnchorBottom(AnchorPoint ap);
        void setAnchorRight(AnchorPoint ap);

        // --- Edge Positions --- //
        float getLeft() const;
        float getRight() const;
        float getTop() const;
        float getBottom() const;
        IDisplayObject& setLeft(float p_left);
        IDisplayObject& setRight(float p_right);
        IDisplayObject& setTop(float p_top);
        IDisplayObject& setBottom(float p_bottom);

    protected:
        // --- Local Offset Accessors --- //
        float getLocalLeft() const { return left_; }
        float getLocalRight() const { return right_; }
        float getLocalTop() const { return top_; }
        float getLocalBottom() const { return bottom_; }
        IDisplayObject& setLocalLeft(float value) { left_ = value; return *this; }
        IDisplayObject& setLocalRight(float value) { right_ = value; return *this; }
        IDisplayObject& setLocalTop(float value) { top_ = value; return *this; }
        IDisplayObject& setLocalBottom(float value) { bottom_ = value; return *this; }

        std::string type_;

    private: // --- Member Variables --- //
        float left_, top_, right_, bottom_;
        bool bIsDirty_ = false;
        SDL_Color color_ = {255, 255, 255, 255};
        AnchorPoint anchorTop_ = AnchorPoint::TOP_LEFT;
        AnchorPoint anchorLeft_ = AnchorPoint::TOP_LEFT;
        AnchorPoint anchorBottom_ = AnchorPoint::TOP_LEFT;
        AnchorPoint anchorRight_ = AnchorPoint::TOP_LEFT;
        int z_order_ = 0;
        int priority_ = 0;
        bool isClickable_ = false;
        bool isEnabled_ = true;
        bool isHidden_ = false;
        int tabPriority_ = -1;
        bool tabEnabled_ = false;
        DisplayObject parent_;
        std::vector<DisplayObject> children_;

        // --- Event Listener Containers --- //
        struct ListenerEntry {
            std::function<void(Event&)> listener;
            int priority;
            EventType eventType;
        };
        std::unordered_map<EventType, std::vector<ListenerEntry>, EventTypeHash> captureEventListeners;
        std::unordered_map<EventType, std::vector<ListenerEntry>, EventTypeHash> bubblingEventListeners;

        // --- Internal/Utility --- //
        IDisplayObject(const IDisplayObject& other) = delete;
        void attachChild_(DisplayObject child, DisplayObject parent, bool useWorld = false, int worldX = 0, int worldY = 0);
        void removeOrphan_(const DisplayObject& orphan);

    protected:
        // --- Lua Registration --- //
        virtual void _registerLua(const std::string& typeName, sol::state_view lua); // Deprecated
        sol::usertype<IDisplayObject> objHandleType_;

        virtual void _registerDisplayObject(const std::string& typeName, sol::state_view lua);
    
    };

} // namespace SDOM

// // include the LUA wrappers for IDisplayObject
// #include "lua_IDisplayObject.hpp"    

#endif // __SDOM_IDISPLAY_OBJECT_HPP__