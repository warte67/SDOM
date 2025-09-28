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

#include "SDOM/SDOM.hpp"
#include "SDOM/SDOM_Core.hpp"
// #include "SDOM/SDOM_Stage.hpp"
#include "SDOM/SDOM_IDisplayObject.hpp"
// #include "SDOM/SDOM_EventType.hpp"
// #include "SDOM/SDOM_EventManager.hpp"
#include "SDOM/SDOM_Factory.hpp"
// #include "SDOM/SDOM_Stage.hpp"

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

        // Initialize from Lua config table
        fromLua(config, config.lua_state());
    }

    IDisplayObject::~IDisplayObject()
    {
        // nameRegistry_.erase(getName());
        onQuit(); // Call the pure virtual method to ensure derived classes clean up
    }



    void IDisplayObject::_registerLua_Usertype(sol::state_view lua)
    {
        SUPER::_registerLua_Usertype(lua);

        lua.new_usertype<IDisplayObject>("IDisplayObject",
            // --- Properties ---
            "getName", &IDisplayObject::getName,
            "setName", &IDisplayObject::setName,
            "getType", &IDisplayObject::getType,
            "getX", &IDisplayObject::getX,
            "setX", &IDisplayObject::setX,
            "getY", &IDisplayObject::getY,
            "setY", &IDisplayObject::setY,
            "getLeft", &IDisplayObject::getLeft,
            "setLeft", &IDisplayObject::setLeft,
            "getRight", &IDisplayObject::getRight,
            "setRight", &IDisplayObject::setRight,
            "getTop", &IDisplayObject::getTop,
            "setTop", &IDisplayObject::setTop,
            "getBottom", &IDisplayObject::getBottom,
            "setBottom", &IDisplayObject::setBottom,
            "getWidth", &IDisplayObject::getWidth,
            "setWidth", &IDisplayObject::setWidth,
            "getHeight", &IDisplayObject::getHeight,
            "setHeight", &IDisplayObject::setHeight,
            "getPriority", [](IDisplayObject& obj) { return obj.priority_; },
            "setPriority", &IDisplayObject::setPriority,
            "isClickable", &IDisplayObject::isClickable,
            "setClickable", &IDisplayObject::setClickable,
            "isEnabled", &IDisplayObject::isEnabled,
            "setEnabled", &IDisplayObject::setEnabled,
            "isHidden", &IDisplayObject::isHidden,
            "setHidden", &IDisplayObject::setHidden,
            "getTabPriority", &IDisplayObject::getTabPriority,
            "setTabPriority", &IDisplayObject::setTabPriority,
            "isTabEnabled", &IDisplayObject::isTabEnabled,
            "setTabEnabled", &IDisplayObject::setTabEnabled,
            "getZOrder", &IDisplayObject::getZOrder,
            "setZOrder", &IDisplayObject::setZOrder,
            "getColor", &IDisplayObject::getColor,
            "setColor", [](IDisplayObject& obj, sol::table colorTable) {
                SDL_Color color; // Garish Magenta Default
                color.r = colorTable["r"].get_or(255);
                color.g = colorTable["g"].get_or(0);
                color.b = colorTable["b"].get_or(255);
                color.a = colorTable["a"].get_or(255);
                obj.setColor(color);
            },            
            "getAnchorTop", &IDisplayObject::getAnchorTop,
            "setAnchorTop", &IDisplayObject::setAnchorTop,
            "getAnchorLeft", &IDisplayObject::getAnchorLeft,
            "setAnchorLeft", &IDisplayObject::setAnchorLeft,
            "getAnchorBottom", &IDisplayObject::getAnchorBottom,
            "setAnchorBottom", &IDisplayObject::setAnchorBottom,
            "getAnchorRight", &IDisplayObject::getAnchorRight,
            "setAnchorRight", &IDisplayObject::setAnchorRight,
            "bIsDirty", [](IDisplayObject& obj) { return obj.bIsDirty_; },
            "setBIsDirty", [](IDisplayObject& obj, bool val) { obj.bIsDirty_ = val; },

            // --- Hierarchy Management ---
            "addChild", &IDisplayObject::addChild_lua,
            "removeChild", &IDisplayObject::removeChild,
            "hasChild", &IDisplayObject::hasChild,

            // --- Utility ---
            "cleanAll", &IDisplayObject::cleanAll,
            "printTree", &IDisplayObject::printTree_lua,

            // --- Focus & Interactivity ---
            "setKeyboardFocus", &IDisplayObject::setKeyboardFocus,
            "isKeyboardFocused", &IDisplayObject::isKeyboardFocused,
            "isMouseHovered", &IDisplayObject::isMouseHovered,

            // --- Tab Management ---
            // Already included above

            // --- Priority & Z-Order ---
            "getMaxPriority", &IDisplayObject::getMaxPriority,
            "getMinPriority", &IDisplayObject::getMinPriority,
            "setToHighestPriority", &IDisplayObject::setToHighestPriority,
            "setToLowestPriority", &IDisplayObject::setToLowestPriority,
            "sortChildrenByPriority", &IDisplayObject::sortChildrenByPriority,
            "getChildrenPriorities", &IDisplayObject::getChildrenPriorities,
            "moveToTop", &IDisplayObject::moveToTop,

            // --- Visibility & Interactivity ---
            "setVisible", &IDisplayObject::setVisible,

            // --- Event Handling ---
            // For these, you may need to wrap with lambdas if signatures are not directly compatible
            "addEventListener", [](IDisplayObject& obj, EventType type, std::function<void(Event&)> listener, bool useCapture, int priority) {
                obj.addEventListener(type, listener, useCapture, priority);
            },
            "removeEventListener", [](IDisplayObject& obj, EventType type, std::function<void(Event&)> listener, bool useCapture) {
                obj.removeEventListener(type, listener, useCapture);
            },
            "triggerEventListeners", [](IDisplayObject& obj, Event& event, bool useCapture) {
                obj.triggerEventListeners(event, useCapture);
            }
        );
        // std::cout << "IDisplayObject: Registered Lua bindings." << std::endl;
    } // end _registerLua_Usertype


    void IDisplayObject::_registerLua(const std::string& typeName, sol::state_view lua)
    {
        std::string typeNameLocal = "IDisplayObject";
        std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                    << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN << typeName << CLR::RESET << std::endl;

        // 1. Call base class registration to include inherited properties/commands
        SUPER::_registerLua(typeName, lua);

        // 2. Register this class's properties and commands
        //    factory_->registerLuaProperty(typeName, ...);
        //    factory_->registerLuaCommand(typeName, ...);

        // 3. Register the Lua usertype using the registry
        getFactory().registerLuaUsertype<IDisplayObject>(typeName, lua);
    } // end _registerLua()


    bool IDisplayObject::onInit()
    {
        // Default implementation, can be overridden by derived classes
        // registerSelfName(); // Register this object in the name registry
        return true; // Indicate successful initialization
    }


    void IDisplayObject::attachChild_(DomHandle p_child, DomHandle p_parent, bool useWorld, int worldX, int worldY)
    {
        IDisplayObject* child = dynamic_cast<IDisplayObject*>(p_child.get());
        IDisplayObject* parent = dynamic_cast<IDisplayObject*>(p_parent.get());
        if (child && parent) 
        {
            auto& parentChildren = parent->children_;
            auto it = std::find(parentChildren.begin(), parentChildren.end(), p_child);
            if (it == parentChildren.end()) 
            {
                // Record world edges BEFORE changing parent
                float leftWorld = child->getLeft();
                float rightWorld = child->getRight();
                float topWorld = child->getTop();
                float bottomWorld = child->getBottom();

                child->setParent(p_parent); 
                parentChildren.push_back(p_child);

                if (useWorld)
                {
                    child->setLeft(leftWorld);
                    child->setRight(rightWorld);
                    child->setTop(topWorld);
                    child->setBottom(bottomWorld);
                    child->setX(worldX);
                    child->setY(worldY);
                }
            }
        }
    } // IDisplayObject::attachChild_(DomHandle p_child, DomHandle p_parent, bool useWorld, int worldX, int worldY)

    void IDisplayObject::removeOrphan_(const DomHandle& orphan) 
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
                    // Reset orphan's parent using DomHandle (nullptr)
                    orphanObj->setParent(DomHandle());
                    parentChildren.erase(it); // Remove orphan from parent's children
                }
            }
        }
    }

    void IDisplayObject::addChild(DomHandle child, bool useWorld, int worldX, int worldY)
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
            factory->addToFutureChildrenList(child, DomHandle(getName(), child->getType()), useWorld, worldX, worldY);
        }
        else
        {
            attachChild_(child, DomHandle(getName(), getType()), useWorld, worldX, worldY);
        }
    }
    void IDisplayObject::addChild_lua(DomHandle child)
    {
        addChild(child, false, 0, 0);
    }


    bool IDisplayObject::removeChild(DomHandle child)
    {
        if (!child) 
        {
            ERROR("removeChild: child handle is null in " + name_);
            return false;
        }
        auto it = std::find(children_.begin(), children_.end(), child);
        if (it != children_.end()) 
        {
            children_.erase(it);
            // Reset child's parent pointer if possible
            if (auto* childObj = dynamic_cast<IDisplayObject*>(child.get())) 
            {
                childObj->setParent(DomHandle());
            }
            Core* core = &Core::getInstance();
            Factory* factory = &core->getFactory();
            if (core->getIsTraversing())
            {
                factory->addToOrphanList(child); // Defer orphan removal
            }
            else
            {
                removeOrphan_(child); // Remove orphan immediately
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

        // Traverse children using DomHandle
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




    // void IDisplayObject::registerSelfName() 
    // {
    //     std::string name = getName();
    //     if (nameRegistry_.find(name_) != nameRegistry_.end()) {
    //         ERROR("IDisplayObject name '" + name_ + "' is already registered!");
    //     }
    //     nameRegistry_[name_] = DomHandle(name_, getType());
    // }  

    // std::shared_ptr<IDisplayObject> IDisplayObject::getByName(const std::string& name) 
    // {
    //     std::function<std::shared_ptr<IDisplayObject>(const std::shared_ptr<IDisplayObject>&)> search;
    //     search = [&](const std::shared_ptr<IDisplayObject>& node) -> std::shared_ptr<IDisplayObject> {
    //         if (!node) return nullptr;
    //         if (node->getName() == name) return node;
    //         for (const auto& child : node->children_) {
    //             auto result = search(child);
    //             if (result) return result;
    //         }
    //         return nullptr;
    //     };
    //     return search(Core::instance().getStage("mainStage"));
    // }

    int IDisplayObject::getMaxPriority() const
    {
        if (children_.empty()) 
            return priority_;
        auto it = std::max_element(
            children_.begin(), children_.end(),
            [](const DomHandle& a, const DomHandle& b) {
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
            [](const DomHandle& a, const DomHandle& b) {
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
        children_.erase(std::remove_if(children_.begin(), children_.end(), [](const DomHandle& child) {
            return !child;
        }), children_.end());

        // Sort by priority (ascending)
        std::sort(children_.begin(), children_.end(), [](const DomHandle& a, const DomHandle& b) {
            auto* aObj = dynamic_cast<IDisplayObject*>(a.get());
            auto* bObj = dynamic_cast<IDisplayObject*>(b.get());
            int aPriority = aObj ? aObj->priority_ : std::numeric_limits<int>::min();
            int bPriority = bObj ? bObj->priority_ : std::numeric_limits<int>::min();
            return aPriority < bPriority;
        });

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

    bool IDisplayObject::hasChild(const DomHandle child) const
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
        // Initialization logic for Box
        static int s_nextIndex = 0;
        if (enabled)
        {
            tabPriority_ = s_nextIndex;
            s_nextIndex++;    
            tabEnabled_ = true;   
        }
        tabEnabled_ = enabled; 
        return *this; 
    }

    void IDisplayObject::setKeyboardFocus() 
    { 
        Core::getInstance().setKeyboardFocusedObject(DomHandle(getName(), getType())); 
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


} // namespace SDOM