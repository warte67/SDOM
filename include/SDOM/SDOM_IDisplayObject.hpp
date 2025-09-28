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

    class IDisplayObject : public IDataObject
    {
        using SUPER = IDataObject;

    public:
            // Note:Comparison operators for Sol2/Lua ...
            //      The extra comparison operators in IDisplayObject are only needed 
            //      to satisfy Sol2's requirements for automagic and Lua meta-methods.
            bool operator==(const IDisplayObject& other) const {
                return this == &other;
            }
            bool operator<(const IDisplayObject& other) const {
                return this < &other;
            }
            bool operator!=(const IDisplayObject& other) const {
                return !(*this == other);
            }
            bool operator>(const IDisplayObject& other) const {
                return other < *this;
            }
            bool operator<=(const IDisplayObject& other) const {
                return !(other < *this);
            }
            bool operator>=(const IDisplayObject& other) const {
                return !(*this < other);
            }

        static constexpr const char* TypeName = "IDisplayObject";
        struct InitStruct
        {
            std::string name = TypeName;     
            std::string type = TypeName;           
            float x = 0.0f;                                     
            float y = 0.0f;                                     
            float width = 0.0f;                                 
            float height = 0.0f;                                
            SDL_Color color = { 255, 0, 255, 255 };           
            AnchorPoint anchorTop = AnchorPoint::TOP_LEFT;      
            AnchorPoint anchorLeft = AnchorPoint::TOP_LEFT;     
            AnchorPoint anchorBottom = AnchorPoint::TOP_LEFT;   
            AnchorPoint anchorRight = AnchorPoint::TOP_LEFT;    
            int z_order = 0;                                    
            int priority = 0;                                   
            bool isClickable = true;                            
            bool isEnabled = true;                              
            bool isHidden = false;                              
            int tabPriority = 0;                                
            bool tabEnabled = true;                             
        };

        friend class Factory; // Allow Factory to create IDisplayObjects

    protected:   // protected constructor so only the Factory can create the object
        IDisplayObject(const InitStruct& init); 
        IDisplayObject(const sol::table& config); // NEW: Lua-based constructor
        IDisplayObject();                       

    public:

        virtual ~IDisplayObject();

        virtual bool onInit() override;
        virtual void onQuit() override {};
        virtual void onUpdate(float fElapsedTime)=0;
        virtual void onEvent(const Event& event)=0;
        virtual void onRender()=0;
        virtual bool onUnitTest() { return true; }


        void cleanAll();
        void printTree(int depth = 0, bool isLast = true, const std::vector<bool>& hasMoreSiblings = {}) const;
        bool getDirty() const { return bIsDirty_; }
        IDisplayObject& setDirty() { bIsDirty_ = true; return *this; }
        bool isDirty() const { return bIsDirty_; }

        SDL_Color getColor() const { return color_; }
        IDisplayObject& setColor(const SDL_Color& color) { color_ = color; return *this; }
        
        void addEventListener(EventType& type, std::function<void(Event&)> listener, bool useCapture = false, int priority = 0);
        void removeEventListener(EventType& type, std::function<void(Event&)> listener, bool useCapture = false);
        void triggerEventListeners(Event& event, bool useCapture);       

        void addChild(DomHandle child, bool useWorld=false, int worldX=0, int worldY=0);
        bool removeChild(DomHandle child);

        const std::vector<DomHandle>& getChildren() const { return children_; }

        DomHandle getParent() const { return parent_; }
        IDisplayObject& setParent(const DomHandle& parent) { parent_ = parent; return *this; }

        // Moved to IDataObject
        // std::string getName() const { return name_; }
        // IDisplayObject& setName(const std::string& newName) { name_ = newName; return *this; }

        std::string getType() const { return type_; }
        IDisplayObject& setType(const std::string& newType) { type_ = newType; return *this; }

        Bounds getBounds() const { return { getLeft(), getTop(), getRight(), getBottom() }; }


        int getMaxPriority() const;
        int getMinPriority() const;
        int getPriority() const { return priority_; }
        IDisplayObject& setToHighestPriority();
        IDisplayObject& setToLowestPriority();
        IDisplayObject& sortChildrenByPriority();
        IDisplayObject& setPriority(int priority);
        std::vector<int> getChildrenPriorities() const;
        IDisplayObject& moveToTop();
        bool hasChild(DomHandle child) const;
        int getZOrder() const { return z_order_; } // Getter for z_order_
        IDisplayObject& setZOrder(int z_order) { z_order_ = z_order; return *this; } // Setter for z_order_
        
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

        // Tab stop management
        int getTabPriority() const;
        IDisplayObject& setTabPriority(int index);
        bool isTabEnabled() const;
        IDisplayObject& setTabEnabled(bool enabled);


        // Coordinate Accessors:
        int getX() const;       // get the accumilated ancestrial horizontal coordinate
        int getY() const;       // get the accumilated ancestrial vertical coordinate
        int getWidth() const;   // get the width of the object
        int getHeight() const;  // get the height of the object

        // These setters may need refactoring:
        IDisplayObject& setX(int p_x);   // convert horizontal world coordinate and save as horizontal offset
        IDisplayObject& setY(int p_y);   // convert vertical world coordinate and save as vertical offset
        IDisplayObject& setWidth(int width);    // set the width of the object
        IDisplayObject& setHeight(int height);  // set the height of the object

        // Edge Anchor Getters:
        AnchorPoint getAnchorTop() const { return anchorTop_; }
        AnchorPoint getAnchorLeft() const { return anchorLeft_; }
        AnchorPoint getAnchorBottom() const { return anchorBottom_; }
        AnchorPoint getAnchorRight() const { return anchorRight_; }

        // Edge Anchor Setters:
        void setAnchorTop(AnchorPoint ap);      // adjust y_ and height_ based on new origin point
        void setAnchorLeft(AnchorPoint ap);     // adjust x_ and width_ based on a new origin point
        void setAnchorBottom(AnchorPoint ap);   // adjust y_ and height_ based on new origin point
        void setAnchorRight(AnchorPoint ap);    // adjust x_ and width_ based on a new origin point

        // Edge Position Getters:
        float getLeft() const;
        float getRight() const;
        float getTop() const;
        float getBottom() const;

        // Edge Position setters:
        IDisplayObject& setLeft(float p_left);
        IDisplayObject& setRight(float p_right);
        IDisplayObject& setTop(float p_top);
        IDisplayObject& setBottom(float p_bottom);

    protected:

        // Getters for Local Offsets
        float getLocalLeft() const { return left_; }
        float getLocalRight() const { return right_; }
        float getLocalTop() const { return top_; }
        float getLocalBottom() const { return bottom_; }  
        // Setters for Local Offsets
        IDisplayObject& setLocalLeft(float value) { left_ = value; return *this; }
        IDisplayObject& setLocalRight(float value) { right_ = value; return *this; }
        IDisplayObject& setLocalTop(float value) { top_ = value; return *this; }
        IDisplayObject& setLocalBottom(float value) { bottom_ = value; return *this; }
   
        // std::string name_;  // moved to IDataObject
        std::string type_;

    public:  // temporarily public for testing
        float left_, top_, right_, bottom_; // Calculated bounds based on anchors
        // std::string name_; // Name of the display object
        bool bIsDirty_ = false; // Flag to indicate if the display object needs to be redrawn
        SDL_Color color_ = { 255, 255, 255, 255 }; // Default color WHITE

        // default anchor is TOP_LEFT
        AnchorPoint anchorTop_    = AnchorPoint::TOP_LEFT;
        AnchorPoint anchorLeft_   = AnchorPoint::TOP_LEFT;
        AnchorPoint anchorBottom_ = AnchorPoint::TOP_LEFT;
        AnchorPoint anchorRight_  = AnchorPoint::TOP_LEFT;

        int z_order_ = 0; // Default z_order value
        int priority_ = 0; // Default priority value
        bool isClickable_ = false; // not clickable by default

        // isEnabled: Determines whether the object is interactive (e.g., clickable). 
        // If false, the object and its children are grayed out and unclickable.
        bool isEnabled_ = true; // Default enabled state

        // isHidden: Determines whether the object is rendered. If true, the object 
        // is not rendered but still responds to events if part of the DOM tree.        
        bool isHidden_ = false; // Default hidden state

        // tab stops
        int tabPriority_ = -1;       // tabPriority (i.e. tabStop within a priority queue)   
        bool tabEnabled_ = false;    // Indicates if the object can be focused via Tab

        DomHandle parent_;
        std::vector<DomHandle> children_;

        // Containers to store ListenerEntrys for different event phases
        struct ListenerEntry {
            std::function<void(Event&)> listener;
            int priority;
            EventType eventType; // Debugging field to store the event type name            
        };
        std::unordered_map<EventType, std::vector<ListenerEntry>, EventTypeHash> captureEventListeners;
        std::unordered_map<EventType, std::vector<ListenerEntry>, EventTypeHash> bubblingEventListeners;
      
        // Delete copy constructor and assignment operator
        IDisplayObject(const IDisplayObject& other) = delete;
        // IDisplayObject& operator=(const IDisplayObject& other) = delete;      

        void attachChild_(DomHandle child, DomHandle parent, bool useWorld=false, int worldX=0, int worldY=0);
        void removeOrphan_(const DomHandle& orphan);

        void registerLua_();

        // --- LUA Registration --- //
    protected:
        virtual void _registerLua_Usertype(sol::state_view lua)      { SUPER::_registerLua_Usertype(lua); }  
        virtual void _registerLua_Properties(sol::state_view lua)    { SUPER::_registerLua_Properties(lua); }
        virtual void _registerLua_Commands(sol::state_view lua)      { SUPER::_registerLua_Commands(lua); }
        virtual void _registerLua_Meta(sol::state_view lua)          { SUPER::_registerLua_Meta(lua); }
        virtual void _registerLua_Children(sol::state_view lua)      { SUPER::_registerLua_Children(lua); }

        virtual void _registerLua_All(sol::state_view lua) override       
        {
            SUPER::_registerLua_All(lua);
            _registerLua_Usertype(lua);
            _registerLua_Properties(lua);
            _registerLua_Commands(lua);
            _registerLua_Meta(lua);
            _registerLua_Children(lua);

            std::cout << "IDisplayObject: Registered Lua bindings." << std::endl;
        }
    };

} // namespace SDOM

#endif // __SDOM_IDISPLAY_OBJECT_HPP__