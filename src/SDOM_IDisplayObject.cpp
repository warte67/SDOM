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
#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Utils.hpp>
#include <SDOM/lua_IDisplayObject.hpp>

#include <chrono>

namespace SDOM
{
    // IDisplayObject implementation

    IDisplayObject::IDisplayObject(const InitStruct& init)
         : IDataObject()
    {
        name_ = init.name;
        color_ = init.color;
        z_order_ = init.z_order;
        priority_ = init.priority;
        isClickable_ = init.isClickable;
        isEnabled_ = init.isEnabled;
        isHidden_ = init.isHidden;
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
            try {
                if (o.is<double>()) return static_cast<float>(o.as<double>());
                if (o.is<int>()) return static_cast<float>(o.as<int>());
                if (o.is<std::string>()) {
                    std::string s = o.as<std::string>();
                    if (s.empty()) return d;
                    return std::stof(s);
                }
            } catch(...) {}
            return d;
        };
        auto get_bool = [&](const char* k, bool d = false) -> bool 
        {
            return config[k].valid() ? config[k].get<bool>() : d;
        };
        auto read_color = [&](const char* k, SDL_Color d = {255,0,255,255}) -> SDL_Color 
        {
            if (!config[k].valid()) return d;
            sol::table t = config[k];
            SDL_Color c = d;
            if (t["r"].valid()) c.r = (Uint8)t["r"].get<int>();
            if (t["g"].valid()) c.g = (Uint8)t["g"].get<int>();
            if (t["b"].valid()) c.b = (Uint8)t["b"].get<int>();
            if (t["a"].valid()) c.a = (Uint8)t["a"].get<int>();
            // array-style [r,g,b,a]
            if (!t["r"].valid() && t[1].valid()) 
            {
                c.r = (Uint8)t[1].get<int>();
                if (t[2].valid()) c.g = (Uint8)t[2].get<int>();
                if (t[3].valid()) c.b = (Uint8)t[3].get<int>();
                if (t[4].valid()) c.a = (Uint8)t[4].get<int>();
            }
            return c;
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

    } // END IDisplayObject::IDisplayObject(const sol::table& config)

    IDisplayObject::~IDisplayObject()
    {
        // nameRegistry_.erase(getName());
        onQuit(); // Call the pure virtual method to ensure derived classes clean up
    }

    bool IDisplayObject::onInit()
    {
        // Default implementation, can be overridden by derived classes
        // registerSelfName(); // Register this object in the name registry
        return true; // Indicate successful initialization
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
            try {
                std::string newName = p_child.getName();
                for (const auto& existing : parentChildren) {
                    if (existing && existing.getName() == newName) { nameExists = true; break; }
                }
            } catch(...) { nameExists = false; }

            // Diagnostic logging to trace duplicate insertions seen by unit tests
            try {
                std::ostringstream dbg;
                dbg << "attachChild_: parent='" << parent->getName() << "' child='" << p_child.getName() << "' children_before=" << parentChildren.size() << " nameExists=" << (nameExists?"1":"0");
                LUA_INFO(dbg.str());
            } catch(...) {}

            if (it == parentChildren.end() && !nameExists) 
            {
                    try { std::ostringstream dbg; dbg << "attachChild_: pushing child='" << p_child.getName() << "' to parent='" << parent->getName() << "'"; LUA_INFO(dbg.str()); } catch(...) {}
                // Record world edges BEFORE changing parent
                float leftWorld = child->getLeft();
                float rightWorld = child->getRight();
                float topWorld = child->getTop();
                float bottomWorld = child->getBottom();

                child->setParent(p_parent);
                // setParent may already add the child to the new parent's children_ vector.
                // Only push_back if it's not present to avoid duplicate entries.
                auto it_after = std::find(parentChildren.begin(), parentChildren.end(), p_child);
                if (it_after == parentChildren.end()) {
                    parentChildren.push_back(p_child);
                } else {
                    try { std::ostringstream dbg; dbg << "attachChild_: setParent already added child='" << p_child.getName() << "' to parent='" << parent->getName() << "'"; LUA_INFO(dbg.str()); } catch(...) {}
                }
                try { std::ostringstream dbg; dbg << "attachChild_: children_after=" << parentChildren.size() << " parent='" << parent->getName() << "'"; LUA_INFO(dbg.str()); } catch(...) {}

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
                if (child) {
                    auto& eventManager = getCore().getEventManager();
                    std::unique_ptr<Event> addedEvent = std::make_unique<Event>(EventType::Added, p_child);
                    // Set related target to parent handle for listener use
                    addedEvent->setRelatedTarget(p_parent);
                    eventManager.dispatchEvent(std::move(addedEvent), p_child);
                }

                // Dispatch EventType::AddedToStage if the child is now part of the active stage.
                DisplayHandle stageHandle = getCore().getRootNode();
                if (stageHandle) {
                    DisplayHandle cur = p_child;
                    bool onStage = false;
                    while (cur) {
                        if (cur == stageHandle) { onStage = true; break; }
                        IDisplayObject* curObj = dynamic_cast<IDisplayObject*>(cur.get());
                        if (!curObj) break;
                        cur = curObj->getParent();
                    }
                    if (onStage) {
                        auto& eventManager = getCore().getEventManager();
                        std::unique_ptr<Event> addedToStage = std::make_unique<Event>(EventType::AddedToStage, p_child);
                        addedToStage->setRelatedTarget(stageHandle);
                        eventManager.dispatchEvent(std::move(addedToStage), p_child);
                    }
                }
            }
        }
    } // IDisplayObject::attachChild_(DisplayHandle p_child, DisplayHandle p_parent, bool useWorld, int worldX, int worldY)

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
                    // Reset orphan's parent using DisplayHandle (nullptr)
                    orphanObj->setParent(DisplayHandle());
                    parentChildren.erase(it); // Remove orphan from parent's children

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
            // Prefer pointer-style access to the underlying object, fall back to handle accessor if needed
            try 
            {
                if (child->getName() == name) return child;
            } 
            catch (...) 
            {
                try 
                {
                    if (child.getName() == name) return child;
                } catch (...) { /* ignore and continue */ }
            }
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

        auto it = std::find(children_.begin(), children_.end(), child);
        if (it != children_.end()) 
        {
            children_.erase(it);
            // Reset child's parent pointer if possible
            if (auto* childObj = dynamic_cast<IDisplayObject*>(child.get())) 
            {
                childObj->setParent(DisplayHandle());
            }
            Core* core = &Core::getInstance();
            Factory* factory = &core->getFactory();
            if (!core->getIsTraversing() && 
                child->getOrphanRetentionPolicy() == OrphanRetentionPolicy::AutoDestroy)
            {
                removeOrphan_(child); // Remove orphan immediately
            }
            else if (child->getOrphanRetentionPolicy() != OrphanRetentionPolicy::RetainUntilManual)
            {
                child->orphanedAt_ = std::chrono::steady_clock::now();
                factory->addToOrphanList(child); // Defer orphan removal
            }
            return true;
        } 
        else 
        {
            ERROR("removeChild: child not found in children_ vector of " + name_);
            return false;
        }
    }    

    const std::vector<DisplayHandle>& IDisplayObject::getChildren() const { return children_; }
    DisplayHandle IDisplayObject::getParent() const { return parent_; }

    IDisplayObject& IDisplayObject::setParent(const DisplayHandle& parent)
    {
        // Preserve world bounds across the parent change
        Bounds world = this->getBounds();

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
            try {
                if (child.getName() == name) {
                    return removeChild(child);
                }
            } catch(...) {}
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
        if (it != targetListeners.end()) {
            auto& listeners = it->second;
            listeners.erase(std::remove_if(listeners.begin(), listeners.end(), [&](const ListenerEntry& entry) {
                // Compare the target function pointers to identify the listener
                return entry.listener.target_type() == listener.target_type() &&
                       entry.listener.target<void(const Event&)>() == listener.target<void(const Event&)>();
            }), listeners.end());
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

    bool IDisplayObject::hasEventListeners(const EventType& type, bool useCapture) const
    {
        const auto& targetListeners = useCapture ? captureEventListeners : bubblingEventListeners;
        auto it = targetListeners.find(type);
        if (it == targetListeners.end()) return false;
        return !it->second.empty();
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
        children_.erase(std::remove_if(children_.begin(), children_.end(), [](const DisplayHandle& child) {
            return !child;
        }), children_.end());

        try {
            std::ostringstream oss;
            oss << "sortChildrenByPriority: before dedupe children_count=" << children_.size() << " [";
            for (const auto& ch : children_) {
                try { oss << (ch ? ch.getName() : std::string("<null>")) << ":" << (ch ? std::to_string(ch.get()->getPriority()) : std::string("0")) << ","; } catch(...) { oss << "<err>"; }
            }
            oss << "]";
            LUA_INFO(oss.str());
        } catch(...) {}

        // Deduplicate children by name: keep the most-recent occurrence (last in vector)
        try {
            std::unordered_set<std::string> seen;
            std::vector<DisplayHandle> newChildren;
            // Iterate in reverse and keep first-seen (which corresponds to most-recent in original order)
            for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
                const DisplayHandle& ch = *it;
                if (!ch) continue;
                std::string cname;
                try { cname = ch.getName(); } catch(...) { cname.clear(); }
                if (cname.empty()) {
                    newChildren.push_back(ch);
                } else {
                    if (seen.find(cname) == seen.end()) {
                        seen.insert(cname);
                        newChildren.push_back(ch);
                    } else {
                        // duplicate found; skip it
                    }
                }
            }
            // Restore original order
            std::reverse(newChildren.begin(), newChildren.end());
            children_.swap(newChildren);
        } catch(...) {}

        try {
            std::ostringstream oss;
            oss << "sortChildrenByPriority: after dedupe children_count=" << children_.size() << " [";
            for (const auto& ch : children_) {
                try { oss << (ch ? ch.getName() : std::string("<null>")) << ":" << (ch ? std::to_string(ch.get()->getPriority()) : std::string("0")) << ","; } catch(...) { oss << "<err>"; }
            }
            oss << "]";
            LUA_INFO(oss.str());
        } catch(...) {}

        // Sort by priority (ascending)
        std::sort(children_.begin(), children_.end(), [](const DisplayHandle& a, const DisplayHandle& b) {
            auto* aObj = dynamic_cast<IDisplayObject*>(a.get());
            auto* bObj = dynamic_cast<IDisplayObject*>(b.get());
            int aPriority = aObj ? aObj->priority_ : std::numeric_limits<int>::min();
            int bPriority = bObj ? bObj->priority_ : std::numeric_limits<int>::min();
            return aPriority < bPriority;
        });

        try {
            std::ostringstream oss;
            oss << "sortChildrenByPriority: after sort children_count=" << children_.size() << " [";
            for (const auto& ch : children_) {
                try { oss << (ch ? ch.getName() : std::string("<null>")) << ":" << (ch ? std::to_string(ch.get()->getPriority()) : std::string("0")) << ","; } catch(...) { oss << "<err>"; }
            }
            oss << "]";
            LUA_INFO(oss.str());
        } catch(...) {}
        return *this;
    }

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
        if (parentObj) {
            // Set this object's priority to highest among siblings
            int maxPriority = parentObj->getMaxPriority();
            setPriority(maxPriority + 1);
            parentObj->sortChildrenByPriority();
            // Recursively move parent to top
            parentObj->moveToTop();
        } else {
            // If no parent, just set to highest among children (root node)
            setPriority(getMaxPriority() + 1);
            sortChildrenByPriority();
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
        while (cur) {
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
        for (const auto& child : children_) {
            if (!child) continue;
            try {
                if (child.getName() == name) return true;
            } catch(...) {}
            IDisplayObject* childObj = dynamic_cast<IDisplayObject*>(child.get());
            if (childObj && childObj->isAncestorOf(name)) return true;
        }
        return false;
    }

    bool IDisplayObject::isDescendantOf(DisplayHandle ancestor) const
    {
        if (!ancestor) return false;
        DisplayHandle cur = getParent();
        while (cur) {
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
        while (cur) {
            try { if (cur.getName() == name) return true; } catch(...) {}
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
        if (it != children_.end()) {
            return removeChild(descendant);
        }
        // Recurse into children
        for (auto& child : children_) {
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
        for (const auto& child : children_) {
            if (!child) continue;
            try { if (child.getName() == descendantName) return removeChild(child); } catch(...) {}
        }
        // Recurse into children
        for (auto& child : children_) {
            if (!child) continue;
            IDisplayObject* childObj = dynamic_cast<IDisplayObject*>(child.get());
            if (!childObj) continue;
            if (childObj->removeDescendant(descendantName)) return true;
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
        if (parent == this) {
            ERROR("Cycle detected: node is its own parent!");
        }
        float parentAnchor = 0.0f;
        if (parent) {
            switch (anchorLeft_) {
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
        if (parent == this) {
            ERROR("Cycle detected: node is its own parent!");
        }
        float parentAnchor = 0.0f;
        if (parent) {
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
        if (parent == this) {
            ERROR("Cycle detected: node is its own parent!");
        }
        float parentAnchor = 0.0f;
        if (parent) {
            switch (anchorBottom_) {
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


    void IDisplayObject::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "IDisplayObject";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                    << typeName << CLR::RESET << std::endl;
        }

        // Bind onto the single augmentable handle: "DisplayHandle"
        sol::table handle = DisplayHandle::ensure_handle_table(lua);

        auto absent = [&](const char* name) -> bool {
            sol::object cur = handle.raw_get_or(name, sol::lua_nil);
            return !cur.valid() || cur == sol::lua_nil;
        };

        auto require_obj = [&](DisplayHandle& self, const char* method) -> IDisplayObject* {
            IDisplayObject* obj = dynamic_cast<IDisplayObject*>(self.get());
            if (!obj) {
                std::string msg = std::string("Invalid DisplayHandle handle for method '") + method + "'";
                throw sol::error(msg);
            }
            return obj;
        };

        // Tiny helpers following naming: bind_{returnType}_{args}
        // returnType: void | R
        // args: 0 | do (DisplayHandle) | str (const std::string&)
        // constness: default uses const IDisplayObject* when possible; _nc variants use IDisplayObject*
        // Examples:
        // - bind_void_0:        void f(IDisplayObject*)
        // - bind_R_0:            R   f(const IDisplayObject*)
        // - bind_void_do:        void f(IDisplayObject*, DisplayHandle)
        // - bind_R_do:           R   f(const IDisplayObject*, DisplayHandle)
        // - bind_R_str:          R   f(const IDisplayObject*, const std::string&)
        // - bind_R_0_nc:         R   f(IDisplayObject*)
        // - bind_R_do_nc:        R   f(IDisplayObject*, DisplayHandle)
        // - bind_R_str_nc:       R   f(IDisplayObject*, const std::string&)
        [[maybe_unused]] auto bind_void_0 = [&](const char* n, auto f) {
            if (absent(n)) {
                handle.set_function(n, [require_obj, f, n](DisplayHandle& self) {
                    f(require_obj(self, n));
                });
            }
        };
        [[maybe_unused]] auto bind_R_0 = [&](const char* n, auto f) {
            if (absent(n)) {
                handle.set_function(n, [require_obj, f, n](DisplayHandle& self) {
                    return f(require_obj(self, n));
                });
            }
        };
        [[maybe_unused]] auto bind_void_do = [&](const char* n, auto f) {
            if (absent(n)) {
                handle.set_function(n, [require_obj, f, n](DisplayHandle& self, const DisplayHandle& a) {
                    f(require_obj(self, n), a);
                });
            }
        };
        [[maybe_unused]] auto bind_R_do = [&](const char* n, auto f) {
            if (absent(n)) {
                handle.set_function(n, [require_obj, f, n](DisplayHandle& self, const DisplayHandle& a) {
                    return f(require_obj(self, n), a);
                });
            }
        };
        // One string-argument, returning a value: R f(const IDisplayObject*, const std::string&)
        [[maybe_unused]] auto bind_R_str = [&](const char* n, auto f) {
            if (absent(n)) {
                handle.set_function(n, [require_obj, f, n](DisplayHandle& self, const std::string& a) {
                    return f(require_obj(self, n), a);
                });
            }
        };
        // One string-argument, void return: void f(IDisplayObject*, const std::string&)
        [[maybe_unused]] auto bind_void_str = [&](const char* n, auto f) {
            if (absent(n)) {
                handle.set_function(n, [require_obj, f, n](DisplayHandle& self, const std::string& a) {
                    f(require_obj(self, n), a);
                });
            }
        };
        // One sol::object-argument, void return: void f(IDisplayObject*, const sol::object&)
        [[maybe_unused]] auto bind_void_obj = [&](const char* n, auto f) {
            if (absent(n)) {
                handle.set_function(n, [require_obj, f, n](DisplayHandle& self, const sol::object& a) {
                    f(require_obj(self, n), a);
                });
            }
        };
        // One int-argument, void return: void f(IDisplayObject*, int)
        [[maybe_unused]] auto bind_void_i = [&](const char* n, auto f) {
            if (absent(n)) {
                handle.set_function(n, [require_obj, f, n](DisplayHandle& self, int a) {
                    f(require_obj(self, n), a);
                });
            }
        };
        // One bool-argument, void return: void f(IDisplayObject*, bool)
        [[maybe_unused]] auto bind_void_b = [&](const char* n, auto f) {
            if (absent(n)) {
                handle.set_function(n, [require_obj, f, n](DisplayHandle& self, bool a) {
                    f(require_obj(self, n), a);
                });
            }
        };
        // One float-argument, void return: void f(IDisplayObject*, float)
        [[maybe_unused]] auto bind_void_f = [&](const char* n, auto f) {
            if (absent(n)) {
                handle.set_function(n, [require_obj, f, n](DisplayHandle& self, float a) {
                    f(require_obj(self, n), a);
                });
            }
        };
        // One AnchorPoint-argument, void return: void f(IDisplayObject*, AnchorPoint)
        [[maybe_unused]] auto bind_void_ap = [&](const char* n, auto f) {
            if (absent(n)) {
                handle.set_function(n, [require_obj, f, n](DisplayHandle& self, AnchorPoint a) {
                    f(require_obj(self, n), a);
                });
            }
        };
        // One OrphanRetentionPolicy-argument, returning a value: R f(const IDisplayObject*, OrphanRetentionPolicy)
        [[maybe_unused]] auto bind_R_orp = [&](const char* n, auto f) {
            if (absent(n)) {
                handle.set_function(n, [require_obj, f, n](DisplayHandle& self, IDisplayObject::OrphanRetentionPolicy a) {
                    return f(require_obj(self, n), a);
                });
            }
        };
        // Non-const pointer variants for wrappers that mutate or require non-const IDisplayObject*
        // - bind_R_0_nc:   R f(IDisplayObject*)
        // - bind_R_do_nc:  R f(IDisplayObject*, T1)
        // - bind_R_str_nc: R f(IDisplayObject*, const std::string&)
        [[maybe_unused]] auto bind_R_0_nc = [&](const char* n, auto f) {
            if (absent(n)) {
                handle.set_function(n, [require_obj, f, n](DisplayHandle& self) {
                    return f(require_obj(self, n));
                });
            }
        };
        [[maybe_unused]] auto bind_R_do_nc = [&](const char* n, auto f) {
            if (absent(n)) {
                handle.set_function(n, [require_obj, f, n](DisplayHandle& self, const DisplayHandle& a) {
                    return f(require_obj(self, n), a);
                });
            }
        };
        [[maybe_unused]] auto bind_R_str_nc = [&](const char* n, auto f) {
            if (absent(n)) {
                handle.set_function(n, [require_obj, f, n](DisplayHandle& self, const std::string& a) {
                    return f(require_obj(self, n), a);
                });
            }
        };

        // Ensure Bounds is registered (once)
        if (!lua["Bounds"].valid())
        {
            lua.new_usertype<Bounds>("Bounds",
                "left", &Bounds::left,
                "top", &Bounds::top,
                "right", &Bounds::right,
                "bottom", &Bounds::bottom,
                "x", sol::property([](const Bounds& b) { return static_cast<int>(b.left); }),
                "y", sol::property([](const Bounds& b) { return static_cast<int>(b.top); }),
                "width", sol::property([](const Bounds& b) { return static_cast<int>(b.width()); }),
                "height", sol::property([](const Bounds& b) { return static_cast<int>(b.height()); }),
                "widthf", &Bounds::width,
                "heightf", &Bounds::height,
                "getLeft", [](const Bounds& b) { return b.left; },
                "getTop", [](const Bounds& b) { return b.top; },
                "getRight", [](const Bounds& b) { return b.right; },
                "getBottom", [](const Bounds& b) { return b.bottom; },
                "getX", [](const Bounds& b) { return static_cast<int>(b.left); },
                "getY", [](const Bounds& b) { return static_cast<int>(b.top); },
                "getWidth", [](const Bounds& b) { return static_cast<int>(b.width()); },
                "getHeight", [](const Bounds& b) { return static_cast<int>(b.height()); },
                "almostEqual", [](const Bounds& a, const Bounds& other, sol::optional<float> eps) {
                    float e = eps ? *eps : 0.0001f;
                    return std::abs(a.left - other.left) <= e
                        && std::abs(a.top - other.top) <= e
                        && std::abs(a.right - other.right) <= e
                        && std::abs(a.bottom - other.bottom) <= e;
                }
            );
        }
        
// --- ALL IDisplayObject Properties: --- //
// float x = 0.0f;                                   // left_ (IDisplayObject)
// float y = 0.0f;                                   // top_  (IDisplayObject)
// float width = 0.0f;                               // right_ - left_  (IDisplayObject)
// float height = 0.0f;                              // bottom_ - top_  (IDisplayObject)
// SDL_Color color = {255, 0, 255, 255};             // color_ (IDisplayObject)
// AnchorPoint anchorTop = AnchorPoint::TOP_LEFT;    // anchorTop_ (IDisplayObject)
// AnchorPoint anchorLeft = AnchorPoint::TOP_LEFT;   // anchorLeft_ (IDisplayObject)
// AnchorPoint anchorBottom = AnchorPoint::TOP_LEFT; // anchorBottom_ (IDisplayObject)
// AnchorPoint anchorRight = AnchorPoint::TOP_LEFT;  // anchorRight_ (IDisplayObject)
// int z_order = 0;                                  // z_order_ (IDisplayObject)
// int priority = 0;                                 // priority_ (IDisplayObject)
// bool isClickable = true;                          // isClickable_ (IDisplayObject)
// bool isEnabled = true;                            // isEnabled_ (IDisplayObject)
// bool isHidden = false;                            // isHidden_ (IDisplayObject)
// int tabPriority = 0;                              // tabPriority_ (IDisplayObject)
// bool tabEnabled = true;                           // tabEnabled_ (IDisplayObject)

        // Event handling
        if (absent("addEventListener")) {
            handle.set_function("addEventListener", sol::overload(
                // typed form
                [require_obj](DisplayHandle& self, EventType& t, sol::function fn, bool useCapture, int prio) {
                    ::SDOM::addEventListener_lua(require_obj(self, "addEventListener"), t, std::move(fn), useCapture, prio);
                },
                // flexible forms
                [require_obj](DisplayHandle& self, const sol::object& a, const sol::object& b, const sol::object& c, const sol::object& d) {
                    ::SDOM::addEventListener_lua_any(require_obj(self, "addEventListener"), a, b, c, d);
                },
                [require_obj](DisplayHandle& self, const sol::object& desc) {
                    ::SDOM::addEventListener_lua_any_short(require_obj(self, "addEventListener"), desc);
                }
            ));
        }
        if (absent("removeEventListener")) {
            handle.set_function("removeEventListener", sol::overload(
                [require_obj](DisplayHandle& self, EventType& t, sol::function fn, bool useCapture) {
                    ::SDOM::removeEventListener_lua(require_obj(self, "removeEventListener"), t, std::move(fn), useCapture);
                },
                [require_obj](DisplayHandle& self, const sol::object& a, const sol::object& b, const sol::object& c) {
                    ::SDOM::removeEventListener_lua_any(require_obj(self, "removeEventListener"), a, b, c);
                },
                [require_obj](DisplayHandle& self, const sol::object& desc) {
                    ::SDOM::removeEventListener_lua_any_short(require_obj(self, "removeEventListener"), desc);
                }
            ));
        }

        // Basic state and debug (simplified via helpers)
        bind_void_0("cleanAll",       ::SDOM::cleanAll_lua);
        bind_R_0("getDirty",          ::SDOM::getDirty_lua);
        bind_void_0("setDirty",       ::SDOM::setDirty_lua);
        bind_R_0("isDirty",           ::SDOM::isDirty_lua);
        bind_void_0("printTree",      ::SDOM::printTree_lua);

        // Hierarchy
        if (absent("addChild"))      handle.set_function("addChild",      [require_obj](DisplayHandle& self, DisplayHandle child){ ::SDOM::addChild_lua(require_obj(self, "addChild"), child); });
        if (absent("removeChild"))   
        {
            handle.set_function("removeChild",   sol::overload(
                [require_obj](DisplayHandle& self, const std::string& name){ return ::SDOM::removeChild_lua(require_obj(self, "removeChild"), name); },
                [require_obj](DisplayHandle& self, DisplayHandle child){ return ::SDOM::removeChild_lua(require_obj(self, "removeChild"), child); }
            ));
        }
        bind_R_do("hasChild",                   ::SDOM::hasChild_lua);
        bind_R_str("getChild",                  ::SDOM::getChild_lua);
        bind_R_0("getParent",                   ::SDOM::getParent_lua);
        bind_void_do("setParent",               ::SDOM::setParent_lua);
        bind_R_do_nc("isAncestorOf",            [](IDisplayObject* o, const DisplayHandle& d){ return ::SDOM::isAncestorOf_lua(o, d); });
        bind_R_str_nc("isAncestorOfName",       [](IDisplayObject* o, const std::string& n){ return ::SDOM::isAncestorOf_lua(o, n); });
        bind_R_do_nc("isDescendantOf",          [](IDisplayObject* o, const DisplayHandle& a){ return ::SDOM::isDescendantOf_lua(o, a); });
        bind_R_str_nc("isDescendantOfName",     [](IDisplayObject* o, const std::string& n){ return ::SDOM::isDescendantOf_lua(o, n); });
        bind_R_0_nc("removeFromParent",         [](IDisplayObject* o){ return ::SDOM::removeFromParent_lua(o); });
        bind_R_do_nc("removeChildByHandle",     [](IDisplayObject* o, const DisplayHandle& d){ return ::SDOM::removeChild_lua(o, d); });
        bind_R_str_nc("removeChildByName",      [](IDisplayObject* o, const std::string& n){ return ::SDOM::removeChild_lua(o, n); });
        bind_R_do_nc("removeDescendant",        [](IDisplayObject* o, const DisplayHandle& d){ return ::SDOM::removeDescendant_lua(o, d); });
        bind_R_str_nc("removeDescendantByName", [](IDisplayObject* o, const std::string& n){ return ::SDOM::removeDescendant_lua(o, n); });

        // Type & properties (via helpers)
        // Use handle-aware name getter so bare DisplayHandle handles without a live object still return their cached name
        if (absent("getName")) {
            handle.set_function("getName", [require_obj](DisplayHandle& self) {
                // Try through live object first for authoritative name, else fall back to handle's cached name
                try { if (auto* o = dynamic_cast<IDisplayObject*>(self.get())) return ::SDOM::getName_lua(o); } catch(...) {}
                return ::SDOM::getName_handle_lua(self);
            });
        }
        bind_void_str("setName",    ::SDOM::setName_lua);
        bind_R_0("getType",         ::SDOM::getType_lua);
        bind_void_str("setType",    ::SDOM::setType_lua);
        bind_R_0("getBounds",       ::SDOM::getBounds_lua);
        bind_void_obj("setBounds",  [](IDisplayObject* o, const sol::object& a){ ::SDOM::setBounds_lua(o, a); });
        bind_R_0("getColor",        ::SDOM::getColor_lua);
        bind_void_obj("setColor",   [](IDisplayObject* o, const sol::object& a){ ::SDOM::setColor_lua(o, a); });

        // Priority & Z-Order
        bind_R_0("getMaxPriority",            ::SDOM::getMaxPriority_lua);
        bind_R_0("getMinPriority",            ::SDOM::getMinPriority_lua);
        bind_R_0("getPriority",               ::SDOM::getPriority_lua);
        bind_void_0("sortChildrenByPriority", ::SDOM::sortChildrenByPriority_lua);
        // Provide both simple and flexible forms using sol::overload where needed
        if (absent("setToHighestPriority")) {
            handle.set_function("setToHighestPriority", sol::overload(
                [require_obj](DisplayHandle& self) { ::SDOM::setToHighestPriority_lua(require_obj(self, "setToHighestPriority")); },
                [require_obj](DisplayHandle& self, const sol::object& desc) { ::SDOM::setToHighestPriority_lua_any(require_obj(self, "setToHighestPriority"), desc); }
            ));
        }
        if (absent("setToLowestPriority")) {
            handle.set_function("setToLowestPriority", sol::overload(
                [require_obj](DisplayHandle& self) { ::SDOM::setToLowestPriority_lua(require_obj(self, "setToLowestPriority")); },
                [require_obj](DisplayHandle& self, const sol::object& desc) { ::SDOM::setToLowestPriority_lua_any(require_obj(self, "setToLowestPriority"), desc); }
            ));
        }
        if (absent("setPriority")) {
            handle.set_function("setPriority", sol::overload(
                [require_obj](DisplayHandle& self, int pr) { ::SDOM::setPriority_lua(require_obj(self, "setPriority"), pr); },
                [require_obj](DisplayHandle& self, const sol::object& desc) { ::SDOM::setPriority_lua_any(require_obj(self, "setPriority"), desc); },
                [require_obj](DisplayHandle& self, const sol::object& desc, int value) { ::SDOM::setPriority_lua_target(require_obj(self, "setPriority"), desc, value); }
            ));
        }
        bind_R_0("getChildrenPriorities",     ::SDOM::getChildrenPriorities_lua);
        if (absent("moveToTop")) {
            handle.set_function("moveToTop", sol::overload(
                [require_obj](DisplayHandle& self) { ::SDOM::moveToTop_lua(require_obj(self, "moveToTop")); },
                [require_obj](DisplayHandle& self, const sol::object& desc) { ::SDOM::moveToTop_lua_any(require_obj(self, "moveToTop"), desc); }
            ));
        }
        bind_R_0("getZOrder",                 ::SDOM::getZOrder_lua);
        if (absent("setZOrder")) {
            handle.set_function("setZOrder", sol::overload(
                [require_obj](DisplayHandle& self, int z) { ::SDOM::setZOrder_lua(require_obj(self, "setZOrder"), z); },
                [require_obj](DisplayHandle& self, const sol::object& desc) { ::SDOM::setZOrder_lua_any(require_obj(self, "setZOrder"), desc); }
            ));
        }

        // Focus & interactivity
        bind_void_0("setKeyboardFocus",   ::SDOM::setKeyboardFocus_lua);
        bind_R_0("isKeyboardFocused",     ::SDOM::isKeyboardFocused_lua);
        bind_R_0("isMouseHovered",        ::SDOM::isMouseHovered_lua);
        bind_R_0("isClickable",           ::SDOM::isClickable_lua);
        bind_void_b("setClickable",       ::SDOM::setClickable_lua);
        bind_R_0("isEnabled",             ::SDOM::isEnabled_lua);
        bind_void_b("setEnabled",         ::SDOM::setEnabled_lua);
        bind_R_0("isHidden",              ::SDOM::isHidden_lua);
        bind_void_b("setHidden",          ::SDOM::setHidden_lua);
        bind_R_0("isVisible",             ::SDOM::isVisible_lua);
        bind_void_b("setVisible",         ::SDOM::setVisible_lua);

        // Tab management
        bind_R_0("getTabPriority",      ::SDOM::getTabPriority_lua);
        bind_void_i("setTabPriority",   ::SDOM::setTabPriority_lua);
        bind_R_0("isTabEnabled",        ::SDOM::isTabEnabled_lua);
        bind_void_b("setTabEnabled",    ::SDOM::setTabEnabled_lua);

        // Geometry & layout
        bind_R_0("getX",            ::SDOM::getX_lua);
        bind_R_0("getY",            ::SDOM::getY_lua);
        bind_R_0("getWidth",        ::SDOM::getWidth_lua);
        bind_R_0("getHeight",       ::SDOM::getHeight_lua);
        bind_void_i("setX",         ::SDOM::setX_lua);
        bind_void_i("setY",         ::SDOM::setY_lua);
        bind_void_i("setWidth",     ::SDOM::setWidth_lua);
        bind_void_i("setHeight",    ::SDOM::setHeight_lua);

        // Anchors
        bind_R_0("getAnchorTop",        ::SDOM::getAnchorTop_lua);
        bind_R_0("getAnchorLeft",       ::SDOM::getAnchorLeft_lua);
        bind_R_0("getAnchorBottom",     ::SDOM::getAnchorBottom_lua);
        bind_R_0("getAnchorRight",      ::SDOM::getAnchorRight_lua);
        bind_void_ap("setAnchorTop",    ::SDOM::setAnchorTop_lua);
        bind_void_ap("setAnchorLeft",   ::SDOM::setAnchorLeft_lua);
        bind_void_ap("setAnchorBottom", ::SDOM::setAnchorBottom_lua);
        bind_void_ap("setAnchorRight",  ::SDOM::setAnchorRight_lua);

        // World Edge positions
        bind_R_0("getLeft",         ::SDOM::getLeft_lua);
        bind_R_0("getRight",        ::SDOM::getRight_lua);
        bind_R_0("getTop",          ::SDOM::getTop_lua);
        bind_R_0("getBottom",       ::SDOM::getBottom_lua);
        bind_void_f("setLeft",      ::SDOM::setLeft_lua);
        bind_void_f("setRight",     ::SDOM::setRight_lua);
        bind_void_f("setTop",       ::SDOM::setTop_lua);
        bind_void_f("setBottom",    ::SDOM::setBottom_lua);

        // Local Edge positions
        bind_R_0("getLocalLeft",         ::SDOM::getLocalLeft_lua);
        bind_R_0("getLocalRight",        ::SDOM::getLocalRight_lua);
        bind_R_0("getLocalTop",          ::SDOM::getLocalTop_lua);
        bind_R_0("getLocalBottom",       ::SDOM::getLocalBottom_lua);
        bind_void_f("setLocalLeft",      ::SDOM::setLocalLeft_lua);
        bind_void_f("setLocalRight",     ::SDOM::setLocalRight_lua);
        bind_void_f("setLocalTop",       ::SDOM::setLocalTop_lua);
        bind_void_f("setLocalBottom",    ::SDOM::setLocalBottom_lua);

        // Orphan retention helpers
        bind_R_str("orphanPolicyFromString",        ::SDOM::orphanPolicyFromString_lua);
        bind_R_orp("orphanPolicyToString",          ::SDOM::orphanPolicyToString_lua);
        bind_void_str("setOrphanRetentionPolicy",   ::SDOM::setOrphanRetentionPolicy_lua);
        bind_R_0("getOrphanRetentionPolicyString",  ::SDOM::getOrphanRetentionPolicyString_lua);
            
        // Expose orphan grace (milliseconds) to Lua
        bind_R_0("getOrphanGrace",                   ::SDOM::getOrphanGrace_lua);
        // Accept integer milliseconds from Lua and convert to chrono::milliseconds
        bind_void_i("setOrphanGrace",                [](IDisplayObject* o, int ms){ ::SDOM::setOrphanGrace_lua(o, std::chrono::milliseconds(ms)); });

    } // END: IDisplayObject::_registerDisplayObject()

} // namespace SDOM