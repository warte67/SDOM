/***  SDOM_DisplayObject.hpp  ****************************
 *   ___ ___   ___  __  __   ___  _         _            ___  _     _        _     _              
 *  / __|   \ / _ \|  \/  | |   \(_)____ __| |__ _ _  _ / _ \| |__ (_)___ __| |_  | |_  _ __ _ __ 
 *  \__ \ |) | (_) | |\/| | | |) | (_-< '_ \ / _` | || | (_) | '_ \| / -_) _|  _|_| ' \| '_ \ '_ \
 *  |___/___/ \___/|_|  |_|_|___/|_/__/ .__/_\__,_|\_, |\___/|_.__// \___\__|\__(_)_||_| .__/ .__/
 *                       |___|        |_|          |__/          |__/                  |_|  |_|     
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

#ifndef __SDOM_IDISPLAY_OBJECT_HPP__
#define __SDOM_IDISPLAY_OBJECT_HPP__

#include <SDOM/SDOM.hpp>   
#include <SDOM/SDOM_IResourceObject.hpp>
#include <SDOM/SDOM_ResourceHandle.hpp>
#include <SDOM/SDOM_EventTypeHash.hpp>

namespace SDOM
{

    class Event;
    class EventType;
    class EventTypeHash;
    class IResourceObject;
    class Stage;

    /**
     * @interface IDisplayObject
     * @brief Abstract interface for displayable objects in SDOM.
     * @details
     * Provides the core API for all objects that can be rendered, positioned, and interacted with in the SDOM system.
     * Implementations must define methods for drawing, updating, event handling, and layout management.
     *
     * Typical responsibilities:
     * - Rendering to a display surface
     * - Managing position, size, and bounds
     * - Handling user input and events
     * - Supporting layout and anchoring
     *
     * @note
     * This is a pure interface; all methods are expected to be overridden by derived classes.
     */
    class IDisplayObject : public IResourceObject
    {

    public:
        struct InitStruct
        {
            std::string name = "IDisplayObject";                ///< Name of the display object
            float x = 0.0f;                                     //< Local x
            float y = 0.0f;                                     //< Local y
            float width = 0.0f;                                 //< Local width
            float height = 0.0f;                                //< Local height
            SDL_Color color = { 255, 255, 255, 255 };           ///< Color (default WHITE)
            AnchorPoint anchorTop = AnchorPoint::TOP_LEFT;      ///< Anchor point for the top edge
            AnchorPoint anchorLeft = AnchorPoint::TOP_LEFT;     ///< Anchor point for the left edge
            AnchorPoint anchorBottom = AnchorPoint::TOP_LEFT;   ///< Anchor point for the bottom edge
            AnchorPoint anchorRight = AnchorPoint::TOP_LEFT;    ///< Anchor point for the right edge
            int z_order = 0;                                    ///< Z-order for rendering
            int priority = 0;                                   ///< Rendering priority
            bool isClickable = true;                            ///< Clickable flag
            bool isEnabled = true;                              ///< Enabled flag
            bool isHidden = false;                              ///< Hidden flag
            int tabPriority = 0;                                ///< Tab order priority
            bool tabEnabled = true;                             ///< Tab enabled flag
        };

        friend class Factory; // Allow Factory to create IDisplayObjects

    protected:   // protected constructor so only the Factory can create the object
        IDisplayObject(const InitStruct& init); ///< Constructor using InitDisplayObject structure
        IDisplayObject(const Json& config);     ///< Constructor using JSON configuration
        IDisplayObject();                       ///< Default constructor (registers JSON properties)

    public:
        /**
         * @brief Destructor.
         * @note Constructors are private; use Factory to create instances.
         */
        virtual ~IDisplayObject();

        /**
         * @name Virtual Methods
         * @brief All virtual methods for this class.
         * @{
         */

        /**
         * @brief Called during object initialization.
         * @return True if initialization succeeds, false otherwise.
         */
        virtual bool onInit() override;                 ///< Called when the display object is initialized
                
        /**
         * @brief Called during object shutdown.
         */
        virtual void onQuit() override {};              ///< Called when the display object is being destroyed

        /**
         * @brief Called every frame to update the display object
         * 
         * @param fElapsedTime Amount of time since the last update (in seconds)
         */
        virtual void onUpdate(float fElapsedTime)=0;    ///< Called every frame to update the display object

        /**
         * @brief Called when an event occurs.
         * 
         * @param event The event that occurred.
         */
        virtual void onEvent(const Event& event)=0;     ///< Called when an event occurs

        /**
         * @brief Called to render the display object.
         */
        virtual void onRender()=0;                      ///< Called to render the display object

        /**
         * @brief Runs unit tests for this data object.
         * @details
         * Called during startup or explicit unit test runs. Each object should validate its own state and behavior.
         * @return true if all tests pass, false otherwise.
         */
        virtual bool onUnitTest() { return true; }      ///< Called during startup or explicit unit test runs.
        /** @} */

        void cleanAll();
        void printTree(int depth = 0, bool isLast = true, const std::vector<bool>& hasMoreSiblings = {}) const;
        // std::string getName() const { return name_; }
        // IDisplayObject& setName(const std::string& newName) { name_ = newName; return *this; }
        bool getDirty() const { return bIsDirty_; }
        IDisplayObject& setDirty() { bIsDirty_ = true; return *this; }
        bool isDirty() const { return bIsDirty_; }

        SDL_Color getColor() const { return color_; }
        IDisplayObject& setColor(const SDL_Color& color) { color_ = color; return *this; }
        
        void addEventListener(const EventType& type, std::function<void(const Event&)> listener, bool useCapture = false, int priority = 0);
        void removeEventListener(const EventType& type, std::function<void(const Event&)> listener, bool useCapture = false);
        void triggerEventListeners(const Event& event, bool useCapture);       


        void addChild(ResourceHandle child, bool useWorld=false, int worldX=0, int worldY=0);
        bool removeChild(ResourceHandle child);


        const std::vector<ResourceHandle>& getChildren() const { return children_; }
        
        ResourceHandle getParent() const { return parent_; }
        IDisplayObject& setParent(const ResourceHandle& parent) { parent_ = parent; return *this; }

        int getMaxPriority() const;
        int getMinPriority() const;
        int getPriority() const { return priority_; }
        IDisplayObject& setToHighestPriority();
        IDisplayObject& setToLowestPriority();
        IDisplayObject& sortChildrenByPriority();
        IDisplayObject& setPriority(int priority);
        std::vector<int> getChildrenPriorities() const;
        IDisplayObject& moveToTop();
        bool hasChild(ResourceHandle child) const;
        int getZOrder() const { return z_order_; } // Getter for z_order_
        IDisplayObject& setZOrder(int z_order) { z_order_ = z_order; return *this; } // Setter for z_order_
        
        void setKeyboardFocus();
        bool isKeyboardFocused() const;
        bool isMouseHovered() const;

        // THIS SHOULD ALREADY BE HANDLED BY THE FACTORY
        // void registerSelfName();    // Register this object in the name registry
        // static std::shared_ptr<IDisplayObject> getByName(const std::string& name);

        bool isClickable() const { return isClickable_; }
        IDisplayObject& setClickable(bool clickable) { isClickable_ = clickable; setDirty(); return *this; }

        bool isEnabled() const { return isEnabled_; }
        IDisplayObject& setEnabled(bool enabled) { isEnabled_ = enabled; setDirty(); return *this; }

        bool isHidden() const { return isHidden_; }
        IDisplayObject& setHidden(bool hidden) { isHidden_ = hidden; setDirty(); return *this; }

        bool isVisible() const { return !isHidden_; }
        IDisplayObject& setVisible(bool visible) { isHidden_ = !visible; setDirty(); return *this; }

        // Tab stop management
        int getTabPriority() const;
        IDisplayObject& setTabPriority(int index);
        bool isTabEnabled() const;
        IDisplayObject& setTabEnabled(bool enabled);


        // Coordinate Accessors:
        int getX() const;       // get the accumilated ancestrial horizontal coordinate
        int getY() const;       // get the accumilated ancestrial vertical coordinate
        int getWidth() const;   // get the width of the object
        int getHeight() const;  // get the height of the object

        // These setters may need refactoring:
        IDisplayObject& setX(int p_x);   // convert horizontal world coordinate and save as horizontal offset
        IDisplayObject& setY(int p_y);   // convert vertical world coordinate and save as vertical offset
        IDisplayObject& setWidth(int width);    // set the width of the object
        IDisplayObject& setHeight(int height);  // set the height of the object

        // Edge Anchor Getters:
        AnchorPoint getAnchorTop() const { return anchorTop_; }
        AnchorPoint getAnchorLeft() const { return anchorLeft_; }
        AnchorPoint getAnchorBottom() const { return anchorBottom_; }
        AnchorPoint getAnchorRight() const { return anchorRight_; }

        // Edge Anchor Setters:
        void setAnchorTop(AnchorPoint ap);      // adjust y_ and height_ based on new origin point
        void setAnchorLeft(AnchorPoint ap);     // adjust x_ and width_ based on a new origin point
        void setAnchorBottom(AnchorPoint ap);   // adjust y_ and height_ based on new origin point
        void setAnchorRight(AnchorPoint ap);    // adjust x_ and width_ based on a new origin point

        // Edge Position Getters:
        float getLeft() const;
        float getRight() const;
        float getTop() const;
        float getBottom() const;

        // Edge Position setters:
        IDisplayObject& setLeft(float p_left);
        IDisplayObject& setRight(float p_right);
        IDisplayObject& setTop(float p_top);
        IDisplayObject& setBottom(float p_bottom);

    protected:

        // Getters for Local Offsets
        float getLocalLeft() const { return left_; }
        float getLocalRight() const { return right_; }
        float getLocalTop() const { return top_; }
        float getLocalBottom() const { return bottom_; }  
        // Setters for Local Offsets
        IDisplayObject& setLocalLeft(float value) { left_ = value; return *this; }
        IDisplayObject& setLocalRight(float value) { right_ = value; return *this; }
        IDisplayObject& setLocalTop(float value) { top_ = value; return *this; }
        IDisplayObject& setLocalBottom(float value) { bottom_ = value; return *this; }

    private:
        
        float left_, top_, right_, bottom_; // Calculated bounds based on anchors
        // std::string name_; // Name of the display object
        bool bIsDirty_ = false; // Flag to indicate if the display object needs to be redrawn
        SDL_Color color_ = { 255, 255, 255, 255 }; // Default color WHITE

        // default anchor is TOP_LEFT
        AnchorPoint anchorTop_    = AnchorPoint::TOP_LEFT;
        AnchorPoint anchorLeft_   = AnchorPoint::TOP_LEFT;
        AnchorPoint anchorBottom_ = AnchorPoint::TOP_LEFT;
        AnchorPoint anchorRight_  = AnchorPoint::TOP_LEFT;

        int z_order_ = 0; // Default z_order value
        int priority_ = 0; // Default priority value
        bool isClickable_ = false; // not clickable by default

        // isEnabled: Determines whether the object is interactive (e.g., clickable). 
        // If false, the object and its children are grayed out and unclickable.
        bool isEnabled_ = true; // Default enabled state

        // isHidden: Determines whether the object is rendered. If true, the object 
        // is not rendered but still responds to events if part of the DOM tree.        
        bool isHidden_ = false; // Default hidden state

        // tab stops
        int tabPriority_ = -1;       // tabPriority (i.e. tabStop within a priority queue)   
        bool tabEnabled_ = false;    // Indicates if the object can be focused via Tab

        // std::weak_ptr<IDisplayObject> parent_;
        // std::vector<std::shared_ptr<IDisplayObject>> children_;
        ResourceHandle parent_;
        std::vector<ResourceHandle> children_;

        // // Static registry for IDisplayObject name lookup (This needs to be in Factory if not already)
        // inline static std::unordered_map<std::string, ResourceHandle> nameRegistry_;

        // Containers to store ListenerEntrys for different event phases
        struct ListenerEntry {
            std::function<void(const Event&)> listener;
            int priority;
        };
        std::unordered_map<EventType, std::vector<ListenerEntry>, EventTypeHash> captureEventListeners;
        std::unordered_map<EventType, std::vector<ListenerEntry>, EventTypeHash> bubblingEventListeners;

        // Delete copy constructor and assignment operator
        IDisplayObject(const IDisplayObject& other) = delete;
        // IDisplayObject& operator=(const IDisplayObject& other) = delete;      

        void attachChild_(ResourceHandle child, ResourceHandle parent, bool useWorld=false, int worldX=0, int worldY=0);
        void removeOrphan_(const ResourceHandle& orphan);

        void registerJson_();

    };

} // namespace SDOM

#endif // __SDOM_IDISPLAY_OBJECT_HPP__