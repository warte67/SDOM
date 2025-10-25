// SDOM_IDisplayObject_Lua.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_EventType.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>

#include <SDOM/SDOM_IDisplayObject_Lua.hpp>

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
    bool hasEventListener_lua(const IDisplayObject* obj, const EventType& type, bool useCapture)
    {
        if (!obj) return false;
        return obj->hasEventListener(type, useCapture);
    }
    void queue_event_lua(IDisplayObject* obj, const EventType& type, std::function<void(Event&)> init_payload)
    {
        if (!obj) return;
        obj->queue_event(type, init_payload);
    }


    // --- Hierarchy Management --- //
    void addChild_lua(IDisplayObject* obj, DisplayHandle child) { if (!obj) return; obj->addChild(child); }
    DisplayHandle getChild_lua(const IDisplayObject* obj, std::string name) { if (!obj) return DisplayHandle(); return obj->getChild(name); }
    bool removeChild_lua(IDisplayObject* obj, DisplayHandle child) { if (!obj) return false; return obj->removeChild(child); }
    bool removeChild_lua(IDisplayObject* obj, const std::string& name) { if (!obj) return false; return obj->removeChild(name); }
    bool hasChild_lua(const IDisplayObject* obj, DisplayHandle child) { if (!obj) return false; return obj->hasChild(child); }
    DisplayHandle getParent_lua(const IDisplayObject* obj) { if (!obj) return DisplayHandle(); return obj->getParent(); }
    void setParent_lua(IDisplayObject* obj, const DisplayHandle& parent) { if (!obj) return; obj->setParent(parent); }

    // Ancestor/Descendant helpers (Lua wrappers)
    bool isAncestorOf_lua(IDisplayObject* obj, DisplayHandle descendant) { if (!obj) return false; return obj->isAncestorOf(descendant); }
    bool isAncestorOf_lua(IDisplayObject* obj, const std::string& name) { if (!obj) return false; return obj->isAncestorOf(name); }
    bool isDescendantOf_lua(IDisplayObject* obj, DisplayHandle ancestor) { if (!obj) return false; return obj->isDescendantOf(ancestor); }
    bool isDescendantOf_lua(IDisplayObject* obj, const std::string& name) { if (!obj) return false; return obj->isDescendantOf(name); }
    bool removeFromParent_lua(IDisplayObject* obj) { if (!obj) return false; return obj->removeFromParent(); }
    bool removeDescendant_lua(IDisplayObject* obj, DisplayHandle descendant) { if (!obj) return false; return obj->removeDescendant(descendant); }
    bool removeDescendant_lua(IDisplayObject* obj, const std::string& descendantName) { if (!obj) return false; return obj->removeDescendant(descendantName); }

    // --- Type & Property Access --- //
    std::string getName_lua(const IDisplayObject* obj) { if (!obj) return std::string(); return obj->getName(); }
    void setName_lua(IDisplayObject* obj, const std::string& newName) { if (!obj) return; obj->setName(newName); }
    std::string getType_lua(const IDisplayObject* obj) { if (!obj) return std::string(); return obj->getType(); }
    Bounds getBounds_lua(const IDisplayObject* obj) { 
        if (!obj) {
            // Return a zero-initialized Bounds to avoid leaking uninitialized memory
            return Bounds{0.0f, 0.0f, 0.0f, 0.0f};
        }
        Bounds b = obj->getBounds(); 
        // If any component is NaN or infinite, fall back to safer getters
        bool bad = false;
        try {
            if (std::isnan(b.left) || std::isnan(b.top) || std::isnan(b.right) || std::isnan(b.bottom)) bad = true;
            if (std::isinf(b.left) || std::isinf(b.top) || std::isinf(b.right) || std::isinf(b.bottom)) bad = true;
        } catch(...) { bad = true; }
        if (bad) {
            try {
                int x = obj->getX();
                int y = obj->getY();
                int w = obj->getWidth();
                int h = obj->getHeight();
                b.left = static_cast<float>(x);
                b.top = static_cast<float>(y);
                b.right = static_cast<float>(x + w);
                b.bottom = static_cast<float>(y + h);
            } catch(...) {
                // last resort: zero-out
                b.left = b.top = b.right = b.bottom = 0.0f;
            }
        }
        try {
            // Focused debug: log bounds for the particular object that intermittently fails tests
            if (obj->getName() == "blueishBox") {
                std::ostringstream oss;
                oss << "getBounds_lua: blueishBox bounds -> left=" << b.left << " top=" << b.top
                    << " right=" << b.right << " bottom=" << b.bottom;
                LUA_INFO(oss.str());
            }
        } catch(...) {}
        return b; }
    
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

    // --- Per-color getters/setters ---
    SDL_Color getForegroundColor_lua(const IDisplayObject* obj) { if (!obj) return SDL_Color{0,0,0,0}; return obj->getForegroundColor(); }
    void setForegroundColor_lua(IDisplayObject* obj, const sol::object& colorObj)
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
        obj->setForegroundColor(c);
        return;
    }
    void setForegroundColor_lua(IDisplayObject* obj, const SDL_Color& color) { if (!obj) return; obj->setForegroundColor(color); }

    SDL_Color getBackgroundColor_lua(const IDisplayObject* obj) { if (!obj) return SDL_Color{0,0,0,0}; return obj->getBackgroundColor(); }
    void setBackgroundColor_lua(IDisplayObject* obj, const sol::object& colorObj)
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
        obj->setBackgroundColor(c);
        return;
    }
    void setBackgroundColor_lua(IDisplayObject* obj, const SDL_Color& color) { if (!obj) return; obj->setBackgroundColor(color); }

    SDL_Color getBorderColor_lua(const IDisplayObject* obj) { if (!obj) return SDL_Color{0,0,0,0}; return obj->getBorderColor(); }
    void setBorderColor_lua(IDisplayObject* obj, const sol::object& colorObj)
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
        obj->setBorderColor(c);
        return;
    }
    void setBorderColor_lua(IDisplayObject* obj, const SDL_Color& color) { if (!obj) return; obj->setBorderColor(color); }

    SDL_Color getOutlineColor_lua(const IDisplayObject* obj) { if (!obj) return SDL_Color{0,0,0,0}; return obj->getOutlineColor(); }
    void setOutlineColor_lua(IDisplayObject* obj, const sol::object& colorObj)
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
        obj->setOutlineColor(c);
        return;
    }
    void setOutlineColor_lua(IDisplayObject* obj, const SDL_Color& color) { if (!obj) return; obj->setOutlineColor(color); }

    SDL_Color getDropshadowColor_lua(const IDisplayObject* obj) { if (!obj) return SDL_Color{0,0,0,0}; return obj->getDropshadowColor(); }
    void setDropshadowColor_lua(IDisplayObject* obj, const sol::object& colorObj)
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
        obj->setDropshadowColor(c);
        return;
    }
    void setDropshadowColor_lua(IDisplayObject* obj, const SDL_Color& color) { if (!obj) return; obj->setDropshadowColor(color); }


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

    // Flexible priority setter: accepts a descriptor table and optional numeric value
    // Usage examples:
    //  - child:setPriority({ priority = 7 })
    //  - parent:setPriority({ child = 'name' }, 9)
    //  - parent:setPriority({ child = DisplayObjectHandle }, 9)
    void setPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor)
    {
        if (!obj) return;
        if (!descriptor.valid()) return;
        if (descriptor.is<sol::table>()) {
            sol::table t = descriptor.as<sol::table>();
            // If 'priority' specified, set on self
            sol::object pobj = t.get<sol::object>("priority");
            if (pobj.valid()) {
                try {
                    int pr = 0;
                    if (pobj.is<int>()) pr = pobj.as<int>();
                    else if (pobj.is<double>()) pr = static_cast<int>(pobj.as<double>());
                    obj->setPriority(pr);
                } catch(...) { /* ignore */ }
                return;
            }
            // If 'child' provided without explicit value, do nothing here; two-arg overload handles targeted forms
        }
    }

    void setPriority_lua_target(IDisplayObject* obj, const sol::object& descriptor, int value)
    {
        if (!obj) return;
        if (!descriptor.valid()) return;
        IDisplayObject* target = nullptr;
        if (descriptor.is<sol::table>()) {
            sol::table t = descriptor.as<sol::table>();
            sol::object cobj = t.get<sol::object>("child");
            if (cobj.valid()) {
                try {
                    if (cobj.is<DisplayHandle>()) {
                        DisplayHandle h = cobj.as<DisplayHandle>();
                        target = dynamic_cast<IDisplayObject*>(h.get());
                    } else if (cobj.is<std::string>()) {
                        std::string name = cobj.as<std::string>();
                        DisplayHandle h = obj->getChild(name);
                        target = dynamic_cast<IDisplayObject*>(h.get());
                    }
                } catch(...) { target = nullptr; }
            }
        }
        if (!target) return;
        target->setPriority(value);
        obj->sortChildrenByPriority();
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

    // Targeted moveToTop via parent: parent:moveToTop({ child = <name|handle> })
    void moveToTop_lua_any(IDisplayObject* obj, const sol::object& descriptor)
    {
        if (!obj) return;
        if (!descriptor.valid() || !descriptor.is<sol::table>()) return;
        sol::table t = descriptor.as<sol::table>();
        IDisplayObject* target = nullptr;
        sol::object cobj = t.get<sol::object>("child");
        if (cobj.valid()) {
            try {
                if (cobj.is<DisplayHandle>()) {
                    DisplayHandle h = cobj.as<DisplayHandle>();
                    target = dynamic_cast<IDisplayObject*>(h.get());
                } else if (cobj.is<std::string>()) {
                    std::string name = cobj.as<std::string>();
                    DisplayHandle h = obj->getChild(name);
                    target = dynamic_cast<IDisplayObject*>(h.get());
                }
            } catch(...) { target = nullptr; }
        }
        if (!target) return;
        target->moveToTop();
        obj->sortChildrenByPriority();
    }

    int getZOrder_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getZOrder(); }
    void setZOrder_lua(IDisplayObject* obj, int z_order) { if (!obj) return; obj->setZOrder(z_order); }

    bool getBorder_lua(const IDisplayObject* obj) { return obj ? obj->getBorder() : false; }
    bool getBackground_lua(const IDisplayObject* obj) { return obj ? obj->getBackground() : false; }
    void setBorder_lua(IDisplayObject* obj, bool hasBorder) { if (obj) obj->setBorder(hasBorder); }
    void setBackground_lua(IDisplayObject* obj, bool hasBackground) { if (obj) obj->setBackground(hasBackground); }

    // Flexible setZOrder: supports { z = number } on self or { child = <name|handle>, z = number } on parent
    void setZOrder_lua_any(IDisplayObject* obj, const sol::object& descriptor)
    {
        if (!obj) return;
        if (!descriptor.valid() || !descriptor.is<sol::table>()) return;
        sol::table t = descriptor.as<sol::table>();
        int z = 0;
        try {
            sol::object zobj = t.get<sol::object>("z");
            if (zobj.valid()) {
                if (zobj.is<int>()) z = zobj.as<int>();
                else if (zobj.is<double>()) z = static_cast<int>(zobj.as<double>());
            }
        } catch(...) { /* ignore */ }

        // If child specified, target that child; otherwise set on self
        IDisplayObject* target = nullptr;
        sol::object cobj = t.get<sol::object>("child");
        if (cobj.valid()) {
            try {
                if (cobj.is<DisplayHandle>()) {
                    DisplayHandle h = cobj.as<DisplayHandle>();
                    target = dynamic_cast<IDisplayObject*>(h.get());
                } else if (cobj.is<std::string>()) {
                    std::string name = cobj.as<std::string>();
                    DisplayHandle h = obj->getChild(name);
                    target = dynamic_cast<IDisplayObject*>(h.get());
                }
            } catch(...) { target = nullptr; }
        } else {
            target = obj;
        }
        if (!target) return;
        target->setZOrder(z);
    }

    // Descriptor forms for highest/lowest via parent: parent:setToHighestPriority({ child = ... })
    void setToHighestPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor)
    {
        if (!obj) return;
        if (!descriptor.valid() || !descriptor.is<sol::table>()) return;
        sol::table t = descriptor.as<sol::table>();
        IDisplayObject* target = nullptr;
        sol::object cobj = t.get<sol::object>("child");
        if (cobj.valid()) {
            try {
                if (cobj.is<DisplayHandle>()) {
                    DisplayHandle h = cobj.as<DisplayHandle>();
                    target = dynamic_cast<IDisplayObject*>(h.get());
                } else if (cobj.is<std::string>()) {
                    std::string name = cobj.as<std::string>();
                    DisplayHandle h = obj->getChild(name);
                    target = dynamic_cast<IDisplayObject*>(h.get());
                }
            } catch(...) { target = nullptr; }
        }
        if (!target) return;
        target->setToHighestPriority();
        obj->sortChildrenByPriority();
    }

    void setToLowestPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor)
    {
        if (!obj) return;
        if (!descriptor.valid() || !descriptor.is<sol::table>()) return;
        sol::table t = descriptor.as<sol::table>();
        IDisplayObject* target = nullptr;
        sol::object cobj = t.get<sol::object>("child");
        if (cobj.valid()) {
            try {
                if (cobj.is<DisplayHandle>()) {
                    DisplayHandle h = cobj.as<DisplayHandle>();
                    target = dynamic_cast<IDisplayObject*>(h.get());
                } else if (cobj.is<std::string>()) {
                    std::string name = cobj.as<std::string>();
                    DisplayHandle h = obj->getChild(name);
                    target = dynamic_cast<IDisplayObject*>(h.get());
                }
            } catch(...) { target = nullptr; }
        }
        if (!target) return;
        target->setToLowestPriority();
        obj->sortChildrenByPriority();
    }

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

    // --- World Edge Positions --- //
    float getLeft_lua(const IDisplayObject* obj) { if (!obj) return 0.0f; return obj->getLeft(); }
    float getRight_lua(const IDisplayObject* obj) { if (!obj) return 0.0f; return obj->getRight(); }
    float getTop_lua(const IDisplayObject* obj) { if (!obj) return 0.0f; return obj->getTop(); }
    float getBottom_lua(const IDisplayObject* obj) { if (!obj) return 0.0f; return obj->getBottom(); }
    void setLeft_lua(IDisplayObject* obj, float p_left) { if (!obj) return; obj->setLeft(p_left); }
    void setRight_lua(IDisplayObject* obj, float p_right) { if (!obj) return; obj->setRight(p_right); }
    void setTop_lua(IDisplayObject* obj, float p_top) { if (!obj) return; obj->setTop(p_top); }
    void setBottom_lua(IDisplayObject* obj, float p_bottom) { if (!obj) return; obj->setBottom(p_bottom); }

    // --- Local Edge Position --- //
    float getLocalLeft_lua(const IDisplayObject* obj) { if (!obj) return 0.0f; return obj->getLocalLeft(); }
    float getLocalRight_lua(const IDisplayObject* obj) { if (!obj) return 0.0f; return obj->getLocalRight(); }
    float getLocalTop_lua(const IDisplayObject* obj) { if (!obj) return 0.0f; return obj->getLocalTop(); }
    float getLocalBottom_lua(const IDisplayObject* obj) { if (!obj) return 0.0f; return obj->getLocalBottom(); }
    void setLocalLeft_lua(IDisplayObject* obj, float p_left) { if (!obj) return; obj->setLocalLeft(p_left); }
    void setLocalRight_lua(IDisplayObject* obj, float p_right) { if (!obj) return; obj->setLocalRight(p_right); }
    void setLocalTop_lua(IDisplayObject* obj, float p_top) { if (!obj) return; obj->setLocalTop(p_top); }
    void setLocalBottom_lua(IDisplayObject* obj, float p_bottom) { if (!obj) return; obj->setLocalBottom(p_bottom); }   

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

    int getOrphanGrace_lua(const IDisplayObject* obj)
    {
        if (!obj) return 0;
        return static_cast<int>(obj->getOrphanGrace().count());
    }

    void setOrphanGrace_lua(IDisplayObject* obj, std::chrono::milliseconds grace)
    {
        if (!obj) return;
        obj->setOrphanGrace(grace);
    }

} // END: namespace SDOM