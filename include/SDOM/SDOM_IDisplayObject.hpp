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

// #include <SDOM/SDOM.hpp>   
#include <SDOM/SDOM_IDataObject.hpp>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <SDOM/SDOM_EventTypeHash.hpp>

#include <SDOM/SDOM.hpp>


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
    elapsed since it became orphaned; allows reparenting via DisplayHandle within the grace window.
- `OrphanRetentionPolicy::RetainUntilManual` — object is never auto-destroyed; requires explicit Factory 
    or user code to destroy. (useful for editor templates and long-lived resources)
- `OrphanRetentionPolicy::RetainWhenReferenced` — object is retained while any DisplayHandle or external 
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

namespace SDOM
{

    class Event;
    class EventType;
    class EventTypeHash;
    class Stage;
    class IDisplayObject;

    namespace IDisplayObjectAPI {
        void registerBindings(IDisplayObject& object, const std::string& typeName);
    }

    // Helper: convert a JSON array to a SDL_Color  (Move to SDL related helpers?)
    static inline SDL_Color json_to_color(const nlohmann::json& j)
    {
        SDL_Color c{0,0,0,255};

        if (j.is_object())
        {
            const auto readComponent = [&](const char* key, uint8_t& channel)
            {
                auto it = j.find(key);
                if (it != j.end() && it->is_number())
                    channel = static_cast<uint8_t>(it->get<int>());
            };

            readComponent("r", c.r);
            readComponent("g", c.g);
            readComponent("b", c.b);
            readComponent("a", c.a);
            return c;
        }

        if (!j.is_array())
            return c;

        if (j.size() > 0 && j[0].is_number()) c.r = j[0].get<int>();
        if (j.size() > 1 && j[1].is_number()) c.g = j[1].get<int>();
        if (j.size() > 2 && j[2].is_number()) c.b = j[2].get<int>();
        if (j.size() > 3 && j[3].is_number()) c.a = j[3].get<int>();

        return c;
    }


    class IDisplayObject : public IDataObject
    {
        using SUPER = IDataObject;

    public:
        friend void IDisplayObjectAPI::registerBindings(IDisplayObject& object, const std::string& typeName);

        // --- Orphan Retention --- //
        enum class OrphanRetentionPolicy : int
        {
            RetainUntilManual,  // object is never auto-destroyed; requires explicit Factory or user code to destroy.
            AutoDestroy,        // object is eligible for destruction immediately when orphaned.
            GracePeriod         // allows reparenting via DisplayHandle within the grace window.
        };
        OrphanRetentionPolicy getOrphanRetentionPolicy() const { return orphanPolicy_; }
        IDisplayObject& setOrphanRetentionPolicy(OrphanRetentionPolicy policy) { orphanPolicy_ = policy; return *this; }
        std::chrono::milliseconds getOrphanGrace() const { return orphanGrace; }
        IDisplayObject& setOrphanGrace(std::chrono::milliseconds grace) { orphanGrace = grace; return *this; }

    private:
        std::chrono::milliseconds orphanGrace{ORPHAN_GRACE_PERIOD};
        std::chrono::steady_clock::time_point orphanedAt_ = std::chrono::steady_clock::now(); // Time when the object became orphaned
        OrphanRetentionPolicy orphanPolicy_ = OrphanRetentionPolicy::AutoDestroy; // Default policy


    public:
        // --- Comparison Operators (for Sol2/Lua) --- //
        bool operator==(const IDisplayObject& other) const { return this == &other; }
        bool operator<(const IDisplayObject& other) const { return this < &other; }
        bool operator!=(const IDisplayObject& other) const { return !(*this == other); }
        bool operator>(const IDisplayObject& other) const { return other < *this; }
        bool operator<=(const IDisplayObject& other) const { return !(other < *this); }
        bool operator>=(const IDisplayObject& other) const { return !(*this < other); }

        static constexpr const char* TypeName = "IDisplayObject";

        // --- Performance Introspection --- //
        // Returns microseconds spent in the last onUpdate/onRender call for this object
        float getLastUpdateDelta() const;
        float getLastRenderDelta() const;

        // --- Construction & Initialization --- //
        struct InitStruct {
            std::string name = TypeName;
            std::string type = TypeName;
            float x = 0.0f;
            float y = 0.0f;
            float width  = 0.0f;
            float height = 0.0f;

            SDL_Color color            = {255, 0, 255, 255};

            SDL_Color foregroundColor  = {255, 255, 255, 255};
            SDL_Color backgroundColor  = {255, 255, 255, 128};
            SDL_Color borderColor      = {0, 0, 0, 128};
            SDL_Color outlineColor     = {0, 0, 0, 255};
            SDL_Color dropshadowColor  = {0, 0, 0, 128};

            AnchorPoint anchorTop    = AnchorPoint::TOP_LEFT;
            AnchorPoint anchorLeft   = AnchorPoint::TOP_LEFT;
            AnchorPoint anchorBottom = AnchorPoint::TOP_LEFT;
            AnchorPoint anchorRight  = AnchorPoint::TOP_LEFT;

            int  z_order      = 0;
            int  priority     = 0;
            bool isClickable  = true;
            bool isEnabled    = true;
            bool isHidden     = false;
            int  tabPriority  = 0;
            bool tabEnabled   = false;
            bool hasBorder    = true;
            bool hasBackground = true;

            // 🔽 Correct signature
            static void from_json(const nlohmann::json& j, InitStruct& init)
            {
                using nlohmann::json;
                using SDOM::json_to_color;     // you already defined this helper
                using SDOM::parseAnchorPoint;  // from SDOM_Utils.hpp/.cpp

                // ========== Scalars ==========
                if (j.contains("name"))            init.name        = j["name"].get<std::string>();
                if (j.contains("type"))            init.type        = j["type"].get<std::string>();
                if (j.contains("x"))               init.x           = j["x"].get<float>();
                if (j.contains("y"))               init.y           = j["y"].get<float>();
                if (j.contains("width"))           init.width       = j["width"].get<float>();
                if (j.contains("height"))          init.height      = j["height"].get<float>();

                if (j.contains("z_order"))         init.z_order     = j["z_order"].get<int>();
                if (j.contains("priority"))        init.priority    = j["priority"].get<int>();

                if (j.contains("is_clickable"))    init.isClickable = j["is_clickable"].get<bool>();
                if (j.contains("is_enabled"))      init.isEnabled   = j["is_enabled"].get<bool>();
                if (j.contains("is_hidden"))       init.isHidden    = j["is_hidden"].get<bool>();

                if (j.contains("tab_priority"))    init.tabPriority = j["tab_priority"].get<int>();
                if (j.contains("tab_enabled"))     init.tabEnabled  = j["tab_enabled"].get<bool>();

                if (j.contains("has_border"))      init.hasBorder     = j["has_border"].get<bool>();
                if (j.contains("has_background"))  init.hasBackground = j["has_background"].get<bool>();

                // ========== Colors ==========
                if (j.contains("color"))             init.color           = json_to_color(j["color"]);
                if (j.contains("foreground_color"))  init.foregroundColor = json_to_color(j["foreground_color"]);
                if (j.contains("background_color"))  init.backgroundColor = json_to_color(j["background_color"]);
                if (j.contains("border_color"))      init.borderColor     = json_to_color(j["border_color"]);
                if (j.contains("outline_color"))     init.outlineColor    = json_to_color(j["outline_color"]);
                if (j.contains("dropshadow_color"))  init.dropshadowColor = json_to_color(j["dropshadow_color"]);

                // ========== Anchors (string or int) ==========
                if (j.contains("anchor_top"))    init.anchorTop    = parseAnchorPoint(j["anchor_top"]);
                if (j.contains("anchor_left"))   init.anchorLeft   = parseAnchorPoint(j["anchor_left"]);
                if (j.contains("anchor_bottom")) init.anchorBottom = parseAnchorPoint(j["anchor_bottom"]);
                if (j.contains("anchor_right"))  init.anchorRight  = parseAnchorPoint(j["anchor_right"]);
            }
        }; // END: struct InitStruct


        friend class Factory;

    protected:
        IDisplayObject(const InitStruct& init);
        IDisplayObject(const sol::table& config);
        // Defaults-aware constructor: use provided defaults when parsing Lua table.
        IDisplayObject(const sol::table& config, const InitStruct& defaults);
        IDisplayObject();

    public:
    
        virtual ~IDisplayObject();

        // Public non-virtual lifecycle methods. Call these from owners (Factory, Core)
        // to ensure derived classes receive their virtual hooks while object
        // lifetime is still under owner control. These methods are safe to call
        // repeatedly (idempotent).
        bool startup();   // calls virtual onInit() once
        void shutdown();  // calls virtual onQuit() once

        // --- Lifecycle & Core Virtuals --- //
        bool onInit() override;
        // onLoad/onUnload mirror AssetObject semantics for renderer/device-backed
        // resources. onLoad may be called multiple times across device rebuilds
        // or stage transitions; implementations must be idempotent.
        // Default: no-op returning true.
        virtual bool onLoad() { return true; }
        virtual void onUnload() {}
        void onQuit() override;
        virtual void onUpdate(float fElapsedTime);
        virtual void onEvent(const Event& event);
        virtual void onRender() = 0;
        bool onUnitTest(int frame) override { (void)frame; return true; }

        // Called when the Core's logical render size changes or when SDL
        // render resources are rebuilt. Override in derived classes that
        // cache renderer-owned resources (e.g., textures) to proactively
        // invalidate/rebuild. Default: no-op.
        virtual void onWindowResize(int /*logicalWidth*/, int /*logicalHeight*/) {}

        // --- Dirty/State Management --- //
        void cleanAll();
        bool getDirty() const { return bIsDirty_; }
        IDisplayObject& setDirty() 
        { 
            bIsDirty_ = true; 
            return *this; 
        }
        IDisplayObject& setDirty(bool grime) 
        { 
            bIsDirty_ = grime; 
            return *this; 
        }
        bool isDirty() const { return bIsDirty_; }

        // --- Debug/Utility --- //
        void printTree(int depth = 0, bool isLast = true, const std::vector<bool>& hasMoreSiblings = {}) const;

        // --- Event Handling --- //
        void addEventListener(EventType& type, std::function<void(Event&)> listener, bool useCapture = false, int priority = 0);
        void removeEventListener(EventType& type, std::function<void(Event&)> listener, bool useCapture = false);
        void triggerEventListeners(Event& event, bool useCapture);
        bool hasEventListener(const EventType& type, bool useCapture) const;

        // --- Debug helpers: print registered listeners --- //
        // Prints a summary of the capture-phase listeners grouped by EventType.
        void printCaptureEventListeners() const;
        // Prints a summary of the bubbling-phase listeners grouped by EventType.
        void printBubblingEventListeners() const;
        void queue_event(const EventType& type, std::function<void(Event&)> init_payload);
        
        // --- Hierarchy Management --- //
        void addChild(DisplayHandle child, bool useWorld = false, int worldX = 0, int worldY = 0);
        DisplayHandle getChild(std::string name) const;
        bool removeChild(DisplayHandle child);
        bool removeChild(const std::string& name);
        const std::vector<DisplayHandle>& getChildren() const;
        std::vector<DisplayHandle>& getChildrenMutable() { return children_; }
        int countChildren() const { return static_cast<int>(children_.size()); }
        DisplayHandle getParent() const;
        IDisplayObject& setParent(const DisplayHandle& parent);
        bool hasChild(DisplayHandle child) const;

        // Ancestor/Descendant helpers
        bool isAncestorOf(DisplayHandle descendant) const;
        bool isAncestorOf(const std::string& name) const;
        bool isDescendantOf(DisplayHandle ancestor) const;
        bool isDescendantOf(const std::string& name) const;
        // Remove this object from its parent (convenience). Returns true if removed.
        bool removeFromParent();
        // Recursive descendant removal: search depth-first and remove first match. Returns true if removed.
        bool removeDescendant(DisplayHandle descendant);
        bool removeDescendant(const std::string& descendantName);
        bool isOnStage() const;

        // --- Type & Property Access --- //
        std::string getType() const { return type_; }
        IDisplayObject& setType(const std::string& newType) { type_ = newType; return *this; }
        Bounds getBounds() const { return { getLeft(), getTop(), getRight(), getBottom() }; }
        IDisplayObject& setBounds(const Bounds& b) { setLeft(b.left); setTop(b.top); setRight(b.right); setBottom(b.bottom); return *this; }  // **NEW**
        SDL_Color getColor() const { return color_; }
        IDisplayObject& setColor(const SDL_Color& color) { color_ = color; return *this; }
        SDL_Color getForegroundColor() const { return foregroundColor_; }
        IDisplayObject& setForegroundColor(const SDL_Color& color) { foregroundColor_ = color; setDirty(); return *this; }
        SDL_Color getBackgroundColor() const { return backgroundColor_; }
        IDisplayObject& setBackgroundColor(const SDL_Color& color) { backgroundColor_ = color; setDirty(); return *this; }
        SDL_Color getBorderColor() const { return borderColor_; }
        IDisplayObject& setBorderColor(const SDL_Color& color) { borderColor_ = color; setDirty(); return *this; }
        SDL_Color getOutlineColor() const { return outlineColor_; }
        IDisplayObject& setOutlineColor(const SDL_Color& color) { outlineColor_ = color; setDirty(); return *this; }
        SDL_Color getDropshadowColor() const { return dropshadowColor_; }
        IDisplayObject& setDropshadowColor(const SDL_Color& color) { dropshadowColor_ = color; setDirty(); return *this; }

        // --- Priority & Z-Order --- //
        int getMaxPriority() const;
        int getMinPriority() const;
        int getPriority() const { return priority_; }
        IDisplayObject& setToHighestPriority();
        IDisplayObject& setToLowestPriority();
        IDisplayObject& sortChildrenByPriority();
        IDisplayObject& setPriority(int priority);
        std::vector<int> getChildrenPriorities() const;
        IDisplayObject& moveToTop(); // NEW
        IDisplayObject& moveToBottom(); // NEW
        IDisplayObject& bringToFront(); // NEW
        IDisplayObject& sendToBack();   // NEW
        IDisplayObject& sendToBackAfter(const IDisplayObject* limitObj); // NEW
        int getZOrder() const { return z_order_; }        
        IDisplayObject& setZOrder(int z) 
        { 
            if (z_order_ != z) 
            {
                z_order_ = z;             
                setParentZOrderDirty(true); 
            }
            return *this; 
        }
        void setParentZOrderDirty(bool dirty_z) 
        {
            if (parent_) {
                parent_->zOrderDirty_ = dirty_z;
            }
        }            
        bool isZOrderDirty() const { return zOrderDirty_; }
        void sortByZOrder()
        {
            if (!zOrderDirty_) return;
            auto& kids = getChildrenMutable();
            std::sort(kids.begin(), kids.end(),
                [](auto& a, auto& b){ return a->getZOrder() < b->getZOrder(); });
            zOrderDirty_ = false;             
        }


        bool hasBorder() const { return border_; }
        bool hasBackground() const { return background_; }
        IDisplayObject& setBorder(bool hasBorder) { border_ = hasBorder; setDirty(); return *this; }
        IDisplayObject& setBackground(bool hasBackground) { background_ = hasBackground; setDirty(); return *this; }

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

        // --- Geometry & Layout (In World Coordinates) --- //
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

        // --- Edge Positions (In World Coordinates)--- //
        float getLeft() const;
        float getRight() const;
        float getTop() const;
        float getBottom() const;
        IDisplayObject& setLeft(float p_left);
        IDisplayObject& setRight(float p_right);
        IDisplayObject& setTop(float p_top);
        IDisplayObject& setBottom(float p_bottom);

        // --- Local Offset Accessors --- //
        float getLocalX() const { return left_; }
        float getLocalY() const { return top_; }
        float getLocalWidth() const { return right_ - left_; }
        float getLocalHeight() const { return bottom_ - top_; }

        float getLocalLeft() const { return left_; }
        float getLocalRight() const { return right_; }
        float getLocalTop() const { return top_; }
        float getLocalBottom() const { return bottom_; }

        IDisplayObject& setLocalLeft(float value) { left_ = value; return *this; }
        IDisplayObject& setLocalRight(float value) { right_ = value; return *this; }
        IDisplayObject& setLocalTop(float value) { top_ = value; return *this; }
        IDisplayObject& setLocalBottom(float value) { bottom_ = value; return *this; }


        // --- Temporary Lua Registration --- //
        void addChild_lua(IDisplayObject* obj, DisplayHandle child) { if (!obj) return; obj->addChild(child); }



    protected: // --- Member Variables --- //
        // std::string name_;  // defined in IDataObject
        float left_ = 0.0f, top_ = 0.0f, right_ = 0.0f, bottom_ = 0.0f;  // these are in terms of local not world coordinates
        std::string type_;  // Type identifier (e.g., "Button", "Panel", etc.)
        bool bIsDirty_ = false;
        bool zOrderDirty_ = true;
        SDL_Color color_ = {255, 255, 255, 255};

        SDL_Color foregroundColor_ = {255, 255, 255, 255};   // white
        SDL_Color backgroundColor_ = {255, 255, 255, 128};   // transparent
        SDL_Color borderColor_ = {0, 0, 0, 128};             // transparent
        SDL_Color outlineColor_ = {0, 0, 0, 255};            // black
        SDL_Color dropshadowColor_ = {0, 0, 0, 128};         // semi-transparent black
        
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
        bool border_ = false;
        bool background_ = false;
        DisplayHandle parent_;
        std::vector<DisplayHandle> children_;
        // Lifecycle state: true if startup() completed successfully and onInit()
        // has been run. Owners must call shutdown() before destroying objects
        // to ensure virtual cleanup hooks run outside destructors.
        bool started_ = false;
    public:
        // --- Event Listener Containers --- //
        struct ListenerEntry {
            std::function<void(Event&)> listener;
            int priority;
            EventType eventType;
        };
    protected:
        std::unordered_map<EventType, std::vector<ListenerEntry>, EventTypeHash> captureEventListeners;
        std::unordered_map<EventType, std::vector<ListenerEntry>, EventTypeHash> bubblingEventListeners;

        // --- Internal/Utility --- //
        IDisplayObject(const IDisplayObject& other) = delete;
        void attachChild_(DisplayHandle child, DisplayHandle parent, bool useWorld = false, int worldX = 0, int worldY = 0);
        void removeOrphan_(const DisplayHandle& orphan);

    protected:
    
        // --- Lua Registration --- //
        sol::usertype<IDisplayObject> objHandleType_;  

        // -----------------------------------------------------------------
        // 📜 Data Registry Integration
        // -----------------------------------------------------------------
        void registerBindingsImpl(const std::string& typeName) override;         

    };  // END: class IDisplayObject

} // namespace SDOM


#endif // __SDOM_IDISPLAY_OBJECT_HPP__
