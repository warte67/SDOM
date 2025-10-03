// lua_IDisplayObject.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_EventType.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_DomHandle.hpp>

namespace SDOM 
{

    // --- Dirty/State Management --- //
    void cleanAll_lua(IDisplayObject* obj) { if (!obj) return; obj->cleanAll(); }
    bool getDirty_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->getDirty(); }
    IDisplayObject* setDirty_lua(IDisplayObject* obj) { if (!obj) return nullptr; obj->setDirty(); return obj; }
    bool isDirty_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isDirty(); }

    // --- Debug/Utility --- //
    void printTree_lua(const IDisplayObject* obj) { if (!obj) return; obj->printTree_lua(); }

    // --- Event Handling --- //
    void addEventListener_lua(IDisplayObject* obj, EventType& type, sol::function listener, bool useCapture, int priority) {
        if (!obj) return;
        obj->addEventListener(type, [listener](Event& e) mutable {
            sol::protected_function pf = listener;
            sol::protected_function_result r = pf(e);
            if (!r.valid()) {
                sol::error err = r;
                ERROR(std::string("Lua listener error: ") + err.what());
            }
        }, useCapture, priority);
    }
    void removeEventListener_lua(IDisplayObject* obj, EventType& type, sol::function listener, bool useCapture) {
        if (!obj) return;
        // removal uses function pointer equality; preserve same wrapping behavior
        obj->removeEventListener(type, [listener](Event& e) mutable {
            sol::protected_function pf = listener;
            sol::protected_function_result r = pf(e);
            if (!r.valid()) {
                sol::error err = r;
                ERROR(std::string("Lua listener error (remove): ") + err.what());
            }
        }, useCapture);
    }

    // --- Hierarchy Management --- //
    void addChild_lua(IDisplayObject* obj, DomHandle child) { if (!obj) return; obj->addChild_lua(child); }
    bool removeChild_lua(IDisplayObject* obj, DomHandle child) { if (!obj) return false; return obj->removeChild(child); }
    bool hasChild_lua(const IDisplayObject* obj, DomHandle child) { if (!obj) return false; return obj->hasChild(child); }
    DomHandle getParent_lua(const IDisplayObject* obj) { if (!obj) return DomHandle(); return obj->getParent(); }
    IDisplayObject* setParent_lua(IDisplayObject* obj, const DomHandle& parent) { if (!obj) return nullptr; obj->setParent(parent); return obj; }

    // --- Type & Property Access --- //
    std::string getType_lua(const IDisplayObject* obj) { if (!obj) return std::string(); return obj->getType(); }
    IDisplayObject* setType_lua(IDisplayObject* obj, const std::string& newType) { if (!obj) return nullptr; obj->setType(newType); return obj; }
    Bounds getBounds_lua(const IDisplayObject* obj) { if (!obj) return Bounds(); return obj->getBounds(); }
    IDisplayObject* setBounds_lua(IDisplayObject* obj, const Bounds& b) { if (!obj) return nullptr; obj->setBounds(b); return obj; }
    SDL_Color getColor_lua(const IDisplayObject* obj) { if (!obj) return SDL_Color{0,0,0,0}; return obj->getColor(); }
    IDisplayObject* setColor_lua(IDisplayObject* obj, const SDL_Color& color) { if (!obj) return nullptr; obj->setColor(color); return obj; }

    // --- Priority & Z-Order --- //
    int getMaxPriority_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getMaxPriority(); }
    int getMinPriority_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getMinPriority(); }
    int getPriority_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getPriority(); }
    IDisplayObject* setToHighestPriority_lua(IDisplayObject* obj) { if (!obj) return nullptr; obj->setToHighestPriority(); return obj; }
    IDisplayObject* setToLowestPriority_lua(IDisplayObject* obj) { if (!obj) return nullptr; obj->setToLowestPriority(); return obj; }
    IDisplayObject* sortChildrenByPriority_lua(IDisplayObject* obj) { if (!obj) return nullptr; obj->sortChildrenByPriority(); return obj; }
    IDisplayObject* setPriority_lua(IDisplayObject* obj, int priority) { if (!obj) return nullptr; obj->setPriority(priority); return obj; }
    std::vector<int> getChildrenPriorities_lua(const IDisplayObject* obj) { if (!obj) return std::vector<int>(); return obj->getChildrenPriorities(); }
    IDisplayObject* moveToTop_lua(IDisplayObject* obj) { if (!obj) return nullptr; obj->moveToTop(); return obj; }
    int getZOrder_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getZOrder(); }
    IDisplayObject* setZOrder_lua(IDisplayObject* obj, int z_order) { if (!obj) return nullptr; obj->setZOrder(z_order); return obj; }

    // --- Focus & Interactivity --- //
    void setKeyboardFocus_lua(IDisplayObject* obj) { if (!obj) return; obj->setKeyboardFocus(); }
    bool isKeyboardFocused_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isKeyboardFocused(); }
    bool isMouseHovered_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isMouseHovered(); }
    bool isClickable_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isClickable(); }
    IDisplayObject* setClickable_lua(IDisplayObject* obj, bool clickable) { if (!obj) return nullptr; obj->setClickable(clickable); return obj; }
    bool isEnabled_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isEnabled(); }
    IDisplayObject* setEnabled_lua(IDisplayObject* obj, bool enabled) { if (!obj) return nullptr; obj->setEnabled(enabled); return obj; }
    bool isHidden_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isHidden(); }
    IDisplayObject* setHidden_lua(IDisplayObject* obj, bool hidden) { if (!obj) return nullptr; obj->setHidden(hidden); return obj; }
    bool isVisible_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isVisible(); }
    IDisplayObject* setVisible_lua(IDisplayObject* obj, bool visible) { if (!obj) return nullptr; obj->setVisible(visible); return obj; }

    // --- Tab Management --- //
    int getTabPriority_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getTabPriority(); }
    IDisplayObject* setTabPriority_lua(IDisplayObject* obj, int index) { if (!obj) return nullptr; obj->setTabPriority(index); return obj; }
    bool isTabEnabled_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isTabEnabled(); }
    IDisplayObject* setTabEnabled_lua(IDisplayObject* obj, bool enabled) { if (!obj) return nullptr; obj->setTabEnabled(enabled); return obj; }

    // --- Geometry & Layout --- //
    int getX_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getX(); }
    int getY_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getY(); }
    int getWidth_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getWidth(); }
    int getHeight_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getHeight(); }
    IDisplayObject* setX_lua(IDisplayObject* obj, int p_x) { if (!obj) return nullptr; obj->setX(p_x); return obj; }
    IDisplayObject* setY_lua(IDisplayObject* obj, int p_y) { if (!obj) return nullptr; obj->setY(p_y); return obj; }
    IDisplayObject* setWidth_lua(IDisplayObject* obj, int width) { if (!obj) return nullptr; obj->setWidth(width); return obj; }
    IDisplayObject* setHeight_lua(IDisplayObject* obj, int height) { if (!obj) return nullptr; obj->setHeight(height); return obj; }

    // --- Edge Anchors --- //
    AnchorPoint getAnchorTop_lua(const IDisplayObject* obj) { if (!obj) return AnchorPoint::TOP_LEFT; return obj->getAnchorTop(); }
    AnchorPoint getAnchorLeft_lua(const IDisplayObject* obj) { if (!obj) return AnchorPoint::TOP_LEFT; return obj->getAnchorLeft(); }
    AnchorPoint getAnchorBottom_lua(const IDisplayObject* obj) { if (!obj) return AnchorPoint::TOP_LEFT; return obj->getAnchorBottom(); }
    AnchorPoint getAnchorRight_lua(const IDisplayObject* obj) { if (!obj) return AnchorPoint::TOP_LEFT; return obj->getAnchorRight(); }
    void setAnchorTop_lua(IDisplayObject* obj, AnchorPoint ap) { if (!obj) return; obj->setAnchorTop(ap); }
    void setAnchorLeft_lua(IDisplayObject* obj, AnchorPoint ap) { if (!obj) return; obj->setAnchorLeft(ap); }
    void setAnchorBottom_lua(IDisplayObject* obj, AnchorPoint ap) { if (!obj) return; obj->setAnchorBottom(ap); }
    void setAnchorRight_lua(IDisplayObject* obj, AnchorPoint ap) { if (!obj) return; obj->setAnchorRight(ap); }

    // --- Edge Positions --- //
    float getLeft_lua(const IDisplayObject* obj) { if (!obj) return 0.0f; return obj->getLeft(); }
    float getRight_lua(const IDisplayObject* obj) { if (!obj) return 0.0f; return obj->getRight(); }
    float getTop_lua(const IDisplayObject* obj) { if (!obj) return 0.0f; return obj->getTop(); }
    float getBottom_lua(const IDisplayObject* obj) { if (!obj) return 0.0f; return obj->getBottom(); }
    IDisplayObject* setLeft_lua(IDisplayObject* obj, float p_left) { if (!obj) return nullptr; obj->setLeft(p_left); return obj; }
    IDisplayObject* setRight_lua(IDisplayObject* obj, float p_right) { if (!obj) return nullptr; obj->setRight(p_right); return obj; }
    IDisplayObject* setTop_lua(IDisplayObject* obj, float p_top) { if (!obj) return nullptr; obj->setTop(p_top); return obj; }
    IDisplayObject* setBottom_lua(IDisplayObject* obj, float p_bottom) { if (!obj) return nullptr; obj->setBottom(p_bottom); return obj; }

} // namespace SDOM
