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

        // Anchor conversion helpers (assume stringToAnchorPoint_ is accessible)
        auto getAnchor = [&](const char* key) {
            std::string str = config[key].valid() ? config[key].get<std::string>() : "top_left";
            auto it = stringToAnchorPoint_.find(str);
            return it != stringToAnchorPoint_.end() ? it->second : AnchorPoint::TOP_LEFT;
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
    void IDisplayObject::addChild_lua(DisplayObject child)
    {
        addChild(child, false, 0, 0);
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
        if (DEBUG_REGISTER_LUA)
        {
            std::string typeNameLocal = "IDisplayObject";
            std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN 
                    << typeName << CLR::RESET << std::endl;
        }

        // Call base class registration to include inherited properties/commands
        SUPER::_registerDisplayObject(typeName, lua);

        // Create the IDisplayObject usertype and bind the available Lua helper wrappers
        sol::usertype<IDisplayObject> objHandleType = lua.new_usertype<IDisplayObject>(
            typeName,
            sol::no_constructor,
            sol::base_classes, sol::bases<SUPER>()
        );

        // Ensure Bounds is registered as a userdata for Lua (compatibility)
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

        // MOVED TO: SDOM_SDL_Utils.cpp
        // // Register SDL_Color userdata so Lua can access r/g/b/a and getter helpers
        // if (!lua["SDL_Color"].valid()) {
        //     lua.new_usertype<SDL_Color>("SDL_Color",
        //         "r", &SDL_Color::r,
        //         "g", &SDL_Color::g,
        //         "b", &SDL_Color::b,
        //         "a", &SDL_Color::a,
        //         "getR", [](const SDL_Color& c) { return c.r; },
        //         "getG", [](const SDL_Color& c) { return c.g; },
        //         "getB", [](const SDL_Color& c) { return c.b; },
        //         "getA", [](const SDL_Color& c) { return c.a; }
        //     );
        // }

        // Event handling (wrappers expect: IDisplayObject*, EventType&, sol::function, bool, int)
        // (Accept either the typed signature or a flexible Lua table/function descriptor)
        objHandleType.set_function("addEventListener", sol::overload(
            static_cast<void(*)(IDisplayObject*, EventType&, sol::function, bool, int)>(&::SDOM::addEventListener_lua),
            static_cast<void(*)(IDisplayObject*, const sol::object&, const sol::object&, const sol::object&, const sol::object&)>(&::SDOM::addEventListener_lua_any),
            static_cast<void(*)(IDisplayObject*, const sol::object&)>(&::SDOM::addEventListener_lua_any_short)
        ));

        objHandleType.set_function("removeEventListener", sol::overload(
            static_cast<void(*)(IDisplayObject*, EventType&, sol::function, bool)>(&::SDOM::removeEventListener_lua),
            static_cast<void(*)(IDisplayObject*, const sol::object&, const sol::object&, const sol::object&)>(&::SDOM::removeEventListener_lua_any),
            static_cast<void(*)(IDisplayObject*, const sol::object&)>(&::SDOM::removeEventListener_lua_any_short)
        ));
        
        // Basic state and debug
        objHandleType.set_function("cleanAll", &::SDOM::cleanAll_lua);
        objHandleType.set_function("getDirty", &::SDOM::getDirty_lua);
        objHandleType.set_function("setDirty", &::SDOM::setDirty_lua);
        objHandleType.set_function("isDirty", &::SDOM::isDirty_lua);
        objHandleType.set_function("printTree", &::SDOM::printTree_lua);

            // Hierarchy management
        objHandleType.set_function("addChild", &::SDOM::addChild_lua);
        objHandleType.set_function("removeChild", &::SDOM::removeChild_lua);
        objHandleType.set_function("hasChild", &::SDOM::hasChild_lua);
        objHandleType.set_function("getParent", &::SDOM::getParent_lua);
        objHandleType.set_function("setParent", &::SDOM::setParent_lua);

            // Type & property access
        objHandleType.set_function("getType", &::SDOM::getType_lua);
        objHandleType.set_function("setType", &::SDOM::setType_lua);

    objHandleType.set_function("getBounds", &::SDOM::getBounds_lua);
    // Bind the Lua-facing overload (takes sol::object) explicitly to avoid overload ambiguity
    objHandleType.set_function("setBounds", static_cast<IDisplayObject*(*)(IDisplayObject*, const sol::object&)>(&::SDOM::setBounds_lua));
        objHandleType.set_function("getColor", &::SDOM::getColor_lua);
    // Bind the Lua-friendly setColor (accepts table or SDL_Color userdata)
    objHandleType.set_function("setColor", static_cast<IDisplayObject*(*)(IDisplayObject*, const sol::object&)>(&::SDOM::setColor_lua));

            // Priority & Z-Order
        objHandleType.set_function("getMaxPriority", &::SDOM::getMaxPriority_lua);
        objHandleType.set_function("getMinPriority", &::SDOM::getMinPriority_lua);
        objHandleType.set_function("getPriority", &::SDOM::getPriority_lua);
        objHandleType.set_function("setToHighestPriority", &::SDOM::setToHighestPriority_lua);
        objHandleType.set_function("setToLowestPriority", &::SDOM::setToLowestPriority_lua);
        objHandleType.set_function("sortChildrenByPriority", &::SDOM::sortChildrenByPriority_lua);
        objHandleType.set_function("setPriority", sol::overload(
            static_cast<IDisplayObject*(*)(IDisplayObject*, int)>(&::SDOM::setPriority_lua),
            // table descriptor form: setPriority({ priority = N })
            static_cast<IDisplayObject*(*)(IDisplayObject*, const sol::object&)> ([](IDisplayObject* obj, const sol::object& descriptor) -> IDisplayObject* {
                if (!obj) return nullptr;
                if (!descriptor.is<sol::table>()) return obj;
                sol::table t = descriptor.as<sol::table>();
                sol::object pobj = t.get<sol::object>("priority");
                if (!pobj.valid()) return obj;
                try {
                    int p = pobj.as<int>();
                    obj->setPriority(p);
                } catch(...) {}
                return obj;
            })
        ));
        objHandleType.set_function("getChildrenPriorities", &::SDOM::getChildrenPriorities_lua);
        objHandleType.set_function("moveToTop", &::SDOM::moveToTop_lua);
        objHandleType.set_function("getZOrder", &::SDOM::getZOrder_lua);
        objHandleType.set_function("setZOrder", &::SDOM::setZOrder_lua);

            // Focus & interactivity
        objHandleType.set_function("setKeyboardFocus", &::SDOM::setKeyboardFocus_lua);
        objHandleType.set_function("isKeyboardFocused", &::SDOM::isKeyboardFocused_lua);
        objHandleType.set_function("isMouseHovered", &::SDOM::isMouseHovered_lua);
        objHandleType.set_function("isClickable", &::SDOM::isClickable_lua);
        objHandleType.set_function("setClickable", &::SDOM::setClickable_lua);
        objHandleType.set_function("isEnabled", &::SDOM::isEnabled_lua);
        objHandleType.set_function("setEnabled", &::SDOM::setEnabled_lua);
        objHandleType.set_function("isHidden", &::SDOM::isHidden_lua);
        objHandleType.set_function("setHidden", &::SDOM::setHidden_lua);
        objHandleType.set_function("isVisible", &::SDOM::isVisible_lua);
        objHandleType.set_function("setVisible", &::SDOM::setVisible_lua);

            // Tab management
        objHandleType.set_function("getTabPriority", &::SDOM::getTabPriority_lua);
        objHandleType.set_function("setTabPriority", &::SDOM::setTabPriority_lua);
        objHandleType.set_function("isTabEnabled", &::SDOM::isTabEnabled_lua);
        objHandleType.set_function("setTabEnabled", &::SDOM::setTabEnabled_lua);

            // Geometry & layout
        objHandleType.set_function("getX", &::SDOM::getX_lua);
        objHandleType.set_function("getY", &::SDOM::getY_lua);
        objHandleType.set_function("getWidth", &::SDOM::getWidth_lua);
        objHandleType.set_function("getHeight", &::SDOM::getHeight_lua);
        objHandleType.set_function("setX", &::SDOM::setX_lua);
        objHandleType.set_function("setY", &::SDOM::setY_lua);
        objHandleType.set_function("setWidth", &::SDOM::setWidth_lua);
        objHandleType.set_function("setHeight", &::SDOM::setHeight_lua);

            // Anchors
        objHandleType.set_function("getAnchorTop", &::SDOM::getAnchorTop_lua);
        objHandleType.set_function("getAnchorLeft", &::SDOM::getAnchorLeft_lua);
        objHandleType.set_function("getAnchorBottom", &::SDOM::getAnchorBottom_lua);
        objHandleType.set_function("getAnchorRight", &::SDOM::getAnchorRight_lua);
        objHandleType.set_function("setAnchorTop", &::SDOM::setAnchorTop_lua);
        objHandleType.set_function("setAnchorLeft", &::SDOM::setAnchorLeft_lua);
        objHandleType.set_function("setAnchorBottom", &::SDOM::setAnchorBottom_lua);
        objHandleType.set_function("setAnchorRight", &::SDOM::setAnchorRight_lua);

            // Edge positions
        objHandleType.set_function("getLeft", &::SDOM::getLeft_lua);
        objHandleType.set_function("getRight", &::SDOM::getRight_lua);
        objHandleType.set_function("getTop", &::SDOM::getTop_lua);
        objHandleType.set_function("getBottom", &::SDOM::getBottom_lua);
        objHandleType.set_function("setLeft", &::SDOM::setLeft_lua);
        objHandleType.set_function("setRight", &::SDOM::setRight_lua);
        objHandleType.set_function("setTop", &::SDOM::setTop_lua);
        objHandleType.set_function("setBottom", &::SDOM::setBottom_lua);

            // Convenience: expose IDataObject::getName/setName for scripts
        objHandleType.set_function("getName", &::SDOM::IDataObject::getName);
        objHandleType.set_function("setName", &::SDOM::IDataObject::setName);

        // Orphan retention helpers: string-based helpers for Lua scripts
        objHandleType.set_function("orphanPolicyFromString", &::SDOM::orphanPolicyFromString_lua);
        objHandleType.set_function("orphanPolicyToString", &::SDOM::orphanPolicyToString_lua);
        // Expose set/get helpers that operate with string names for ease of use in Lua
        objHandleType.set_function("setOrphanRetentionPolicy", &::SDOM::setOrphanRetentionPolicy_lua);
        objHandleType.set_function("getOrphanRetentionPolicyString", &::SDOM::getOrphanRetentionPolicyString_lua);


        // Save the usertype for later use by derived classes
        this->objHandleType_ = objHandleType;

    } // End IDisplayObject::_registerDisplayObject()


} // namespace SDOM