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
    IDisplayObject* setDirty_lua(IDisplayObject* obj); 
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
    bool removeChild_lua(IDisplayObject* obj, DisplayObject child); 
    bool hasChild_lua(const IDisplayObject* obj, DisplayObject child); 
    DisplayObject getParent_lua(const IDisplayObject* obj); 
    IDisplayObject* setParent_lua(IDisplayObject* obj, const DisplayObject& parent); 

    // --- Type & Property Access --- //
    std::string getType_lua(const IDisplayObject* obj); 
    IDisplayObject* setType_lua(IDisplayObject* obj, const std::string& newType); 
    Bounds getBounds_lua(const IDisplayObject* obj); 
    // Accept either a Bounds userdata or a Lua table describing bounds
    IDisplayObject* setBounds_lua(IDisplayObject* obj, const sol::object& bobj); 
    // C++ caller-friendly overload: accept Bounds directly
    IDisplayObject* setBounds_lua(IDisplayObject* obj, const Bounds& b); 
    SDL_Color getColor_lua(const IDisplayObject* obj); 
    // Accept either an SDL_Color userdata or a Lua table describing color
    IDisplayObject* setColor_lua(IDisplayObject* obj, const sol::object& colorObj); 
    // C++ overload: accept SDL_Color directly
    IDisplayObject* setColor_lua(IDisplayObject* obj, const SDL_Color& color); 

    // --- Priority & Z-Order --- //
    int getMaxPriority_lua(const IDisplayObject* obj); 
    int getMinPriority_lua(const IDisplayObject* obj); 
    int getPriority_lua(const IDisplayObject* obj); 
    IDisplayObject* setToHighestPriority_lua(IDisplayObject* obj); 
    IDisplayObject* setToLowestPriority_lua(IDisplayObject* obj); 
    IDisplayObject* sortChildrenByPriority_lua(IDisplayObject* obj); 
    IDisplayObject* setPriority_lua(IDisplayObject* obj, int priority); 
    std::vector<int> getChildrenPriorities_lua(const IDisplayObject* obj); 
    IDisplayObject* moveToTop_lua(IDisplayObject* obj); 
    int getZOrder_lua(const IDisplayObject* obj); 
    IDisplayObject* setZOrder_lua(IDisplayObject* obj, int z_order); 

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
    IDisplayObject* setClickable_lua(IDisplayObject* obj, bool clickable); 
    bool isEnabled_lua(const IDisplayObject* obj); 
    IDisplayObject* setEnabled_lua(IDisplayObject* obj, bool enabled); 
    bool isHidden_lua(const IDisplayObject* obj); 
    IDisplayObject* setHidden_lua(IDisplayObject* obj, bool hidden); 
    bool isVisible_lua(const IDisplayObject* obj); 
    IDisplayObject* setVisible_lua(IDisplayObject* obj, bool visible); 

    // --- Tab Management --- //

    // NOTE: these are object-level/tab-instance helpers (operate on the specific
    //       IDisplayObject). Some tab-related operations are also performed at
    //       a higher/global level (e.g. by Core or the Factory) — when we
    //       expand bindings we'll provide the appropriate global variants.

    int getTabPriority_lua(const IDisplayObject* obj); 
    IDisplayObject* setTabPriority_lua(IDisplayObject* obj, int index); 
    bool isTabEnabled_lua(const IDisplayObject* obj); 
    IDisplayObject* setTabEnabled_lua(IDisplayObject* obj, bool enabled); 

    // --- Geometry & Layout --- //
    int getX_lua(const IDisplayObject* obj); 
    int getY_lua(const IDisplayObject* obj); 
    int getWidth_lua(const IDisplayObject* obj); 
    int getHeight_lua(const IDisplayObject* obj); 
    IDisplayObject* setX_lua(IDisplayObject* obj, int p_x); 
    IDisplayObject* setY_lua(IDisplayObject* obj, int p_y); 
    IDisplayObject* setWidth_lua(IDisplayObject* obj, int width); 
    IDisplayObject* setHeight_lua(IDisplayObject* obj, int height); 

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
    IDisplayObject* setLeft_lua(IDisplayObject* obj, float p_left); 
    IDisplayObject* setRight_lua(IDisplayObject* obj, float p_right); 
    IDisplayObject* setTop_lua(IDisplayObject* obj, float p_top); 
    IDisplayObject* setBottom_lua(IDisplayObject* obj, float p_bottom); 

    // --- Orphan Retention Policy --- //
    IDisplayObject::OrphanRetentionPolicy orphanPolicyFromString_lua(IDisplayObject* obj, const std::string& s);
    std::string orphanPolicyToString_lua(IDisplayObject* obj, IDisplayObject::OrphanRetentionPolicy p);
    IDisplayObject* setOrphanRetentionPolicy_lua(IDisplayObject* obj, const std::string& policyStr);
    std::string getOrphanRetentionPolicyString_lua(IDisplayObject* obj);

}

