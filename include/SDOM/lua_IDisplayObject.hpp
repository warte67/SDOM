// lua_IDisplayObject.hpp
#pragma once

#include <sol/sol.hpp>
#include <SDOM/SDOM.hpp>

namespace SDOM 
{
    class IDisplayObject;
    class EventType;
    class DomHandle;
    struct Bounds;

    // --- Dirty/State Management --- //
    void cleanAll_lua(IDisplayObject& obj); // **VERIFIED**
    bool getDirty_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setDirty_lua(IDisplayObject& obj); // **VERIFIED**
    bool isDirty_lua(const IDisplayObject& obj); // **VERIFIED**

    // --- Debug/Utility --- //
    void printTree_lua(const IDisplayObject& obj); // **VERIFIED**

    // --- Event Handling --- //
    void addEventListener_lua(IDisplayObject& obj, EventType& type, sol::function listener, bool useCapture, int priority); // **VERIFIED**
    void removeEventListener_lua(IDisplayObject& obj, EventType& type, sol::function listener, bool useCapture);  // **VERIFIED**

    // --- Hierarchy Management --- //
    void addChild_lua(IDisplayObject& obj, DomHandle child); // **VERIFIED**
    bool removeChild_lua(IDisplayObject& obj, DomHandle child); // **VERIFIED**
    bool hasChild_lua(const IDisplayObject& obj, DomHandle child); // **VERIFIED**
    DomHandle getParent_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setParent_lua(IDisplayObject& obj, const DomHandle& parent); // **VERIFIED**

    // --- Type & Property Access --- //
    std::string getType_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setType_lua(IDisplayObject& obj, const std::string& newType); // **VERIFIED**
    Bounds getBounds_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setBounds_lua(IDisplayObject& obj, const Bounds& b); // **VERIFIED**
    SDL_Color getColor_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setColor_lua(IDisplayObject& obj, const SDL_Color& color); // **VERIFIED**

    // --- Priority & Z-Order --- //
    int getMaxPriority_lua(const IDisplayObject& obj); // **VERIFIED**
    int getMinPriority_lua(const IDisplayObject& obj); // **VERIFIED**
    int getPriority_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setToHighestPriority_lua(IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setToLowestPriority_lua(IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& sortChildrenByPriority_lua(IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setPriority_lua(IDisplayObject& obj, int priority); // **VERIFIED**
    std::vector<int> getChildrenPriorities_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& moveToTop_lua(IDisplayObject& obj); // **VERIFIED**
    int getZOrder_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setZOrder_lua(IDisplayObject& obj, int z_order); // **VERIFIED**

    // --- Focus & Interactivity --- //

    // NOTE: object-level convenience. Calling `h:setKeyboardFocus()` will invoke
    //       `IDisplayObject::setKeyboardFocus()` which sets Core's keyboard focus
    //       via a DomHandle constructed from the object's name/type. To set focus
    //       directly by handle use the Core-level `setKeyboardFocusedObject` wrapper.
    //       (Planned: allow name/string overloads in bindings later.)

    void setKeyboardFocus_lua(IDisplayObject& obj); // **VERIFIED**
    bool isKeyboardFocused_lua(const IDisplayObject& obj); // **VERIFIED**
    bool isMouseHovered_lua(const IDisplayObject& obj); // **VERIFIED**
    bool isClickable_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setClickable_lua(IDisplayObject& obj, bool clickable); // **VERIFIED**
    bool isEnabled_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setEnabled_lua(IDisplayObject& obj, bool enabled); // **VERIFIED**
    bool isHidden_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setHidden_lua(IDisplayObject& obj, bool hidden); // **VERIFIED**
    bool isVisible_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setVisible_lua(IDisplayObject& obj, bool visible); // **VERIFIED**

    // --- Tab Management --- //

    // NOTE: these are object-level/tab-instance helpers (operate on the specific
    //       IDisplayObject). Some tab-related operations are also performed at
    //       a higher/global level (e.g. by Core or the Factory) — when we
    //       expand bindings we'll provide the appropriate global variants.

    int getTabPriority_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setTabPriority_lua(IDisplayObject& obj, int index); // **VERIFIED**
    bool isTabEnabled_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setTabEnabled_lua(IDisplayObject& obj, bool enabled); // **VERIFIED**

    // --- Geometry & Layout --- //
    int getX_lua(const IDisplayObject& obj); // **VERIFIED**
    int getY_lua(const IDisplayObject& obj); // **VERIFIED**
    int getWidth_lua(const IDisplayObject& obj); // **VERIFIED**
    int getHeight_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setX_lua(IDisplayObject& obj, int p_x); // **VERIFIED**
    IDisplayObject& setY_lua(IDisplayObject& obj, int p_y); // **VERIFIED**
    IDisplayObject& setWidth_lua(IDisplayObject& obj, int width); // **VERIFIED**
    IDisplayObject& setHeight_lua(IDisplayObject& obj, int height); // **VERIFIED**

    // --- Edge Anchors --- //
    AnchorPoint getAnchorTop_lua(const IDisplayObject& obj);
    AnchorPoint getAnchorLeft_lua(const IDisplayObject& obj);
    AnchorPoint getAnchorBottom_lua(const IDisplayObject& obj);
    AnchorPoint getAnchorRight_lua(const IDisplayObject& obj);
    void setAnchorTop_lua(IDisplayObject& obj, AnchorPoint ap);
    void setAnchorLeft_lua(IDisplayObject& obj, AnchorPoint ap);
    void setAnchorBottom_lua(IDisplayObject& obj, AnchorPoint ap);
    void setAnchorRight_lua(IDisplayObject& obj, AnchorPoint ap);

    // --- Edge Positions --- //
    float getLeft_lua(const IDisplayObject& obj); // **VERIFIED**
    float getRight_lua(const IDisplayObject& obj); // **VERIFIED**
    float getTop_lua(const IDisplayObject& obj); // **VERIFIED**
    float getBottom_lua(const IDisplayObject& obj); // **VERIFIED**
    IDisplayObject& setLeft_lua(IDisplayObject& obj, float p_left);
    IDisplayObject& setRight_lua(IDisplayObject& obj, float p_right);
    IDisplayObject& setTop_lua(IDisplayObject& obj, float p_top);
    IDisplayObject& setBottom_lua(IDisplayObject& obj, float p_bottom);
}

