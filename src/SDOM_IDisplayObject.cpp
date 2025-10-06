/***  SDOM_DisplayObject.cpp  ****************************
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
        name_ = config["name"].get_or(std::string("IDisplayObject"));

        float x      = config["x"].get_or(0.0f);
        float y      = config["y"].get_or(0.0f);
        float width  = config["width"].get_or(0.0f);
        float height = config["height"].get_or(0.0f);

        setLeft(x);
        setTop(y);
        setRight(x + width);
        setBottom(y + height);

        // SDL_Color expects r, g, b, a fields in a Lua table
        if (config["color"].valid()) {
            sol::table colorTbl = config["color"];
            color_.r = colorTbl["r"].get_or(255);
            color_.g = colorTbl["g"].get_or(255);
            color_.b = colorTbl["b"].get_or(255);
            color_.a = colorTbl["a"].get_or(255);
        } else {
            color_ = {255, 255, 255, 255};
        }

        // Anchor conversion helpers:
        // - Accept string or integer enum
        // - Normalize string joiners/spaces via SDOM::normalizeAnchorString
        auto getAnchor = [&](const char* key) {
            if (config[key].valid()) {
                sol::object o = config[key].get<sol::object>();
                if (o.is<std::string>()) {
                    std::string str = o.as<std::string>();
                    std::string norm = SDOM::normalizeAnchorString(str);
                    auto it = stringToAnchorPoint_.find(norm);
                    return it != stringToAnchorPoint_.end() ? it->second : AnchorPoint::TOP_LEFT;
                } else if (o.is<int>()) {
                    return static_cast<AnchorPoint>(o.as<int>());
                } else if (o.is<double>()) {
                    return static_cast<AnchorPoint>(static_cast<int>(o.as<double>()));
                }
            }
            return AnchorPoint::TOP_LEFT;
        };

        setAnchorTop(getAnchor("anchorTop"));
        setAnchorLeft(getAnchor("anchorLeft"));
        setAnchorBottom(getAnchor("anchorBottom"));
        setAnchorRight(getAnchor("anchorRight"));

        z_order_      = config["z_order"].get_or(0);
        priority_     = config["priority"].get_or(0);
        isClickable_  = config["isClickable"].get_or(true);
        isEnabled_    = config["isEnabled"].get_or(true);
        isHidden_     = config["isHidden"].get_or(false);
        tabPriority_  = config["tabPriority"].get_or(0);
        tabEnabled_   = config["tabEnabled"].get_or(true);

        // // Register Lua properties and commands
        // registerLua_();

        // // Initialize from Lua config table
        // fromLua(config, config.lua_state());
    }

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

    void IDisplayObject::attachChild_(DisplayObject p_child, DisplayObject p_parent, bool useWorld, int worldX, int worldY)
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
                DisplayObject stageHandle = getCore().getRootNode();
                if (stageHandle) {
                    DisplayObject cur = p_child;
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
    } // IDisplayObject::attachChild_(DisplayObject p_child, DisplayObject p_parent, bool useWorld, int worldX, int worldY)

    void IDisplayObject::removeOrphan_(const DisplayObject& orphan) 
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
                    DisplayObject parentHandle = orphanObj->getParent();
                    // Reset orphan's parent using DisplayObject (nullptr)
                    orphanObj->setParent(DisplayObject());
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
                    DisplayObject stageHandle = getCore().getRootNode();
                    if (stageHandle) 
                    {
                        DisplayObject cur = parentHandle;
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

    void IDisplayObject::addChild(DisplayObject child, bool useWorld, int worldX, int worldY)
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
            factory->addToFutureChildrenList(child, DisplayObject(getName(), child->getType()), useWorld, worldX, worldY);
        }
        else
        {
            attachChild_(child, DisplayObject(getName(), getType()), useWorld, worldX, worldY);
        }
    }

    DisplayObject IDisplayObject::getChild(std::string name) const 
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
        return DisplayObject(); // invalid handle
    }    

    bool IDisplayObject::removeChild(DisplayObject child)
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
                childObj->setParent(DisplayObject());
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

    const std::vector<DisplayObject>& IDisplayObject::getChildren() const { return children_; }
    DisplayObject IDisplayObject::getParent() const { return parent_; }

    IDisplayObject& IDisplayObject::setParent(const DisplayObject& parent)
    {
        // Preserve world bounds across the parent change
        Bounds world = this->getBounds();

        // Remove from old parent's children_ vector if present
        if (parent_.isValid()) 
        {
            IDisplayObject* oldParentObj = dynamic_cast<IDisplayObject*>(parent_.get());
            if (oldParentObj) 
            {
                DisplayObject me(getName(), getType());
                auto& vec = oldParentObj->children_;
                vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const DisplayObject& d) { return d == me; }), vec.end());
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
                DisplayObject me(getName(), getType());
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

        // Traverse children using DisplayObject
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
            [](const DisplayObject& a, const DisplayObject& b) {
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
            [](const DisplayObject& a, const DisplayObject& b) {
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
        children_.erase(std::remove_if(children_.begin(), children_.end(), [](const DisplayObject& child) {
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
            std::vector<DisplayObject> newChildren;
            // Iterate in reverse and keep first-seen (which corresponds to most-recent in original order)
            for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
                const DisplayObject& ch = *it;
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
        std::sort(children_.begin(), children_.end(), [](const DisplayObject& a, const DisplayObject& b) {
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

    bool IDisplayObject::hasChild(const DisplayObject child) const
    {
        auto it = std::find(children_.begin(), children_.end(), child);
        return it != children_.end() && child.isValid();
    }

    // Ancestor/Descendant helpers
    bool IDisplayObject::isAncestorOf(DisplayObject descendant) const
    {
        if (!descendant) return false;
        // Walk up from descendant's parent to root looking for this
        IDisplayObject* descObj = dynamic_cast<IDisplayObject*>(descendant.get());
        if (!descObj) return false;
        DisplayObject cur = descObj->getParent();
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

    bool IDisplayObject::isDescendantOf(DisplayObject ancestor) const
    {
        if (!ancestor) return false;
        DisplayObject cur = getParent();
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
        DisplayObject cur = getParent();
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
        DisplayObject parentHandle = getParent();
        if (!parentHandle) return false;
        IDisplayObject* parentObj = dynamic_cast<IDisplayObject*>(parentHandle.get());
        if (!parentObj) return false;
        // Attempt removal; parentObj->removeChild will perform checks and orphan handling
        DisplayObject me(getName(), getType());
        return parentObj->removeChild(me);
    }

    // (Predicate-style const overloads for removeFromParent were removed to avoid
    // confusion with the mutating removeFromParent() convenience method.)

    // Recursive descendant removal: search depth-first and remove first match
    bool IDisplayObject::removeDescendant(DisplayObject descendant)
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
        Core::getInstance().setKeyboardFocusedObject(DisplayObject(getName(), getType())); 
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


    // Compatibility shim: preserve _registerLua() signature but forward to
    // the consolidated _registerDisplayObject() implementation. This keeps
    // external callers working while we centralize registration.
    void IDisplayObject::_registerLua(const std::string& typeName, sol::state_view lua)
    {
    }

    void IDisplayObject::_registerDisplayObject(const std::string& typeName, sol::state_view lua)
    {
        // Call base class registration to include inherited properties/commands
        SUPER::_registerDisplayObject(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "IDisplayObject";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                    << typeName << CLR::RESET << std::endl;
        }

        // Bind onto the single augmentable handle: "DisplayObject"
        sol::table handle = DisplayObject::ensure_handle_table(lua);

        auto absent = [&](const char* name) -> bool {
            sol::object cur = handle.raw_get_or(name, sol::lua_nil);
            return !cur.valid() || cur == sol::lua_nil;
        };

        auto require_obj = [&](DisplayObject& self, const char* method) -> IDisplayObject* {
            IDisplayObject* obj = dynamic_cast<IDisplayObject*>(self.get());
            if (!obj) {
                std::string msg = std::string("Invalid DisplayObject handle for method '") + method + "'";
                throw sol::error(msg);
            }
            return obj;
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

        // Event handling
        if (absent("addEventListener")) {
            handle.set_function("addEventListener", sol::overload(
                // typed form
                [require_obj](DisplayObject& self, EventType& t, sol::function fn, bool useCapture, int prio) {
                    ::SDOM::addEventListener_lua(require_obj(self, "addEventListener"), t, std::move(fn), useCapture, prio);
                },
                // flexible forms
                [require_obj](DisplayObject& self, const sol::object& a, const sol::object& b, const sol::object& c, const sol::object& d) {
                    ::SDOM::addEventListener_lua_any(require_obj(self, "addEventListener"), a, b, c, d);
                },
                [require_obj](DisplayObject& self, const sol::object& desc) {
                    ::SDOM::addEventListener_lua_any_short(require_obj(self, "addEventListener"), desc);
                }
            ));
        }
        if (absent("removeEventListener")) {
            handle.set_function("removeEventListener", sol::overload(
                [require_obj](DisplayObject& self, EventType& t, sol::function fn, bool useCapture) {
                    ::SDOM::removeEventListener_lua(require_obj(self, "removeEventListener"), t, std::move(fn), useCapture);
                },
                [require_obj](DisplayObject& self, const sol::object& a, const sol::object& b, const sol::object& c) {
                    ::SDOM::removeEventListener_lua_any(require_obj(self, "removeEventListener"), a, b, c);
                },
                [require_obj](DisplayObject& self, const sol::object& desc) {
                    ::SDOM::removeEventListener_lua_any_short(require_obj(self, "removeEventListener"), desc);
                }
            ));
        }

        // Basic state and debug
        if (absent("cleanAll"))      handle.set_function("cleanAll",      [require_obj](DisplayObject& self){ ::SDOM::cleanAll_lua(require_obj(self, "cleanAll")); });
        if (absent("getDirty"))      handle.set_function("getDirty",      [require_obj](DisplayObject& self){ return ::SDOM::getDirty_lua(require_obj(self, "getDirty")); });
        if (absent("setDirty"))      handle.set_function("setDirty",      [require_obj](DisplayObject& self){ ::SDOM::setDirty_lua(require_obj(self, "setDirty")); });
        if (absent("isDirty"))       handle.set_function("isDirty",       [require_obj](DisplayObject& self){ return ::SDOM::isDirty_lua(require_obj(self, "isDirty")); });
        if (absent("printTree"))     handle.set_function("printTree",     [require_obj](DisplayObject& self){ ::SDOM::printTree_lua(require_obj(self, "printTree")); });

        // Hierarchy
        if (absent("addChild"))      handle.set_function("addChild",      [require_obj](DisplayObject& self, DisplayObject child){ ::SDOM::addChild_lua(require_obj(self, "addChild"), child); });
        if (absent("removeChild"))   
        {
            handle.set_function("removeChild",   sol::overload(
                [require_obj](DisplayObject& self, const std::string& name){ return ::SDOM::removeChild_lua(require_obj(self, "removeChild"), name); },
                [require_obj](DisplayObject& self, DisplayObject child){ return ::SDOM::removeChild_lua(require_obj(self, "removeChild"), child); }
            ));
        }
        if (absent("hasChild"))      handle.set_function("hasChild",      [require_obj](DisplayObject& self, DisplayObject child){ return ::SDOM::hasChild_lua(require_obj(self, "hasChild"), child); });
        if (absent("getParent"))     handle.set_function("getParent",     [require_obj](DisplayObject& self){ return ::SDOM::getParent_lua(require_obj(self, "getParent")); });
        if (absent("setParent"))     handle.set_function("setParent",     [require_obj](DisplayObject& self, const DisplayObject& p){ ::SDOM::setParent_lua(require_obj(self, "setParent"), p); });

        if (absent("isAncestorOf"))  handle.set_function("isAncestorOf",  [require_obj](DisplayObject& self, DisplayObject d){ return ::SDOM::isAncestorOf_lua(require_obj(self, "isAncestorOf"), d); });
        if (absent("isAncestorOfName"))
                                    handle.set_function("isAncestorOfName", [require_obj](DisplayObject& self, const std::string& n){ return ::SDOM::isAncestorOf_lua(require_obj(self, "isAncestorOfName"), n); });
        if (absent("isDescendantOf"))handle.set_function("isDescendantOf",[require_obj](DisplayObject& self, DisplayObject a){ return ::SDOM::isDescendantOf_lua(require_obj(self, "isDescendantOf"), a); });
        if (absent("isDescendantOfName"))
                                    handle.set_function("isDescendantOfName", [require_obj](DisplayObject& self, const std::string& n){ return ::SDOM::isDescendantOf_lua(require_obj(self, "isDescendantOfName"), n); });
        if (absent("removeFromParent"))
                                    handle.set_function("removeFromParent", [require_obj](DisplayObject& self){ return ::SDOM::removeFromParent_lua(require_obj(self, "removeFromParent")); });
        if (absent("removeChildByHandle"))
                                    handle.set_function("removeChildByHandle", [require_obj](DisplayObject& self, DisplayObject d){ return ::SDOM::removeChild_lua(require_obj(self, "removeChildByHandle"), d); });
        if (absent("removeChildByName"))
                                    handle.set_function("removeChildByName", [require_obj](DisplayObject& self, const std::string& n){ return ::SDOM::removeChild_lua(require_obj(self, "removeChildByName"), n); });
        if (absent("removeDescendant"))
                                    handle.set_function("removeDescendant", [require_obj](DisplayObject& self, DisplayObject d){ return ::SDOM::removeDescendant_lua(require_obj(self, "removeDescendant"), d); });
        if (absent("removeDescendantByName"))
                                    handle.set_function("removeDescendantByName", [require_obj](DisplayObject& self, const std::string& n){ return ::SDOM::removeDescendant_lua(require_obj(self, "removeDescendantByName"), n); });

        // Type & properties
        if (absent("getType"))       handle.set_function("getType",       [require_obj](DisplayObject& self){ return ::SDOM::getType_lua(require_obj(self, "getType")); });
        if (absent("setType"))       handle.set_function("setType",       [require_obj](DisplayObject& self, const std::string& t){ ::SDOM::setType_lua(require_obj(self, "setType"), t); });

        if (absent("getBounds"))     handle.set_function("getBounds",     [require_obj](DisplayObject& self){ return ::SDOM::getBounds_lua(require_obj(self, "getBounds")); });
        if (absent("setBounds"))     handle.set_function("setBounds",     [require_obj](DisplayObject& self, const sol::object& o){ ::SDOM::setBounds_lua(require_obj(self, "setBounds"), o); });

        if (absent("getColor"))      handle.set_function("getColor",      [require_obj](DisplayObject& self){ return ::SDOM::getColor_lua(require_obj(self, "getColor")); });
        if (absent("setColor"))      handle.set_function("setColor",      [require_obj](DisplayObject& self, const sol::object& o){ ::SDOM::setColor_lua(require_obj(self, "setColor"), o); });

        // Priority & Z-Order
        if (absent("getMaxPriority"))handle.set_function("getMaxPriority",[require_obj](DisplayObject& self){ return ::SDOM::getMaxPriority_lua(require_obj(self, "getMaxPriority")); });
        if (absent("getMinPriority"))handle.set_function("getMinPriority",[require_obj](DisplayObject& self){ return ::SDOM::getMinPriority_lua(require_obj(self, "getMinPriority")); });
        if (absent("getPriority"))   handle.set_function("getPriority",   [require_obj](DisplayObject& self){ return ::SDOM::getPriority_lua(require_obj(self, "getPriority")); });
        if (absent("setToHighestPriority"))
                                    handle.set_function("setToHighestPriority", [require_obj](DisplayObject& self){ ::SDOM::setToHighestPriority_lua(require_obj(self, "setToHighestPriority")); });
        if (absent("setToLowestPriority"))
                                    handle.set_function("setToLowestPriority", [require_obj](DisplayObject& self){ ::SDOM::setToLowestPriority_lua(require_obj(self, "setToLowestPriority")); });
        if (absent("sortChildrenByPriority"))
                                    handle.set_function("sortChildrenByPriority", [require_obj](DisplayObject& self){ ::SDOM::sortChildrenByPriority_lua(require_obj(self, "sortChildrenByPriority")); });
        if (absent("setPriority"))   handle.set_function("setPriority", sol::overload(
            [require_obj](DisplayObject& self, int p){ ::SDOM::setPriority_lua(require_obj(self, "setPriority"), p); },
            [require_obj](DisplayObject& self, const sol::object& desc) {
                IDisplayObject* obj = require_obj(self, "setPriority");
                if (!desc.is<sol::table>()) return;
                sol::table t = desc.as<sol::table>();
                sol::object pobj = t.get<sol::object>("priority");
                if (pobj.valid()) {
                    try { obj->setPriority(pobj.as<int>()); } catch(...) {}
                }
            }
        ));
        if (absent("getChildrenPriorities"))
                                    handle.set_function("getChildrenPriorities", [require_obj](DisplayObject& self){ return ::SDOM::getChildrenPriorities_lua(require_obj(self, "getChildrenPriorities")); });
        if (absent("moveToTop"))     handle.set_function("moveToTop",     [require_obj](DisplayObject& self){ ::SDOM::moveToTop_lua(require_obj(self, "moveToTop")); });
        if (absent("getZOrder"))     handle.set_function("getZOrder",     [require_obj](DisplayObject& self){ return ::SDOM::getZOrder_lua(require_obj(self, "getZOrder")); });
        if (absent("setZOrder"))     handle.set_function("setZOrder",     [require_obj](DisplayObject& self, int z){ ::SDOM::setZOrder_lua(require_obj(self, "setZOrder"), z); });

        // Focus & interactivity
        if (absent("setKeyboardFocus"))  handle.set_function("setKeyboardFocus",  [require_obj](DisplayObject& self){ ::SDOM::setKeyboardFocus_lua(require_obj(self, "setKeyboardFocus")); });
        if (absent("isKeyboardFocused")) handle.set_function("isKeyboardFocused", [require_obj](DisplayObject& self){ return ::SDOM::isKeyboardFocused_lua(require_obj(self, "isKeyboardFocused")); });
        if (absent("isMouseHovered"))    handle.set_function("isMouseHovered",    [require_obj](DisplayObject& self){ return ::SDOM::isMouseHovered_lua(require_obj(self, "isMouseHovered")); });
        if (absent("isClickable"))       handle.set_function("isClickable",       [require_obj](DisplayObject& self){ return ::SDOM::isClickable_lua(require_obj(self, "isClickable")); });
        if (absent("setClickable"))      handle.set_function("setClickable",      [require_obj](DisplayObject& self, bool c){ ::SDOM::setClickable_lua(require_obj(self, "setClickable"), c); });
        if (absent("isEnabled"))         handle.set_function("isEnabled",         [require_obj](DisplayObject& self){ return ::SDOM::isEnabled_lua(require_obj(self, "isEnabled")); });
        if (absent("setEnabled"))        handle.set_function("setEnabled",        [require_obj](DisplayObject& self, bool e){ ::SDOM::setEnabled_lua(require_obj(self, "setEnabled"), e); });
        if (absent("isHidden"))          handle.set_function("isHidden",          [require_obj](DisplayObject& self){ return ::SDOM::isHidden_lua(require_obj(self, "isHidden")); });
        if (absent("setHidden"))         handle.set_function("setHidden",         [require_obj](DisplayObject& self, bool v){ ::SDOM::setHidden_lua(require_obj(self, "setHidden"), v); });
        if (absent("isVisible"))         handle.set_function("isVisible",         [require_obj](DisplayObject& self){ return ::SDOM::isVisible_lua(require_obj(self, "isVisible")); });
        if (absent("setVisible"))        handle.set_function("setVisible",        [require_obj](DisplayObject& self, bool v){ ::SDOM::setVisible_lua(require_obj(self, "setVisible"), v); });

        // Tab management
        if (absent("getTabPriority")) handle.set_function("getTabPriority", [require_obj](DisplayObject& self){ return ::SDOM::getTabPriority_lua(require_obj(self, "getTabPriority")); });
        if (absent("setTabPriority")) handle.set_function("setTabPriority", [require_obj](DisplayObject& self, int i){ ::SDOM::setTabPriority_lua(require_obj(self, "setTabPriority"), i); });
        if (absent("isTabEnabled"))   handle.set_function("isTabEnabled",  [require_obj](DisplayObject& self){ return ::SDOM::isTabEnabled_lua(require_obj(self, "isTabEnabled")); });
        if (absent("setTabEnabled"))  handle.set_function("setTabEnabled", [require_obj](DisplayObject& self, bool e){ ::SDOM::setTabEnabled_lua(require_obj(self, "setTabEnabled"), e); });

        // Geometry & layout
        if (absent("getX"))      handle.set_function("getX",      [require_obj](DisplayObject& self){ return ::SDOM::getX_lua(require_obj(self, "getX")); });
        if (absent("getY"))      handle.set_function("getY",      [require_obj](DisplayObject& self){ return ::SDOM::getY_lua(require_obj(self, "getY")); });
        if (absent("getWidth"))  handle.set_function("getWidth",  [require_obj](DisplayObject& self){ return ::SDOM::getWidth_lua(require_obj(self, "getWidth")); });
        if (absent("getHeight")) handle.set_function("getHeight", [require_obj](DisplayObject& self){ return ::SDOM::getHeight_lua(require_obj(self, "getHeight")); });
        if (absent("setX"))      handle.set_function("setX",      [require_obj](DisplayObject& self, int v){ ::SDOM::setX_lua(require_obj(self, "setX"), v); });
        if (absent("setY"))      handle.set_function("setY",      [require_obj](DisplayObject& self, int v){ ::SDOM::setY_lua(require_obj(self, "setY"), v); });
        if (absent("setWidth"))  handle.set_function("setWidth",  [require_obj](DisplayObject& self, int v){ ::SDOM::setWidth_lua(require_obj(self, "setWidth"), v); });
        if (absent("setHeight")) handle.set_function("setHeight", [require_obj](DisplayObject& self, int v){ ::SDOM::setHeight_lua(require_obj(self, "setHeight"), v); });

        // Anchors
        if (absent("getAnchorTop"))    handle.set_function("getAnchorTop",    [require_obj](DisplayObject& self){ return ::SDOM::getAnchorTop_lua(require_obj(self, "getAnchorTop")); });
        if (absent("getAnchorLeft"))   handle.set_function("getAnchorLeft",   [require_obj](DisplayObject& self){ return ::SDOM::getAnchorLeft_lua(require_obj(self, "getAnchorLeft")); });
        if (absent("getAnchorBottom")) handle.set_function("getAnchorBottom", [require_obj](DisplayObject& self){ return ::SDOM::getAnchorBottom_lua(require_obj(self, "getAnchorBottom")); });
        if (absent("getAnchorRight"))  handle.set_function("getAnchorRight",  [require_obj](DisplayObject& self){ return ::SDOM::getAnchorRight_lua(require_obj(self, "getAnchorRight")); });
        if (absent("setAnchorTop"))    handle.set_function("setAnchorTop",    [require_obj](DisplayObject& self, AnchorPoint ap){ ::SDOM::setAnchorTop_lua(require_obj(self, "setAnchorTop"), ap); });
        if (absent("setAnchorLeft"))   handle.set_function("setAnchorLeft",   [require_obj](DisplayObject& self, AnchorPoint ap){ ::SDOM::setAnchorLeft_lua(require_obj(self, "setAnchorLeft"), ap); });
        if (absent("setAnchorBottom")) handle.set_function("setAnchorBottom", [require_obj](DisplayObject& self, AnchorPoint ap){ ::SDOM::setAnchorBottom_lua(require_obj(self, "setAnchorBottom"), ap); });
        if (absent("setAnchorRight"))  handle.set_function("setAnchorRight",  [require_obj](DisplayObject& self, AnchorPoint ap){ ::SDOM::setAnchorRight_lua(require_obj(self, "setAnchorRight"), ap); });

        // Edge positions
        if (absent("getLeft"))   handle.set_function("getLeft",   [require_obj](DisplayObject& self){ return ::SDOM::getLeft_lua(require_obj(self, "getLeft")); });
        if (absent("getRight"))  handle.set_function("getRight",  [require_obj](DisplayObject& self){ return ::SDOM::getRight_lua(require_obj(self, "getRight")); });
        if (absent("getTop"))    handle.set_function("getTop",    [require_obj](DisplayObject& self){ return ::SDOM::getTop_lua(require_obj(self, "getTop")); });
        if (absent("getBottom")) handle.set_function("getBottom", [require_obj](DisplayObject& self){ return ::SDOM::getBottom_lua(require_obj(self, "getBottom")); });
        if (absent("setLeft"))   handle.set_function("setLeft",   [require_obj](DisplayObject& self, float v){ ::SDOM::setLeft_lua(require_obj(self, "setLeft"), v); });
        if (absent("setRight"))  handle.set_function("setRight",  [require_obj](DisplayObject& self, float v){ ::SDOM::setRight_lua(require_obj(self, "setRight"), v); });
        if (absent("setTop"))    handle.set_function("setTop",    [require_obj](DisplayObject& self, float v){ ::SDOM::setTop_lua(require_obj(self, "setTop"), v); });
        if (absent("setBottom")) handle.set_function("setBottom", [require_obj](DisplayObject& self, float v){ ::SDOM::setBottom_lua(require_obj(self, "setBottom"), v); });

        // Orphan retention helpers
        if (absent("orphanPolicyFromString"))
            handle.set_function("orphanPolicyFromString", &::SDOM::orphanPolicyFromString_lua);
        if (absent("orphanPolicyToString"))
            handle.set_function("orphanPolicyToString", &::SDOM::orphanPolicyToString_lua);
        if (absent("setOrphanRetentionPolicy"))
            handle.set_function("setOrphanRetentionPolicy", [require_obj](DisplayObject& self, const std::string& s){ ::SDOM::setOrphanRetentionPolicy_lua(require_obj(self, "setOrphanRetentionPolicy"), s); });
        if (absent("getOrphanRetentionPolicyString"))
            handle.set_function("getOrphanRetentionPolicyString", [require_obj](DisplayObject& self){ return ::SDOM::getOrphanRetentionPolicyString_lua(require_obj(self, "getOrphanRetentionPolicyString")); });

        // Note: getName/setName are already bound on the minimal handle; do not rebind here.
    } // END: IDisplayObject::_registerDisplayObject()

} // namespace SDOM