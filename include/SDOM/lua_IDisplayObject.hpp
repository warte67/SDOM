// lua_IDisplayObject.hpp
#pragma once

#include <sol/sol.hpp>
#include <SDOM/SDOM.hpp>
// #include <SDOM/SDOM_IDisplayObject.hpp>

namespace SDOM 
{
    class IDisplayObject;
    class EventType;
    class DisplayObject;
    struct Bounds;

    // --- Dirty/State Management --- //
    void cleanAll_lua(IDisplayObject* obj); 
    bool getDirty_lua(const IDisplayObject* obj); 
    void setDirty_lua(IDisplayObject* obj); 
    bool isDirty_lua(const IDisplayObject* obj); 

    // --- Debug/Utility --- //
    void printTree_lua(const IDisplayObject* obj); 

    // --- Event Handling --- //
    void addEventListener_lua(IDisplayObject* obj, EventType& type, sol::function listener, bool useCapture, int priority); 
    void removeEventListener_lua(IDisplayObject* obj, EventType& type, sol::function listener, bool useCapture);  
    // Flexible variants that accept a Lua table descriptor or multiple-arg form
    void addEventListener_lua_any(IDisplayObject* obj, const sol::object& descriptor, const sol::object& maybe_listener, const sol::object& maybe_useCapture, const sol::object& maybe_priority);
    void removeEventListener_lua_any(IDisplayObject* obj, const sol::object& descriptor, const sol::object& maybe_listener, const sol::object& maybe_useCapture);
    // Short variants accepting only the descriptor table (common colon-call)
    void addEventListener_lua_any_short(IDisplayObject* obj, const sol::object& descriptor);
    void removeEventListener_lua_any_short(IDisplayObject* obj, const sol::object& descriptor);

    // --- Hierarchy Management --- //
    void addChild_lua(IDisplayObject* obj, DisplayObject child); 
    DisplayObject getChild_lua(const IDisplayObject* obj, std::string name);
    bool removeChild_lua(IDisplayObject* obj, DisplayObject child); 
    bool removeChild_lua(IDisplayObject* obj, const std::string& name);
    bool hasChild_lua(const IDisplayObject* obj, DisplayObject child); 
    DisplayObject getParent_lua(const IDisplayObject* obj); 
    void setParent_lua(IDisplayObject* obj, const DisplayObject& parent); 

    // Ancestor/Descendant helpers
    bool isAncestorOf_lua(IDisplayObject* obj, DisplayObject descendant);
    bool isAncestorOf_lua(IDisplayObject* obj, const std::string& name);
    bool isDescendantOf_lua(IDisplayObject* obj, DisplayObject ancestor);
    bool isDescendantOf_lua(IDisplayObject* obj, const std::string& name);
    // Remove this object from its parent (convenience). Returns true if removed.
    bool removeFromParent_lua(IDisplayObject* obj);
    // Recursive descendant removal: search depth-first and remove first match. Returns true if removed.
    bool removeDescendant_lua(IDisplayObject* obj, DisplayObject descendant);
    bool removeDescendant_lua(IDisplayObject* obj, const std::string& descendantName);

    // --- Type & Property Access --- //
    std::string getType_lua(const IDisplayObject* obj); 
    void setType_lua(IDisplayObject* obj, const std::string& newType); 
    Bounds getBounds_lua(const IDisplayObject* obj); 
    // Accept either a Bounds userdata or a Lua table describing bounds
    void setBounds_lua(IDisplayObject* obj, const sol::object& bobj); 
    // C++ caller-friendly overload: accept Bounds directly
    void setBounds_lua(IDisplayObject* obj, const Bounds& b); 
    SDL_Color getColor_lua(const IDisplayObject* obj); 
    // Accept either an SDL_Color userdata or a Lua table describing color
    void setColor_lua(IDisplayObject* obj, const sol::object& colorObj); 
    // C++ overload: accept SDL_Color directly
    void setColor_lua(IDisplayObject* obj, const SDL_Color& color); 

    // --- Priority & Z-Order --- //
    int getMaxPriority_lua(const IDisplayObject* obj); 
    int getMinPriority_lua(const IDisplayObject* obj); 
    int getPriority_lua(const IDisplayObject* obj); 
    void setToHighestPriority_lua(IDisplayObject* obj); 
    void setToLowestPriority_lua(IDisplayObject* obj); 
    void sortChildrenByPriority_lua(IDisplayObject* obj); 
    void setPriority_lua(IDisplayObject* obj, int priority); 
    std::vector<int> getChildrenPriorities_lua(const IDisplayObject* obj); 
    void moveToTop_lua(IDisplayObject* obj); 
    int getZOrder_lua(const IDisplayObject* obj); 
    void setZOrder_lua(IDisplayObject* obj, int z_order); 

    // --- Focus & Interactivity --- //

    // NOTE: object-level convenience. Calling `h:setKeyboardFocus()` will invoke
    //       `IDisplayObject::setKeyboardFocus()` which sets Core's keyboard focus
    //       via a DomHandle constructed from the object's name/type. To set focus
    //       directly by handle use the Core-level `setKeyboardFocusedObject` wrapper.
    //       (Planned: allow name/string overloads in bindings later.)

    void setKeyboardFocus_lua(IDisplayObject* obj); 
    bool isKeyboardFocused_lua(const IDisplayObject* obj); 
    bool isMouseHovered_lua(const IDisplayObject* obj); 
    bool isClickable_lua(const IDisplayObject* obj); 
    void setClickable_lua(IDisplayObject* obj, bool clickable); 
    bool isEnabled_lua(const IDisplayObject* obj); 
    void setEnabled_lua(IDisplayObject* obj, bool enabled); 
    bool isHidden_lua(const IDisplayObject* obj); 
    void setHidden_lua(IDisplayObject* obj, bool hidden); 
    bool isVisible_lua(const IDisplayObject* obj); 
    void setVisible_lua(IDisplayObject* obj, bool visible); 

    // --- Tab Management --- //

    // NOTE: these are object-level/tab-instance helpers (operate on the specific
    //       IDisplayObject). Some tab-related operations are also performed at
    //       a higher/global level (e.g. by Core or the Factory) — when we
    //       expand bindings we'll provide the appropriate global variants.

    int getTabPriority_lua(const IDisplayObject* obj); 
    void setTabPriority_lua(IDisplayObject* obj, int index); 
    bool isTabEnabled_lua(const IDisplayObject* obj); 
    void setTabEnabled_lua(IDisplayObject* obj, bool enabled); 

    // --- Geometry & Layout --- //
    int getX_lua(const IDisplayObject* obj); 
    int getY_lua(const IDisplayObject* obj); 
    int getWidth_lua(const IDisplayObject* obj); 
    int getHeight_lua(const IDisplayObject* obj); 
    void setX_lua(IDisplayObject* obj, int p_x); 
    void setY_lua(IDisplayObject* obj, int p_y); 
    void setWidth_lua(IDisplayObject* obj, int width); 
    void setHeight_lua(IDisplayObject* obj, int height); 

    // --- Edge Anchors --- //
    AnchorPoint getAnchorTop_lua(const IDisplayObject* obj); 
    AnchorPoint getAnchorLeft_lua(const IDisplayObject* obj); 
    AnchorPoint getAnchorBottom_lua(const IDisplayObject* obj); 
    AnchorPoint getAnchorRight_lua(const IDisplayObject* obj); 
    void setAnchorTop_lua(IDisplayObject* obj, AnchorPoint ap); 
    void setAnchorLeft_lua(IDisplayObject* obj, AnchorPoint ap); 
    void setAnchorBottom_lua(IDisplayObject* obj, AnchorPoint ap); 
    void setAnchorRight_lua(IDisplayObject* obj, AnchorPoint ap); 

    // --- Edge Positions --- //
    float getLeft_lua(const IDisplayObject* obj); 
    float getRight_lua(const IDisplayObject* obj); 
    float getTop_lua(const IDisplayObject* obj); 
    float getBottom_lua(const IDisplayObject* obj); 
    void setLeft_lua(IDisplayObject* obj, float p_left); 
    void setRight_lua(IDisplayObject* obj, float p_right); 
    void setTop_lua(IDisplayObject* obj, float p_top); 
    void setBottom_lua(IDisplayObject* obj, float p_bottom); 

    // --- Orphan Retention Policy --- //
    IDisplayObject::OrphanRetentionPolicy orphanPolicyFromString_lua(IDisplayObject* obj, const std::string& s);
    std::string orphanPolicyToString_lua(IDisplayObject* obj, IDisplayObject::OrphanRetentionPolicy p);
    void setOrphanRetentionPolicy_lua(IDisplayObject* obj, const std::string& policyStr);
    std::string getOrphanRetentionPolicyString_lua(IDisplayObject* obj);

}

