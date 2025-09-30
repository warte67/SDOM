// SDOM_DomHandle.cpp

#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_DomHandle.hpp>

namespace SDOM
{
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

        // Forwarded setParent: accept a DomHandle, a string name, or a table { parent = ... }
        objHandleType_["setParent"] = [](DomHandle& dh, sol::object arg) {
            IDisplayObject* child = dh.get();
            if (!child) return;

            DomHandle parentHandle;
            if (arg.is<std::string>()) {
                std::string name = arg.as<std::string>();
                parentHandle = Core::getInstance().getDisplayHandle(name);
            } else if (arg.is<DomHandle>()) {
                parentHandle = arg.as<DomHandle>();
            } else if (arg.is<sol::table>()) {
                sol::table t = arg.as<sol::table>();
                if (t["parent"].valid()) {
                    if (t["parent"].get_type() == sol::type::string) {
                        parentHandle = Core::getInstance().getDisplayHandle(t["parent"].get<std::string>());
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

        // Forwarded property getters for convenience in Lua (stage coordinates, size)
        objHandleType_["getX"] = [](DomHandle& dh) -> int {
            IDisplayObject* target = dh.get(); if (!target) return 0; return target->getX(); };
        objHandleType_["getY"] = [](DomHandle& dh) -> int {
            IDisplayObject* target = dh.get(); if (!target) return 0; return target->getY(); };
        objHandleType_["getWidth"] = [](DomHandle& dh) -> int {
            IDisplayObject* target = dh.get(); if (!target) return 0; return target->getWidth(); };
        objHandleType_["getHeight"] = [](DomHandle& dh) -> int {
            IDisplayObject* target = dh.get(); if (!target) return 0; return target->getHeight(); };

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

    // Finally, apply any remaining registry-driven properties/commands to
    // the DomHandle usertype for the concrete typeName. This attaches the
    // forwarding commands into the DomHandle metatable so Lua sees them
    // when a handle refers to an instance of that concrete type.
    factory_->registerLuaPropertiesAndCommands(typeName, objHandleType_);
    }


} // END namespace SDOM