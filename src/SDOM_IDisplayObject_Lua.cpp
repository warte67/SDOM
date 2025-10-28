// SDOM_IDisplayObject_Lua.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_EventType.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>

#include <SDOM/SDOM_IDisplayObject_Lua.hpp>
#include <type_traits>

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
        // state view not needed here; avoid relying on API specifics

        EventType* et = nullptr;
        sol::function listener;
        bool useCapture = false;
        int priority = 0;

        // If descriptor is a table, parse fields: type, listener, useCapture, priority
        if (descriptor.is<sol::table>()) {
            sol::table t = descriptor.as<sol::table>();
            sol::object t_type = t.get<sol::object>("type");
            if (t_type.valid()) {
                // Try EventType userdata (by reference) first
                try { EventType& r = t_type.as<EventType&>(); et = &r; } catch(...) { et = nullptr; }
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
            try { EventType& r = descriptor.as<EventType&>(); et = &r; } catch(...) { et = nullptr; }
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
        // state view not needed here; avoid relying on API specifics

        EventType* et = nullptr;
        sol::function listener;
        bool useCapture = false;

        if (descriptor.is<sol::table>()) {
            sol::table t = descriptor.as<sol::table>();
            sol::object t_type = t.get<sol::object>("type");
            if (t_type.valid()) {
                try { EventType& r = t_type.as<EventType&>(); et = &r; } catch(...) { et = nullptr; }
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
            try { EventType& r = descriptor.as<EventType&>(); et = &r; } catch(...) { et = nullptr; }
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
    sol::table getChildren_lua(const IDisplayObject* obj, sol::this_state ts)
    {
        sol::state_view lua(ts);
        sol::table childrenTbl = lua.create_table();

        if (!obj)
            return childrenTbl;

        const auto& children = obj->getChildren();
        int index = 1; // Lua tables are 1-indexed

        for (const auto& handle : children) {
            childrenTbl[index++] = handle;
        }

        return childrenTbl;
    }
    int countChildren_lua(const IDisplayObject* obj) { if (!obj) return 0; return static_cast<int>(obj->getChildren().size()); }    
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
        SDL_Color c{255,0,255,255};
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
        SDL_Color c{255,0,255,255};
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
        SDL_Color c{255,0,255,255};
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
        SDL_Color c{255,0,255,255};
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
        SDL_Color c{255,0,255,255};
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
        SDL_Color c{255,0,255,255};
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
    //  - parent:setPriority({ child = DisplayHandle }, 9)
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
        if (!obj || !descriptor.valid())
            return;

        DisplayHandle target;

        // Case 1: descriptor is a string → treat as child name
        if (descriptor.is<std::string>()) {
            target = obj->getChild(descriptor.as<std::string>());
        }
        // Case 2: descriptor is a DisplayHandle
        else if (descriptor.is<DisplayHandle>()) {
            target = descriptor.as<DisplayHandle>();
        }
        // Case 3: descriptor is a Lua table with "name" or "child"
        else if (descriptor.is<sol::table>()) {
            sol::table tbl = descriptor.as<sol::table>();

            // Prefer `name` key (official form)
            if (tbl["name"].valid() && tbl["name"].get_type() == sol::type::string) {
                target = obj->getChild(tbl["name"].get<std::string>());
            }
            // Accept `child` key as fallback
            else if (tbl["child"].valid() && tbl["child"].get_type() == sol::type::string) {
                target = obj->getChild(tbl["child"].get<std::string>());
            }
        }

        if (!target.isValid())
            return;

        // Set priority, then sort
        target->setPriority(value);
        obj->sortChildrenByPriority();
    }
 

    std::vector<int> getChildrenPriorities_lua(const IDisplayObject* obj) 
    {
        if (!obj) return std::vector<int>();
        auto v = obj->getChildrenPriorities();
        return v;
    }

    void moveToTop_lua(IDisplayObject* obj) { if (obj) obj->moveToTop(); return; }
    void moveToTop_lua_any(IDisplayObject* obj, const sol::object& descriptor)
    {
        if (!obj || !descriptor.valid())
            return;

        DisplayHandle target;

        // Resolve descriptor
        if (descriptor.is<std::string>()) {
            target = obj->getChild(descriptor.as<std::string>());
        }
        else if (descriptor.is<DisplayHandle>()) {
            target = descriptor.as<DisplayHandle>();
        }
        else if (descriptor.is<sol::table>()) {
            sol::table tbl = descriptor.as<sol::table>();
            if (tbl["name"].valid() && tbl["name"].get_type() == sol::type::string)
                target = obj->getChild(tbl["name"].get<std::string>());
            else if (tbl["child"].valid() && tbl["child"].get_type() == sol::type::string)
                target = obj->getChild(tbl["child"].get<std::string>());
        }

        if (!target.isValid())
            return;

        // Real parent
        IDisplayObject* parent = target->getParent().as<IDisplayObject>();
        if (!parent)
            return;

        // Increase Z-Order to top-most
        int newTop = parent->countChildren() - 1;
        target->setZOrder(newTop);

        // Sort children based on updated z-orders
        parent->sortByZOrder();
    }

    void moveToBottom_lua(IDisplayObject* obj) { if (obj) obj->moveToBottom(); }    

    void moveToBottom_lua_any(IDisplayObject* parent, const sol::object& descriptor)
    {
        if (!parent || !descriptor.is<sol::table>()) return;

        sol::table t = descriptor.as<sol::table>();
        IDisplayObject* target = nullptr;

        sol::object childObj = t.get<sol::object>("child");
        if (childObj.is<DisplayHandle>()) {
            target = dynamic_cast<IDisplayObject*>(childObj.as<DisplayHandle>().get());
        } else if (childObj.is<std::string>()) {
            target = dynamic_cast<IDisplayObject*>(parent->getChild(childObj.as<std::string>()).get());
        }

        if (!target) return;

        target->moveToBottom();
        parent->sortChildrenByPriority(); // ensure z-order updated
    }

    void bringToFront_lua(IDisplayObject* obj) { if (obj) obj->bringToFront(); }
    void bringToFront_lua_any(IDisplayObject* parent, const sol::object& descriptor)
    {
        if (!parent || !descriptor.is<sol::table>()) return;

        sol::table t = descriptor.as<sol::table>();
        sol::object childObj = t.get<sol::object>("child");

        IDisplayObject* target = nullptr;

        if (childObj.is<DisplayHandle>()) {
            target = dynamic_cast<IDisplayObject*>(childObj.as<DisplayHandle>().get());
        } else if (childObj.is<std::string>()) {
            target = dynamic_cast<IDisplayObject*>(parent->getChild(childObj.as<std::string>()).get());
        }

        if (target) target->bringToFront();
    }

    void sendToBack_lua(IDisplayObject* obj)
    {
        if (obj) obj->sendToBack();
    }

    void sendToBack_lua_any(IDisplayObject* obj, const sol::object& descriptor)
    {
        if (!obj || !descriptor.is<sol::table>()) return;

        sol::table t = descriptor.as<sol::table>();
        sol::object limitObj = t.get<sol::object>("limit");

        IDisplayObject* limit = nullptr;
        if (limitObj.is<DisplayHandle>())
            limit = dynamic_cast<IDisplayObject*>(limitObj.as<DisplayHandle>().get());
        else if (limitObj.is<std::string>())
            limit = dynamic_cast<IDisplayObject*>(obj->getChild(limitObj.as<std::string>()).get());

        obj->sendToBackAfter(limit);
    }

    void sendToBackAfter_lua(IDisplayObject* obj, const IDisplayObject* limitObj) {
        if (!obj) return;
        obj->sendToBackAfter(limitObj);
    }

    

    void sendToBackAfter_lua_any(IDisplayObject* obj, const sol::object& descriptor, const IDisplayObject* limitObj)
    {
        if (!obj || !descriptor.valid() || !limitObj)
            return;

        DisplayHandle child;

        if (descriptor.is<sol::table>()) {
            sol::table t = descriptor.as<sol::table>();

            // Prefer an explicit 'child' entry if provided
            if (sol::object c = t["child"]; c.valid()) {
                // Try general resolver first to support userdata/string/table specs
                try { child = SDOM::DisplayHandle::resolveChildSpec(c); } catch(...) {}

                // Fallbacks
                if (!child.isValid() && c.is<std::string>()) {
                    std::string nm = c.as<std::string>();
                    // Try direct child then global registry
                    child = obj->getChild(nm);
                    if (!child.isValid()) child = SDOM::getCore().getDisplayObject(nm);
                }
            }

            // Accept { name = "..." } as shorthand
            if (!child.isValid()) {
                sol::object n = t["name"];
                if (n.valid() && n.is<std::string>()) {
                    std::string nm = n.as<std::string>();
                    child = obj->getChild(nm);
                    if (!child.isValid()) child = SDOM::getCore().getDisplayObject(nm);
                }
            }
        } else {
            // Non-table: accept DisplayHandle or string directly
            try { if (descriptor.is<DisplayHandle>()) child = descriptor.as<DisplayHandle>(); } catch(...) {}
            if (!child.isValid()) {
                try { if (descriptor.is<std::string>()) {
                    std::string nm = descriptor.as<std::string>();
                    child = obj->getChild(nm);
                    if (!child.isValid()) child = SDOM::getCore().getDisplayObject(nm);
                } } catch(...) {}
            }
        }

        if (!child.isValid() || !child.get()) return; // No-op if invalid

        // If parents differ, reparent child to limitObj's parent
        DisplayHandle limitParent = limitObj->getParent();
        if (!limitParent.isValid() || !limitParent.get()) return;

        if (child->getParent() != limitParent) {
            child->removeFromParent();
            limitParent->addChild(child);
        }

        // Now move child to immediately after limitObj among siblings
        child->sendToBackAfter(limitObj);

        // Ensure order is consistent post-move
        limitParent->sortChildrenByPriority();
    }


    int getZOrder_lua(const IDisplayObject* obj) { if (!obj) return 0; return obj->getZOrder(); }
    void setZOrder_lua(IDisplayObject* obj, int z_order) { if (!obj) return; obj->setZOrder(z_order); }

    bool hasBorder_lua(const IDisplayObject* obj) { return obj ? obj->hasBorder() : false; }
    bool hasBackground_lua(const IDisplayObject* obj) { return obj ? obj->hasBackground() : false; }
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

        DisplayHandle target;

        // Case 1: descriptor is a string
        if (descriptor.is<std::string>()) {
            target = obj->getChild(descriptor.as<std::string>());
        }
        // Case 2: descriptor is a DisplayHandle
        else if (descriptor.is<DisplayHandle>()) {
            target = descriptor.as<DisplayHandle>();
        }
        // Case 3: descriptor is a Lua table with .name
        else if (descriptor.is<sol::table>()) {
            sol::table tbl = descriptor.as<sol::table>();
            if (tbl["name"].valid() && tbl["name"].get_type() == sol::type::string) {
                std::string name = tbl["name"].get<std::string>();
                target = obj->getChild(name);
            }
        }

        if (!target.isValid()) return;
        target->setToHighestPriority();
    }

    void setToLowestPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor)
    {
        if (!obj) return;

        DisplayHandle target;

        if (descriptor.is<std::string>()) {
            target = obj->getChild(descriptor.as<std::string>());
        }
        else if (descriptor.is<DisplayHandle>()) {
            target = descriptor.as<DisplayHandle>();
        }
        else if (descriptor.is<sol::table>()) {
            sol::table tbl = descriptor.as<sol::table>();
            if (tbl["name"].valid() && tbl["name"].get_type() == sol::type::string) {
                std::string name = tbl["name"].get<std::string>();
                target = obj->getChild(name);
            }
        }

        if (!target.isValid()) return;
        target->setToLowestPriority();
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

    // void setOrphanGrace_lua(IDisplayObject* obj, std::chrono::milliseconds grace)
    // {
    //     if (!obj) return;
    //     obj->setOrphanGrace(grace);
    // }

    void setOrphanGrace_lua(IDisplayObject* obj, int grace_in_ms)
    {
        if (!obj) return;
        obj->setOrphanGrace(std::chrono::milliseconds(grace_in_ms));
    }

    // Overload-aware binder helper: adapts pointer-based wrappers to DisplayHandle colon-calls.
    namespace {
        template <typename Ret, typename... Args>
        inline void set_if_absent(sol::table& tbl, const char* name, Ret(*fn)(IDisplayObject*, Args...))
        {
            sol::object cur = tbl.raw_get_or(name, sol::lua_nil);
            if (!cur.valid() || cur == sol::lua_nil) {
                tbl.set_function(name, [fn](DisplayHandle& self, Args... args) -> Ret {
                    IDisplayObject* obj = self.get();
                    if constexpr (std::is_void_v<Ret>) {
                        if (!obj) return;
                        fn(obj, std::forward<Args>(args)...);
                        return;
                    } else {
                        if (!obj) return Ret{};
                        return fn(obj, std::forward<Args>(args)...);
                    }
                });
            }
        }

        template <typename Ret, typename... Args>
        inline void set_if_absent(sol::table& tbl, const char* name, Ret(*fn)(const IDisplayObject*, Args...))
        {
            sol::object cur = tbl.raw_get_or(name, sol::lua_nil);
            if (!cur.valid() || cur == sol::lua_nil) {
                tbl.set_function(name, [fn](DisplayHandle& self, Args... args) -> Ret {
                    const IDisplayObject* obj = self.get();
                    if constexpr (std::is_void_v<Ret>) {
                        if (!obj) return;
                        fn(obj, std::forward<Args>(args)...);
                        return;
                    } else {
                        if (!obj) return Ret{};
                        return fn(obj, std::forward<Args>(args)...);
                    }
                });
            }
        }

        template <typename F>
        inline void set_if_absent(sol::table& tbl, const char* name, F&& fn)
        {
            sol::object cur = tbl.raw_get_or(name, sol::lua_nil);
            if (!cur.valid() || cur == sol::lua_nil) {
                tbl.set_function(name, std::forward<F>(fn));
            }
        }

        // Create a DisplayHandle-aware wrapper for pointer-based wrappers
        template <typename Ret, typename... Args>
        auto make_handle_wrapper(Ret(*fn)(IDisplayObject*, Args...)) {
            return [fn](DisplayHandle& self, Args... args) -> Ret {
                IDisplayObject* obj = self.get();
                if constexpr (std::is_void_v<Ret>) {
                    if (!obj) return;
                    fn(obj, std::forward<Args>(args)...);
                    return;
                } else {
                    if (!obj) return Ret{};
                    return fn(obj, std::forward<Args>(args)...);
                }
            };
        }
        template <typename Ret, typename... Args>
        auto make_handle_wrapper(Ret(*fn)(const IDisplayObject*, Args...)) {
            return [fn](DisplayHandle& self, Args... args) -> Ret {
                const IDisplayObject* obj = self.get();
                if constexpr (std::is_void_v<Ret>) {
                    if (!obj) return;
                    fn(obj, std::forward<Args>(args)...);
                    return;
                } else {
                    if (!obj) return Ret{};
                    return fn(obj, std::forward<Args>(args)...);
                }
            };
        }

        // --- new reusable property helper (lambda-like usage) ---
        template <typename Getter, typename Setter>
        inline void set_property(sol::table& tbl,
                                 sol::optional<sol::usertype<SDOM::DisplayHandle>>& maybeUT,
                                 const char* name,
                                 Getter getter_fn,
                                 Setter setter_fn)
        {
            sol::object cur = tbl.raw_get_or(name, sol::lua_nil);
            if (!cur.valid() || cur == sol::lua_nil) {
                tbl.set(name, sol::property(make_handle_wrapper(getter_fn), make_handle_wrapper(setter_fn)));
            }
            if (maybeUT) {
                try {
                    (*maybeUT)[name] = sol::property(make_handle_wrapper(getter_fn), make_handle_wrapper(setter_fn));
                } catch(...) {}
            }
        }

        // Bind the function on both the table and the usertype (if present)
        template <typename F>
        void bind_both(sol::table& tbl,
                       sol::optional<sol::usertype<SDOM::DisplayHandle>>& maybeUT,
                       const char* name,
                       F&& fn) {
            // set on table if absent
            sol::object cur = tbl.raw_get_or(name, sol::lua_nil);
            if (!cur.valid() || cur == sol::lua_nil) {
                tbl.set_function(name, std::forward<F>(fn));
            }
            // set on usertype if present
            if (maybeUT) {
                try { (*maybeUT)[name] = fn; } catch(...) {}
            }
        }

        // Overloads that adapt pointer wrappers and then bind both
        template <typename Ret, typename... Args>
        void bind_both(sol::table& tbl,
                       sol::optional<sol::usertype<SDOM::DisplayHandle>>& maybeUT,
                       const char* name,
                       Ret(*fn)(IDisplayObject*, Args...)) {
            auto wrapper = make_handle_wrapper(fn);
            bind_both(tbl, maybeUT, name, wrapper);
        }
        template <typename Ret, typename... Args>
        void bind_both(sol::table& tbl,
                       sol::optional<sol::usertype<SDOM::DisplayHandle>>& maybeUT,
                       const char* name,
                       Ret(*fn)(const IDisplayObject*, Args...)) {
            auto wrapper = make_handle_wrapper(fn);
            bind_both(tbl, maybeUT, name, wrapper);
        }

        // Color property binder: handles SDL_Color <-> Lua table conversions
        template <typename GetterFn, typename SetterFn>
        inline void bind_color_property(sol::table& handleTbl,
                                        sol::optional<sol::usertype<SDOM::DisplayHandle>>& maybeUT,
                                        const char* name,
                                        GetterFn getter,
                                        SetterFn setter)
        {
            auto color_getter = [getter](DisplayHandle& self) -> sol::table {
                sol::state_view lua = getLua();
                sol::table t = lua.create_table();
                if (!self.isValid() || !self.get()) return t;
                IDisplayObject* obj = self.get();
                SDL_Color c = getter(obj);
                return SDL_Utils::get_lua_color(lua, c);
            };

            auto color_setter = [setter](DisplayHandle& self, const sol::object& val) {
                if (!self.isValid() || !self.get()) return;
                IDisplayObject* obj = self.get();

                if (val.is<sol::table>()) {
                    sol::table t = val.as<sol::table>();
                    SDL_Color c = SDL_Utils::get__lua_color(t);
                    setter(obj, c);
                }
                else if (val.is<SDL_Color>()) {
                    try { SDL_Color c = val.as<SDL_Color>(); setter(obj, c); } catch(...) {}
                }
                else if (val.is<int>() || val.is<double>()) {
                    int v = val.is<int>() ? val.as<int>() : static_cast<int>(val.as<double>());
                    SDL_Color c{static_cast<Uint8>(v), static_cast<Uint8>(v), static_cast<Uint8>(v), 255};
                    setter(obj, c);
                }
            };

            sol::object cur = handleTbl.raw_get_or(name, sol::lua_nil);
            if (!cur.valid() || cur == sol::lua_nil)
                handleTbl.set(name, sol::property(color_getter, color_setter));

            if (maybeUT) {
                try { (*maybeUT)[name] = sol::property(color_getter, color_setter); }
                catch (...) {}
            }
        }

    } // END namespace

    // Central binder: attach IDisplayObject Lua-facing helpers to the shared
    // DisplayHandle surface (idempotent). This mirrors the in-class binding
    // but allows SDOM_IDisplayObject.cpp to delegate to this module.
    void bind_IDisplayObject_lua(const std::string& /*typeName*/, sol::state_view lua)
    {
        // Acquire or create the DisplayHandle table (do not clobber usertype).
        sol::table handleTbl;
        try { handleTbl = lua[SDOM::DisplayHandle::LuaHandleName]; } catch(...) {}
        if (!handleTbl.valid()) {
            handleTbl = SDOM::IDataObject::ensure_sol_table(lua, SDOM::DisplayHandle::LuaHandleName);
        }

        // set_if_absent overloads provided above handle both raw pointer wrappers and custom lambdas

        // Also try to acquire the usertype so we can bind on both surfaces.
        sol::optional<sol::usertype<SDOM::DisplayHandle>> maybeUT;
        try { maybeUT = lua[SDOM::DisplayHandle::LuaHandleName]; } catch(...) {}

        // DisplayHandle-aware hierarchy helpers (mirror SDOM_IDisplayObject.cpp)
        auto addChild_impl = [](SDOM::DisplayHandle& self, const sol::object& childSpec) -> bool {
            if (!self.isValid() || !self.get()) return false;
            SDOM::DisplayHandle child;
            try {
                if (childSpec.is<SDOM::DisplayHandle>()) {
                    child = childSpec.as<SDOM::DisplayHandle>();
                } else if (childSpec.is<std::string>()) {
                    std::string nm = childSpec.as<std::string>();
                    child = SDOM::getCore().getDisplayObject(nm);
                } else if (childSpec.is<sol::table>()) {
                    sol::table t = childSpec.as<sol::table>();
                    sol::object nameObj = t.get<sol::object>("name");
                    if (nameObj.valid() && nameObj.is<std::string>()) {
                        std::string nm = nameObj.as<std::string>();
                        child = SDOM::getCore().getDisplayObject(nm);
                    }
                }
            } catch(...) {}
            if (!child.isValid()) return false;
            auto* obj = self.get();
            obj->addChild(child);
            return true;
        };
        set_if_absent(handleTbl, "addChild", addChild_impl);
        if (maybeUT) { try { (*maybeUT)["addChild"] = addChild_impl; } catch(...) {} }

        auto removeChild_impl = [](SDOM::DisplayHandle& self, const sol::object& childSpec) -> bool {
            if (!self.isValid() || !self.get()) return false;
            auto* obj = self.get();
            try {
                if (childSpec.is<SDOM::DisplayHandle>()) {
                    SDOM::DisplayHandle child = childSpec.as<SDOM::DisplayHandle>();
                    return obj->removeChild(child);
                } else if (childSpec.is<std::string>()) {
                    std::string nm = childSpec.as<std::string>();
                    return obj->removeChild(nm);
                } else if (childSpec.is<sol::table>()) {
                    sol::table t = childSpec.as<sol::table>();
                    sol::object nameObj = t.get<sol::object>("name");
                    if (nameObj.valid() && nameObj.is<std::string>()) {
                        std::string nm = nameObj.as<std::string>();
                        return obj->removeChild(nm);
                    }
                }
            } catch(...) {}
            return false;
        };
        set_if_absent(handleTbl, "removeChild", removeChild_impl);
        if (maybeUT) { try { (*maybeUT)["removeChild"] = removeChild_impl; } catch(...) {} }

        auto hasChild_impl = [](SDOM::DisplayHandle& self, const sol::object& childSpec) -> bool {
            if (!self.isValid() || !self.get()) return false;
            auto* obj = self.get();
            try {
                if (childSpec.is<SDOM::DisplayHandle>()) {
                    SDOM::DisplayHandle child = childSpec.as<SDOM::DisplayHandle>();
                    return obj->hasChild(child);
                } else if (childSpec.is<std::string>()) {
                    std::string nm = childSpec.as<std::string>();
                    return obj->isDescendantOf(nm) || (obj->getChild(nm).isValid());
                } else if (childSpec.is<sol::table>()) {
                    sol::table t = childSpec.as<sol::table>();
                    sol::object nameObj = t.get<sol::object>("name");
                    if (nameObj.valid() && nameObj.is<std::string>()) {
                        std::string nm = nameObj.as<std::string>();
                        return obj->isDescendantOf(nm) || (obj->getChild(nm).isValid());
                    }
                }
            } catch(...) {}
            return false;
        };
        set_if_absent(handleTbl, "hasChild", hasChild_impl);
        if (maybeUT) { try { (*maybeUT)["hasChild"] = hasChild_impl; } catch(...) {} }

        auto getChild_impl = [](SDOM::DisplayHandle& self, const sol::object& childSpec, sol::this_state ts) -> sol::object {
            sol::state_view lua(ts);
            if (!self.isValid() || !self.get()) return sol::make_object(lua, sol::lua_nil);
            auto* obj = self.get();
            std::string nm;
            try {
                if (childSpec.is<std::string>()) {
                    nm = childSpec.as<std::string>();
                } else if (childSpec.is<sol::table>()) {
                    sol::table t = childSpec.as<sol::table>();
                    sol::object nameObj = t.get<sol::object>("name");
                    if (nameObj.valid() && nameObj.is<std::string>()) nm = nameObj.as<std::string>();
                }
            } catch(...) {}
            if (nm.empty()) return sol::make_object(lua, sol::lua_nil);
            SDOM::DisplayHandle ch = obj->getChild(nm);
            if (ch.isValid() && ch.get()) return sol::make_object(lua, ch);
            return sol::make_object(lua, sol::lua_nil);
        };
        bind_both(handleTbl, maybeUT, "getChild", getChild_impl);

        // Dirty/state (bind to both table and usertype)
        bind_both(handleTbl, maybeUT, "cleanAll", cleanAll_lua);
        bind_both(handleTbl, maybeUT, "getDirty", getDirty_lua);
        bind_both(handleTbl, maybeUT, "setDirty", setDirty_lua);
        bind_both(handleTbl, maybeUT, "isDirty", isDirty_lua);

        // Debug/utility
        bind_both(handleTbl, maybeUT, "printTree", printTree_lua);

        // Events: Provide DisplayHandle-aware overloads to avoid self-type issues
        auto addEventListener_bind1 = [](SDOM::DisplayHandle& self, const sol::object& descriptor) {
            IDisplayObject* obj = self.get(); if (!obj) return; addEventListener_lua_any_short(obj, descriptor);
        };
        auto addEventListener_bind4 = [](SDOM::DisplayHandle& self, const sol::object& a, const sol::object& b, const sol::object& c, const sol::object& d) {
            IDisplayObject* obj = self.get(); if (!obj) return;
            // Table descriptor form
            if (a.valid() && a.is<sol::table>()) { addEventListener_lua_any(obj, a, b, c, d); return; }
            // EventType + function + [useCapture] + [priority]
            try {
                EventType& et = a.as<EventType&>();
                sol::function fn; if (b.valid() && b.is<sol::function>()) fn = b.as<sol::function>();
                bool useCap = false; int pri = 0;
                if (c.valid()) {
                    if (c.is<bool>()) useCap = c.as<bool>();
                    else if (c.is<int>()) pri = c.as<int>();
                }
                if (d.valid() && d.is<int>()) pri = d.as<int>();
                if (fn.valid()) addEventListener_lua(obj, et, fn, useCap, pri);
                return;
            } catch (...) {
                // Fallback: accept string type name
                try {
                    if (a.valid() && a.is<std::string>()) {
                        std::string name = a.as<std::string>();
                        const auto& reg = EventType::getRegistry();
                        auto it = reg.find(name);
                        if (it != reg.end() && it->second) {
                            EventType& et = *it->second;
                            sol::function fn; if (b.valid() && b.is<sol::function>()) fn = b.as<sol::function>();
                            bool useCap = false; int pri = 0;
                            if (c.valid()) {
                                if (c.is<bool>()) useCap = c.as<bool>();
                                else if (c.is<int>()) pri = c.as<int>();
                            }
                            if (d.valid() && d.is<int>()) pri = d.as<int>();
                            if (fn.valid()) addEventListener_lua(obj, et, fn, useCap, pri);
                        }
                    }
                } catch(...) { /* swallow */ }
            }
        };
        auto removeEventListener_bind1 = [](SDOM::DisplayHandle& self, const sol::object& descriptor) {
            IDisplayObject* obj = self.get(); if (!obj) return; removeEventListener_lua_any_short(obj, descriptor);
        };
        auto removeEventListener_bind3 = [](SDOM::DisplayHandle& self, const sol::object& a, const sol::object& b, const sol::object& c) {
            IDisplayObject* obj = self.get(); if (!obj) return;
            if (a.valid() && a.is<sol::table>()) { removeEventListener_lua_any(obj, a, b, c); return; }
            try {
                EventType& et = a.as<EventType&>();
                sol::function fn; if (b.valid() && b.is<sol::function>()) fn = b.as<sol::function>();
                bool useCap = false; if (c.valid() && c.is<bool>()) useCap = c.as<bool>();
                if (fn.valid()) removeEventListener_lua(obj, et, fn, useCap);
                return;
            } catch (...) {
                try {
                    if (a.valid() && a.is<std::string>()) {
                        std::string name = a.as<std::string>();
                        const auto& reg = EventType::getRegistry();
                        auto it = reg.find(name);
                        if (it != reg.end() && it->second) {
                            EventType& et = *it->second;
                            sol::function fn; if (b.valid() && b.is<sol::function>()) fn = b.as<sol::function>();
                            bool useCap = false; if (c.valid() && c.is<bool>()) useCap = c.as<bool>();
                            if (fn.valid()) removeEventListener_lua(obj, et, fn, useCap);
                        }
                    }
                } catch(...) { /* swallow */ }
            }
        };
        // Register on table and usertype if present
        bind_both(handleTbl, maybeUT, "addEventListener", sol::overload(addEventListener_bind1, addEventListener_bind4));
        bind_both(handleTbl, maybeUT, "removeEventListener", sol::overload(removeEventListener_bind1, removeEventListener_bind3));
        bind_both(handleTbl, maybeUT, "hasEventListener", hasEventListener_lua);
        bind_both(handleTbl, maybeUT, "queue_event", queue_event_lua);

        // Hierarchy helpers continued
        bind_both(handleTbl, maybeUT, "getChildren", getChildren_lua);
        bind_both(handleTbl, maybeUT, "countChildren", countChildren_lua);
        // hasChild is already bound in core with DisplayHandle-aware wrapper
        // set_if_absent(handleTbl, "hasChild", hasChild_lua);
        bind_both(handleTbl, maybeUT, "getParent", getParent_lua);
        bind_both(handleTbl, maybeUT, "setParent", setParent_lua);
        bind_both(handleTbl, maybeUT, "isAncestorOf",
            sol::overload(
                static_cast<bool(*)(IDisplayObject*, DisplayHandle)>(isAncestorOf_lua),
                static_cast<bool(*)(IDisplayObject*, const std::string&)>(isAncestorOf_lua)
            )
        );
        bind_both(handleTbl, maybeUT, "isDescendantOf",
            sol::overload(
                static_cast<bool(*)(IDisplayObject*, DisplayHandle)>(isDescendantOf_lua),
                static_cast<bool(*)(IDisplayObject*, const std::string&)>(isDescendantOf_lua)
            )
        );
        bind_both(handleTbl, maybeUT, "removeFromParent", removeFromParent_lua);
        bind_both(handleTbl, maybeUT, "removeDescendant",
            sol::overload(
                static_cast<bool(*)(IDisplayObject*, DisplayHandle)>(removeDescendant_lua),
                static_cast<bool(*)(IDisplayObject*, const std::string&)>(removeDescendant_lua)
            )
        );

        // Names and types
        bind_both(handleTbl, maybeUT, "getName", getName_lua);
        bind_both(handleTbl, maybeUT, "setName", setName_lua);
        bind_both(handleTbl, maybeUT, "getType", getType_lua);

        // Bounds and colors
        bind_both(handleTbl, maybeUT, "getBounds", getBounds_lua);
        bind_both(handleTbl, maybeUT, "setBounds",
            sol::overload(
                static_cast<void(*)(IDisplayObject*, const sol::object&)>(setBounds_lua),
                static_cast<void(*)(IDisplayObject*, const Bounds&)>(setBounds_lua)
            )
        );
        bind_both(handleTbl, maybeUT, "getColor", getColor_lua);
        // bind_both(handleTbl, maybeUT, "setColor",
        //     sol::overload(
        //         static_cast<void(*)(IDisplayObject*, const sol::object&)>(setColor_lua),
        //         static_cast<void(*)(IDisplayObject*, const SDL_Color&)>(setColor_lua)
        //     )
        // );        
        bind_both(handleTbl, maybeUT, "setColor", static_cast<void(*)(IDisplayObject*, const sol::object&)>(setColor_lua) );

        bind_both(handleTbl, maybeUT, "getForegroundColor", getForegroundColor_lua);
        bind_both(handleTbl, maybeUT, "setForegroundColor", static_cast<void(*)(IDisplayObject*, const sol::object&)>(setForegroundColor_lua) );

        bind_both(handleTbl, maybeUT, "getBackgroundColor", getBackgroundColor_lua);
        bind_both(handleTbl, maybeUT, "setBackgroundColor", static_cast<void(*)(IDisplayObject*, const sol::object&)>(setBackgroundColor_lua) );

        bind_both(handleTbl, maybeUT, "getBorderColor", getBorderColor_lua);
        bind_both(handleTbl, maybeUT, "setBorderColor", static_cast<void(*)(IDisplayObject*, const sol::object&)>(setBorderColor_lua) );

        bind_both(handleTbl, maybeUT, "getOutlineColor", getOutlineColor_lua);
        bind_both(handleTbl, maybeUT, "setOutlineColor", static_cast<void(*)(IDisplayObject*, const sol::object&)>(setOutlineColor_lua) );

        bind_both(handleTbl, maybeUT, "getDropshadowColor", getDropshadowColor_lua);
        bind_both(handleTbl, maybeUT, "setDropshadowColor", static_cast<void(*)(IDisplayObject*, const sol::object&)>(setDropshadowColor_lua) );

        bind_both(handleTbl, maybeUT, "hasBorder", hasBorder_lua);
        bind_both(handleTbl, maybeUT, "hasBackground", hasBackground_lua);
        bind_both(handleTbl, maybeUT, "setBorder", setBorder_lua);
        bind_both(handleTbl, maybeUT, "setBackground", setBackground_lua);

        // Priority & Z-Order
        bind_both(handleTbl, maybeUT, "getMaxPriority", getMaxPriority_lua);
        bind_both(handleTbl, maybeUT, "getMinPriority", getMinPriority_lua);
        bind_both(handleTbl, maybeUT, "getPriority", getPriority_lua);
        bind_both(handleTbl, maybeUT, "setPriority",
            sol::overload(
                setPriority_lua,
                setPriority_lua_any,
                setPriority_lua_target
            )
        );
        bind_both(handleTbl, maybeUT, "sortChildrenByPriority", sortChildrenByPriority_lua);
        bind_both(handleTbl, maybeUT, "setToHighestPriority",
            sol::overload(
                setToHighestPriority_lua,
                setToHighestPriority_lua_any
            )
        );
        bind_both(handleTbl, maybeUT, "setToLowestPriority",
            sol::overload(
                setToLowestPriority_lua,
                setToLowestPriority_lua_any
            )
        );
        // DisplayHandle-aware wrappers for z-order helpers to avoid mismatch with IDO* overloads
        auto moveToTop_bind0 = [](SDOM::DisplayHandle& self) {
            if (auto* obj = self.get()) moveToTop_lua(obj);
        };
        auto moveToTop_bind1 = [](SDOM::DisplayHandle& self, const sol::object& descriptor) {
            if (auto* obj = self.get()) moveToTop_lua_any(obj, descriptor);
        };
        bind_both(handleTbl, maybeUT, "moveToTop", sol::overload(moveToTop_bind0, moveToTop_bind1));

        auto moveToBottom_bind0 = [](SDOM::DisplayHandle& self) {
            if (auto* obj = self.get()) moveToBottom_lua(obj);
        };
        auto moveToBottom_bind1 = [](SDOM::DisplayHandle& self, const sol::object& descriptor) {
            if (auto* obj = self.get()) moveToBottom_lua_any(obj, descriptor);
        };
        bind_both(handleTbl, maybeUT, "moveToBottom", sol::overload(moveToBottom_bind0, moveToBottom_bind1));

        auto bringToFront_bind0 = [](SDOM::DisplayHandle& self) {
            if (auto* obj = self.get()) bringToFront_lua(obj);
        };
        auto bringToFront_bind1 = [](SDOM::DisplayHandle& self, const sol::object& descriptor) {
            if (auto* obj = self.get()) bringToFront_lua_any(obj, descriptor);
        };
        bind_both(handleTbl, maybeUT, "bringToFront", sol::overload(bringToFront_bind0, bringToFront_bind1));
        // sendToBack wrappers
        auto sendToBack_bind0 = [](SDOM::DisplayHandle& self) {
            if (auto* obj = self.get()) sendToBack_lua(obj);
        };
        auto sendToBack_bind1 = [](SDOM::DisplayHandle& self, const sol::object& descriptor) {
            if (auto* obj = self.get()) sendToBack_lua_any(obj, descriptor);
        };
        bind_both(handleTbl, maybeUT, "sendToBack", sol::overload(sendToBack_bind0, sendToBack_bind1));

        // sendToBackAfter wrappers
        auto resolve_limit_ptr = [](const sol::object& spec, SDOM::IDisplayObject* contextParent) -> const SDOM::IDisplayObject* {
            if (!spec.valid()) return nullptr;
            try {
                if (spec.is<SDOM::DisplayHandle>()) {
                    return dynamic_cast<const SDOM::IDisplayObject*>(spec.as<SDOM::DisplayHandle>().get());
                }
            } catch(...) {}
            try {
                if (spec.is<std::string>()) {
                    std::string nm = spec.as<std::string>();
                    SDOM::DisplayHandle h;
                    if (contextParent) {
                        h = contextParent->getChild(nm);
                    }
                    if (!h.isValid()) h = SDOM::getCore().getDisplayObject(nm);
                    return dynamic_cast<const SDOM::IDisplayObject*>(h.get());
                }
            } catch(...) {}
            try {
                if (spec.is<sol::table>()) {
                    sol::table t = spec.as<sol::table>();
                    sol::object nameObj = t.get<sol::object>("name");
                    if (nameObj.valid() && nameObj.is<std::string>()) {
                        std::string nm = nameObj.as<std::string>();
                        SDOM::DisplayHandle h;
                        if (contextParent) h = contextParent->getChild(nm);
                        if (!h.isValid()) h = SDOM::getCore().getDisplayObject(nm);
                        return dynamic_cast<const SDOM::IDisplayObject*>(h.get());
                    }
                }
            } catch(...) {}
            return nullptr;
        };

        // Child-style: child:sendToBackAfter(limitSpec)
        auto sendToBackAfter_bind1 = [resolve_limit_ptr](SDOM::DisplayHandle& self, const sol::object& limitSpec) {
            SDOM::IDisplayObject* child = self.get();
            if (!child) return;
            // Prefer child's parent for name resolution
            SDOM::IDisplayObject* parent = nullptr;
            try { parent = self.get() ? self.get()->getParent().as<SDOM::IDisplayObject>() : nullptr; } catch(...) { parent = nullptr; }
            const SDOM::IDisplayObject* limit = resolve_limit_ptr(limitSpec, parent);
            sendToBackAfter_lua(child, limit);
        };

        // Parent-style: parent:sendToBackAfter(childDescriptor, limitSpec)
        auto sendToBackAfter_bind2 = [resolve_limit_ptr](SDOM::DisplayHandle& self, const sol::object& childDesc, const sol::object& limitSpec) {
            SDOM::IDisplayObject* parent = self.get();
            if (!parent) return;
            const SDOM::IDisplayObject* limit = resolve_limit_ptr(limitSpec, parent);
            sendToBackAfter_lua_any(parent, childDesc, limit);
        };
        bind_both(handleTbl, maybeUT, "sendToBackAfter", sol::overload(sendToBackAfter_bind1, sendToBackAfter_bind2));
        bind_both(handleTbl, maybeUT, "getZOrder", getZOrder_lua);
        bind_both(handleTbl, maybeUT, "setZOrder",
            sol::overload(
                setZOrder_lua,
                setZOrder_lua_any
            )
        );

        // Focus & interactivity
        bind_both(handleTbl, maybeUT, "setKeyboardFocus", setKeyboardFocus_lua);
        bind_both(handleTbl, maybeUT, "isKeyboardFocused", isKeyboardFocused_lua);
        bind_both(handleTbl, maybeUT, "isMouseHovered", isMouseHovered_lua);
        bind_both(handleTbl, maybeUT, "isClickable", isClickable_lua);
        bind_both(handleTbl, maybeUT, "setClickable", setClickable_lua);
        bind_both(handleTbl, maybeUT, "isEnabled", isEnabled_lua);
        bind_both(handleTbl, maybeUT, "setEnabled", setEnabled_lua);
        bind_both(handleTbl, maybeUT, "isHidden", isHidden_lua);
        bind_both(handleTbl, maybeUT, "setHidden", setHidden_lua);
        bind_both(handleTbl, maybeUT, "isVisible", isVisible_lua);
        bind_both(handleTbl, maybeUT, "setVisible", setVisible_lua);

        // Tab management
        bind_both(handleTbl, maybeUT, "getTabPriority", getTabPriority_lua);
        bind_both(handleTbl, maybeUT, "setTabPriority", setTabPriority_lua);
        bind_both(handleTbl, maybeUT, "isTabEnabled", isTabEnabled_lua);
        bind_both(handleTbl, maybeUT, "setTabEnabled", setTabEnabled_lua);

        // Geometry & layout
        bind_both(handleTbl, maybeUT, "getX", getX_lua);
        bind_both(handleTbl, maybeUT, "getY", getY_lua);
        bind_both(handleTbl, maybeUT, "getWidth", getWidth_lua);
        bind_both(handleTbl, maybeUT, "getHeight", getHeight_lua);
        bind_both(handleTbl, maybeUT, "setX", setX_lua);
        bind_both(handleTbl, maybeUT, "setY", setY_lua);
        bind_both(handleTbl, maybeUT, "setWidth", setWidth_lua);
        bind_both(handleTbl, maybeUT, "setHeight", setHeight_lua);

        // Anchors
        bind_both(handleTbl, maybeUT, "getAnchorTop", getAnchorTop_lua);
        bind_both(handleTbl, maybeUT, "getAnchorLeft", getAnchorLeft_lua);
        bind_both(handleTbl, maybeUT, "getAnchorBottom", getAnchorBottom_lua);
        bind_both(handleTbl, maybeUT, "getAnchorRight", getAnchorRight_lua);
        bind_both(handleTbl, maybeUT, "setAnchorTop", setAnchorTop_lua);
        bind_both(handleTbl, maybeUT, "setAnchorLeft", setAnchorLeft_lua);
        bind_both(handleTbl, maybeUT, "setAnchorBottom", setAnchorBottom_lua);
        bind_both(handleTbl, maybeUT, "setAnchorRight", setAnchorRight_lua);

        // World edges
        bind_both(handleTbl, maybeUT, "getLeft", getLeft_lua);
        bind_both(handleTbl, maybeUT, "getRight", getRight_lua);
        bind_both(handleTbl, maybeUT, "getTop", getTop_lua);
        bind_both(handleTbl, maybeUT, "getBottom", getBottom_lua);
        bind_both(handleTbl, maybeUT, "setLeft", setLeft_lua);
        bind_both(handleTbl, maybeUT, "setRight", setRight_lua);
        bind_both(handleTbl, maybeUT, "setTop", setTop_lua);
        bind_both(handleTbl, maybeUT, "setBottom", setBottom_lua);

        // Local edges
        bind_both(handleTbl, maybeUT, "getLocalLeft", getLocalLeft_lua);
        bind_both(handleTbl, maybeUT, "getLocalRight", getLocalRight_lua);
        bind_both(handleTbl, maybeUT, "getLocalTop", getLocalTop_lua);
        bind_both(handleTbl, maybeUT, "getLocalBottom", getLocalBottom_lua);
        bind_both(handleTbl, maybeUT, "setLocalLeft", setLocalLeft_lua);
        bind_both(handleTbl, maybeUT, "setLocalRight", setLocalRight_lua);
        bind_both(handleTbl, maybeUT, "setLocalTop", setLocalTop_lua);
        bind_both(handleTbl, maybeUT, "setLocalBottom", setLocalBottom_lua);

        // Orphan retention & grace
        bind_both(handleTbl, maybeUT, "orphanPolicyFromString", orphanPolicyFromString_lua);
        bind_both(handleTbl, maybeUT, "orphanPolicyToString", orphanPolicyToString_lua);
        bind_both(handleTbl, maybeUT, "setOrphanRetentionPolicy", setOrphanRetentionPolicy_lua);
        bind_both(handleTbl, maybeUT, "getOrphanRetentionPolicyString", getOrphanRetentionPolicyString_lua);
        bind_both(handleTbl, maybeUT, "getOrphanGrace", getOrphanGrace_lua);
        bind_both(handleTbl, maybeUT, "setOrphanGrace", setOrphanGrace_lua);

        // --- IDisplayObject Lua Properties --- //

        /*
        -- Legend:
        -- ✅ Test Verified
        -- 🔄 In Progress
        -- ⚠️ Failing
        -- ☐ Planned

        | Property        | Type        | Getter                             | Setter                            | Notes                  |
        | --------------- | ----------- | ---------------------------------- | --------------------------------- | ---------------------- |
        | `name`          | string      | `getName()`                        | `setName(string)`                 | ☐ planned             |
        | `type`          | string      | `getType()`                        | n/a                               | ☐ planned             |
        | `x`             | number      | `getX()`                           | `setX(number)`                    | ☐ planned             |
        | `y`             | number      | `getY()`                           | `setY(number)`                    | ☐ planned             |
        | `width`         | number      | `getWidth()`                       | `setWidth(number)`                | ☐ planned             |
        | `height`        | number      | `getHeight()`                      | `setHeight(number)`               | ☐ planned             |
        | `w` *(alias)*   | number      | → `width`                          | → `width`                         | ☐ planned             |
        | `h` *(alias)*   | number      | → `height`                         | → `height`                        | ☐ planned             |
        | `color`         | `{r,g,b,a}` | `getColor()`                       | `setColor(SDL_Color)`             | ☐ planned             |
        | `anchor_top`    | enum/int    | `getAnchorTop()`                   | `setAnchorTop(int)`               | ☐ planned             |
        | `anchor_left`   | enum/int    | `getAnchorLeft()`                  | `setAnchorLeft(int)`              | ☐ planned             |
        | `anchor_bottom` | enum/int    | `getAnchorBottom()`                | `setAnchorBottom(int)`            | ☐ planned             |
        | `anchor_right`  | enum/int    | `getAnchorRight()`                 | `setAnchorRight(int)`             | ☐ planned             |
        | `z_order`       | number      | `getZOrder()`                      | `setZOrder(number)`               | ☐ planned             |
        | `priority`      | number      | `getPriority()`                    | `setPriority(number)`             | ☐ planned             |
        | `is_clickable`  | boolean     | `isClickable()`                    | `setClickable(bool)`              | ☐ planned             |
        | `is_enabled`    | boolean     | `isEnabled()`                      | `setEnabled(bool)`                | ☐ planned             |
        | `is_hidden`     | boolean     | `isHidden()`                       | `setHidden(bool)`                 | ☐ planned             |
        | `tab_priority`  | number      | `getTabPriority()`                 | `setTabPriority(number)`          | ☐ planned             |
        | `tab_enabled`   | boolean     | `isTabEnabled()`                   | `setTabEnabled(bool)`             | ☐ planned             |
        | `left`          | number      | `getLeft()`                        | `setLeft(number)`                 | ☐ planned             |
        | `right`         | number      | `getRight()`                       | `setRight(number)`                | ☐ planned             |
        | `top`           | number      | `getTop()`                         | `setTop(number)`                  | ☐ planned             |
        | `bottom`        | number      | `getBottom()`                      | `setBottom(number)`               | ☐ planned             |
        | `local_left`    | number      | `getLocalLeft()`                   | `setLocalLeft(number)`            | ☐ planned             |
        | `local_right`   | number      | `getLocalRight()`                  | `setLocalRight(number)`           | ☐ planned             |
        | `local_top`     | number      | `getLocalTop()`                    | `setLocalTop(number)`             | ☐ planned             |
        | `local_bottom`  | number      | `getLocalBottom()`                 | `setLocalBottom(number)`          | ☐ planned             |
        | `orphan_policy` | string      | `getOrphanRetentionPolicyString()` | `setOrphanRetentionPolicy("auto") | ☐ planned             |
        | `orphan_grace`  | number      | `getOrphanGrace()`                 | `setOrphanGrace(number)`          | ☐ planned             |
        */

        // Expose properties as both dot-style and snake_case
        set_property(handleTbl, maybeUT, "x", getX_lua, setX_lua);
        set_property(handleTbl, maybeUT, "y", getY_lua, setY_lua);
        set_property(handleTbl, maybeUT, "width", getWidth_lua, setWidth_lua);
        set_property(handleTbl, maybeUT, "height", getHeight_lua, setHeight_lua);
        set_property(handleTbl, maybeUT, "w", getWidth_lua, setWidth_lua);
        set_property(handleTbl, maybeUT, "h", getHeight_lua, setHeight_lua);
        set_property(handleTbl, maybeUT, "anchor_top", getAnchorTop_lua, setAnchorTop_lua);
        set_property(handleTbl, maybeUT, "anchor_left", getAnchorLeft_lua, setAnchorLeft_lua);
        set_property(handleTbl, maybeUT, "anchor_bottom", getAnchorBottom_lua, setAnchorBottom_lua);
        set_property(handleTbl, maybeUT, "anchor_right", getAnchorRight_lua, setAnchorRight_lua);
        set_property(handleTbl, maybeUT, "z_order", getZOrder_lua, setZOrder_lua);
        set_property(handleTbl, maybeUT, "priority", getPriority_lua, setPriority_lua);
        set_property(handleTbl, maybeUT, "is_clickable", isClickable_lua, setClickable_lua);
        set_property(handleTbl, maybeUT, "is_enabled", isEnabled_lua, setEnabled_lua);
        set_property(handleTbl, maybeUT, "is_hidden", isHidden_lua, setHidden_lua);
        set_property(handleTbl, maybeUT, "tab_priority", getTabPriority_lua, setTabPriority_lua);
        set_property(handleTbl, maybeUT, "tab_enabled", isTabEnabled_lua, setTabEnabled_lua);
        set_property(handleTbl, maybeUT, "left", getLeft_lua, setLeft_lua);
        set_property(handleTbl, maybeUT, "right", getRight_lua, setRight_lua);
        set_property(handleTbl, maybeUT, "top", getTop_lua, setTop_lua);
        set_property(handleTbl, maybeUT, "bottom", getBottom_lua, setBottom_lua);
        set_property(handleTbl, maybeUT, "local_left", getLocalLeft_lua, setLocalLeft_lua);
        set_property(handleTbl, maybeUT, "local_right", getLocalRight_lua, setLocalRight_lua);
        set_property(handleTbl, maybeUT, "local_top", getLocalTop_lua, setLocalTop_lua);
        set_property(handleTbl, maybeUT, "local_bottom", getLocalBottom_lua, setLocalBottom_lua);        
        set_property(handleTbl, maybeUT, "orphan_grace", getOrphanGrace_lua, setOrphanGrace_lua);

        // ☐ set_property(handleTbl, maybeUT, "orphan_policy", getOrphanRetentionPolicyString_lua, setOrphanRetentionPolicy_lua);


        // Color properties Work in progress
        bind_color_property(handleTbl, maybeUT, "color",
            [](IDisplayObject* o){ return o->getColor(); },
            [](IDisplayObject* o, const SDL_Color& c){ o->setColor(c); });

        bind_color_property(handleTbl, maybeUT, "foreground_color",
            [](IDisplayObject* o){ return o->getForegroundColor(); },
            [](IDisplayObject* o, const SDL_Color& c){ o->setForegroundColor(c); });

        bind_color_property(handleTbl, maybeUT, "background_color",
            [](IDisplayObject* o){ return o->getBackgroundColor(); },
            [](IDisplayObject* o, const SDL_Color& c){ o->setBackgroundColor(c); });

        bind_color_property(handleTbl, maybeUT, "border_color",
            [](IDisplayObject* o){ return o->getBorderColor(); },
            [](IDisplayObject* o, const SDL_Color& c){ o->setBorderColor(c); });

        bind_color_property(handleTbl, maybeUT, "outline_color",
            [](IDisplayObject* o){ return o->getOutlineColor(); },
            [](IDisplayObject* o, const SDL_Color& c){ o->setOutlineColor(c); });

        bind_color_property(handleTbl, maybeUT, "dropshadow_color",
            [](IDisplayObject* o){ return o->getDropshadowColor(); },
            [](IDisplayObject* o, const SDL_Color& c){ o->setDropshadowColor(c); });


        
        // auto color_getter = [](DisplayHandle& self) -> sol::table {
        //     sol::state_view lua = getLua();
        //     sol::table t = lua.create_table();
        //     if (!self.isValid() || !self.get()) return t;
        //     IDisplayObject* obj = self.get();
        //     SDL_Color c = obj->getColor();
        //     return SDL_Utils::get_lua_color(lua, c);
        // };

        // auto color_setter = [](DisplayHandle& self, const sol::object& val) {
        //     if (!self.isValid() || !self.get()) return;
        //     IDisplayObject* obj = self.get();
        //     // Table -> SDL_Color
        //     if (val.is<sol::table>()) {
        //         sol::table t = val.as<sol::table>();
        //         SDL_Color c = SDL_Utils::get__lua_color(t);
        //         obj->setColor(c);
        //     }
        //     // SDL_Color userdata
        //     else if (val.is<SDL_Color>()) {
        //         try { SDL_Color c = val.as<SDL_Color>(); obj->setColor(c); } catch(...) {}
        //     }
        //     // Single number -> grayscale
        //     else if (val.is<int>() || val.is<double>()) {
        //         int v = val.is<int>() ? val.as<int>() : static_cast<int>(val.as<double>());
        //         SDL_Color c{static_cast<Uint8>(v), static_cast<Uint8>(v), static_cast<Uint8>(v), 255};
        //         obj->setColor(c);
        //     }
        // };     
        
        // // Ensure 'color' property is not already set
        // sol::object cur = handleTbl.raw_get_or("color", sol::lua_nil);
        // if (!cur.valid() || cur == sol::lua_nil) {
        //     handleTbl.set("color", sol::property(color_getter, color_setter));
        // }
        // if (maybeUT) {
        //     try { (*maybeUT)["color"] = sol::property(color_getter, color_setter); } catch(...) {}
        // }
        // // set_property(handleTbl, maybeUT, "color", getColor_lua, setColor_lua);

    }

} // END: namespace SDOM
