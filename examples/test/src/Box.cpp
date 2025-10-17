// Box.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>

#include <SDOM/SDOM_DisplayHandle.hpp>

#include "Box.hpp"
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <atomic>



// Box(const std::string& name, int x = 0, int y = 0, int width = 100, int height = 100);

// Static test counter definition
std::atomic<int> Box::test_click_count_{0};

Box::Box(const SDOM::IDisplayObject::InitStruct& init)
: IDisplayObject(init)
{
    // std::cout << "Box constructed with InitStruct: " << getName() 
    //           << " at address: " << this << std::endl;

    if (init.type != TypeName) {
        ERROR("Error: Box constructed with incorrect type: " + init.type);
    }
    setTabEnabled(true);
    setClickable(true);
}

Box::Box(const sol::table& config)
: IDisplayObject(config, Box::InitStruct())
{
    // std::cout << "Box constructed with Lua config: " << getName() 
    //         << " at address: " << this << std::endl;            

    std::string type = config["type"].valid() ? config["type"].get<std::string>() : "";
    if (type != TypeName) {
        ERROR("Error: Box constructed with incorrect type: " + type);
    }
    setTabEnabled(true);
    setClickable(true);
}

Box::~Box() 
{
}

bool Box::onInit()  
{
    // Add a test-only event listener for MouseClick that increments a static counter.
    // This allows unit tests to synthesize SDL events and verify listeners fire.
    addEventListener(SDOM::EventType::MouseClick, [this](SDOM::Event& event)
    {
        // Only count clicks where this Box is the target
        if (event.getTarget() && event.getTarget()->getName() == this->getName()) {
            Box::test_click_count_.fetch_add(1);
        }
    }, false);

    // addEventListener(SDOM::EventType::MouseClick, [](SDOM::Event& event)
    // {
    //     event.stopPropagation(); // Stop further propagation

    //     std::cout << CLR::BLUE << "Phase: " << CLR::LT_BLUE << event.getPhaseString() << CLR::NORMAL << std::endl;
    //     if (event.getTarget() && event.getTarget()->getType() == "Box") 
    //     {
    //         std::cout << CLR::GREEN << event.getTarget()->getName() << "::" << CLR::LT_GRN << "MouseClick" << CLR::NORMAL << std::endl;
    //     }
    //     std::cout << CLR::BLUE << "currentTarget: " << CLR::LT_BLUE << event.getCurrentTarget()->getName() << CLR::NORMAL << std::endl;
    //     std::cout << CLR::BLUE << "target: " << CLR::LT_BLUE << event.getTarget()->getName() << CLR::NORMAL << std::endl;
    //     std::cout << CLR::BLUE << "relatedTarget: " << CLR::LT_BLUE << (event.getRelatedTarget() ? event.getRelatedTarget()->getName() : "null") << CLR::NORMAL << std::endl;
    // },false); // Do not use capture phase




    return true;
}

void Box::onQuit() 
{
    // Cleanup logic for Box
}

void Box::onEvent(const SDOM::Event& event) 
{
    // std::cout << "target: " << event.getTarget()->getName() << std::endl;
    // std::cout << "currentTarget: " << (event.getCurrentTarget() ? event.getCurrentTarget()->getName() : "null") << std::endl;
    // std::cout << "relatedTarget: " << (event.getRelatedTarget() ? event.getRelatedTarget()->getName() : "null") << std::endl;
    SUPER::onEvent(event); // Call base class event handler

    // only target phase
    if (event.getPhase() != SDOM::Event::Phase::Target) return;

    if (event.getTarget() == SDOM::getStageHandle()) 
    {
        // DEBUG_LOG("Event handling ignored: target object is the stage.");
        return; // Skip event handling for the stage
    }

    // std::cout << "Box " << getName() << " received event: " << event.getTypeName() << std::endl;

    if (event.getType() == SDOM::EventType::KeyDown)
    {
        int keycode = event.getKeycode();
        std::cout << "KeyDown event received for Box: " << getName() << std::endl;
        std::cout << "keycode: " << keycode <<  "'" << SDL_GetKeyName(static_cast<SDL_Keycode>(keycode)) << "'" << std::endl;
        std::cout << "ascii: " << (char)event.getAsciiCode() << std::endl;
        std::cout << "scancode: " << event.getScanCode() << std::endl;
        std::cout << "keymod: " << event.getKeymod() << std::endl;
        std::cout << "target: " << event.getTarget()->getName() << std::endl;

        static const std::vector<SDOM::AnchorPoint> baseAnchors = 
        {
            SDOM::AnchorPoint::TOP_LEFT,
            SDOM::AnchorPoint::TOP_CENTER,
            SDOM::AnchorPoint::TOP_RIGHT,
            SDOM::AnchorPoint::MIDDLE_LEFT,
            SDOM::AnchorPoint::MIDDLE_CENTER,
            SDOM::AnchorPoint::MIDDLE_RIGHT,
            SDOM::AnchorPoint::BOTTOM_LEFT,
            SDOM::AnchorPoint::BOTTOM_CENTER,
            SDOM::AnchorPoint::BOTTOM_RIGHT
        };

        auto cycleAnchor = [&](SDOM::AnchorPoint anchor) -> SDOM::AnchorPoint 
        {
            auto it = std::find(baseAnchors.begin(), baseAnchors.end(), anchor);
            if (it != baseAnchors.end()) 
            {
                size_t idx = std::distance(baseAnchors.begin(), it);
                return baseAnchors[(idx + 1) % baseAnchors.size()];
            }
            else 
            {
                return baseAnchors[0];
            }
        };

        int key = event.getKeycode();
        if (key == SDLK_X) 
        {
            setAnchorLeft(cycleAnchor(getAnchorLeft()));
            std::cout << getName() << ": anchorLeft: " << SDOM::anchorPointToString_.at(getAnchorLeft()) << std::endl;
        }
        else if (key == SDLK_Y) 
        {
            setAnchorTop(cycleAnchor(getAnchorTop()));
            std::cout << getName() << ": anchorTop: " << SDOM::anchorPointToString_.at(getAnchorTop()) << std::endl;
        }
        else if (key == SDLK_W) 
        {
            setAnchorRight(cycleAnchor(getAnchorRight()));
            std::cout << getName() << ": anchorRight: " << SDOM::anchorPointToString_.at(getAnchorRight()) << std::endl;
        }
        else if (key == SDLK_H) 
        {
            setAnchorBottom(cycleAnchor(getAnchorBottom()));
            std::cout << getName() << ": anchorBottom: " << SDOM::anchorPointToString_.at(getAnchorBottom()) << std::endl;
        }
    }

    float mX = event.getMouseX();
    float mY = event.getMouseY();

    SDOM::DisplayHandle stage = SDOM::getStageHandle();
    static int original_Width = getWidth();
    static int original_Height = getHeight();
    static SDOM::DisplayHandle draggedObject = SDOM::DisplayHandle();
    static SDOM::DisplayHandle original_parent = getParent();

    // Check which mouse button is used
    Uint8 button = event.getButton();

    // Handle Drag/Dragging/Drop for left mouse button   
    if (button & SDL_BUTTON_LMASK)  
    {
        if (event.getType() == SDOM::EventType::Drag) 
        {
            draggedObject = SDOM::DisplayHandle(getName(), getType()); 
            if (!draggedObject)
                ERROR("draggedObject is invalid");
            // Ensure the dragged object is not the stage
            if (draggedObject == stage) 
            {
                // DEBUG_LOG("Drag operation ignored: draggedObject is the stage.");
                return; // Skip the drag operation
            }

            if (draggedObject->getParent()) 
            {
                original_parent = draggedObject->getParent();
            } 
            else 
            {
                ERROR("draggedObject or its parent is invalid");
                return; // return is not needed since the ERROR() macro throws a critical error
            }

            // Remove the dragged object from its current parent
            if (auto parent = original_parent) 
            {
                if (parent && draggedObject)
                    parent->removeChild(draggedObject);
            }
            // add the dragged object to the stage
            int worldX = mX - event.getDragOffsetX();
            int worldY = mY - event.getDragOffsetY();
            original_Width = getWidth();
            original_Height = getHeight();
            if (stage) stage->addChild(draggedObject, true, worldX, worldY);
        }

        // Handle Dragging event
        else if (event.getType() == SDOM::EventType::Dragging) 
        {
            if (draggedObject) 
            {
                int worldX = mX - event.getDragOffsetX();
                int worldY = mY - event.getDragOffsetY();
                draggedObject->setX(worldX);
                draggedObject->setY(worldY);

                draggedObject->setWidth(original_Width);
                draggedObject->setHeight(original_Height);
            }
        }

        // Handle Drop event        
        else if (event.getType() == SDOM::EventType::Drop) 
        {
            // IDisplayObject* relatedTarget = dynamic_cast<Box*>(event.getRelatedTarget());
            SDOM::DisplayHandle relatedTarget = event.getRelatedTarget();
            if (stage && draggedObject)
                stage->removeChild(draggedObject);

            if (relatedTarget && draggedObject) 
            {
                // Dropped onto another Box
                int worldX = mX - event.getDragOffsetX();
                int worldY = mY - event.getDragOffsetY();

                relatedTarget->addChild(draggedObject, true, worldX, worldY);
            } 
            // else if (draggedObject) 
            // {         
            //     IDisplayObject* boardImage = dynamic_cast<Image*>(event.getRelatedTarget());
            //     // Dropped onto the image (parent of boxes)
            //     if (auto parent = original_parent.lock())
            //     {
            //         if (dynamic_cast<Image*>(parent.get()))
            //         {
            //             int worldX = mX - event.getDragOffsetX();
            //             int worldY = mY - event.getDragOffsetY();
            //             parent->addChild(draggedObject, true, worldX, worldY);                        
            //         }
            //         else if (boardImage)
            //         {
            //             if (!draggedObject)
            //                 ERROR("draggedObject is invalid");
            //             // Fallback: add to the stage label at current location
            //             int worldX = mX - event.getDragOffsetX();
            //             int worldY = mY - event.getDragOffsetY();
            //             boardImage->addChild(draggedObject, true, worldX, worldY);                        
            //         }
            //         else
            //         {
            //             std::shared_ptr<Image> boardImage = std::dynamic_pointer_cast<Image>(stage->getByName("boardImage")); 
            //             if (!boardImage)
            //                 ERROR("boardImage is invalid");
            //             int worldX = mX - event.getDragOffsetX();
            //             int worldY = mY - event.getDragOffsetY();
            //             boardImage->addChild(draggedObject, true, worldX, worldY);
            //         }
            //     }
            // }
            draggedObject = nullptr;
        }
    } // end if SDL_BUTTON_LEFT

    // Handle resizing for right mouse button
    else if (button & SDL_BUTTON_RMASK)
    {
        if (event.getType() == SDOM::EventType::Drag) 
        {
            original_Width = getWidth();
            original_Height = getHeight();
        }
        if (event.getType() == SDOM::EventType::Dragging) 
        {
            int dragOffsetX = event.getDragOffsetX() - original_Width;
            int dragOffsetY = event.getDragOffsetY() - original_Height;
            int newWidth = std::max(10, int((mX - dragOffsetX) - getX()));
            int newHeight = std::max(10, int((mY - dragOffsetY) - getY()));
            setWidth(newWidth);
            setHeight(newHeight);
        }
        // Optionally, handle Drop for right button if we need to finalize the resize
    }        
}


void Box::onUpdate(float fElapsedTime) 
{
    SUPER::onUpdate(fElapsedTime); // Call base class update handler
}

void Box::onRender() 
{
    // Render the Box object
    SDL_Renderer* renderer = SDOM::getRenderer();
    SDL_FRect rect = { float(getX()), float(getY()), float(getWidth()), float(getHeight()) };
    SDL_Color color = getColor();
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);

    // test hover
    if (isMouseHovered())
    {
        // highlight the box when hovered
        static Uint64 lastToggleTime = SDL_GetTicks(); // Track the last toggle time
        static bool useWhite = true; // Toggle between white and black
        // Get the current time.
        Uint64 currentTime = SDL_GetTicks();
        if (currentTime - lastToggleTime >= 250) // Check if 250ms have passed
        {
            useWhite = !useWhite; // Toggle the color
            lastToggleTime = currentTime; // Reset the timer
        }
        // Draw a semi-transparent overlay
        SDL_Color hoverColor = useWhite ? SDL_Color{255, 255, 255, 32} : SDL_Color{0, 0, 0, 32}; // Alternate colors
        SDL_SetRenderDrawColor(renderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer, &rect);
    }

    // BEGIN: SpriteSheet rendering (uses AssetHandle / Factory -> SpriteSheet)
    {
        SDL_FRect dstRect = { float(getX()), float(getY()), float(getWidth()), float(getHeight()) };
        static Uint8 spriteIndex = 0; // Example sprite index
        static Uint64 tmr_acc = SDL_GetTicks();

        // Attempt to resolve a SpriteSheet asset by name from the Factory.
        // Replace "XO_SpriteSheet" with whichever asset name you want to render.
        SDOM::AssetHandle asset = SDOM::getFactory().getAssetObject("default_incon_8x8"); // default_icon_8x8
        if (asset.isValid()) 
        {
            SDOM::SpriteSheet* ss = asset.as<SDOM::SpriteSheet>();
            if (ss) 
            {
                try
                {
                    // Ensure loaded (AssetHandle::as/get will lazily load, but explicit call is safe)
                    ss->onLoad();
                    static SDL_Color color = {255,255,255,255};
                    ss->drawSprite(spriteIndex, dstRect, color);

                    if (SDL_GetTicks() >= tmr_acc + 50) 
                    {
                        tmr_acc = SDL_GetTicks();
                        spriteIndex++;
                        color.r = static_cast<Uint8>(random() % 256);
                        color.g = static_cast<Uint8>(random() % 256);
                        color.b = static_cast<Uint8>(random() % 256);
                        color.a = 255;
                        int count = 1;
                        try { count = ss->getSpriteCount(); } catch(...) { count = 1; }
                        if (count > 0 && spriteIndex >= count) spriteIndex = 0;
                    }
                }
                catch(...) 
                {
                    // Drawing failed; ignore and continue.
                }
            }
        }
    }
    // END: SpriteSheet rendering
}

bool Box::onUnitTest() 
{
    // std::cout << CLR::CYAN << "Box::onUnitTest() called for Box: " << getName() << std::endl;

    return true;
}



void Box::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
{
    // Include inherited bindings first
    SUPER::_registerLuaBindings(typeName, lua);

    // if (DEBUG_REGISTER_LUA)
    if (false) // TEMP DISABLE
    {
        std::string typeNameLocal = "Box";
        std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                  << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                  << typeName << CLR::RESET << std::endl;
    }

    // Augment the single shared DisplayHandle handle usertype
    sol::table handle = SDOM::DisplayHandle::ensure_handle_table(lua);

    auto absent = [&](const char* name) -> bool {
        sol::object cur = handle.raw_get_or(name, sol::lua_nil);
        return !cur.valid() || cur == sol::lua_nil;
    };

    auto require_box = [&](SDOM::DisplayHandle& self, const char* method) -> Box* {
        Box* b = dynamic_cast<Box*>(self.get());
        if (!b) {
            std::string msg = std::string(method)
                + " requires Box; got '" + self.getType() + "' (name='" + self.getName() + "')";
            throw sol::error(msg);
        }
        return b;
    };

    // Box-specific methods on the DisplayHandle handle, guarded by type
    if (absent("doSomething")) {
        handle.set_function("doSomething",
            [require_box](SDOM::DisplayHandle& self, sol::object /*args*/, sol::state_view lua) -> sol::object {
                Box* b = require_box(self, "doSomething");
                std::cout << "Box::doSomething called on " << b->getName() << std::endl;
                return sol::make_object(lua, true);
            }
        );
    }

    if (absent("resetColor")) {
        handle.set_function("resetColor",
            [require_box](SDOM::DisplayHandle& self, sol::object /*args*/, sol::state_view lua) -> sol::object {
                Box* b = require_box(self, "resetColor");
                SDL_Color defaultColor = {255, 0, 255, 255};
                b->setColor(defaultColor);
                std::cout << "Box::resetColor called on " << b->getName() << std::endl;
                return sol::make_object(lua, true);
            }
        );
    }

    // Note: no usertype creation and no this->objHandleType_ assignment.

 } // End Box::_registerDisplayObject()
