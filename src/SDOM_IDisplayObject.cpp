/***  SDOM_IDisplayObject.cpp  ****************************
 *   ___ ___   ___  __  __   ___  _         _            ___  _     _        _                  
 *  / __|   \ / _ \|  \/  | |   \(_)____ __| |__ _ _  _ / _ \| |__ (_)___ __| |_   __ _ __ _ __ 
 *  \__ \ |) | (_) | |\/| | | |) | (_-< '_ \ / _` | || | (_) | '_ \| / -_) _|  _|_/ _| '_ \ '_ \
 *  |___/___/ \___/|_|  |_|_|___/|_/__/ .__/_\__,_|\_, |\___/|_.__// \___\__|\__(_)__| .__/ .__/
 *                       |___|        |_|          |__/          |__/                |_|  |_|       
 *  
 * The SDOM_IDisplayObject class is the abstract base class for all visual and interactive 
 * elements in the SDOM framework. It defines the core interface and behavior for display 
 * objects, including rendering, event handling, parent-child hierarchy, anchoring, layout, 
 * and property management. IDisplayObject supports flexible positioning and sizing, z-ordering, 
 * visibility, and interactivity, as well as integration with the event system for robust user 
 * input and event-driven programming. Through its extensible design, it enables the creation of 
 * complex, nested GUI structures and serves as the foundation for all concrete display 
 * objects—such as panels, buttons, images, and labels—within SDOM-based applications.
 * 
 * 
 *   This software is provided 'as-is', without any express or implied
 *   warranty.  In no event will the authors be held liable for any damages
 *   arising from the use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose,
 *   including commercial applications, and to alter it and redistribute it
 *   freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not
 *       claim that you wrote the original software. If you use this software
 *       in a product, an acknowledgment in the product documentation would be
 *       appreciated but is not required.
 *   2. Altered source versions must be plainly marked as such, and must not be
 *       misrepresented as being the original software.
 *   3. This notice may not be removed or altered from any source distribution.
 *
 * Released under the ZLIB License.
 * Original Author: Jay Faries (warte67)
 *
 ******************/

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <SDOM/SDOM_IDisplayObjectAPI.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>
#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Utils.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>

#include <chrono>

namespace SDOM
{
    namespace {
        // Recursively dispatch AddedToStage for an object's subtree
        void dispatchAddedToStageRecursive(const DisplayHandle& node, const DisplayHandle& stageHandle)
        {
            if (!node.isValid()) return;
            IDisplayObject* obj = dynamic_cast<IDisplayObject*>(node.get());
            if (!obj) return;

            auto& eventManager = getCore().getEventManager();
            const auto& children = obj->getChildren();
            for (const auto& ch : children)
            {
                if (!ch.isValid()) continue;
                std::unique_ptr<Event> ev = std::make_unique<Event>(EventType::AddedToStage, ch);
                ev->setRelatedTarget(stageHandle);
                eventManager.dispatchEvent(std::move(ev), ch);
                // Recurse into subtree
                dispatchAddedToStageRecursive(ch, stageHandle);
            }
        }

        // Recursively dispatch RemovedFromStage for an object's subtree
        void dispatchRemovedFromStageRecursive(const DisplayHandle& node, const DisplayHandle& stageHandle)
        {
            if (!node.isValid()) return;
            IDisplayObject* obj = dynamic_cast<IDisplayObject*>(node.get());
            if (!obj) return;

            auto& eventManager = getCore().getEventManager();
            const auto& children = obj->getChildren();
            for (const auto& ch : children)
            {
                if (!ch.isValid()) continue;
                std::unique_ptr<Event> ev = std::make_unique<Event>(EventType::RemovedFromStage, ch);
                ev->setRelatedTarget(stageHandle);
                eventManager.dispatchEvent(std::move(ev), ch);
                // Recurse into subtree
                dispatchRemovedFromStageRecursive(ch, stageHandle);
            }
        }
    } // unnamed namespace

    // IDisplayObject implementation

    // Performance Introspection
    float IDisplayObject::getLastUpdateDelta() const
    {
        return getFactory().getLastUpdateDelta(this);
    }

    float IDisplayObject::getLastRenderDelta() const
    {
        return getFactory().getLastRenderDelta(this);
    }

    IDisplayObject::IDisplayObject(const InitStruct& init)
         : IDataObject()
    {
        name_ = init.name;
        type_ = init.type.empty() ? TypeName : init.type;
        color_ = init.color;

        foregroundColor_ = init.foregroundColor;
        backgroundColor_ = init.backgroundColor;
        borderColor_ = init.borderColor;
        outlineColor_ = init.outlineColor;
        dropshadowColor_ = init.dropshadowColor;

        z_order_ = init.z_order;
        priority_ = init.priority;
        isClickable_ = init.isClickable;
        isEnabled_ = init.isEnabled;
        isHidden_ = init.isHidden;
        background_ = init.hasBackground;
        border_ = init.hasBorder;
        tabPriority_ = init.tabPriority;
        tabEnabled_ = init.tabEnabled;
        anchorTop_ = init.anchorTop;
        anchorLeft_ = init.anchorLeft;
        anchorBottom_ = init.anchorBottom;
        anchorRight_ = init.anchorRight;
        left_ = init.x;
        right_ = init.x + init.width;
        top_ = init.y;
        bottom_ = init.y + init.height;
        parent_ = nullptr;

    }

    IDisplayObject::IDisplayObject(const sol::table& config)
        : IDataObject()
    {
        InitStruct init_default; // default initialization values

        // --- Initialization Lambdas --- //
        auto get_str = [&](const char* k, const std::string& d = "") -> std::string 
        {
            return config[k].valid() ? config[k].get<std::string>() : d;
        };
        auto get_int = [&](const char* k, int d = 0) -> int 
        {
            return config[k].valid() ? config[k].get<int>() : d;
        };
        auto get_float = [&](const char* k, float d = 0.0f) -> float {
            if (!config[k].valid()) return d;
            sol::object o = config[k];
            if (o.is<double>()) return static_cast<float>(o.as<double>());
            if (o.is<int>()) return static_cast<float>(o.as<int>());
            if (o.is<std::string>()) {
                std::string s = o.as<std::string>();
                if (s.empty()) return d;
                return std::stof(s);
            }
            return d;
        };
        auto read_color = [&](const char* k, SDL_Color d = {255,0,255,255}) -> SDL_Color 
        {
            // if key missing, return default
            if (!config[k].valid()) return d;

            // if present but not a table, return default to avoid conversion errors
            sol::object o = config[k];
            if (!o.is<sol::table>()) return d;

            sol::table t = o.as<sol::table>();
            SDL_Color c = d;

            // Prefer array-style [r,g,b,a] if present
            if (t[1].valid()) 
            {
                c.r = static_cast<Uint8>(t[1].get<int>());
                if (t[2].valid()) c.g = static_cast<Uint8>(t[2].get<int>());
                if (t[3].valid()) c.b = static_cast<Uint8>(t[3].get<int>());
                if (t[4].valid()) c.a = static_cast<Uint8>(t[4].get<int>());
                return c;
            }

            // Named fields: preserve defaults for any missing channel
            if (t["r"].valid()) c.r = static_cast<Uint8>(t["r"].get<int>());
            if (t["g"].valid()) c.g = static_cast<Uint8>(t["g"].get<int>());
            if (t["b"].valid()) c.b = static_cast<Uint8>(t["b"].get<int>());
            if (t["a"].valid()) c.a = static_cast<Uint8>(t["a"].get<int>());

            return c;
        };

        auto get_bool = [&](const char* k, bool d = false) -> bool 
        {
            return config[k].valid() ? config[k].get<bool>() : d;
        };  

        // --- Required Properties --- //
        name_ = config["name"].get_or(std::string(TypeName));
        type_ = config["type"].get_or(std::string(TypeName));

        // fetch coordinates:
        float x = get_float("x",     init_default.x);
        float y = get_float("y",     init_default.y);
        float width  = config["width"].valid()  ? get_float("width",  init_default.width)
                      : config["w"].valid()     ? get_float("w",      init_default.width)
                      : init_default.width;
        float height = config["height"].valid() ? get_float("height", init_default.height)
                      : config["h"].valid()     ? get_float("h",      init_default.height)
                      : init_default.height;
        // set coordinates:
        setX( x );
        setY( y );
        setWidth(  width );
        setHeight( height );
        if (config["left"].valid()) setLeft(get_float("left", x));
        if (config["top"].valid()) setTop(get_float("top", y));
        if (config["right"].valid()) setRight(get_float("left") + get_float("right", x + width));
        if (config["bottom"].valid()) setBottom(get_float("top") + get_float("bottom", y + height));

        // set color
        color_ = read_color("color", init_default.color);
        foregroundColor_ = read_color("foreground_color", init_default.foregroundColor);
        backgroundColor_ = read_color("background_color", init_default.backgroundColor);
        borderColor_ = read_color("border_color", init_default.borderColor);
        outlineColor_ = read_color("outline_color", init_default.outlineColor);
        dropshadowColor_ = read_color("dropshadow_color", init_default.dropshadowColor);

        // --- Optional Properties --- //
        setAnchorLeft(  stringToAnchorPoint_.at(normalizeAnchorString( get_str("anchor_left", 
                        anchorPointToString_.at(init_default.anchorLeft)))));
        setAnchorTop(   stringToAnchorPoint_.at(normalizeAnchorString( get_str("anchor_top", 
                        anchorPointToString_.at(init_default.anchorTop)))));
        setAnchorRight( stringToAnchorPoint_.at(normalizeAnchorString( get_str("anchor_right", 
                        anchorPointToString_.at(init_default.anchorRight)))));
        setAnchorBottom(stringToAnchorPoint_.at(normalizeAnchorString( get_str("anchor_bottom", 
                        anchorPointToString_.at(init_default.anchorBottom)))));

        setZOrder(      get_int("z_order",      init_default.z_order     ));
        setPriority(    get_int("priority",     init_default.priority    ));
        setClickable(   get_bool("clickable",   init_default.isClickable ));
        setEnabled(     get_bool("is_enabled",  init_default.isEnabled   ));
        setHidden(      get_bool("is_hidden",   init_default.isHidden    ));
        setTabPriority( get_int("tab_priority", init_default.tabPriority ));
        setTabEnabled(  get_bool("tab_enabled", init_default.tabEnabled  ));
        setBackground(  get_bool("background", init_default.hasBackground) );
        setBorder(      get_bool("border",     init_default.hasBorder) );

    } // END IDisplayObject::IDisplayObject(const sol::table& config)


    // Defaults-aware Lua constructor: uses provided defaults as the baseline instead of the
    // internal InitStruct defaults. This lets derived classes pass their InitStruct so that
    // base parsing will honor derived defaults when Lua omits keys.
    IDisplayObject::IDisplayObject(const sol::table& config, const InitStruct& defaults)
        : IDataObject()
    {
        InitStruct init_default = defaults;

        // Reuse the same lambdas as the primary Lua ctor by copying logic here.
        auto get_str = [&](const char* k, const std::string& d = "") -> std::string 
        {
            return config[k].valid() ? config[k].get<std::string>() : d;
        };
        auto get_int = [&](const char* k, int d = 0) -> int 
        {
            return config[k].valid() ? config[k].get<int>() : d;
        };
        auto get_float = [&](const char* k, float d = 0.0f) -> float {
            if (!config[k].valid()) return d;
            sol::object o = config[k];
            if (o.is<double>()) return static_cast<float>(o.as<double>());
            if (o.is<int>()) return static_cast<float>(o.as<int>());
            if (o.is<std::string>()) {
                std::string s = o.as<std::string>();
                if (s.empty()) return d;
                return std::stof(s);
            }
            return d;
        };
        auto read_color = [&](const char* k, SDL_Color d = {255,0,255,255}) -> SDL_Color 
        {
            if (!config[k].valid()) return d;
            sol::object o = config[k];
            if (!o.is<sol::table>()) return d;
            sol::table t = o.as<sol::table>();
            SDL_Color c = d;
            if (t[1].valid()) 
            {
                c.r = static_cast<Uint8>(t[1].get<int>());
                if (t[2].valid()) c.g = static_cast<Uint8>(t[2].get<int>());
                if (t[3].valid()) c.b = static_cast<Uint8>(t[3].get<int>());
                if (t[4].valid()) c.a = static_cast<Uint8>(t[4].get<int>());
                return c;
            }
            if (t["r"].valid()) c.r = static_cast<Uint8>(t["r"].get<int>());
            if (t["g"].valid()) c.g = static_cast<Uint8>(t["g"].get<int>());
            if (t["b"].valid()) c.b = static_cast<Uint8>(t["b"].get<int>());
            if (t["a"].valid()) c.a = static_cast<Uint8>(t["a"].get<int>());
            return c;
        };
        auto get_bool = [&](const char* k, bool d = false) -> bool 
        {
            return config[k].valid() ? config[k].get<bool>() : d;
        };

        // --- Required Properties --- //
        name_ = config["name"].get_or(std::string(TypeName));
        type_ = config["type"].get_or(std::string(TypeName));

        // fetch coordinates:
        float x = get_float("x",     init_default.x);
        float y = get_float("y",     init_default.y);
        float width  = config["width"].valid()  ? get_float("width",  init_default.width)
                      : config["w"].valid()     ? get_float("w",      init_default.width)
                      : init_default.width;
        float height = config["height"].valid() ? get_float("height", init_default.height)
                      : config["h"].valid()     ? get_float("h",      init_default.height)
                      : init_default.height;
        // set coordinates:
        setX( x );
        setY( y );
        setWidth(  width );
        setHeight( height );
        if (config["left"].valid()) setLeft(get_float("left", x));
        if (config["top"].valid()) setTop(get_float("top", y));
        if (config["right"].valid()) setRight(get_float("left") + get_float("right", x + width));
        if (config["bottom"].valid()) setBottom(get_float("top") + get_float("bottom", y + height));

        // set color
        color_ = read_color("color", init_default.color);
        foregroundColor_ = read_color("foreground_color", init_default.foregroundColor);
        backgroundColor_ = read_color("background_color", init_default.backgroundColor);
        borderColor_ = read_color("border_color", init_default.borderColor);
        outlineColor_ = read_color("outline_color", init_default.outlineColor);
        dropshadowColor_ = read_color("dropshadow_color", init_default.dropshadowColor);

        // --- Optional Properties --- //
        setAnchorLeft(  stringToAnchorPoint_.at(normalizeAnchorString( get_str("anchor_left", 
                        anchorPointToString_.at(init_default.anchorLeft)))));
        setAnchorTop(   stringToAnchorPoint_.at(normalizeAnchorString( get_str("anchor_top", 
                        anchorPointToString_.at(init_default.anchorTop)))));
        setAnchorRight( stringToAnchorPoint_.at(normalizeAnchorString( get_str("anchor_right", 
                        anchorPointToString_.at(init_default.anchorRight)))));
        setAnchorBottom(stringToAnchorPoint_.at(normalizeAnchorString( get_str("anchor_bottom", 
                        anchorPointToString_.at(init_default.anchorBottom)))));
        setZOrder(      get_int("z_order",      init_default.z_order     ));
        setPriority(    get_int("priority",     init_default.priority    ));
        setClickable(   get_bool("clickable",   init_default.isClickable ));
        setEnabled(     get_bool("is_enabled",  init_default.isEnabled   ));
        setHidden(      get_bool("is_hidden",   init_default.isHidden    ));
        setTabPriority( get_int("tab_priority", init_default.tabPriority ));
        setTabEnabled(  get_bool("tab_enabled", init_default.tabEnabled  ));
        setBackground(  get_bool("background", init_default.hasBackground) );
        setBorder(      get_bool("border",     init_default.hasBorder) );

    } // END IDisplayObject::IDisplayObject(const sol::table& config, const InitStruct& defaults)

    IDisplayObject::~IDisplayObject()
    {
        // nameRegistry_.erase(getName());
        // Important: Do NOT call virtual cleanup hooks from the destructor.
        // Owners (Factory/Core) should call shutdown() before destroying objects
        // so that derived virtual onQuit() implementations run while the object
        // is still fully-formed. Previously this destructor invoked onQuit()
        // which triggered analyzer noise and can be unsafe in derived types.
    }

    bool IDisplayObject::onInit()
    {
        // Default implementation, can be overridden by derived classes
        // registerSelfName(); // Register this object in the name registry
        return true; // Indicate successful initialization
    }


    void IDisplayObject::onQuit()
    {
        // Default implementation, can be overridden by derived classes
    }

    // Public non-virtual lifecycle helpers ----------------------------------
    bool IDisplayObject::startup()
    {
        if (started_) return true;
        bool ok = false;
        try {
            ok = onInit();
        } catch(...) { ok = false; }
        if (ok) started_ = true;
        return ok;
    }

    void IDisplayObject::shutdown()
    {
        if (!started_) return; // nothing to do
        try {
            onQuit();
        } catch(...) {}
        started_ = false;
    }

    void IDisplayObject::onUpdate(float fElapsedTime)
    {
        // Default implementation, can be overridden by derived classes
        (void)fElapsedTime; // Unused --- IGNORE ---
    }   

    void IDisplayObject::onEvent(const Event& event)
    {
        // Default implementation, can be overridden by derived classes
        (void)event; // Unused --- IGNORE ---
    }

    void IDisplayObject::attachChild_(DisplayHandle p_child, DisplayHandle p_parent, bool useWorld, int worldX, int worldY)
    {
        IDisplayObject* child = dynamic_cast<IDisplayObject*>(p_child.get());
        IDisplayObject* parent = dynamic_cast<IDisplayObject*>(p_parent.get());
        if (child && parent) 
        {
            auto& parentChildren = parent->children_;
            auto it = std::find(parentChildren.begin(), parentChildren.end(), p_child);

            // Also prevent adding a child with the same name as an existing child
            bool nameExists = false;
            std::string newName = p_child.getName();
            for (const auto& existing : parentChildren) {
                if (existing && existing.getName() == newName) {
                    nameExists = true;
                    break;
                }
            }

            if (it == parentChildren.end() && !nameExists) 
            {
                // Record world edges BEFORE changing parent
                float leftWorld = child->getLeft();
                float rightWorld = child->getRight();
                float topWorld = child->getTop();
                float bottomWorld = child->getBottom();

                child->setParent(p_parent);

                // setParent may already add the child to the new parent's children_ vector.
                // Only push_back if it's not present to avoid duplicate entries.
                auto it_after = std::find(parentChildren.begin(), parentChildren.end(), p_child);
                if (it_after == parentChildren.end()) 
                {
                    parentChildren.push_back(p_child);
                }

                if (useWorld)
                {
                    child->setLeft(leftWorld);
                    child->setRight(rightWorld);
                    child->setTop(topWorld);
                    child->setBottom(bottomWorld);
                    child->setX(worldX);
                    child->setY(worldY);
                }

                // Dispatch EventType::Added using dispatchEvent for proper capture/bubble/target
                if (child) 
                {
                    auto& eventManager = getCore().getEventManager();
                    std::unique_ptr<Event> addedEvent = std::make_unique<Event>(EventType::Added, p_child);
                    // Set related target to parent handle for listener use
                    addedEvent->setRelatedTarget(p_parent);
                    eventManager.dispatchEvent(std::move(addedEvent), p_child);
                }

                // Dispatch EventType::AddedToStage if the child is now part of the active stage.
                DisplayHandle stageHandle = getCore().getRootNode();
                if (stageHandle) 
                {
                    DisplayHandle cur = p_child;
                    bool onStage = false;
                    while (cur) 
                    {
                        if (cur == stageHandle) 
                        {
                            onStage = true;
                            break;
                        }
                        IDisplayObject* curObj = dynamic_cast<IDisplayObject*>(cur.get());
                        if (!curObj) break;
                        cur = curObj->getParent();
                    }
                    if (onStage) 
                    {
                        auto& eventManager = getCore().getEventManager();
                        std::unique_ptr<Event> addedToStage = std::make_unique<Event>(EventType::AddedToStage, p_child);
                        addedToStage->setRelatedTarget(stageHandle);
                        eventManager.dispatchEvent(std::move(addedToStage), p_child);

                        // Propagate AddedToStage to entire subtree now connected to stage
                        dispatchAddedToStageRecursive(p_child, stageHandle);
                    }
                }
            }
        }
    }


    void IDisplayObject::removeOrphan_(const DisplayHandle& orphan) 
    {
        IDisplayObject* orphanObj = dynamic_cast<IDisplayObject*>(orphan.get());
        if (orphanObj) 
        {
            IDisplayObject* parent = dynamic_cast<IDisplayObject*>(orphanObj->getParent().get());
            if (parent) 
            {
                auto& parentChildren = parent->children_;
                auto it = std::find(parentChildren.begin(), parentChildren.end(), orphan);
                if (it != parentChildren.end()) 
                {
                    // Capture parent handle before resetting
                    DisplayHandle parentHandle = orphanObj->getParent();
                    // Reset orphan's parent using DisplayHandle (nullptr).
                    // setParent(nullptr) will remove the child from the old parent's children_ vector.
                    orphanObj->setParent(DisplayHandle());

                    // Dispatch EventType::Removed using dispatchEvent for proper capture/bubble/target
                    if (orphan) {
                        auto& eventManager = getCore().getEventManager();
                        std::unique_ptr<Event> removedEvent = std::make_unique<Event>(EventType::Removed, orphan);
                        removedEvent->setRelatedTarget(parentHandle);
                        eventManager.dispatchEvent(std::move(removedEvent), orphan);
                    }

                    // Dispatch EventType::RemovedFromStage if the orphan was on the active stage
                    // prior to removal (parent chain included the stage).
                    DisplayHandle stageHandle = getCore().getRootNode();
                    if (stageHandle) 
                    {
                        DisplayHandle cur = parentHandle;
                        bool wasOnStage = false;
                        while (cur) 
                        {
                            if (cur == stageHandle) { wasOnStage = true; break; }
                            IDisplayObject* curObj = dynamic_cast<IDisplayObject*>(cur.get());
                            if (!curObj) break;
                            cur = curObj->getParent();
                        }
                        if (wasOnStage) 
                        {
                            auto& eventManager = getCore().getEventManager();
                            std::unique_ptr<Event> removedFromStage = std::make_unique<Event>(EventType::RemovedFromStage, orphan);
                            removedFromStage->setRelatedTarget(stageHandle);
                            eventManager.dispatchEvent(std::move(removedFromStage), orphan);

                            // Propagate RemovedFromStage to entire subtree detached from stage
                            dispatchRemovedFromStageRecursive(orphan, stageHandle);
                        }
                    }
                }
            }
        }
    }

    void IDisplayObject::addChild(DisplayHandle child, bool useWorld, int worldX, int worldY)
    {
        if (!child) 
        {
            ERROR("Attempted to add a null child to IDisplayObject: " + name_);
            return;
        }
        Core* core = &Core::getInstance();
        Factory* factory = &core->getFactory();
        if (core->getIsTraversing())
        {
            factory->addToFutureChildrenList(child, DisplayHandle(getName(), child->getType()), useWorld, worldX, worldY);
        }
        else
        {
            attachChild_(child, DisplayHandle(getName(), getType()), useWorld, worldX, worldY);
        }
    }

    DisplayHandle IDisplayObject::getChild(std::string name) const 
    {
        for (const auto& child : children_) 
        {
            if (!child.isValid()) continue;
            if (child->getName() == name) return child;
            if (child.getName() == name) return child;
        }
        return DisplayHandle(); // invalid handle
    }    

    bool IDisplayObject::removeChild(DisplayHandle child)
    {
        if (!child)
        {
            ERROR("removeChild: child handle is null in " + name_);
            return false;
        }
        if (child == getCore().getRootNode())
        {
            // ERROR("removeChild: cannot remove the root node: " + name_);
            return false;
        }

        // Verify that this object is indeed the parent of the provided child.
        auto it = std::find(children_.begin(), children_.end(), child);
        if (it == children_.end())
        {
            ERROR("removeChild: child not found in children_ vector of " + name_);
            return false;
        }

        Core* core = &Core::getInstance();
        Factory* factory = &core->getFactory();

        // If currently traversing, defer the removal to avoid mutating
        // the children_ storage while iterating.
        if (core->getIsTraversing())
        {
            factory->addToOrphanList(child);
            return true;
        }

        // Detach and dispatch lifecycle events now (safe when not traversing)
        removeOrphan_(child);

        // For policies that defer destruction (e.g., GracePeriod), track orphans
        // for later garbage collection. AutoDestroy and RetainUntilManual skip scheduling.
        auto policy = child->getOrphanRetentionPolicy();
        if (policy != OrphanRetentionPolicy::AutoDestroy &&
            policy != OrphanRetentionPolicy::RetainUntilManual)
        {
            child->orphanedAt_ = std::chrono::steady_clock::now();
            factory->addToOrphanList(child);
        }
        return true;
    }    

    const std::vector<DisplayHandle>& IDisplayObject::getChildren() const { return children_; }
    DisplayHandle IDisplayObject::getParent() const { return parent_; }

    IDisplayObject& IDisplayObject::setParent(const DisplayHandle& parent)
    {
        // Preserve world bounds across the parent change
        Bounds world = this->getBounds();

        // If we are traversing the tree, defer the reparent operation
        // to avoid mutating parent/child vectors during iteration.
        if (Core::getInstance().getIsTraversing())
        {
            // Schedule via Factory so it applies after traversal
            int worldX = static_cast<int>(getX());
            int worldY = static_cast<int>(getY());
            DisplayHandle me(getName(), getType());
            Core::getInstance().addToFutureChildrenList(me, parent, /*useWorld*/true, worldX, worldY);
            return *this;
        }

        // Remove from old parent's children_ vector if present
        if (parent_.isValid()) 
        {
            IDisplayObject* oldParentObj = dynamic_cast<IDisplayObject*>(parent_.get());
            if (oldParentObj) 
            {
                DisplayHandle me(getName(), getType());
                auto& vec = oldParentObj->children_;
                vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const DisplayHandle& d) { return d == me; }), vec.end());
            }
        }

        // Assign new parent handle
        parent_ = parent;

        if (getName() == "blueishBox" || (parent_.isValid() && parent_.getName() == "redishBox")) {
            std::ostringstream oss; oss << "[DBG] setParent: child='" << getName() << "' newParent='" << (parent_.isValid() ? parent_.getName() : std::string("<null>")) << "' worldLeft=" << world.left << " worldTop=" << world.top << " worldRight=" << world.right << " worldBottom=" << world.bottom;
            if (parent_.isValid()) {
                IDisplayObject* p = dynamic_cast<IDisplayObject*>(parent_.get());
                if (p) {
                    oss << " parent_left=" << p->left_ << " parent_right=" << p->right_ << " parent_top=" << p->top_ << " parent_bottom=" << p->bottom_;
                }
            }
            // LUA_INFO(oss.str());
        }

        // Add to new parent's children_ vector if not already present
        if (parent_.isValid()) 
        {
            IDisplayObject* newParentObj = dynamic_cast<IDisplayObject*>(parent_.get());
            if (newParentObj) 
            {
                DisplayHandle me(getName(), getType());
                auto& vec = newParentObj->children_;
                // DEBUG_LOG("setParent newParent='" << newParentObj->getName() << "' children_count_before=" << vec.size());
                auto it = std::find(vec.begin(), vec.end(), me);
                if (it == vec.end()) 
                {
                    vec.push_back(me);
                } 
            }
        }

        // Restore world bounds so the object's world position is unchanged
        this->setBounds(world);
        return *this;
    }

    bool IDisplayObject::removeChild(const std::string& name)
    {
        if (name.empty()) {
            ERROR("removeChild: empty name provided for " + name_);
            return false;
        }
        for (const auto& child : children_) {
            if (!child) continue;
            if (child.getName() == name) {
                return removeChild(child);
            }
        }
        ERROR("removeChild: child with name '" + name + "' not found in " + name_);
        return false;
    }
    
    void IDisplayObject::cleanAll() 
    {
        bIsDirty_ = false;
        for (auto& childHandle : children_) 
        {
            if (auto* child = dynamic_cast<IDisplayObject*>(childHandle.get())) 
            {
                child->cleanAll();
            }
        }
    }

    void IDisplayObject::printTree(int depth, bool isLast, const std::vector<bool>& hasMoreSiblings) const
    {
        // Print the current node with indentation and tree characters
        for (size_t i = 0; i < hasMoreSiblings.size(); ++i) 
        {
            std::cout << (hasMoreSiblings[i] ? "│   " : "    ");
        }
        if (depth > 0) 
        {
            std::cout << (isLast ? "└── " : "├── ");
        }
        std::cout << getName() << std::endl;

        // Traverse children using DisplayHandle
        for (size_t i = 0; i < children_.size(); ++i) 
        {
            const auto& childHandle = children_[i];
            if (auto* child = dynamic_cast<IDisplayObject*>(childHandle.get())) 
            {
                auto newHasMoreSiblings = hasMoreSiblings;
                newHasMoreSiblings.push_back(i != children_.size() - 1);
                child->printTree(depth + 1, i == children_.size() - 1, newHasMoreSiblings);
            }
        }
    }

    void IDisplayObject::removeEventListener(
        EventType& type,
        std::function<void(Event&)> listener,
        bool useCapture)
    {
        auto& targetListeners = useCapture ? captureEventListeners : bubblingEventListeners;
        auto it = targetListeners.find(type);
        if (it == targetListeners.end()) return;

        auto& listeners = it->second;
        // If a null/empty function is provided, remove all listeners for this type
        if (!listener) {
            listeners.clear();
            return;
        }

        const std::size_t before = listeners.size();
        listeners.erase(std::remove_if(listeners.begin(), listeners.end(), [&](const ListenerEntry& entry) {
            // Prefer function pointer identity when available
            if (auto* a = entry.listener.target<void(Event&)>())
                if (auto* b = listener.target<void(Event&)>())
                    if (*a == *b) return true;
            if (auto* ca = entry.listener.target<void(const Event&)>())
                if (auto* cb = listener.target<void(const Event&)>())
                    if (*ca == *cb) return true;

            // Fallback: match by target_type (works for lambdas capturing state)
            // This mirrors prior behavior and enables removal when the same lambda
            // variable is passed back in, even though std::function holds a copy.
            return entry.listener.target_type() == listener.target_type();
        }), listeners.end());

        // Safety fallback: if nothing matched but all remaining entries are the same
        // target_type as the provided listener for this EventType bucket, clear them.
        if (listeners.size() == before && !listeners.empty()) {
            bool allSameType = true;
            const auto& want = listener.target_type();
            for (const auto& entry : listeners) {
                if (entry.listener.target_type() != want) { allSameType = false; break; }
            }
            if (allSameType) listeners.clear();
        }
    }


    void IDisplayObject::addEventListener(
        EventType& type, 
        std::function<void(Event&)> listener, 
        bool useCapture, 
        int priority)
    {
        auto& targetListeners = useCapture ? captureEventListeners : bubblingEventListeners;
        targetListeners[type].push_back({std::move(listener), priority, type});

        // Sort listeners by priority if the new listener has a non-zero priority
        std::sort(targetListeners[type].begin(), targetListeners[type].end(),
            [](const ListenerEntry& a, const ListenerEntry& b) {
                return a.priority > b.priority; // Higher priority first
            });
    }


    void IDisplayObject::triggerEventListeners(Event& event, bool useCapture)
    {
        const auto& targetListeners = useCapture ? captureEventListeners : bubblingEventListeners;

        auto it = targetListeners.find(event.getType());
        if (it != targetListeners.end()) 
        {
            for (const auto& entry : it->second) 
            {
                entry.listener(event);
            }
        }
    }

    bool IDisplayObject::hasEventListener(const EventType& type, bool useCapture) const
    {
        const auto& targetListeners = useCapture ? captureEventListeners : bubblingEventListeners;
        auto it = targetListeners.find(type);
        if (it == targetListeners.end()) return false;
        return !it->second.empty();
    }


    // --- Debug utilities: dump registered listeners to stdout -------------------- //
    namespace {
        static void print_listener_bucket(const char* bucketName,
                                           const std::unordered_map<EventType,
                                                                     std::vector<IDisplayObject::ListenerEntry>,
                                                                     EventTypeHash>& bucket)
        {
            std::cout << bucketName << ": size=" << bucket.size() << std::endl;
            for (const auto& kv : bucket)
            {
                const EventType& et = kv.first;
                const auto& vec = kv.second;
                std::cout << "  - [" << et.getName() << "] listeners=" << vec.size() << std::endl;
                for (std::size_t i = 0; i < vec.size(); ++i)
                {
                    const auto& e = vec[i];
                    const char* typeName = e.listener.target_type().name();
                    std::cout << "      [" << i << "] priority=" << e.priority
                              << " type=" << (typeName ? typeName : "<unknown>")
                              << std::endl;
                }
            }
        }
    } // anonymous namespace

    void IDisplayObject::printCaptureEventListeners() const
    {
        std::cout << "[EventListeners] Object='" << getName() << "' (capture-phase)" << std::endl;
        print_listener_bucket("Capture", captureEventListeners);
    }

    void IDisplayObject::printBubblingEventListeners() const
    {
        std::cout << "[EventListeners] Object='" << getName() << "' (bubbling-phase)" << std::endl;
        print_listener_bucket("Bubbling", bubblingEventListeners);
    }

    
    void IDisplayObject::queue_event(const EventType& type, std::function<void(Event&)> init_payload)
    {
        EventManager& em = getCore().getEventManager();
        DisplayHandle self = getFactory().getDisplayObject(getName());
        auto ev = std::make_unique<Event>(type, self, getCore().getElapsedTime());
        ev->setTarget(self);
        if (init_payload) {
            init_payload(*ev);
        }
        em.addEvent(std::move(ev));
    } 
     

    int IDisplayObject::getMaxPriority() const
    {
        if (children_.empty()) 
            return priority_;
        auto it = std::max_element(
            children_.begin(), children_.end(),
            [](const DisplayHandle& a, const DisplayHandle& b) {
                auto* aObj = dynamic_cast<IDisplayObject*>(a.get());
                auto* bObj = dynamic_cast<IDisplayObject*>(b.get());
                int aPriority = aObj ? aObj->priority_ : std::numeric_limits<int>::min();
                int bPriority = bObj ? bObj->priority_ : std::numeric_limits<int>::min();
                return aPriority < bPriority;
            }
        );
        auto* maxObj = dynamic_cast<IDisplayObject*>(it->get());
        return maxObj ? maxObj->priority_ : priority_;
    }

    int IDisplayObject::getMinPriority() const
    {
        if (children_.empty()) return priority_;
        auto it = std::min_element(
            children_.begin(), children_.end(),
            [](const DisplayHandle& a, const DisplayHandle& b) {
                auto* aObj = dynamic_cast<IDisplayObject*>(a.get());
                auto* bObj = dynamic_cast<IDisplayObject*>(b.get());
                // Null children are considered highest priority
                int aPriority = aObj ? aObj->priority_ : std::numeric_limits<int>::max();
                int bPriority = bObj ? bObj->priority_ : std::numeric_limits<int>::max();
                return aPriority < bPriority;
            }
        );
        auto* minObj = dynamic_cast<IDisplayObject*>(it->get());
        return minObj ? minObj->priority_ : priority_;
    }

    IDisplayObject& IDisplayObject::setToHighestPriority()
    {
        IDisplayObject* parentObj = dynamic_cast<IDisplayObject*>(parent_.get());
        if (parentObj) {
            priority_ = parentObj->getMaxPriority() + 1;
            parentObj->sortChildrenByPriority();
        } else {
            priority_ = getMaxPriority() + 1;
            sortChildrenByPriority(); // Ensure children are sorted if this is the root node
        }
        return *this;
    }

    IDisplayObject& IDisplayObject::setToLowestPriority()
    {
        IDisplayObject* parentObj = dynamic_cast<IDisplayObject*>(parent_.get());
        if (parentObj) {
            priority_ = parentObj->getMinPriority() - 1;
            parentObj->sortChildrenByPriority();
        } else {
            priority_ = getMinPriority() - 1;
            sortChildrenByPriority(); // Ensure children are sorted if this is the root node
        }
        return *this;
    }

    IDisplayObject& IDisplayObject::sortChildrenByPriority()
    {
        // Remove invalid children
        children_.erase(std::remove_if(children_.begin(), children_.end(),
            [](const DisplayHandle& child) {
                return !child;
            }), children_.end());

        // Deduplicate children by name (keep last instance)
        std::unordered_set<std::string> seen;
        std::vector<DisplayHandle> newChildren;
        newChildren.reserve(children_.size());

        for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
            const DisplayHandle& ch = *it;
            if (!ch) continue;

            std::string cname = ch.getName();
            if (cname.empty() || seen.insert(cname).second) {
                newChildren.push_back(ch);
            }
        }

        std::reverse(newChildren.begin(), newChildren.end());
        children_.swap(newChildren);

        // Sort by ASCENDING priority (lowest priority first → bottom, highest last → top)
        std::sort(children_.begin(), children_.end(),
            [](const DisplayHandle& a, const DisplayHandle& b) {
                int pa = (a && a.get()) ? a->priority_ : std::numeric_limits<int>::min();
                int pb = (b && b.get()) ? b->priority_ : std::numeric_limits<int>::min();
                return pa < pb;
            });


        // Z-order now naturally matches render order (and EventManager expectations):
        // index 0 = bottom, last index = top (max z)
        for (int i = 0; i < static_cast<int>(children_.size()); ++i) {
            if (auto* obj = dynamic_cast<IDisplayObject*>(children_[i].get())) {
                obj->z_order_ = i;
            }
        }

        return *this;
    } // IDisplayObject& IDisplayObject::sortChildrenByPriority()


    IDisplayObject& IDisplayObject::setPriority(int newPriority)
    {
        priority_ = newPriority;
        IDisplayObject* parentObj = dynamic_cast<IDisplayObject*>(parent_.get());
        if (parentObj) {
            parentObj->sortChildrenByPriority();
        } else {
            sortChildrenByPriority(); // Ensure children are sorted if this is the root node
        }
        return *this;
    }

    std::vector<int> IDisplayObject::getChildrenPriorities() const
    {
        std::vector<int> priorities;
        for (const auto& child : children_) {
            auto* childObj = dynamic_cast<IDisplayObject*>(child.get());
            priorities.push_back(childObj ? childObj->priority_ : 0);
        }
        return priorities;
    }

    IDisplayObject& IDisplayObject::moveToTop()
    {
        IDisplayObject* parentObj = dynamic_cast<IDisplayObject*>(parent_.get());
        if (!parentObj)
            return *this; // Root elements cannot be raised

        // Highest sibling priority → visually top
        int maxPriority = parentObj->getMaxPriority();
        setPriority(maxPriority + 1);

        // Resort siblings so z-order updates
        parentObj->sortChildrenByPriority();
        return *this;
    }    

    // IDisplayObject& IDisplayObject::moveToBottom()
    // {
    //     IDisplayObject* parentObj = dynamic_cast<IDisplayObject*>(parent_.get());
    //     if (!parentObj)
    //         return *this; // Root nodes don't move
    //     // Lowest sibling priority → visually bottom
    //     int minPriority = parentObj->getMinPriority();
    //     setPriority(minPriority - 1);
    //     // Reorder siblings and update z-order
    //     parentObj->sortChildrenByPriority();
    //     return *this;
    // }

    IDisplayObject& IDisplayObject::moveToBottom()
    {
        IDisplayObject* parentObj = dynamic_cast<IDisplayObject*>(parent_.get());
        if (!parentObj)
            return *this;

        int minPriority = parentObj->getMinPriority();
        setPriority(minPriority - 1);

        // Sync Z-Order directly from priority
        setZOrder(priority_);

        parentObj->sortChildrenByPriority();
        return *this;
    }


    IDisplayObject& IDisplayObject::bringToFront()
    {
        // If we have a parent, push this object above its siblings
        IDisplayObject* parentObj = dynamic_cast<IDisplayObject*>(parent_.get());
        if (parentObj)
        {
            int maxPriority = parentObj->getMaxPriority();
            setPriority(maxPriority + 1);
            parentObj->sortChildrenByPriority();

            // Now recursively raise the parent as well
            parentObj->bringToFront();
        }
        else
        {
            // If root (Stage), just ensure children ordering is updated
            sortChildrenByPriority();
        }
        return *this;
    }

    IDisplayObject& IDisplayObject::sendToBack()
    {
        IDisplayObject* parentObj = dynamic_cast<IDisplayObject*>(parent_.get());
        if (parentObj)
        {
            int minPriority = parentObj->getMinPriority();
            setPriority(minPriority - 1);
            parentObj->sortChildrenByPriority();
        }
        else
        {
            // Root stage — still allow ordering among children, but do NOT go below the root
            int minPriority = getMinPriority();
            setPriority(minPriority - 1);
            sortChildrenByPriority();
        }
        return *this;
    }

    IDisplayObject& IDisplayObject::sendToBackAfter(const IDisplayObject* limitObj)
    {
        IDisplayObject* parentObj = dynamic_cast<IDisplayObject*>(parent_.get());
        if (!parentObj) return *this;

        // If no limit provided, behave like sendToBack()
        if (!limitObj) {
            return sendToBack();
        }

        // No-op if the limit is this object
        if (limitObj == this) {
            return *this;
        }

        // Reinsert this object immediately after the limit in the parent's
        // current z-order, then renormalize priorities/z to match order.
        auto& kids = parentObj->children_;

        auto ptrEqual = [](const DisplayHandle& h, const IDisplayObject* p) -> bool {
            return h && h.get() == p;
        };

        // Find indices
        int idxSelf = -1, idxLimit = -1;
        for (int i = 0; i < static_cast<int>(kids.size()); ++i) {
            if (ptrEqual(kids[i], this)) idxSelf = i;
            if (ptrEqual(kids[i], limitObj)) idxLimit = i;
        }
        if (idxSelf < 0 || idxLimit < 0) {
            parentObj->sortChildrenByPriority();
            return *this;
        }

        // Remove self
        DisplayHandle me = kids[idxSelf];
        kids.erase(kids.begin() + idxSelf);

        // After erase, if self was before limit, limit index shifts left by 1.
        if (idxSelf < idxLimit) idxLimit -= 1;

        // Insert immediately after limit
        int insertPos = std::min(idxLimit + 1, static_cast<int>(kids.size()));
        kids.insert(kids.begin() + insertPos, me);

        // Renormalize priorities and z-order to match index order (ascending)
        for (int i = 0; i < static_cast<int>(kids.size()); ++i) {
            if (auto* obj = dynamic_cast<IDisplayObject*>(kids[i].get())) {
                obj->priority_ = i;
                obj->z_order_ = i;
            }
        }

        return *this;
    }
    

    bool IDisplayObject::hasChild(const DisplayHandle child) const
    {
        auto it = std::find(children_.begin(), children_.end(), child);
        return it != children_.end() && child.isValid();
    }

    // Ancestor/Descendant helpers
    bool IDisplayObject::isAncestorOf(DisplayHandle descendant) const
    {
        if (!descendant) return false;
        // Walk up from descendant's parent to root looking for this
        IDisplayObject* descObj = dynamic_cast<IDisplayObject*>(descendant.get());
        if (!descObj) return false;
        DisplayHandle cur = descObj->getParent();
        while (cur) 
        {
            // Compare the underlying pointer to this object
            if (cur.get() == this) return true;
            IDisplayObject* curObj = dynamic_cast<IDisplayObject*>(cur.get());
            if (!curObj) break;
            cur = curObj->getParent();
        }
        return false;
    }

    bool IDisplayObject::isAncestorOf(const std::string& name) const
    {
        // Find a child by name and see if this is its ancestor
        for (const auto& child : children_) 
        {
            if (!child) continue;
            if (child.getName() == name) return true;
            IDisplayObject* childObj = dynamic_cast<IDisplayObject*>(child.get());
            if (childObj && childObj->isAncestorOf(name)) return true;
        }
        return false;
    }

    bool IDisplayObject::isDescendantOf(DisplayHandle ancestor) const
    {
        if (!ancestor) return false;
        DisplayHandle cur = getParent();
        while (cur) 
        {
            if (cur == ancestor) return true;
            IDisplayObject* curObj = dynamic_cast<IDisplayObject*>(cur.get());
            if (!curObj) break;
            cur = curObj->getParent();
        }
        return false;
    }

    bool IDisplayObject::isDescendantOf(const std::string& name) const
    {
        DisplayHandle cur = getParent();
        while (cur) 
        {
            if (cur.getName() == name) return true;
            IDisplayObject* curObj = dynamic_cast<IDisplayObject*>(cur.get());
            if (!curObj) break;
            cur = curObj->getParent();
        }
        return false;
    }

    // Remove this object from its parent (convenience)
    bool IDisplayObject::removeFromParent()
    {
        DisplayHandle parentHandle = getParent();
        if (!parentHandle) return false;
        IDisplayObject* parentObj = dynamic_cast<IDisplayObject*>(parentHandle.get());
        if (!parentObj) return false;
        // Attempt removal; parentObj->removeChild will perform checks and orphan handling
        DisplayHandle me(getName(), getType());
        return parentObj->removeChild(me);
    }

    // (Predicate-style const overloads for removeFromParent were removed to avoid
    // confusion with the mutating removeFromParent() convenience method.)

    // Recursive descendant removal: search depth-first and remove first match
    bool IDisplayObject::removeDescendant(DisplayHandle descendant)
    {
        if (!descendant) return false;
        // Check direct children first
        auto it = std::find(children_.begin(), children_.end(), descendant);
        if (it != children_.end()) 
        {
            return removeChild(descendant);
        }
        // Recurse into children
        for (auto& child : children_) 
        {
            if (!child) continue;
            IDisplayObject* childObj = dynamic_cast<IDisplayObject*>(child.get());
            if (!childObj) continue;
            if (childObj->removeDescendant(descendant)) return true;
        }
        return false;
    }

    bool IDisplayObject::removeDescendant(const std::string& descendantName)
    {
        // Check direct children first
        for (const auto& child : children_) 
        {
            if (!child) continue;
            if (child.getName() == descendantName) return removeChild(child);
        }
        // Recurse into children
        for (auto& child : children_) 
        {
            if (!child) continue;
            IDisplayObject* childObj = dynamic_cast<IDisplayObject*>(child.get());
            if (!childObj) continue;
            if (childObj->removeDescendant(descendantName)) return true;
        }
        return false;
    }

    bool IDisplayObject::isOnStage() const
    {
        DisplayHandle stageHandle = getCore().getRootNode();
        DisplayHandle cur = getFactory().getDisplayObject(getName());
        while (cur)
        {
            if (cur == stageHandle)
                return true;
            IDisplayObject* obj = dynamic_cast<IDisplayObject*>(cur.get());
            if (!obj) break;
            cur = obj->getParent();
        }
        return false;
    }


    int IDisplayObject::getTabPriority() const 
    { 
        return tabPriority_; 
    }

    IDisplayObject& IDisplayObject::setTabPriority(int index) 
    { 
        tabPriority_ = index; 
        return *this; 
    }
    
    bool IDisplayObject::isTabEnabled() const 
    { 
        return tabEnabled_; 
    }

    IDisplayObject& IDisplayObject::setTabEnabled(bool enabled) 
    { 
        // Initialization logic for Box: only auto-assign a tab index when
        // enabling and no explicit priority has been set (tabPriority_ == 0
        // is treated as 'unset' here). This avoids overwriting values parsed
        // from Lua/Factory configuration.
        static int s_nextIndex = 0;
        if (enabled)
        {
            if (tabPriority_ == 0) {
                tabPriority_ = s_nextIndex;
                s_nextIndex++;
            }
            tabEnabled_ = true;
        } else {
            tabEnabled_ = false;
        }
        return *this; 
    }

    void IDisplayObject::setKeyboardFocus() 
    { 
        Core::getInstance().setKeyboardFocusedObject(DisplayHandle(getName(), getType())); 
    }
    bool IDisplayObject::isKeyboardFocused() const
    {
        auto focusedObj = SDOM::Core::getInstance().getKeyboardFocusedObject().get();
        return (focusedObj == this);
    }

    bool IDisplayObject::isMouseHovered() const
    {
        auto hoveredObj = SDOM::Core::getInstance().getMouseHoveredObject().get();
        return (hoveredObj == this);
    }

    int IDisplayObject::getWidth() const 
    {
        // int w = int(getRight() - getLeft());
        // return (w >= 1) ? w : 1;
        return int(std::abs(getRight() - getLeft()));
    }

    int IDisplayObject::getHeight() const 
    {
        // int h = int(getBottom() - getTop());
        // return (h >= 1) ? h : 1;
        return int(std::abs(getBottom() - getTop()));
    }    

    IDisplayObject& IDisplayObject::setWidth(int p_width)
    {
        setRight( getLeft() + p_width);
        return *this;
    }
    
    IDisplayObject& IDisplayObject::setHeight(int p_height)
    {
        setBottom( getTop() + p_height);
        return *this;
    }
    

    int IDisplayObject::getX() const
    {
        // return int(std::min(getLeft(), getRight() - 1));
        return int(getLeft());
    }

    int IDisplayObject::getY() const
    {
        // return int(std::min(getTop(), getBottom() - 1));
        return int(getTop());
    }        

    IDisplayObject& IDisplayObject::setX(int p_x)
    {
        float leftWorld = getLeft();
        float rightWorld = getRight();
        float delta = p_x - leftWorld;
        setLeft(leftWorld + delta);
        setRight(rightWorld + delta);
        setDirty();
        return *this;
    }

    IDisplayObject& IDisplayObject::setY(int p_y)
    {
        float topWorld = getTop();
        float bottomWorld = getBottom();
        float delta = p_y - topWorld;
        setTop(topWorld + delta);
        setBottom(bottomWorld + delta);
        setDirty();
        return *this;
    }

    
    // Anchor Setters:

    void IDisplayObject::setAnchorLeft(AnchorPoint ap)
    {
        if (ap == anchorLeft_) return;
        float leftWorld = getLeft();
        anchorLeft_ = ap;
        setLeft(leftWorld); // Recalculate local left_ using new anchor
    }

    void IDisplayObject::setAnchorRight(AnchorPoint ap)
    {
        if (ap == anchorRight_) return;
        float rightWorld = getRight();
        anchorRight_ = ap;
        setRight(rightWorld); // Recalculate local right_ using new anchor
    }

    void IDisplayObject::setAnchorTop(AnchorPoint ap)
    {
        if (ap == anchorTop_) return;
        float topWorld = getTop();
        anchorTop_ = ap;
        setTop(topWorld); // Recalculate local top_ using new anchor
    }

    void IDisplayObject::setAnchorBottom(AnchorPoint ap)
    {
        if (ap == anchorBottom_) return;
        float bottomWorld = getBottom();
        anchorBottom_ = ap;
        setBottom(bottomWorld); // Recalculate local bottom_ using new anchor
    }


    // Main Edge Getters

    float IDisplayObject::getLeft() const
    {
        auto* parent = dynamic_cast<IDisplayObject*>(parent_.get());
        if (parent == this) 
        {
            ERROR("Cycle detected: node is its own parent!");
        }
        if (parent) 
        {
            switch (anchorLeft_) 
            {
                case AnchorPoint::TOP_LEFT:
                case AnchorPoint::MIDDLE_LEFT:
                case AnchorPoint::BOTTOM_LEFT:
                    return parent->getLeft() + left_;
                case AnchorPoint::TOP_CENTER:
                case AnchorPoint::MIDDLE_CENTER:
                case AnchorPoint::BOTTOM_CENTER:
                    return parent->getLeft() + parent->getWidth() / 2 + left_;
                case AnchorPoint::TOP_RIGHT:
                case AnchorPoint::MIDDLE_RIGHT:
                case AnchorPoint::BOTTOM_RIGHT:
                    return parent->getLeft() + parent->getWidth() + left_;
            }
        }
        return left_; // root node or no parent
    }


    float IDisplayObject::getRight() const
    {
        auto* parent = dynamic_cast<IDisplayObject*>(parent_.get());
        if (parent == this) {
            ERROR("Cycle detected: node is its own parent!");
        }
        if (parent) {
            switch (anchorRight_) {
                case AnchorPoint::TOP_LEFT:
                case AnchorPoint::MIDDLE_LEFT:
                case AnchorPoint::BOTTOM_LEFT:
                    return parent->getLeft() + right_;
                case AnchorPoint::TOP_CENTER:
                case AnchorPoint::MIDDLE_CENTER:
                case AnchorPoint::BOTTOM_CENTER:
                    return parent->getLeft() + parent->getWidth() / 2 + right_;
                case AnchorPoint::TOP_RIGHT:
                case AnchorPoint::MIDDLE_RIGHT:
                case AnchorPoint::BOTTOM_RIGHT:
                    return parent->getLeft() + parent->getWidth() + right_;
            }
        }
        return right_; // root node or no parent
    }

    float IDisplayObject::getTop() const
    {
        auto* parent = dynamic_cast<IDisplayObject*>(parent_.get());
        if (parent == this) {
            ERROR("Cycle detected: node is its own parent!");
        }
        if (parent) {
            switch (anchorTop_) {
                case AnchorPoint::TOP_LEFT:
                case AnchorPoint::TOP_CENTER:
                case AnchorPoint::TOP_RIGHT:
                    return parent->getTop() + top_;
                case AnchorPoint::MIDDLE_LEFT:
                case AnchorPoint::MIDDLE_CENTER:
                case AnchorPoint::MIDDLE_RIGHT:
                    return parent->getTop() + parent->getHeight() / 2 + top_;
                case AnchorPoint::BOTTOM_LEFT:
                case AnchorPoint::BOTTOM_CENTER:
                case AnchorPoint::BOTTOM_RIGHT:
                    return parent->getTop() + parent->getHeight() + top_;
            }
        }
        return top_; // root node or no parent
    }

    float IDisplayObject::getBottom() const
    {
        auto* parent = dynamic_cast<IDisplayObject*>(parent_.get());
        if (parent == this) {
            ERROR("Cycle detected: node is its own parent!");
        }
        if (parent) {
            switch (anchorBottom_) {
                case AnchorPoint::TOP_LEFT:
                case AnchorPoint::TOP_CENTER:
                case AnchorPoint::TOP_RIGHT:
                    return parent->getTop() + bottom_;
                case AnchorPoint::MIDDLE_LEFT:
                case AnchorPoint::MIDDLE_CENTER:
                case AnchorPoint::MIDDLE_RIGHT:
                    return parent->getTop() + parent->getHeight() / 2 + bottom_;
                case AnchorPoint::BOTTOM_LEFT:
                case AnchorPoint::BOTTOM_CENTER:
                case AnchorPoint::BOTTOM_RIGHT:
                    return parent->getTop() + parent->getHeight() + bottom_;
            }
        }
        return bottom_; // root node or no parent
    }

    // Main Edge Setters    

    IDisplayObject& IDisplayObject::setLeft(float p_left)
    {
        auto* parent = dynamic_cast<IDisplayObject*>(parent_.get());
        if (parent == this) 
        {
            ERROR("Cycle detected: node is its own parent!");
        }
        float parentAnchor = 0.0f;
        if (parent)
         {
            switch (anchorLeft_) 
            {
                case AnchorPoint::TOP_LEFT:
                case AnchorPoint::MIDDLE_LEFT:
                case AnchorPoint::BOTTOM_LEFT:
                    parentAnchor = parent->getLeft();
                    break;
                case AnchorPoint::TOP_CENTER:
                case AnchorPoint::MIDDLE_CENTER:
                case AnchorPoint::BOTTOM_CENTER:
                    parentAnchor = parent->getLeft() + parent->getWidth() / 2;
                    break;
                case AnchorPoint::TOP_RIGHT:
                case AnchorPoint::MIDDLE_RIGHT:
                case AnchorPoint::BOTTOM_RIGHT:
                    parentAnchor = parent->getLeft() + parent->getWidth();
                    break;
            }
        }
        left_ = p_left - parentAnchor;
        setDirty();
        return *this;
    }

    IDisplayObject& IDisplayObject::setRight(float p_right)
    {
        auto* parent = dynamic_cast<IDisplayObject*>(parent_.get());
        if (parent == this) {
            ERROR("Cycle detected: node is its own parent!");
        }
        float parentAnchor = 0.0f;
        if (parent) {
            switch (anchorRight_) {
                case AnchorPoint::TOP_LEFT:
                case AnchorPoint::MIDDLE_LEFT:
                case AnchorPoint::BOTTOM_LEFT:
                    parentAnchor = parent->getLeft();
                    break;
                case AnchorPoint::TOP_CENTER:
                case AnchorPoint::MIDDLE_CENTER:
                case AnchorPoint::BOTTOM_CENTER:
                    parentAnchor = parent->getLeft() + parent->getWidth() / 2;
                    break;
                case AnchorPoint::TOP_RIGHT:
                case AnchorPoint::MIDDLE_RIGHT:
                case AnchorPoint::BOTTOM_RIGHT:
                    parentAnchor = parent->getLeft() + parent->getWidth();
                    break;
            }
        }
        right_ = p_right - parentAnchor;
        setDirty();
        return *this;
    }

    IDisplayObject& IDisplayObject::setTop(float p_top)
    {
        auto* parent = dynamic_cast<IDisplayObject*>(parent_.get());
        if (parent == this) 
        {
            ERROR("Cycle detected: node is its own parent!");
        }
        float parentAnchor = 0.0f;
        if (parent) 
        {
            switch (anchorTop_) {
                case AnchorPoint::TOP_LEFT:
                case AnchorPoint::TOP_CENTER:
                case AnchorPoint::TOP_RIGHT:
                    parentAnchor = parent->getTop();
                    break;
                case AnchorPoint::MIDDLE_LEFT:
                case AnchorPoint::MIDDLE_CENTER:
                case AnchorPoint::MIDDLE_RIGHT:
                    parentAnchor = parent->getTop() + parent->getHeight() / 2;
                    break;
                case AnchorPoint::BOTTOM_LEFT:
                case AnchorPoint::BOTTOM_CENTER:
                case AnchorPoint::BOTTOM_RIGHT:
                    parentAnchor = parent->getTop() + parent->getHeight();
                    break;
            }
        }
        top_ = p_top - parentAnchor;
        setDirty();
        return *this;
    }

    IDisplayObject& IDisplayObject::setBottom(float p_bottom)
    {
        auto* parent = dynamic_cast<IDisplayObject*>(parent_.get());
        if (parent == this) 
        {
            ERROR("Cycle detected: node is its own parent!");
        }
        float parentAnchor = 0.0f;
        if (parent) 
        {
            switch (anchorBottom_) 
            {
                case AnchorPoint::TOP_LEFT:
                case AnchorPoint::TOP_CENTER:
                case AnchorPoint::TOP_RIGHT:
                    parentAnchor = parent->getTop();
                    break;
                case AnchorPoint::MIDDLE_LEFT:
                case AnchorPoint::MIDDLE_CENTER:
                case AnchorPoint::MIDDLE_RIGHT:
                    parentAnchor = parent->getTop() + parent->getHeight() / 2;
                    break;
                case AnchorPoint::BOTTOM_LEFT:
                case AnchorPoint::BOTTOM_CENTER:
                case AnchorPoint::BOTTOM_RIGHT:
                    parentAnchor = parent->getTop() + parent->getHeight();
                    break;
            }
        }
        bottom_ = p_bottom - parentAnchor;
        setDirty();
        return *this;
    }


    void IDisplayObject::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "IDisplayObject");

        const std::string cppType = "SDOM::" + typeName;
        const std::string fileStem = typeName;
        const std::string exportName = "SDOM_" + typeName;
        SDOM::TypeInfo& typeInfo = ensureType(typeName,
                                              SDOM::EntryKind::Object,
                                              cppType,
                                              fileStem,
                                              exportName,
                                              "DisplayObjects",
                                              typeName + " display object bindings");

        typeInfo.subject_kind = "DisplayObject";
        typeInfo.subject_uses_handle = true;
        typeInfo.has_handle_override = false;
        typeInfo.dispatch_family_override = "method_table";

        // setModuleBrief(typeInfo,
        //     "Common runtime services for SDOM display objects, including layout, "
        //     "anchoring, and event integration.");
        setModuleBrief(typeInfo,
            "IDisplayObject is the base class for all visible and interactive objects in SDOM, providing \
            transform, sizing, and hierarchical parenting. It defines how an object enters the stage, \
            participates in rendering and input, and transitions through lifecycle events. Through the \
            generated C API, external code safely creates and manipulates display objects via handles \
            without exposing C++ internals.");

        IDisplayObjectAPI::registerBindings(*this, typeName);
    }

} // namespace SDOM
