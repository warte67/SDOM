// lua_IDisplayObject.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_EventType.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_DisplayObject.hpp>

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

    // Flexible variant: accepts either a table descriptor or (EventType, function, [useCapture], [priority])
    void addEventListener_lua_any(IDisplayObject* obj, const sol::object& descriptor, const sol::object& maybe_listener, const sol::object& maybe_useCapture, const sol::object& maybe_priority) {
        if (!obj) return;
        sol::state_view lua = descriptor.lua_state();

        EventType* et = nullptr;
        sol::function listener;
        bool useCapture = false;
        int priority = 0;

        // If descriptor is a table, parse fields: type, listener, useCapture, priority
        if (descriptor.is<sol::table>()) {
            sol::table t = descriptor.as<sol::table>();
            sol::object t_type = t.get<sol::object>("type");
            if (t_type.valid()) {
                // Try EventType userdata first
                try { et = t_type.as<EventType*>(); } catch(...) { et = nullptr; }
                if (!et && t_type.is<std::string>()) {
                    try {
                        std::string tn = t_type.as<std::string>();
                        const auto& reg = EventType::getRegistry();
                        auto it = reg.find(tn);
                        if (it != reg.end()) et = it->second;
                    } catch(...) { et = nullptr; }
                }
            }
            sol::object lobj = t.get<sol::object>("listener");
            if (lobj.valid() && lobj.is<sol::function>()) listener = lobj.as<sol::function>();
            sol::object uobj = t.get<sol::object>("useCapture");
            if (uobj.valid() && uobj.is<bool>()) useCapture = uobj.as<bool>();
            sol::object pobj = t.get<sol::object>("priority");
            if (pobj.valid() && pobj.is<int>()) priority = pobj.as<int>();
        } else {
            // Otherwise treat as (EventType, function, [useCapture], [priority])
            try { et = descriptor.as<EventType*>(); } catch(...) { et = nullptr; }
            try { if (maybe_listener.is<sol::function>()) listener = maybe_listener.as<sol::function>(); } catch(...) {}
            try { if (maybe_useCapture.is<bool>()) useCapture = maybe_useCapture.as<bool>(); } catch(...) {}
            try { if (maybe_priority.is<int>()) priority = maybe_priority.as<int>(); } catch(...) {}
        }

    if (!et) return;
    if (!listener) return;

        obj->addEventListener(*et, [listener](Event& e) mutable {
            sol::protected_function pf = listener;
            sol::protected_function_result r = pf(e);
            if (!r.valid()) {
                sol::error err = r;
                ERROR(std::string("Lua listener error (any): ") + err.what());
            }
        }, useCapture, priority);
    }

    void removeEventListener_lua_any(IDisplayObject* obj, const sol::object& descriptor, const sol::object& maybe_listener, const sol::object& maybe_useCapture) {
        if (!obj) return;
        sol::state_view lua = descriptor.lua_state();

        EventType* et = nullptr;
        sol::function listener;
        bool useCapture = false;

        if (descriptor.is<sol::table>()) {
            sol::table t = descriptor.as<sol::table>();
            sol::object t_type = t.get<sol::object>("type");
            if (t_type.valid()) {
                try { et = t_type.as<EventType*>(); } catch(...) { et = nullptr; }
                if (!et && t_type.is<std::string>()) {
                    try {
                        std::string tn = t_type.as<std::string>();
                        const auto& reg = EventType::getRegistry();
                        auto it = reg.find(tn);
                        if (it != reg.end()) et = it->second;
                    } catch(...) { et = nullptr; }
                }
            }
            sol::object lobj = t.get<sol::object>("listener");
            if (lobj.valid() && lobj.is<sol::function>()) listener = lobj.as<sol::function>();
            sol::object uobj = t.get<sol::object>("useCapture");
            if (uobj.valid() && uobj.is<bool>()) useCapture = uobj.as<bool>();
        } else {
            try { et = descriptor.as<EventType*>(); } catch(...) { et = nullptr; }
            try { if (maybe_listener.is<sol::function>()) listener = maybe_listener.as<sol::function>(); } catch(...) {}
            try { if (maybe_useCapture.is<bool>()) useCapture = maybe_useCapture.as<bool>(); } catch(...) {}
        }

        if (!et) return;
        if (!listener) return;

        obj->removeEventListener(*et, [listener](Event& e) mutable {
            sol::protected_function pf = listener;
            sol::protected_function_result r = pf(e);
            if (!r.valid()) {
                sol::error err = r;
                ERROR(std::string("Lua listener error (remove any): ") + err.what());
            }
        }, useCapture);
    }

    // Short wrappers that accept only a descriptor table (common pattern in Lua)
    void addEventListener_lua_any_short(IDisplayObject* obj, const sol::object& descriptor) {
        addEventListener_lua_any(obj, descriptor, sol::lua_nil, sol::lua_nil, sol::lua_nil);
    }

    void removeEventListener_lua_any_short(IDisplayObject* obj, const sol::object& descriptor) {
        removeEventListener_lua_any(obj, descriptor, sol::lua_nil, sol::lua_nil);
    }

    // --- Hierarchy Management --- //
    void addChild_lua(IDisplayObject* obj, DisplayObject child) { if (!obj) return; obj->addChild(child); }
    bool removeChild_lua(IDisplayObject* obj, DisplayObject child) { if (!obj) return false; return obj->removeChild(child); }
    bool hasChild_lua(const IDisplayObject* obj, DisplayObject child) { if (!obj) return false; return obj->hasChild(child); }
    DisplayObject getParent_lua(const IDisplayObject* obj) { if (!obj) return DisplayObject(); return obj->getParent(); }
    IDisplayObject* setParent_lua(IDisplayObject* obj, const DisplayObject& parent) { if (!obj) return nullptr; obj->setParent(parent); return obj; }

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
