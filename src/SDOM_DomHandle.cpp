// SDOM_DomHandle.cpp

#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_DomHandle.hpp>

namespace SDOM
{
    // Implementation of static helper declared in header
    DomHandle DomHandle::resolveChildSpec(const sol::object& spec)
    {
        DomHandle resolved;
        if (!spec.valid()) return resolved;
        if (spec.is<std::string>()) {
            resolved = Core::getInstance().getDisplayObjectHandle(spec.as<std::string>());
        } else if (spec.is<DomHandle>()) {
            resolved = spec.as<DomHandle>();
        } else if (spec.is<sol::table>()) {
            sol::table t = spec.as<sol::table>();
            if (t["child"].valid()) {
                if (t["child"].get_type() == sol::type::string) resolved = Core::getInstance().getDisplayObjectHandle(t["child"].get<std::string>());
                else resolved = t["child"].get<DomHandle>();
            } else if (t["name"].valid()) {
                if (t["name"].get_type() == sol::type::string) resolved = Core::getInstance().getDisplayObjectHandle(t["name"].get<std::string>());
            }
        }
        return resolved;
    }

    IDisplayObject* SDOM::DomHandle::get() const 
    {
        if (!factory_) return nullptr;
        return factory_->getDomObj(name_);
    }

    // --- LUA Registsration --- //



    void DomHandle::_registerLua(const std::string& typeName, sol::state_view lua)
    {
        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "DomHandle";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                        << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN << typeName << CLR::RESET << std::endl;
        }
        // 1. Create and save a dedicated DomHandle usertype (do NOT reuse the
        // concrete object typeName here - that would conflict with the
        // IDisplayObject usertype registration which uses the concrete
        // typeName). Register the handle under the global name "DomHandle".
        sol::usertype<DomHandle> objHandleType = lua.new_usertype<DomHandle>(
            "DomHandle",
            sol::constructors<DomHandle(), DomHandle(const std::string&, const std::string&)>(),
            sol::base_classes, sol::bases<IDataObject>()
        );
        this->objHandleType_ = objHandleType; // Save in IDataObject

        // Ensure Bounds is available as a userdata in Lua. Register once per-state.
        if (!lua["Bounds"].valid()) {
            lua.new_usertype<Bounds>("Bounds",
                "left", &Bounds::left,
                "top", &Bounds::top,
                "right", &Bounds::right,
                "bottom", &Bounds::bottom,
                // Compatibility helpers: integer-style fields like the previous table
                "x", sol::property([](const Bounds& b) { return static_cast<int>(b.left); }),
                "y", sol::property([](const Bounds& b) { return static_cast<int>(b.top); }),
                "width", sol::property([](const Bounds& b) { return static_cast<int>(b.width()); }),
                "height", sol::property([](const Bounds& b) { return static_cast<int>(b.height()); }),
                // Also expose float getters if users want the precise values
                "widthf", &Bounds::width,
                "heightf", &Bounds::height,
                // Method-style getters for Lua (b:getLeft(), b:getX(), ...)
                "getLeft", [](const Bounds& b) { return b.left; },
                "getTop", [](const Bounds& b) { return b.top; },
                "getRight", [](const Bounds& b) { return b.right; },
                "getBottom", [](const Bounds& b) { return b.bottom; },
                "getX", [](const Bounds& b) { return static_cast<int>(b.left); },
                "getY", [](const Bounds& b) { return static_cast<int>(b.top); },
                "getWidth", [](const Bounds& b) { return static_cast<int>(b.width()); },
                "getHeight", [](const Bounds& b) { return static_cast<int>(b.height()); },
                // epsilon-aware comparison helper: b:almostEqual(other, eps)
                "almostEqual", [](const Bounds& a, const Bounds& other, sol::optional<float> eps) {
                    float e = eps ? *eps : 0.0001f;
                    return std::abs(a.left - other.left) <= e
                        && std::abs(a.top - other.top) <= e
                        && std::abs(a.right - other.right) <= e
                        && std::abs(a.bottom - other.bottom) <= e;
                }
            );
        }

        // Ensure SDL_Color is available as a userdata in Lua. Register once per-state.
        if (!lua["SDL_Color"].valid()) {
            lua.new_usertype<SDL_Color>("SDL_Color",
                "r", &SDL_Color::r,
                "g", &SDL_Color::g,
                "b", &SDL_Color::b,
                "a", &SDL_Color::a,
                // Method-style getters
                "getR", [](const SDL_Color& c) { return c.r; },
                "getG", [](const SDL_Color& c) { return c.g; },
                "getB", [](const SDL_Color& c) { return c.b; },
                "getA", [](const SDL_Color& c) { return c.a; },
                // epsilon-aware comparison helper (byte-wise difference)
                "almostEqual", [](const SDL_Color& a, const SDL_Color& b, sol::optional<int> eps) {
                    int e = eps ? *eps : 1;
                    return std::abs(static_cast<int>(a.r) - static_cast<int>(b.r)) <= e
                        && std::abs(static_cast<int>(a.g) - static_cast<int>(b.g)) <= e
                        && std::abs(static_cast<int>(a.b) - static_cast<int>(b.b)) <= e
                        && std::abs(static_cast<int>(a.a) - static_cast<int>(b.a)) <= e;
                }
            );
        }

        // 2. Call base registration
        SUPER::_registerLua(typeName, lua);

        // 3. Register properties/commands (custom logic)
        // Note: do NOT register basic handle accessors via the Factory here
        // because they would be applied to the concrete type's registry and
        // potentially overwrite the direct method bindings below on the
        // DomHandle usertype. The direct bindings (get/isValid/getName/getType)
        // are applied explicitly further down.

        // NOTE: registry-driven properties/commands will be applied after we
        // register any forwarding commands/functions below so they are present
        // when we bind the registry into the usertype.

        // 4. Register meta functions
        objHandleType[sol::meta_function::to_string] = [](DomHandle& h) 
        {
            auto obj = h.get();
            if (obj)
            {
                return "DomHandle(" + obj->getName() + ":" + obj->getType() + ")";
            }
            return std::string("DomHandle(invalid)");
        };

        // 5. Register direct bindings for basic handle accessors
        // Ensure direct method bindings exist so method calls (e.g. h:get())
        // operate on the userdata. Return the DomHandle itself when calling
        // :get() from Lua so callers receive the handle userdata which has
        // DomHandle-specific methods (including forwarded addChild/removeChild).
        objHandleType_["get"] = [](DomHandle& h) -> DomHandle { return h; };
        objHandleType_["isValid"] = static_cast<bool(DomHandle::*)() const>(&DomHandle::isValid);
        // Forward getName/getType to the underlying object when present so Lua sees
        // the current object's name/type even if the DomHandle's cached fields differ.
        objHandleType_["getName"] = [](DomHandle& h) -> std::string {
            if (auto* obj = h.get()) return obj->getName();
            return h.getName();
        };
        objHandleType_["getType"] = [](DomHandle& h) -> std::string {
            if (auto* obj = h.get()) return obj->getType();
            return h.getType();
        };
        // Allow Lua to set name/type directly on the handle
        objHandleType_["setName"] = [](DomHandle& h, const std::string& newName) {
            if (auto* obj = h.get()) { obj->setName(newName); }
            // update the handle's cached name so subsequent getName() on the
            // DomHandle reflects the change even if no underlying object exists
            h.name_ = newName;
        };
        objHandleType_["setType"] = [](DomHandle& h, const std::string& newType) {
            if (auto* obj = h.get()) { obj->setType(newType); }
            h.type_ = newType;
        };

        // 6. Bind forwarding commands directly on the DomHandle usertype so
        // they receive parameters in the usual Lua calling convention
        // (self, arg). Using direct bindings avoids mismatches when the
        // registry-stored std::function signatures include extra parameters
        // like sol::state_view which Sol2 may not supply automatically.
        objHandleType_["addChild"] = [](DomHandle& dh, sol::object arg) {
            IDisplayObject* target = dh.get();
            if (!target) return;
            if (arg.is<DomHandle>()) {
                target->addChild(arg.as<DomHandle>());
            } else if (arg.is<sol::table>()) {
                sol::table t = arg.as<sol::table>();
                if (t["child"].valid()) {
                    target->addChild(t["child"].get<DomHandle>());
                }
            }
        };

        // Child-spec resolver: use the centralized DomHandle::resolveChildSpec

        // Forwarded setParent: accept a DomHandle, a string name, or a table { parent = ... }
        objHandleType_["setParent"] = [](DomHandle& dh, sol::object arg) {
            IDisplayObject* child = dh.get();
            if (!child) return;

            DomHandle parentHandle;
            if (arg.is<std::string>()) {
                std::string name = arg.as<std::string>();
                parentHandle = Core::getInstance().getDisplayObjectHandle(name);
            } else if (arg.is<DomHandle>()) {
                parentHandle = arg.as<DomHandle>();
            } else if (arg.is<sol::table>()) {
                sol::table t = arg.as<sol::table>();
                if (t["parent"].valid()) {
                    if (t["parent"].get_type() == sol::type::string) {
                        parentHandle = Core::getInstance().getDisplayObjectHandle(t["parent"].get<std::string>());
                    } else {
                        parentHandle = t["parent"].get<DomHandle>();
                    }
                }
            }

            if (!parentHandle) {
                ERROR("DomHandle:setParent: resolved parent is invalid");
                return;
            }

            // Remove child from current parent if present
            DomHandle oldParent = child->getParent();
            if (oldParent) {
                if (auto* oldParentObj = dynamic_cast<IDisplayObject*>(oldParent.get())) {
                    // use removeChild to update parent's children and handle orphaning
                    oldParentObj->removeChild(dh);
                }
            }

            // Add child to new parent (this will set child's parent and add to parent's children)
            if (auto* newParentObj = dynamic_cast<IDisplayObject*>(parentHandle.get())) {
                newParentObj->addChild(dh);
            }
        };

        // Forward addEventListener so DomHandle:addEventListener({...}) works from Lua
    objHandleType_["addEventListener"] = [](DomHandle& dh, sol::object args) {
            IDisplayObject* target = dh.get();
            if (!target) return;
            if (!args.is<sol::table>()) return;
            sol::table t = args.as<sol::table>();
            if (!t["type"].valid() || !t["listener"].valid()) return;
            EventType& et = t["type"].get<EventType&>();
            sol::function fn = t["listener"].get<sol::function>();
            bool useCapture = t["useCapture"].get_or(false);
            int priority = t["priority"].get_or(0);
            // wrap the sol::function into a std::function
            target->addEventListener(et, [fn](Event& e) { fn(e); }, useCapture, priority);
        };

        objHandleType_["removeChild"] = [](DomHandle& dh, sol::object arg) {
            IDisplayObject* target = dh.get();
            if (!target) return;
            if (arg.is<DomHandle>()) {
                target->removeChild(arg.as<DomHandle>());
            } else if (arg.is<sol::table>()) {
                sol::table t = arg.as<sol::table>();
                if (t["child"].valid()) {
                    target->removeChild(t["child"].get<DomHandle>());
                }
            }
        };

        objHandleType_["hasChild"] = [](DomHandle& dh, sol::object arg) -> bool {
            IDisplayObject* target = dh.get();
            if (!target) return false;
            if (arg.is<DomHandle>()) {
                return target->hasChild(arg.as<DomHandle>());
            } else if (arg.is<sol::table>()) {
                sol::table t = arg.as<sol::table>();
                if (t["child"].valid()) {
                    return target->hasChild(t["child"].get<DomHandle>());
                }
            }
            return false;
        };

        // Expose getParent so Lua can query an object's parent handle
        objHandleType_["getParent"] = [](DomHandle& dh) -> DomHandle {
            if (auto* obj = dh.get()) {
                return obj->getParent();
            }
            return DomHandle();
        };

        // Forwarded property getters for convenience in Lua (stage coordinates, size)
        objHandleType_["getX"] = [](DomHandle& dh) -> int {
            IDisplayObject* target = dh.get(); if (!target) return 0; return target->getX(); };
        objHandleType_["getY"] = [](DomHandle& dh) -> int {
            IDisplayObject* target = dh.get(); if (!target) return 0; return target->getY(); };
        objHandleType_["getWidth"] = [](DomHandle& dh) -> int {
            IDisplayObject* target = dh.get(); if (!target) return 0; return target->getWidth(); };
        objHandleType_["getHeight"] = [](DomHandle& dh) -> int {
            IDisplayObject* target = dh.get(); if (!target) return 0; return target->getHeight(); };
        // Geometry setters
        objHandleType_["setX"] = [](DomHandle& dh, int p_x) {
            if (auto* t = dh.get()) t->setX(p_x);
        };
        objHandleType_["setY"] = [](DomHandle& dh, int p_y) {
            if (auto* t = dh.get()) t->setY(p_y);
        };
        objHandleType_["setWidth"] = [](DomHandle& dh, int width) {
            if (auto* t = dh.get()) t->setWidth(width);
        };
        objHandleType_["setHeight"] = [](DomHandle& dh, int height) {
            if (auto* t = dh.get()) t->setHeight(height);
        };
        // Edge getters
        objHandleType_["getLeft"] = [](DomHandle& dh) -> float {
            IDisplayObject* target = dh.get(); if (!target) return 0.0f; return target->getLeft(); };
        objHandleType_["getTop"] = [](DomHandle& dh) -> float {
            IDisplayObject* target = dh.get(); if (!target) return 0.0f; return target->getTop(); };
        objHandleType_["getRight"] = [](DomHandle& dh) -> float {
            IDisplayObject* target = dh.get(); if (!target) return 0.0f; return target->getRight(); };
        objHandleType_["getBottom"] = [](DomHandle& dh) -> float {
            IDisplayObject* target = dh.get(); if (!target) return 0.0f; return target->getBottom(); };

        // --- Focus & Interactivity forwards for DomHandle --- //
        objHandleType_["setKeyboardFocus"] = [](DomHandle& dh) {
            if (auto* t = dh.get()) t->setKeyboardFocus();
        };
        objHandleType_["isKeyboardFocused"] = [](DomHandle& dh) -> bool {
            if (auto* t = dh.get()) return t->isKeyboardFocused();
            return false;
        };
        objHandleType_["isMouseHovered"] = [](DomHandle& dh) -> bool {
            if (auto* t = dh.get()) return t->isMouseHovered();
            return false;
        };
        objHandleType_["isClickable"] = [](DomHandle& dh) -> bool {
            if (auto* t = dh.get()) return t->isClickable();
            return false;
        };
        objHandleType_["setClickable"] = [](DomHandle& dh, bool clickable) {
            if (auto* t = dh.get()) t->setClickable(clickable);
        };
        objHandleType_["isEnabled"] = [](DomHandle& dh) -> bool {
            if (auto* t = dh.get()) return t->isEnabled();
            return false;
        };
        objHandleType_["setEnabled"] = [](DomHandle& dh, bool enabled) {
            if (auto* t = dh.get()) t->setEnabled(enabled);
        };
        objHandleType_["isHidden"] = [](DomHandle& dh) -> bool {
            if (auto* t = dh.get()) return t->isHidden();
            return false;
        };
        objHandleType_["setHidden"] = [](DomHandle& dh, bool hidden) {
            if (auto* t = dh.get()) t->setHidden(hidden);
        };
        objHandleType_["isVisible"] = [](DomHandle& dh) -> bool {
            if (auto* t = dh.get()) return t->isVisible();
            return false;
        };
        objHandleType_["setVisible"] = [](DomHandle& dh, bool visible) {
            if (auto* t = dh.get()) t->setVisible(visible);
        };

        // --- Tab forwards --- //
        objHandleType_["getTabPriority"] = [](DomHandle& dh) -> int {
            if (auto* t = dh.get()) return t->getTabPriority();
            return 0;
        };
        objHandleType_["setTabPriority"] = [](DomHandle& dh, int idx) {
            if (auto* t = dh.get()) t->setTabPriority(idx);
        };
        objHandleType_["isTabEnabled"] = [](DomHandle& dh) -> bool {
            if (auto* t = dh.get()) return t->isTabEnabled();
            return false;
        };
        objHandleType_["setTabEnabled"] = [](DomHandle& dh, bool enabled) {
            if (auto* t = dh.get()) t->setTabEnabled(enabled);
        };

        // getBounds returns a Bounds userdata (was previously a plain table)
        objHandleType_["getBounds"] = [](sol::this_state ts, DomHandle& dh) -> sol::object {
            sol::state_view lua(ts);
            IDisplayObject* target = dh.get();
            if (!target) return sol::make_object(lua, sol::nil);
            Bounds b = target->getBounds();
            return sol::make_object(lua, b);
        };

        // getColor returns an SDL_Color userdata
        objHandleType_["getColor"] = [](sol::this_state ts, DomHandle& dh) -> sol::object {
            sol::state_view lua(ts);
            IDisplayObject* target = dh.get();
            if (!target) return sol::make_object(lua, sol::nil);
            SDL_Color c = target->getColor();
            return sol::make_object(lua, c);
        };

        // Forward setBounds so DomHandle:setBounds({ left=..., top=..., right=..., bottom=... }) works from Lua
        objHandleType_["setBounds"] = [](DomHandle& dh, sol::object args) {
            IDisplayObject* target = dh.get();
            if (!target) return;
            if (!args.is<sol::table>()) return;
            sol::table t = args.as<sol::table>();
            Bounds b;
            if (t[1].valid() && t[2].valid() && t[3].valid() && t[4].valid()) {
                b.left = t[1].get<float>();
                b.top = t[2].get<float>();
                b.right = t[3].get<float>();
                b.bottom = t[4].get<float>();
            } else {
                b.left = t["left"].get_or(0.0f);
                b.top = t["top"].get_or(0.0f);
                b.right = t["right"].get_or(0.0f);
                b.bottom = t["bottom"].get_or(0.0f);
            }
            target->setBounds(b);
        };

        // Forward setColor so DomHandle:setColor({ r=..., g=..., b=..., a=... }) works from Lua
        objHandleType_["setColor"] = [](DomHandle& dh, sol::object arg) {
            IDisplayObject* target = dh.get();
            if (!target) return;
            if (arg.is<SDL_Color>()) {
                target->setColor(arg.as<SDL_Color>());
            } else if (arg.is<sol::table>()) {
                sol::table t = arg.as<sol::table>();
                SDL_Color c;
                c.r = t["r"].get_or(255);
                c.g = t["g"].get_or(255);
                c.b = t["b"].get_or(255);
                c.a = t["a"].get_or(255);
                target->setColor(c);
            }
        };

        // --- Priority & Z-Order forwards --- //
        objHandleType_["getMaxPriority"] = [](DomHandle& dh) -> int {
            IDisplayObject* target = dh.get(); if (!target) return 0; return target->getMaxPriority(); };
        objHandleType_["getMinPriority"] = [](DomHandle& dh) -> int {
            IDisplayObject* target = dh.get(); if (!target) return 0; return target->getMinPriority(); };
        objHandleType_["getPriority"] = [](DomHandle& dh) -> int {
            IDisplayObject* target = dh.get(); if (!target) return 0; return target->getPriority(); };
        objHandleType_["setToHighestPriority"] = [](DomHandle& dh) {
            IDisplayObject* target = dh.get(); if (!target) return; target->setToHighestPriority(); };
        objHandleType_["setToLowestPriority"] = [](DomHandle& dh) {
            IDisplayObject* target = dh.get(); if (!target) return; target->setToLowestPriority(); };
        objHandleType_["sortChildrenByPriority"] = [](DomHandle& dh) {
            IDisplayObject* target = dh.get(); if (!target) return; target->sortChildrenByPriority(); };
        objHandleType_["setPriority"] = [](DomHandle& dh, sol::object arg1, sol::optional<int> maybePriority) {
            // Overload forms:
            //  h:setPriority(10)
            //  h:setPriority({ child = 'name' }, 10)
            //  h:setPriority(childHandle, 10)
            IDisplayObject* target = dh.get(); if (!target) return;
            int priority = 0;
            DomHandle resolvedChild;

            if (maybePriority) {
                // form: (childSpec, priority)
                priority = *maybePriority;
                sol::object childSpec = arg1;
                if (childSpec.is<std::string>()) {
                    resolvedChild = Core::getInstance().getDisplayObjectHandle(childSpec.as<std::string>());
                } else if (childSpec.is<DomHandle>()) {
                    resolvedChild = childSpec.as<DomHandle>();
                } else if (childSpec.is<sol::table>()) {
                    sol::table t = childSpec.as<sol::table>();
                    if (t["child"].valid()) {
                        if (t["child"].get_type() == sol::type::string) resolvedChild = Core::getInstance().getDisplayObjectHandle(t["child"].get<std::string>());
                        else resolvedChild = t["child"].get<DomHandle>();
                    }
                }
            } else {
                // form: (priority)
                if (arg1.is<int>()) {
                    priority = arg1.as<int>();
                } else if (arg1.is<sol::table>()) {
                    // support h:setPriority({ priority = 5 })
                    sol::table t = arg1.as<sol::table>();
                    priority = t["priority"].get_or(0);
                    if (t["child"].valid()) {
                        if (t["child"].get_type() == sol::type::string) resolvedChild = Core::getInstance().getDisplayObjectHandle(t["child"].get<std::string>());
                        else resolvedChild = t["child"].get<DomHandle>();
                    }
                }
            }

            if (resolvedChild) {
                // delegate to child's setPriority
                if (auto* childObj = dynamic_cast<IDisplayObject*>(resolvedChild.get())) {
                    childObj->setPriority(priority);
                }
            } else {
                // set priority on this object
                target->setPriority(priority);
            }
        };
        objHandleType_["moveToTop"] = [](DomHandle& dh, sol::optional<sol::object> maybeSpec) {
            // Optional target spec: moveToTop() or moveToTop({ child = 'name' }) or moveToTop(childHandle)
            IDisplayObject* target = dh.get(); if (!target) return;
            if (!maybeSpec) { target->moveToTop(); return; }
            sol::object spec = *maybeSpec;
            DomHandle resolvedChild;
            if (spec.is<std::string>()) resolvedChild = Core::getInstance().getDisplayObjectHandle(spec.as<std::string>());
            else if (spec.is<DomHandle>()) resolvedChild = spec.as<DomHandle>();
            else if (spec.is<sol::table>()) {
                sol::table t = spec.as<sol::table>();
                if (t["child"].valid()) {
                    if (t["child"].get_type() == sol::type::string) resolvedChild = Core::getInstance().getDisplayObjectHandle(t["child"].get<std::string>());
                    else resolvedChild = t["child"].get<DomHandle>();
                }
            }
            if (resolvedChild) {
                if (auto* childObj = dynamic_cast<IDisplayObject*>(resolvedChild.get())) childObj->moveToTop();
            }
        };
        objHandleType_["getChildrenPriorities"] = [](sol::this_state ts, DomHandle& dh) -> sol::object {
            sol::state_view lua(ts);
            IDisplayObject* target = dh.get(); if (!target) return sol::make_object(lua, sol::nil);
            std::vector<int> v = target->getChildrenPriorities();
            return sol::make_object(lua, v);
        };
        objHandleType_["getZOrder"] = [](DomHandle& dh) -> int { IDisplayObject* target = dh.get(); if (!target) return 0; return target->getZOrder(); };

        objHandleType_["setZOrder"] = [](DomHandle& dh, sol::object arg, sol::optional<int> maybeZ) {
            IDisplayObject* target = dh.get(); if (!target) return;
            // Forms:
            //  setZOrder(5)
            //  setZOrder({ z = 5 })
            //  setZOrder({ child = 'name', z = 5 })
            //  setZOrder(childHandle, 5)
            int z = 0;
            DomHandle child;
            if (maybeZ) {
                z = *maybeZ;
                child = DomHandle::resolveChildSpec(arg);
            } else {
                if (arg.is<int>()) z = arg.as<int>();
                else if (arg.is<sol::table>()) {
                    sol::table t = arg.as<sol::table>();
                    z = t["z"].get_or(0);
                    child = DomHandle::resolveChildSpec(arg);
                } else {
                    child = DomHandle::resolveChildSpec(arg);
                }
            }
            if (child) {
                if (auto* ch = dynamic_cast<IDisplayObject*>(child.get())) ch->setZOrder(z);
            } else {
                target->setZOrder(z);
            }
        };

        objHandleType_["setToHighestPriority"] = [](DomHandle& dh, sol::optional<sol::object> maybeSpec) {
            if (!maybeSpec) { if (auto* t = dh.get()) t->setToHighestPriority(); return; }
            DomHandle child = DomHandle::resolveChildSpec(*maybeSpec);
            if (child) if (auto* c = dynamic_cast<IDisplayObject*>(child.get())) c->setToHighestPriority();
        };

        objHandleType_["setToLowestPriority"] = [](DomHandle& dh, sol::optional<sol::object> maybeSpec) {
            if (!maybeSpec) { if (auto* t = dh.get()) t->setToLowestPriority(); return; }
            DomHandle child = DomHandle::resolveChildSpec(*maybeSpec);
            if (child) if (auto* c = dynamic_cast<IDisplayObject*>(child.get())) c->setToLowestPriority();
        };

        objHandleType_["sortChildrenByPriority"] = [](DomHandle& dh, sol::optional<sol::object> maybeSpec) {
            // Optionally accept a child spec to indicate which parent's children to sort
            if (!maybeSpec) { if (auto* t = dh.get()) t->sortChildrenByPriority(); return; }
            DomHandle parent = DomHandle::resolveChildSpec(*maybeSpec);
            if (parent) if (auto* p = dynamic_cast<IDisplayObject*>(parent.get())) p->sortChildrenByPriority();
        };

    // Finally, apply any remaining registry-driven properties/commands to
    // the DomHandle usertype for the concrete typeName. This attaches the
    // forwarding commands into the DomHandle metatable so Lua sees them
    // when a handle refers to an instance of that concrete type.
    factory_->registerLuaPropertiesAndCommands(typeName, objHandleType_);
    }


} // END namespace SDOM