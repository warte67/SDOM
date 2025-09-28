// lua_IDisplayObject.cpp
#include <SDL3/SDL.h>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_EventType.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_DomHandle.hpp>
#include <vector>
#include <string>

namespace SDOM 
{

    // --- Dirty/State Management --- //
    void cleanAll_lua(IDisplayObject& obj) { obj.cleanAll(); }
    bool getDirty_lua(const IDisplayObject& obj) { return obj.getDirty(); }
    IDisplayObject& setDirty_lua(IDisplayObject& obj) { return obj.setDirty(); }
    bool isDirty_lua(const IDisplayObject& obj) { return obj.isDirty(); }

    // --- Debug/Utility --- //
    void printTree_lua(const IDisplayObject& obj) { obj.printTree_lua(); }

    // --- Event Handling --- //
    void addEventListener_lua(IDisplayObject& obj, EventType& type, sol::function listener, bool useCapture, int priority) {
        obj.addEventListener(type, [listener](Event& e) { listener(e); }, useCapture, priority);
    }
    void removeEventListener_lua(IDisplayObject& obj, EventType& type, sol::function listener, bool useCapture) {
        obj.removeEventListener(type, [listener](Event& e) { listener(e); }, useCapture);
    }

    // --- Hierarchy Management --- //
    void addChild_lua(IDisplayObject& obj, DomHandle child) { obj.addChild_lua(child); }
    bool removeChild_lua(IDisplayObject& obj, DomHandle child) { return obj.removeChild(child); }
    bool hasChild_lua(const IDisplayObject& obj, DomHandle child) { return obj.hasChild(child); }
    DomHandle getParent_lua(const IDisplayObject& obj) { return obj.getParent(); }
    IDisplayObject& setParent_lua(IDisplayObject& obj, const DomHandle& parent) { return obj.setParent(parent); }

    // --- Type & Property Access --- //
    std::string getType_lua(const IDisplayObject& obj) { return obj.getType(); }
    IDisplayObject& setType_lua(IDisplayObject& obj, const std::string& newType) { return obj.setType(newType); }
    Bounds getBounds_lua(const IDisplayObject& obj) { return obj.getBounds(); }
    SDL_Color getColor_lua(const IDisplayObject& obj) { return obj.getColor(); }
    IDisplayObject& setColor_lua(IDisplayObject& obj, const SDL_Color& color) { return obj.setColor(color); }

    // --- Priority & Z-Order --- //
    int getMaxPriority_lua(const IDisplayObject& obj) { return obj.getMaxPriority(); }
    int getMinPriority_lua(const IDisplayObject& obj) { return obj.getMinPriority(); }
    int getPriority_lua(const IDisplayObject& obj) { return obj.getPriority(); }
    IDisplayObject& setToHighestPriority_lua(IDisplayObject& obj) { return obj.setToHighestPriority(); }
    IDisplayObject& setToLowestPriority_lua(IDisplayObject& obj) { return obj.setToLowestPriority(); }
    IDisplayObject& sortChildrenByPriority_lua(IDisplayObject& obj) { return obj.sortChildrenByPriority(); }
    IDisplayObject& setPriority_lua(IDisplayObject& obj, int priority) { return obj.setPriority(priority); }
    std::vector<int> getChildrenPriorities_lua(const IDisplayObject& obj) { return obj.getChildrenPriorities(); }
    IDisplayObject& moveToTop_lua(IDisplayObject& obj) { return obj.moveToTop(); }
    int getZOrder_lua(const IDisplayObject& obj) { return obj.getZOrder(); }
    IDisplayObject& setZOrder_lua(IDisplayObject& obj, int z_order) { return obj.setZOrder(z_order); }

    // --- Focus & Interactivity --- //
    void setKeyboardFocus_lua(IDisplayObject& obj) { obj.setKeyboardFocus(); }
    bool isKeyboardFocused_lua(const IDisplayObject& obj) { return obj.isKeyboardFocused(); }
    bool isMouseHovered_lua(const IDisplayObject& obj) { return obj.isMouseHovered(); }
    bool isClickable_lua(const IDisplayObject& obj) { return obj.isClickable(); }
    IDisplayObject& setClickable_lua(IDisplayObject& obj, bool clickable) { return obj.setClickable(clickable); }
    bool isEnabled_lua(const IDisplayObject& obj) { return obj.isEnabled(); }
    IDisplayObject& setEnabled_lua(IDisplayObject& obj, bool enabled) { return obj.setEnabled(enabled); }
    bool isHidden_lua(const IDisplayObject& obj) { return obj.isHidden(); }
    IDisplayObject& setHidden_lua(IDisplayObject& obj, bool hidden) { return obj.setHidden(hidden); }
    bool isVisible_lua(const IDisplayObject& obj) { return obj.isVisible(); }
    IDisplayObject& setVisible_lua(IDisplayObject& obj, bool visible) { return obj.setVisible(visible); }

    // --- Tab Management --- //
    int getTabPriority_lua(const IDisplayObject& obj) { return obj.getTabPriority(); }
    IDisplayObject& setTabPriority_lua(IDisplayObject& obj, int index) { return obj.setTabPriority(index); }
    bool isTabEnabled_lua(const IDisplayObject& obj) { return obj.isTabEnabled(); }
    IDisplayObject& setTabEnabled_lua(IDisplayObject& obj, bool enabled) { return obj.setTabEnabled(enabled); }

    // --- Geometry & Layout --- //
    int getX_lua(const IDisplayObject& obj) { return obj.getX(); }
    int getY_lua(const IDisplayObject& obj) { return obj.getY(); }
    int getWidth_lua(const IDisplayObject& obj) { return obj.getWidth(); }
    int getHeight_lua(const IDisplayObject& obj) { return obj.getHeight(); }
    IDisplayObject& setX_lua(IDisplayObject& obj, int p_x) { return obj.setX(p_x); }
    IDisplayObject& setY_lua(IDisplayObject& obj, int p_y) { return obj.setY(p_y); }
    IDisplayObject& setWidth_lua(IDisplayObject& obj, int width) { return obj.setWidth(width); }
    IDisplayObject& setHeight_lua(IDisplayObject& obj, int height) { return obj.setHeight(height); }

    // --- Edge Anchors --- //
    AnchorPoint getAnchorTop_lua(const IDisplayObject& obj) { return obj.getAnchorTop(); }
    AnchorPoint getAnchorLeft_lua(const IDisplayObject& obj) { return obj.getAnchorLeft(); }
    AnchorPoint getAnchorBottom_lua(const IDisplayObject& obj) { return obj.getAnchorBottom(); }
    AnchorPoint getAnchorRight_lua(const IDisplayObject& obj) { return obj.getAnchorRight(); }
    void setAnchorTop_lua(IDisplayObject& obj, AnchorPoint ap) { obj.setAnchorTop(ap); }
    void setAnchorLeft_lua(IDisplayObject& obj, AnchorPoint ap) { obj.setAnchorLeft(ap); }
    void setAnchorBottom_lua(IDisplayObject& obj, AnchorPoint ap) { obj.setAnchorBottom(ap); }
    void setAnchorRight_lua(IDisplayObject& obj, AnchorPoint ap) { obj.setAnchorRight(ap); }

    // --- Edge Positions --- //
    float getLeft_lua(const IDisplayObject& obj) { return obj.getLeft(); }
    float getRight_lua(const IDisplayObject& obj) { return obj.getRight(); }
    float getTop_lua(const IDisplayObject& obj) { return obj.getTop(); }
    float getBottom_lua(const IDisplayObject& obj) { return obj.getBottom(); }
    IDisplayObject& setLeft_lua(IDisplayObject& obj, float p_left) { return obj.setLeft(p_left); }
    IDisplayObject& setRight_lua(IDisplayObject& obj, float p_right) { return obj.setRight(p_right); }
    IDisplayObject& setTop_lua(IDisplayObject& obj, float p_top) { return obj.setTop(p_top); }
    IDisplayObject& setBottom_lua(IDisplayObject& obj, float p_bottom) { return obj.setBottom(p_bottom); }

} // namespace SDOM
