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
// #include "SDOM/SDOM_Core.hpp"
// #include "SDOM/SDOM_Stage.hpp"
#include "SDOM/SDOM_IDisplayObject.hpp"
// #include "SDOM/SDOM_EventType.hpp"
// #include "SDOM/SDOM_EventManager.hpp"
// #include "SDOM/SDOM_Factory.hpp"
// #include "SDOM/SDOM_Stage.hpp"

namespace SDOM
{
    // IDisplayObject implementation

    IDisplayObject::IDisplayObject(const InitDisplayObject& init)
        : IDisplayObject()
    {
        // name_ = init.name;
        setName(init.name);
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
        parent_.reset();
    }

    IDisplayObject::IDisplayObject(const Json& config)
        : IDisplayObject()
    {
        // Set anchors from JSON (default to TOP_LEFT if not present)
        setAnchorLeft(stringToAnchorPoint_.count(config.value("anchorLeft", "top_left")) 
            ? stringToAnchorPoint_.at(config.value("anchorLeft", "top_left")) 
            : AnchorPoint::TOP_LEFT);
        setAnchorTop(stringToAnchorPoint_.count(config.value("anchorTop", "top_left")) 
            ? stringToAnchorPoint_.at(config.value("anchorTop", "top_left")) 
            : AnchorPoint::TOP_LEFT);
        setAnchorRight(stringToAnchorPoint_.count(config.value("anchorRight", "top_left")) 
            ? stringToAnchorPoint_.at(config.value("anchorRight", "top_left")) 
            : AnchorPoint::TOP_LEFT);
        setAnchorBottom(stringToAnchorPoint_.count(config.value("anchorBottom", "top_left")) 
            ? stringToAnchorPoint_.at(config.value("anchorBottom", "top_left")) 
            : AnchorPoint::TOP_LEFT);

        setLeft(config.value("x", 0));
        setTop(config.value("y", 0));
        setRight(config.value("x", 0) + config.value("width", 0));
        setBottom(config.value("y", 0) + config.value("height", 0));
        
        bIsDirty_ = config.value("bIsDirty", false);
        priority_ = config.value("priority", 0);
        setClickable(config.value("clickable", false));
    }

    IDisplayObject::IDisplayObject()
    {
        // Register properties and commands
        // Registration of properties and commands
        registerProperty("name",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).getName(); },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).setName(val.get<std::string>());
                return obj;
            });


        registerProperty("x",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).getLeft(); },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).setLeft(val.get<int>());
                return obj;
            });

        registerProperty("y",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).getTop(); },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).setTop(val.get<int>());
                return obj;
            });

        registerProperty("left",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).getLeft(); },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).setLeft(val.get<int>());
                return obj;
            });        
        registerProperty("top",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).getTop(); },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).setTop(val.get<int>());
                return obj;
            });
        registerProperty("right",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).getRight(); },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).setRight(val.get<int>());
                return obj;
            });
        registerProperty("bottom",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).getBottom(); },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).setBottom(val.get<int>());
                return obj;
            });

        registerProperty("width",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).getWidth(); },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).setWidth(val.get<int>());
                return obj;
            });

        registerProperty("height",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).getHeight(); },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).setHeight(val.get<int>());
                return obj;
            });


        registerProperty("priority",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).priority_; },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).priority_ = val.get<int>();
                return obj;
            });
        registerProperty("bIsDirty",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).bIsDirty_; },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).bIsDirty_ = val.get<bool>();
                return obj;
            });
        registerProperty("clickable",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).isClickable(); },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).setClickable(val.get<bool>());
                return obj;
            });
        registerProperty("enabled",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).isEnabled(); },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).setEnabled(val.get<bool>());
                return obj;
            });

            
        registerProperty("hidden",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).isHidden(); },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).setHidden(val.get<bool>());
                return obj;
            });
        // registerProperty("z_order",
        //     [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).getZOrder(); },
        //     [](IDataObject& obj, const Json& val) -> IDataObject& {
        //         static_cast<IDisplayObject&>(obj).setZOrder(val.get<int>());
        //         return obj;
        //     });

        // Set color via JSON like: { "color": { "r": 128, "g": 200, "b": 255, "a": 255 } }
        registerProperty("color",
            [](const IDataObject& obj) { 
                SDL_Color c = static_cast<const IDisplayObject&>(obj).getColor();
                Json j;
                j["r"] = c.r;
                j["g"] = c.g;
                j["b"] = c.b;
                j["a"] = c.a;
                return j;
            },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                SDL_Color color;
                color.r = val.value("r", 255);
                color.g = val.value("g", 255);
                color.b = val.value("b", 255);
                color.a = val.value("a", 255);
                static_cast<IDisplayObject&>(obj).setColor(color);
                return obj;
            }
        );

        registerProperty("tabPriority",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).getTabPriority(); },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).setTabPriority(val.get<int>());
                return obj;
            });

        registerProperty("tabEnabled",
            [](const IDataObject& obj) { return static_cast<const IDisplayObject&>(obj).isTabEnabled(); },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                static_cast<IDisplayObject&>(obj).setTabEnabled(val.get<bool>());
                return obj;
            });
        
        // registerProperty("parent",
        //     [](const IDataObject& obj) { return reinterpret_cast<uintptr_t>(static_cast<const IDisplayObject&>(obj).parent_.lock().get()); },
        //     nullptr); // read-only
        // registerProperty("children",
        //     [](const IDataObject& obj) 
        //     {
        //         const auto& children = static_cast<const IDisplayObject&>(obj).getChildren();
        //         Json arr = Json::array();
        //         for (const auto& child : children) 
        //         {
        //             if (child) arr.push_back(child->getName());
        //         }
        //         return arr;
        //     },
        //     nullptr /* read-only */ );

        registerProperty("anchorTop",
            [](const IDataObject& obj) { 
                auto ap = static_cast<const IDisplayObject&>(obj).getAnchorTop();
                auto it = anchorPointToString_.find(ap);
                return it != anchorPointToString_.end() ? it->second : "default";
            },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                std::string str = val.get<std::string>();
                auto it = stringToAnchorPoint_.find(str);
                if (it != stringToAnchorPoint_.end()) {
                    static_cast<IDisplayObject&>(obj).setAnchorTop(it->second);
                }
                return obj;
            });

        registerProperty("anchorLeft",
            [](const IDataObject& obj) { 
                auto ap = static_cast<const IDisplayObject&>(obj).getAnchorLeft();
                auto it = anchorPointToString_.find(ap);
                return it != anchorPointToString_.end() ? it->second : "default";
            },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                std::string str = val.get<std::string>();
                auto it = stringToAnchorPoint_.find(str);
                if (it != stringToAnchorPoint_.end()) {
                    static_cast<IDisplayObject&>(obj).setAnchorLeft(it->second);
                }
                return obj;
            });

        registerProperty("anchorBottom",
            [](const IDataObject& obj) { 
                auto ap = static_cast<const IDisplayObject&>(obj).getAnchorBottom();
                auto it = anchorPointToString_.find(ap);
                return it != anchorPointToString_.end() ? it->second : "default";
            },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                std::string str = val.get<std::string>();
                auto it = stringToAnchorPoint_.find(str);
                if (it != stringToAnchorPoint_.end()) {
                    static_cast<IDisplayObject&>(obj).setAnchorBottom(it->second);
                }
                return obj;
            });

        registerProperty("anchorRight",
            [](const IDataObject& obj) { 
                auto ap = static_cast<const IDisplayObject&>(obj).getAnchorRight();
                auto it = anchorPointToString_.find(ap);
                return it != anchorPointToString_.end() ? it->second : "default";
            },
            [](IDataObject& obj, const Json& val) -> IDataObject& {
                std::string str = val.get<std::string>();
                auto it = stringToAnchorPoint_.find(str);
                if (it != stringToAnchorPoint_.end()) {
                    static_cast<IDisplayObject&>(obj).setAnchorRight(it->second);
                }
                return obj;
            });


        // // Register commands
        // registerCommand("addChild",
        //     [](IDataObject& obj, const Json& val) {
        //         std::string childName = val.get<std::string>();
        //         auto childPtr = IDisplayObject::getByName(childName);
        //         if (childPtr) {
        //             static_cast<IDisplayObject&>(obj).addChild(childPtr);
        //         } else {
        //             ERROR("addChild: No IDisplayObject found with name '" + childName + "'");
        //         }
        //     });

        // registerCommand("removeChild",
        //     [](IDataObject& obj, const Json& val) {
        //         std::string childName = val.get<std::string>();
        //         auto childPtr = IDisplayObject::getByName(childName);
        //         if (childPtr) {
        //             static_cast<IDisplayObject&>(obj).removeChild(childPtr);
        //         } else {
        //             ERROR("removeChild: No IDisplayObject found with name '" + childName + "'");
        //         }
        //     });

        // registerCommand("sortChildrenByPriority",
        //     [](IDataObject& obj, const Json&) {
        //         static_cast<IDisplayObject&>(obj).sortChildrenByPriority();
        //     });
        // registerCommand("setToHighestPriority",
        //     [](IDataObject& obj, const Json&) {
        //         static_cast<IDisplayObject&>(obj).setToHighestPriority();
        //     });
        // registerCommand("setToLowestPriority",
        //     [](IDataObject& obj, const Json&) {
        //         static_cast<IDisplayObject&>(obj).setToLowestPriority();
        //     });
        // registerCommand("moveToTop",
        //     [](IDataObject& obj, const Json&) {
        //         static_cast<IDisplayObject&>(obj).moveToTop();
        //     });
        registerCommand("cleanAll",
            [](IDataObject& obj, const Json&) {
                static_cast<IDisplayObject&>(obj).cleanAll();
            });
        registerCommand("printTree",
            [](IDataObject& obj, const Json&) {
                static_cast<IDisplayObject&>(obj).printTree(0, true, {});
            });

        // ToDo: Event listeners
        // ...
    }

    IDisplayObject::~IDisplayObject()
    {
        nameRegistry_.erase(getName());
        onQuit(); // Call the pure virtual method to ensure derived classes clean up
    }

    bool IDisplayObject::onInit()
    {
        // Default implementation, can be overridden by derived classes
        // registerSelfName(shared_from_this()); // Register this object in the name registry
        return true; // Indicate successful initialization
    }

    // void IDisplayObject::addChild(std::shared_ptr<IDisplayObject> child, bool useWorld, int worldX, int worldY)
    // {
    //     if (!child) 
    //     {
    //         ERROR("Attempted to add a null child to IDisplayObject: " + name_);
    //         return;
    //     }
    //     // child->setParent(std::weak_ptr<IDisplayObject>{}); // is inside DOM traversal
    //     Factory* factory = Core::instance().getFactory();
    //     factory->addToFutureChildrenList(child, shared_from_this(), useWorld, worldX, worldY);
    // }

    // bool IDisplayObject::removeChild(std::shared_ptr<IDisplayObject> child)
    // {
    //     // Debug: Check validity of 'children_' and 'child'
    //     if (children_.empty()) {
    //         std::cout << "[removeChild] children_ vector is empty for " << name_ << std::endl;
    //     }
    //     if (!child) {
    //         std::cout << "[removeChild] child pointer is null!" << std::endl;
    //         return false;
    //     }
    //     for (size_t i = 0; i < children_.size(); ++i) {
    //         if (!children_[i]) {
    //             std::cout << "[removeChild] children_[" << i << "] is null in " << name_ << std::endl;
    //         }
    //     }
    //     // Print addresses for comparison
    //     std::cout << "[removeChild] this=" << reinterpret_cast<uintptr_t>(this)
    //               << ", child=" << reinterpret_cast<uintptr_t>(child.get()) << std::endl;
    //     for (size_t i = 0; i < children_.size(); ++i) {
    //         std::cout << "[removeChild] children_[" << i << "]=" << reinterpret_cast<uintptr_t>(children_[i].get()) << std::endl;
    //     }

    //     // Unlink child from parent's children vector
    //     auto it = std::find(children_.begin(), children_.end(), child);
    //     if (it != children_.end()) {
    //         children_.erase(it);
    //     } else {
    //         ERROR("removeChild: child not found in children_ vector of " + name_);
    //     }

    //     // // Reset child's parent pointer
    //     // child->setParent(std::weak_ptr<IDisplayObject>{}); // still within DOM traversal

    //     // Defer orphan processing if needed
    //     Factory* factory = Core::instance().getFactory();
    //     factory->addToOrphanList(child);

    //     return true;
    // }

    void IDisplayObject::cleanAll() 
    {
        bIsDirty_ = false;
        for (auto& child : children_) {
            if (child) {
                child->cleanAll();
            }
        }
    }

    void IDisplayObject::printTree(int depth, bool isLast, const std::vector<bool>& hasMoreSiblings) const
    {
        // Print the current node with indentation and tree characters
        for (size_t i = 0; i < hasMoreSiblings.size(); ++i) {
            std::cout << (hasMoreSiblings[i] ? "│   " : "    ");
        }
        if (depth > 0) {
            std::cout << (isLast ? "└── " : "├── ");
        }
        std::cout << getName() << std::endl;

        // Check if the object is a IDisplayObjectContainer
        if (auto container = dynamic_cast<const IDisplayObject*>(this)) {
            for (size_t i = 0; i < container->children_.size(); ++i) {
                auto& weakChild = container->children_[i];
                if (!weakChild) {
                    continue;
                }

                if (auto& child = weakChild) {
                    auto newHasMoreSiblings = hasMoreSiblings;
                    newHasMoreSiblings.push_back(!isLast);
                    child->printTree(depth + 1, i == container->children_.size() - 1, newHasMoreSiblings);
                }
            }
        }
    }

    void IDisplayObject::addEventListener(
        const EventType& type, 
        std::function<void(const Event&)> listener, 
        bool useCapture, 
        int priority)
    {
        auto& targetListeners = useCapture ? captureEventListeners : bubblingEventListeners;
        targetListeners[type].push_back({std::move(listener), priority});

        // Sort listeners by priority if the new listener has a non-zero priority
        std::sort(targetListeners[type].begin(), targetListeners[type].end(),
            [](const ListenerEntry& a, const ListenerEntry& b) {
                return a.priority > b.priority; // Higher priority first
            });
    }

    void IDisplayObject::removeEventListener(
        const EventType& type, 
        std::function<void(const Event&)> listener, 
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

    void IDisplayObject::triggerEventListeners(const Event& event, bool useCapture)
    {
        const auto& targetListeners = useCapture ? captureEventListeners : bubblingEventListeners;
        auto it = targetListeners.find(event.getType());
        if (it != targetListeners.end()) 
        {
            for (const auto& entry : it->second) 
            {
                entry.listener(event);
                if (event.isPropagationStopped()) 
                {
                    break; // Stop further listeners if propagation is stopped
                }
            }
        } else {
            // std::cout << "Debug: No listeners found for event type " << event.getTypeName() << std::endl;
        }
    }

    // void IDisplayObject::registerSelfName(std::shared_ptr<IDisplayObject> self) 
    // {
    //     if (nameRegistry_.find(name_) != nameRegistry_.end()) {
    //         ERROR("IDisplayObject name '" + name_ + "' is already registered!");
    //     }
    //     nameRegistry_[name_] = self;
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

    // int IDisplayObject::getMaxPriority() const
    // {
    //     if (children_.empty()) return priority_;
    //     return std::max_element(children_.begin(), children_.end(), [](const auto& a, const auto& b) {
    //         return a->priority_ < b->priority_;
    //     })->get()->priority_;
    // }

    // int IDisplayObject::getMinPriority() const
    // {
    //     if (children_.empty()) return priority_;
    //     return std::min_element(children_.begin(), children_.end(), [](const auto& a, const auto& b) {
    //         return a->priority_ < b->priority_;
    //     })->get()->priority_;
    // }

    // IDisplayObject& IDisplayObject::setToHighestPriority()
    // {
    //     auto parentShared = parent_.lock(); // Convert weak_ptr to shared_ptr
    //     priority_ = parentShared ? parentShared->getMaxPriority() + 1 : getMaxPriority() + 1;
    //     if (parentShared) {
    //         parentShared->sortChildrenByPriority();
    //     } else {
    //         sortChildrenByPriority(); // Ensure children are sorted if this is the root node
    //     }        
    //     return *this;
    // }

    // IDisplayObject& IDisplayObject::setToLowestPriority()
    // {
    //     auto parentShared = parent_.lock(); // Convert weak_ptr to shared_ptr
    //     priority_ = parentShared ? parentShared->getMinPriority() - 1 : getMinPriority() - 1;
    //     if (parentShared) {
    //         parentShared->sortChildrenByPriority();
    //     } else {
    //         sortChildrenByPriority(); // Ensure children are sorted if this is the root node
    //     }
    //     return *this;
    // }

    // IDisplayObject& IDisplayObject::sortChildrenByPriority()
    // {
    //     // Ensure all children are valid before sorting
    //     children_.erase(std::remove_if(children_.begin(), children_.end(), [](const auto& child) {
    //         return !child;
    //     }), children_.end());
    //     std::sort(children_.begin(), children_.end(), [](const auto& a, const auto& b) {
    //         return a->priority_ < b->priority_; // Sort in ascending order
    //     });
    //     return *this;
    // }

    // IDisplayObject& IDisplayObject::setPriority(int newPriority)
    // {
    //     priority_ = newPriority;
    //     auto parentShared = parent_.lock(); // Convert weak_ptr to shared_ptr
    //     if (parentShared) {
    //         parentShared->sortChildrenByPriority();
    //     } else {
    //         sortChildrenByPriority(); // Ensure children are sorted if this is the root node
    //     }
    //     return *this;
    // }

    // std::vector<int> IDisplayObject::getChildrenPriorities() const
    // {
    //     std::vector<int> priorities;
    //     for (const auto& child : children_) {
    //         priorities.push_back(child->priority_);
    //     }
    //     return priorities;
    // }

    // IDisplayObject& IDisplayObject::moveToTop()
    // {
    //     auto parentShared = parent_.lock();
    //     if (parentShared) {
    //         // Set this object's priority to highest among siblings
    //         int maxPriority = parentShared->getMaxPriority();
    //         setPriority(maxPriority + 1);
    //         parentShared->sortChildrenByPriority();
    //         // Recursively move parent to top
    //         parentShared->moveToTop();
    //     } else {
    //         // If no parent, just set to highest among children (root node)
    //         setPriority(getMaxPriority() + 1);
    //         sortChildrenByPriority();
    //     }
    //     return *this;
    // }

    // bool IDisplayObject::hasChild(const std::shared_ptr<IDisplayObject>& child) const
    // {
    //     return std::find(children_.begin(), children_.end(), child) != children_.end();
    // }

    // int IDisplayObject::getTabPriority() const 
    // { 
    //     return tabPriority_; 
    // }

    // IDisplayObject& IDisplayObject::setTabPriority(int index) 
    // { 
    //     tabPriority_ = index; 
    //     return *this; 
    // }
    
    // bool IDisplayObject::isTabEnabled() const 
    // { 
    //     return tabEnabled_; 
    // }

    // IDisplayObject& IDisplayObject::setTabEnabled(bool enabled) 
    // { 
    //     // Initialization logic for Box
    //     static int s_nextIndex = 0;
    //     if (enabled)
    //     {
    //         tabPriority_ = s_nextIndex;
    //         s_nextIndex++;    
    //         tabEnabled_ = true;   
    //     }
    //     tabEnabled_ = enabled; 
    //     return *this; 
    // }

    // void IDisplayObject::setKeyboardFocus() 
    // { 
    //     Core::instance().setKeyboardFocusedObject(shared_from_this()); 
    // }


    // bool IDisplayObject::isKeyboardFocused() const
    // {
    //     auto focused = SDOM::Core::instance().getKeyboardFocusedObject().lock();
    //     return (focused.get() == this);
    // }
    // bool IDisplayObject::isMouseHovered() const
    // {
    //     auto hovered = SDOM::Core::instance().getMouseHoveredObject().lock();
    //     return (hovered.get() == this);
    // }

    // bool IDisplayObject::isEnabled() const { return isEnabled_; }
    // IDisplayObject& IDisplayObject::setEnabled(bool enabled) 
    // { 
    //     // Dispatch EventType::Enabled or EventType::Disabled
    //     auto eventNode = make_unique<Event>(enabled ? EventType::Enabled : EventType::Disabled, shared_from_this().get());
    //     EventManager& eventManager = Core::instance().getEventManager();
    //     eventManager.addEvent(move(eventNode));        
    //     // set the isEnabled as appropriate
    //     isEnabled_ = enabled; 
    //     setDirty(); 
    //     return *this; 
    // }

    
    // bool IDisplayObject::isHidden() const { return isHidden_; }
    // IDisplayObject& IDisplayObject::setHidden(bool hidden) 
    // { 
    //     // Dispatch EventType::Hidden or EventType::Visible
    //     auto eventNode = make_unique<Event>(hidden ? EventType::Hidden : EventType::Visible, shared_from_this().get());
    //     EventManager& eventManager = Core::instance().getEventManager();
    //     eventManager.addEvent(move(eventNode));
    //     // set the isHidden as appropriate
    //     isHidden_ = hidden; 
    //     setDirty(); 
    //     return *this; 
    // }
    // bool IDisplayObject::isVisible() const { return !isHidden_; }
    // IDisplayObject& IDisplayObject::setVisible(bool visible) 
    // { 
    //     // Dispatch EventType::Hidden or EventType::Visible
    //     auto eventNode = make_unique<Event>(visible ? EventType::Visible : EventType::Hidden, shared_from_this().get());
    //     EventManager& eventManager = Core::instance().getEventManager();
    //     eventManager.addEvent(move(eventNode));
    //     // set isHidden as appropriate
    //     isHidden_ = !visible; 
    //     setDirty(); 
    //     return *this; 
    // }



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
        if (parent_.lock().get() == this) {
            ERROR("Cycle detected: node is its own parent!");
        }
        if (auto parent = parent_.lock()) {
            switch (anchorLeft_) {
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
        if (parent_.lock().get() == this) {
            ERROR("Cycle detected: node is its own parent!");
        }
        if (auto parent = parent_.lock()) {
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
        if (parent_.lock().get() == this) {
            ERROR("Cycle detected: node is its own parent!");
        }
        if (auto parent = parent_.lock()) {
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
        if (parent_.lock().get() == this) {
            ERROR("Cycle detected: node is its own parent!");
        }
        if (auto parent = parent_.lock()) {
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
        if (parent_.lock().get() == this) {
            ERROR("Cycle detected: node is its own parent!");
        }
        float parentAnchor = 0.0f;
        if (auto parent = parent_.lock()) {
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
        if (parent_.lock().get() == this) {
            ERROR("Cycle detected: node is its own parent!");
        }
        float parentAnchor = 0.0f;
        if (auto parent = parent_.lock()) {
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
        if (parent_.lock().get() == this) {
            ERROR("Cycle detected: node is its own parent!");
        }
        float parentAnchor = 0.0f;
        if (auto parent = parent_.lock()) {
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
        if (parent_.lock().get() == this) {
            ERROR("Cycle detected: node is its own parent!");
        }
        float parentAnchor = 0.0f;
        if (auto parent = parent_.lock()) {
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