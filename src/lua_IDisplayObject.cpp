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
    void setDirty_lua(IDisplayObject* obj) { if (!obj) return; obj->setDirty(); }
    bool isDirty_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isDirty(); }

    // --- Debug/Utility --- //
    void printTree_lua(const IDisplayObject* obj) { if (!obj) return; obj->printTree(); }

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

    void removeEventListener_lua_any(IDisplayObject* obj, const sol::object& descriptor, const sol::object& maybe_listener, const sol::object& maybe_useCapture) 
    {
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
    DisplayObject getChild_lua(const IDisplayObject* obj, std::string name) { if (!obj) return DisplayObject(); return obj->getChild(name); }
    bool removeChild_lua(IDisplayObject* obj, DisplayObject child) { if (!obj) return false; return obj->removeChild(child); }
    bool removeChild_lua(IDisplayObject* obj, const std::string& name) { if (!obj) return false; return obj->removeChild(name); }
    bool hasChild_lua(const IDisplayObject* obj, DisplayObject child) { if (!obj) return false; return obj->hasChild(child); }
    DisplayObject getParent_lua(const IDisplayObject* obj) { if (!obj) return DisplayObject(); return obj->getParent(); }
    void setParent_lua(IDisplayObject* obj, const DisplayObject& parent) { if (!obj) return; obj->setParent(parent); }

    // Ancestor/Descendant helpers (Lua wrappers)
    bool isAncestorOf_lua(IDisplayObject* obj, DisplayObject descendant) { if (!obj) return false; return obj->isAncestorOf(descendant); }
    bool isAncestorOf_lua(IDisplayObject* obj, const std::string& name) { if (!obj) return false; return obj->isAncestorOf(name); }
    bool isDescendantOf_lua(IDisplayObject* obj, DisplayObject ancestor) { if (!obj) return false; return obj->isDescendantOf(ancestor); }
    bool isDescendantOf_lua(IDisplayObject* obj, const std::string& name) { if (!obj) return false; return obj->isDescendantOf(name); }
    bool removeFromParent_lua(IDisplayObject* obj) { if (!obj) return false; return obj->removeFromParent(); }
    bool removeDescendant_lua(IDisplayObject* obj, DisplayObject descendant) { if (!obj) return false; return obj->removeDescendant(descendant); }
    bool removeDescendant_lua(IDisplayObject* obj, const std::string& descendantName) { if (!obj) return false; return obj->removeDescendant(descendantName); }

    // --- Type & Property Access --- //
    std::string getName_lua(const IDisplayObject* obj) { if (!obj) return std::string(); return obj->getName(); }
    void setName_lua(IDisplayObject* obj, const std::string& newName) { if (!obj) return; obj->setName(newName); }
    std::string getType_lua(const IDisplayObject* obj) { if (!obj) return std::string(); return obj->getType(); }
    void setType_lua(IDisplayObject* obj, const std::string& newType) { if (!obj) return; obj->setType(newType); }
    Bounds getBounds_lua(const IDisplayObject* obj) { if (!obj) return Bounds(); return obj->getBounds(); }
    
    // Accept either a Bounds userdata or a Lua table describing bounds
    void setBounds_lua(IDisplayObject* obj, const sol::object& bobj) 
    {
        if (!obj) return;
        Bounds b{};
        if (bobj.is<Bounds>()) {
            try { b = bobj.as<Bounds>(); } catch(...) { return; }
        } else if (bobj.is<sol::table>()) {
            sol::table t = bobj.as<sol::table>();
            // Support either left/top/right/bottom or x/y/width/height
            if (t.get<sol::object>("left").valid() && t.get<sol::object>("top").valid()
                && t.get<sol::object>("right").valid() && t.get<sol::object>("bottom").valid()) {
                b.left = t.get<float>("left");
                b.top = t.get<float>("top");
                b.right = t.get<float>("right");
                b.bottom = t.get<float>("bottom");
            } else if (t.get<sol::object>("x").valid() && t.get<sol::object>("y").valid()
                       && t.get<sol::object>("width").valid() && t.get<sol::object>("height").valid()) {
                float x = t.get<float>("x");
                float y = t.get<float>("y");
                float w = t.get<float>("width");
                float h = t.get<float>("height");
                b.left = x;
                b.top = y;
                b.right = x + w;
                b.bottom = y + h;
            } else {
                // fallback: try numeric indices [1..4]
                try {
                    b.left = t.get<float>(1);
                    b.top = t.get<float>(2);
                    b.right = t.get<float>(3);
                    b.bottom = t.get<float>(4);
                } catch(...) { return; }
            }
        } else {
            return;
        }
        obj->setBounds(b);
        return;
    }
    // C++ overload: accept a Bounds directly
    void setBounds_lua(IDisplayObject* obj, const Bounds& b) { if (!obj) return; obj->setBounds(b); }
    SDL_Color getColor_lua(const IDisplayObject* obj) { if (!obj) return SDL_Color{0,0,0,0}; return obj->getColor(); }
    // Accept either SDL_Color userdata or Lua table { r=..., g=..., b=..., a=... }
    void setColor_lua(IDisplayObject* obj, const sol::object& colorObj) 
    {
        if (!obj) return;
        SDL_Color c{0,0,0,255};
        if (colorObj.is<SDL_Color>()) {
            try { c = colorObj.as<SDL_Color>(); } catch(...) { return; }
        } else if (colorObj.is<sol::table>()) {
            sol::table t = colorObj.as<sol::table>();
            sol::object ro = t.get<sol::object>("r");
            sol::object go = t.get<sol::object>("g");
            sol::object bo = t.get<sol::object>("b");
            sol::object ao = t.get<sol::object>("a");
            auto to_int = [](const sol::object& o, int def) -> int {
                if (!o.valid()) return def;
                try { if (o.is<int>()) return o.as<int>(); } catch(...) {}
                try { if (o.is<long>()) return static_cast<int>(o.as<long>()); } catch(...) {}
                try { if (o.is<double>()) return static_cast<int>(o.as<double>()); } catch(...) {}
                return def;
            };
            c.r = static_cast<Uint8>(to_int(ro, 0));
            c.g = static_cast<Uint8>(to_int(go, 0));
            c.b = static_cast<Uint8>(to_int(bo, 0));
            c.a = static_cast<Uint8>(to_int(ao, 255));
        } else {
            return;
        }
        obj->setColor(c);
        return;
    }
    // C++ overload: accept SDL_Color directly
    void setColor_lua(IDisplayObject* obj, const SDL_Color& color) { if (!obj) return; obj->setColor(color); }

    // --- Priority & Z-Order --- //
    int getMaxPriority_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getMaxPriority(); }
    int getMinPriority_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getMinPriority(); }
    int getPriority_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getPriority(); }
    void setToHighestPriority_lua(IDisplayObject* obj) { if (!obj) return; obj->setToHighestPriority(); }
    void setToLowestPriority_lua(IDisplayObject* obj) { if (!obj) return; obj->setToLowestPriority(); }
    void sortChildrenByPriority_lua(IDisplayObject* obj) { if (!obj) return; obj->sortChildrenByPriority(); }
    void setPriority_lua(IDisplayObject* obj, int priority) 
    {
        if (!obj) return;
        obj->setPriority(priority);
        return;
    }

    std::vector<int> getChildrenPriorities_lua(const IDisplayObject* obj) 
    {
        if (!obj) return std::vector<int>();
        auto v = obj->getChildrenPriorities();
        return v;
    }
    void moveToTop_lua(IDisplayObject* obj) 
    {
        if (!obj) return;
        obj->moveToTop();
        return;
    }

    int getZOrder_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getZOrder(); }
    void setZOrder_lua(IDisplayObject* obj, int z_order) { if (!obj) return; obj->setZOrder(z_order); }

    // --- Focus & Interactivity --- //
    void setKeyboardFocus_lua(IDisplayObject* obj) { if (!obj) return; obj->setKeyboardFocus(); }
    bool isKeyboardFocused_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isKeyboardFocused(); }
    bool isMouseHovered_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isMouseHovered(); }
    bool isClickable_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isClickable(); }
    void setClickable_lua(IDisplayObject* obj, bool clickable) { if (!obj) return; obj->setClickable(clickable); }
    bool isEnabled_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isEnabled(); }
    void setEnabled_lua(IDisplayObject* obj, bool enabled) { if (!obj) return; obj->setEnabled(enabled); }
    bool isHidden_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isHidden(); }
    void setHidden_lua(IDisplayObject* obj, bool hidden) { if (!obj) return; obj->setHidden(hidden); }
    bool isVisible_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isVisible(); }
    void setVisible_lua(IDisplayObject* obj, bool visible) { if (!obj) return; obj->setVisible(visible); }

    // --- Tab Management --- //
    int getTabPriority_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getTabPriority(); }
    void setTabPriority_lua(IDisplayObject* obj, int index) { if (!obj) return; obj->setTabPriority(index); }
    bool isTabEnabled_lua(const IDisplayObject* obj) { if (!obj) return false; return obj->isTabEnabled(); }
    void setTabEnabled_lua(IDisplayObject* obj, bool enabled) { if (!obj) return; obj->setTabEnabled(enabled); }

    // --- Geometry & Layout --- //
    int getX_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getX(); }
    int getY_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getY(); }
    int getWidth_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getWidth(); }
    int getHeight_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getHeight(); }
    void setX_lua(IDisplayObject* obj, int p_x) { if (!obj) return; obj->setX(p_x); }
    void setY_lua(IDisplayObject* obj, int p_y) { if (!obj) return; obj->setY(p_y); }
    void setWidth_lua(IDisplayObject* obj, int width) { if (!obj) return; obj->setWidth(width); }
    void setHeight_lua(IDisplayObject* obj, int height) { if (!obj) return; obj->setHeight(height); }

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
    void setLeft_lua(IDisplayObject* obj, float p_left) { if (!obj) return; obj->setLeft(p_left); }
    void setRight_lua(IDisplayObject* obj, float p_right) { if (!obj) return; obj->setRight(p_right); }
    void setTop_lua(IDisplayObject* obj, float p_top) { if (!obj) return; obj->setTop(p_top); }
    void setBottom_lua(IDisplayObject* obj, float p_bottom) { if (!obj) return; obj->setBottom(p_bottom); }

    // --- Orphan Retention Policy --- //
    IDisplayObject::OrphanRetentionPolicy orphanPolicyFromString_lua(IDisplayObject* /*obj*/, const std::string& s)
    {
        std::string key = s;
        std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });

        if (key == "auto" || key == "autodestroy" || key == "immediate") {
            return IDisplayObject::OrphanRetentionPolicy::AutoDestroy;
        }
        if (key == "grace" || key == "graceperiod" || key == "grace_period") {
            return IDisplayObject::OrphanRetentionPolicy::GracePeriod;
        }
        // default / fallback
        return IDisplayObject::OrphanRetentionPolicy::RetainUntilManual;
    }

    std::string orphanPolicyToString_lua(IDisplayObject* /*obj*/, IDisplayObject::OrphanRetentionPolicy p)
    {
        switch (p) {
            case IDisplayObject::OrphanRetentionPolicy::AutoDestroy:   return std::string("auto");
            case IDisplayObject::OrphanRetentionPolicy::GracePeriod:  return std::string("grace");
            case IDisplayObject::OrphanRetentionPolicy::RetainUntilManual:
            default:                                                   return std::string("manual");
        }
    }

    void setOrphanRetentionPolicy_lua(IDisplayObject* obj, const std::string& policyStr)
    {
        if (!obj) return;
        auto p = orphanPolicyFromString_lua(obj, policyStr);
        obj->setOrphanRetentionPolicy(p);
        return;
    }

    std::string getOrphanRetentionPolicyString_lua(IDisplayObject* obj)
    {
        if (!obj) return std::string();
        auto p = obj->getOrphanRetentionPolicy();
        return orphanPolicyToString_lua(obj, p);
    }
} // namespace SDOM

