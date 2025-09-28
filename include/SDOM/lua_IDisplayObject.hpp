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
    void cleanAll_lua(IDisplayObject& obj);
    bool getDirty_lua(const IDisplayObject& obj);
    IDisplayObject& setDirty_lua(IDisplayObject& obj);
    bool isDirty_lua(const IDisplayObject& obj);

    // --- Debug/Utility --- //
    void printTree_lua(const IDisplayObject& obj);

    // --- Event Handling --- //
    void addEventListener_lua(IDisplayObject& obj, EventType& type, sol::function listener, bool useCapture, int priority);
    void removeEventListener_lua(IDisplayObject& obj, EventType& type, sol::function listener, bool useCapture);

    // --- Hierarchy Management --- //
    void addChild_lua(IDisplayObject& obj, DomHandle child);
    bool removeChild_lua(IDisplayObject& obj, DomHandle child);
    bool hasChild_lua(const IDisplayObject& obj, DomHandle child);
    DomHandle getParent_lua(const IDisplayObject& obj);
    IDisplayObject& setParent_lua(IDisplayObject& obj, const DomHandle& parent);

    // --- Type & Property Access --- //
    std::string getType_lua(const IDisplayObject& obj);
    IDisplayObject& setType_lua(IDisplayObject& obj, const std::string& newType);
    Bounds getBounds_lua(const IDisplayObject& obj);
    SDL_Color getColor_lua(const IDisplayObject& obj);
    IDisplayObject& setColor_lua(IDisplayObject& obj, const SDL_Color& color);

    // --- Priority & Z-Order --- //
    int getMaxPriority_lua(const IDisplayObject& obj);
    int getMinPriority_lua(const IDisplayObject& obj);
    int getPriority_lua(const IDisplayObject& obj);
    IDisplayObject& setToHighestPriority_lua(IDisplayObject& obj);
    IDisplayObject& setToLowestPriority_lua(IDisplayObject& obj);
    IDisplayObject& sortChildrenByPriority_lua(IDisplayObject& obj);
    IDisplayObject& setPriority_lua(IDisplayObject& obj, int priority);
    std::vector<int> getChildrenPriorities_lua(const IDisplayObject& obj);
    IDisplayObject& moveToTop_lua(IDisplayObject& obj);
    int getZOrder_lua(const IDisplayObject& obj);
    IDisplayObject& setZOrder_lua(IDisplayObject& obj, int z_order);

    // --- Focus & Interactivity --- //
    void setKeyboardFocus_lua(IDisplayObject& obj);
    bool isKeyboardFocused_lua(const IDisplayObject& obj);
    bool isMouseHovered_lua(const IDisplayObject& obj);
    bool isClickable_lua(const IDisplayObject& obj);
    IDisplayObject& setClickable_lua(IDisplayObject& obj, bool clickable);
    bool isEnabled_lua(const IDisplayObject& obj);
    IDisplayObject& setEnabled_lua(IDisplayObject& obj, bool enabled);
    bool isHidden_lua(const IDisplayObject& obj);
    IDisplayObject& setHidden_lua(IDisplayObject& obj, bool hidden);
    bool isVisible_lua(const IDisplayObject& obj);
    IDisplayObject& setVisible_lua(IDisplayObject& obj, bool visible);

    // --- Tab Management --- //
    int getTabPriority_lua(const IDisplayObject& obj);
    IDisplayObject& setTabPriority_lua(IDisplayObject& obj, int index);
    bool isTabEnabled_lua(const IDisplayObject& obj);
    IDisplayObject& setTabEnabled_lua(IDisplayObject& obj, bool enabled);

    // --- Geometry & Layout --- //
    int getX_lua(const IDisplayObject& obj);
    int getY_lua(const IDisplayObject& obj);
    int getWidth_lua(const IDisplayObject& obj);
    int getHeight_lua(const IDisplayObject& obj);
    IDisplayObject& setX_lua(IDisplayObject& obj, int p_x);
    IDisplayObject& setY_lua(IDisplayObject& obj, int p_y);
    IDisplayObject& setWidth_lua(IDisplayObject& obj, int width);
    IDisplayObject& setHeight_lua(IDisplayObject& obj, int height);

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
    float getLeft_lua(const IDisplayObject& obj);
    float getRight_lua(const IDisplayObject& obj);
    float getTop_lua(const IDisplayObject& obj);
    float getBottom_lua(const IDisplayObject& obj);
    IDisplayObject& setLeft_lua(IDisplayObject& obj, float p_left);
    IDisplayObject& setRight_lua(IDisplayObject& obj, float p_right);
    IDisplayObject& setTop_lua(IDisplayObject& obj, float p_top);
    IDisplayObject& setBottom_lua(IDisplayObject& obj, float p_bottom);
}


// ## SDOM::IDisplayObject Public Methods (Grouped)


// ### --- Dirty/State Management ---
// - `void cleanAll()`
// - `bool getDirty() const`
// - `IDisplayObject& setDirty()`
// - `bool isDirty() const`

// ---

// ### --- Debug/Utility ---
// - `void printTree(int depth = 0, bool isLast = true, const std::vector<bool>& hasMoreSiblings = {}) const`

// ---

// ### --- Event Handling ---
// - `void addEventListener(EventType& type, std::function<void(Event&)> listener, bool useCapture = false, int priority = 0)`
// - `void removeEventListener(EventType& type, std::function<void(Event&)> listener, bool useCapture = false)`

// ---

// ### --- Hierarchy Management ---
// - `void addChild(DomHandle child, bool useWorld = false, int worldX = 0, int worldY = 0)`
// - `bool removeChild(DomHandle child)`
// - `const std::vector<DomHandle>& getChildren() const`
// - `DomHandle getParent() const`
// - `IDisplayObject& setParent(const DomHandle& parent)`
// - `bool hasChild(DomHandle child) const`

// ---

// ### --- Type & Property Access ---
// - `std::string getType() const`
// - `IDisplayObject& setType(const std::string& newType)`
// - `Bounds getBounds() const`
// - `SDL_Color getColor() const`
// - `IDisplayObject& setColor(const SDL_Color& color)`

// ---

// ### --- Priority & Z-Order ---
// - `int getMaxPriority() const`
// - `int getMinPriority() const`
// - `int getPriority() const`
// - `IDisplayObject& setToHighestPriority()`
// - `IDisplayObject& setToLowestPriority()`
// - `IDisplayObject& sortChildrenByPriority()`
// - `IDisplayObject& setPriority(int priority)`
// - `std::vector<int> getChildrenPriorities() const`
// - `IDisplayObject& moveToTop()`
// - `int getZOrder() const`
// - `IDisplayObject& setZOrder(int z_order)`

// ---

// ### --- Focus & Interactivity ---
// - `void setKeyboardFocus()`
// - `bool isKeyboardFocused() const`
// - `bool isMouseHovered() const`
// - `bool isClickable() const`
// - `IDisplayObject& setClickable(bool clickable)`
// - `bool isEnabled() const`
// - `IDisplayObject& setEnabled(bool enabled)`
// - `bool isHidden() const`
// - `IDisplayObject& setHidden(bool hidden)`
// - `bool isVisible() const`
// - `IDisplayObject& setVisible(bool visible)`

// ---

// ### --- Tab Management ---
// - `int getTabPriority() const`
// - `IDisplayObject& setTabPriority(int index)`
// - `bool isTabEnabled() const`
// - `IDisplayObject& setTabEnabled(bool enabled)`

// ---

// ### --- Geometry & Layout ---
// - `int getX() const`
// - `int getY() const`
// - `int getWidth() const`
// - `int getHeight() const`
// - `IDisplayObject& setX(int p_x)`
// - `IDisplayObject& setY(int p_y)`
// - `IDisplayObject& setWidth(int width)`
// - `IDisplayObject& setHeight(int height)`

// ---

// ### --- Edge Anchors ---
// - `AnchorPoint getAnchorTop() const`
// - `AnchorPoint getAnchorLeft() const`
// - `AnchorPoint getAnchorBottom() const`
// - `AnchorPoint getAnchorRight() const`
// - `void setAnchorTop(AnchorPoint ap)`
// - `void setAnchorLeft(AnchorPoint ap)`
// - `void setAnchorBottom(AnchorPoint ap)`
// - `void setAnchorRight(AnchorPoint ap)`

// ---

// ### --- Edge Positions ---
// - `float getLeft() const`
// - `float getRight() const`
// - `float getTop() const`
// - `float getBottom() const`
// - `IDisplayObject& setLeft(float p_left)`
// - `IDisplayObject& setRight(float p_right)`
// - `IDisplayObject& setTop(float p_top)`
// - `IDisplayObject& setBottom(float p_bottom)`

