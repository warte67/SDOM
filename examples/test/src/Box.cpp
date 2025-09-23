// Box.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include "Box.hpp"



// Box(const std::string& name, int x = 0, int y = 0, int width = 100, int height = 100);

Box::Box(const SDOM::IDisplayObject::InitStruct& init)
: IDisplayObject(init)
{
    setTabEnabled(true);
    setClickable(true);
}


Box::Box(const Json& config)
: IDisplayObject(config)
{
    setTabEnabled(true);
    setClickable(true);
}

Box::~Box() 
{
}

bool Box::onInit()  
{
    return true;
}

void Box::onQuit() 
{
    // Cleanup logic for Box
}

void Box::onEvent(const SDOM::Event& event) 
{
    std::cout << "Box " << getName() << " received event: " << event.getTypeName() << std::endl;

    // only target phase
    // if (event.getPhase() != SDOM::Event::Phase::Target) 
    // {
    //     return;
    // }

    // if (event.getType() == SDOM::EventType::KeyDown)
    // {
    //     int keycode = event.getKeycode();
    //     std::cout << "KeyDown event received for Box: " << getName() << std::endl;
    //     std::cout << "keycode: " << keycode <<  "'" << SDL_GetKeyName(static_cast<SDL_Keycode>(keycode)) << "'" << std::endl;
    //     std::cout << "ascii: " << (char)event.getAsciiCode() << std::endl;
    //     std::cout << "scancode: " << event.getScanCode() << std::endl;
    //     std::cout << "keymod: " << event.getKeymod() << std::endl;
    //     std::cout << "target: " << event.getTarget()->getName() << std::endl;

    //     static const std::vector<SDOM::AnchorPoint> baseAnchors = 
    //     {
    //         SDOM::AnchorPoint::TOP_LEFT,
    //         SDOM::AnchorPoint::TOP_CENTER,
    //         SDOM::AnchorPoint::TOP_RIGHT,
    //         SDOM::AnchorPoint::MIDDLE_LEFT,
    //         SDOM::AnchorPoint::MIDDLE_CENTER,
    //         SDOM::AnchorPoint::MIDDLE_RIGHT,
    //         SDOM::AnchorPoint::BOTTOM_LEFT,
    //         SDOM::AnchorPoint::BOTTOM_CENTER,
    //         SDOM::AnchorPoint::BOTTOM_RIGHT
    //     };

    //     auto cycleAnchor = [&](SDOM::AnchorPoint anchor) -> SDOM::AnchorPoint 
    //     {
    //         auto it = std::find(baseAnchors.begin(), baseAnchors.end(), anchor);
    //         if (it != baseAnchors.end()) 
    //         {
    //             size_t idx = std::distance(baseAnchors.begin(), it);
    //             return baseAnchors[(idx + 1) % baseAnchors.size()];
    //         }
    //         else 
    //         {
    //             return baseAnchors[0];
    //         }
    //     };

    //     int key = event.getKeycode();
    //     if (key == SDLK_X) 
    //     {
    //         setAnchorLeft(cycleAnchor(getAnchorLeft()));
    //         std::cout << getName() << ": anchorLeft: " << SDOM::anchorPointToString_.at(getAnchorLeft()) << std::endl;
    //     }
    //     else if (key == SDLK_Y) 
    //     {
    //         setAnchorTop(cycleAnchor(getAnchorTop()));
    //         std::cout << getName() << ": anchorTop: " << SDOM::anchorPointToString_.at(getAnchorTop()) << std::endl;
    //     }
    //     else if (key == SDLK_W) 
    //     {
    //         setAnchorRight(cycleAnchor(getAnchorRight()));
    //         std::cout << getName() << ": anchorRight: " << SDOM::anchorPointToString_.at(getAnchorRight()) << std::endl;
    //     }
    //     else if (key == SDLK_H) 
    //     {
    //         setAnchorBottom(cycleAnchor(getAnchorBottom()));
    //         std::cout << getName() << ": anchorBottom: " << SDOM::anchorPointToString_.at(getAnchorBottom()) << std::endl;
    //     }
    // }

    // float mX = event.getMouseX();
    // float mY = event.getMouseY();

    // static int original_Width = getWidth();
    // static int original_Height = getHeight();
    // static std::shared_ptr<IDisplayObject> draggedObject = nullptr;
    // static std::weak_ptr<IDisplayObject> original_parent;

    // // Check which mouse button is used
    // Uint8 button = event.getButton();

    // // Handle Drag/Dragging/Drop for left mouse button   
    // if (button & SDL_BUTTON_LMASK)  
    // {
    //     if (event.getType() == SDOM::EventType::Drag) 
    //     {
    //         draggedObject = shared_from_this(); // Transfer ownership to draggedObject
    //         stage = draggedObject->getStage();
    //         original_parent = draggedObject->getParent()->shared_from_this(); // Convert to shared_ptr before assigning

    //         // Remove the dragged object from its current parent
    //         if (auto parent = original_parent.lock()) 
    //         {
    //             if (parent && draggedObject)
    //                 parent->removeChild(draggedObject);
    //         }
    //         // add the dragged object to the stage
    //         int worldX = mX - event.getDragOffsetX();
    //         int worldY = mY - event.getDragOffsetY();
    //         original_Width = getWidth();
    //         original_Height = getHeight();
    //         if (stage) stage->addChild(draggedObject, true, worldX, worldY);
    //     }

    //     // Handle Dragging event
    //     else if (event.getType() == EventType::Dragging) 
    //     {
    //         if (draggedObject) 
    //         {
    //             int worldX = mX - event.getDragOffsetX();
    //             int worldY = mY - event.getDragOffsetY();
    //             draggedObject->setX(worldX);
    //             draggedObject->setY(worldY);

    //             draggedObject->setWidth(original_Width);
    //             draggedObject->setHeight(original_Height);

    //             // std::cout << "Dragging: " << draggedObject->getName() << 
    //             //     " to (" << worldX << ", " << worldY << " w:" << original_Width << " h:" << original_Height << ")" << std::endl;
    //         }
    //     }

    //     // Handle Drop event        
    //     else if (event.getType() == EventType::Drop) 
    //     {
    //         IDisplayObject* relatedTarget = dynamic_cast<Box*>(event.getRelatedTarget());
    //         if (stage && draggedObject)
    //             stage->removeChild(draggedObject);

    //         if (relatedTarget && draggedObject) 
    //         {
    //             // Dropped onto another Box
    //             int worldX = mX - event.getDragOffsetX();
    //             int worldY = mY - event.getDragOffsetY();

    //             // std::cout << "droppedObject: " << draggedObject->getName() << " to (" << 
    //             //     worldX << ", " << worldY << ", w:" << draggedObject->getWidth() 
    //             //     << ", h:" << draggedObject->getHeight() << ")" << std::endl;

    //             relatedTarget->addChild(draggedObject, true, worldX, worldY);
    //         } 
    //         else if (draggedObject) 
    //         {         
    //             IDisplayObject* boardImage = dynamic_cast<Image*>(event.getRelatedTarget());
    //             // Dropped onto the image (parent of boxes)
    //             if (auto parent = original_parent.lock())
    //             {
    //                 if (dynamic_cast<Image*>(parent.get()))
    //                 {
    //                     int worldX = mX - event.getDragOffsetX();
    //                     int worldY = mY - event.getDragOffsetY();
    //                     parent->addChild(draggedObject, true, worldX, worldY);                        
    //                 }
    //                 else if (boardImage)
    //                 {
    //                     if (!draggedObject)
    //                         ERROR("draggedObject is invalid");
    //                     // Fallback: add to the stage label at current location
    //                     int worldX = mX - event.getDragOffsetX();
    //                     int worldY = mY - event.getDragOffsetY();
    //                     boardImage->addChild(draggedObject, true, worldX, worldY);                        
    //                 }
    //                 else
    //                 {
    //                     std::shared_ptr<Image> boardImage = std::dynamic_pointer_cast<Image>(stage->getByName("boardImage")); 
    //                     if (!boardImage)
    //                         ERROR("boardImage is invalid");
    //                     int worldX = mX - event.getDragOffsetX();
    //                     int worldY = mY - event.getDragOffsetY();
    //                     boardImage->addChild(draggedObject, true, worldX, worldY);
    //                 }
    //             }
    //         }
    //         draggedObject.reset(); // Release ownership
    //     }
    // } // end if SDL_BUTTON_LEFT

    // // Handle resizing for right mouse button
    // else if (button & SDL_BUTTON_RMASK)
    // {
    //     if (event.getType() == EventType::Drag) 
    //     {
    //         original_Width = getWidth();
    //         original_Height = getHeight();
    //     }
    //     if (event.getType() == EventType::Dragging) 
    //     {
    //         int dragOffsetX = event.getDragOffsetX() - original_Width;
    //         int dragOffsetY = event.getDragOffsetY() - original_Height;
    //         int newWidth = std::max(10, int((mX - dragOffsetX) - getX()));
    //         int newHeight = std::max(10, int((mY - dragOffsetY) - getY()));
    //         setWidth(newWidth);
    //         setHeight(newHeight);
    //     }
    //     // Optionally, handle Drop for right button if you want to finalize the resize
    // }        
}


void Box::onUpdate(float fElapsedTime) 
{
    // properly flash the key focus indication border rectangle
    // Border flashing logic (as before)
    static float delta = 1.0f;
    float speed = 500.0f;

    gray += (fElapsedTime * speed * delta);

    if (gray >= 192.0f) 
    {
        gray = 192.0f;
        delta = -1.0f;
    }
    else if (gray <= 64.0f) 
    {
        gray = 64.0f;
        delta = 1.0f;
    }
}

void Box::onRender() 
{
    // Render the Box object
    SDL_Renderer* renderer = SDOM::getRenderer();
    SDL_FRect rect = { float(getX()), float(getY()), float(getWidth()), float(getHeight()) };
    SDL_Color color = getColor();
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);

    // // Render a border if the Box has keyboard focus
    // if (isKeyboardFocused())
    // {
    //     // properly flash the key focus indication border rectangle
    //     SDL_Color focusColor = { (Uint8)gray, (Uint8)gray, (Uint8)gray, 255 }; // Gray color for focus
    //     SDL_SetRenderDrawColor(renderer, focusColor.r, focusColor.g, focusColor.b, focusColor.a); // Border color
    //     SDL_RenderRect(renderer, &rect);
    // }

    // // test hover
    // if (isMouseHovered())
    // {
    //     // highlight the box when hovered
    //     SDL_Color hoverColor = { 255, 255, 0, 64 }; // Yellow with some transparency
    //     SDL_SetRenderDrawColor(renderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
    //     SDL_RenderFillRect(renderer, &rect);
    // }

    // // BEGIN TESTING SpriteSheets and the Factory Loading from JSON ... 
    //     // Render the SpriteSheet over the Box
    //     SDL_FRect dstRect = { float(getX()), float(getY()), float(getWidth()), float(getHeight()) };
    //     static Uint8 spriteIndex = 0; // Example sprite index
    //     static Uint64 tmr_acc = SDL_GetTicks();
    //     // auto spriteSheet = Core::instance().getFactory().getResource<SpriteSheet>("font_8x8");
    //     auto spriteSheet = Core::instance().getFactory()->getResource<SpriteSheet>("XO_SpriteSheet");
    //     if (spriteSheet)
    //     {
    //         static SDL_Color color = {255,255,255,255};
    //         spriteSheet->drawSprite(dstRect, spriteIndex, color);
    //         if (SDL_GetTicks() >= tmr_acc + 50) // Corrected condition
    //         {
    //             tmr_acc = SDL_GetTicks();    
    //             spriteIndex++;                          
    //             color.r = (Uint8)random()%256;
    //             color.g = (Uint8)random()%256;
    //             color.b = (Uint8)random()%256;
    //             color.a = (Uint8)255;
    //             if (spriteIndex >= spriteSheet->getSpriteCount()) { spriteIndex = 0; }
    //         }            
    //     }
    // // ... END TESTING
}

bool Box::onUnitTest() 
{
    return true;
}

