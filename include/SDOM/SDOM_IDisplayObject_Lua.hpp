// SDOM_IDisplayObject_Lua.hpp
// Unit tests for SDOM::IDisplayObject Lua integration

#pragma once

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_IDisplayObject.hpp>
#include <sol/sol.hpp>

namespace SDOM
{
    class Event;

    class IDisplayObject;
    class EventType;
    class DisplayHandle;
    struct Bounds;

    // --- ✅ Dirty/State Management --- //
    void cleanAll_lua(IDisplayObject* obj);
    bool getDirty_lua(const IDisplayObject* obj);
    void setDirty_lua(IDisplayObject* obj); 
    bool isDirty_lua(const IDisplayObject* obj); 

    // --- ✅ Debug/Utility --- //
    void printTree_lua(const IDisplayObject* obj);

     // --- ✅ Events and Event Listener Handling --- //
    void addEventListener_lua(IDisplayObject* obj, EventType& type, sol::function listener, bool useCapture, int priority);
    void removeEventListener_lua(IDisplayObject* obj, EventType& type, sol::function listener, bool useCapture);
    // Flexible variants that accept a Lua table descriptor or multiple-arg form
    void addEventListener_lua_any(IDisplayObject* obj, const sol::object& descriptor, const sol::object& maybe_listener, const sol::object& maybe_useCapture, const sol::object& maybe_priority);
    void removeEventListener_lua_any(IDisplayObject* obj, const sol::object& descriptor, const sol::object& maybe_listener, const sol::object& maybe_useCapture);
    // Short variants accepting only the descriptor table (common colon-call)
    void addEventListener_lua_any_short(IDisplayObject* obj, const sol::object& descriptor);
    void removeEventListener_lua_any_short(IDisplayObject* obj, const sol::object& descriptor);    
    // Check for presence of event listeners
    bool hasEventListener_lua(const IDisplayObject* obj, const EventType& type, bool useCapture);
    // Queue an event with optional payload initializer
    void queue_event_lua(IDisplayObject* obj, const EventType& type, std::function<void(Event&)> init_payload);

    // --- ✅ Hierarchy Management --- //
    void addChild_lua(IDisplayObject* obj, DisplayHandle child);                
    DisplayHandle getChild_lua(const IDisplayObject* obj, std::string name);    
    bool removeChild_lua(IDisplayObject* obj, DisplayHandle child);             
    bool removeChild_lua(IDisplayObject* obj, const std::string& name);    

    sol::table getChildren_lua(const IDisplayObject* obj, sol::this_state ts);
    int countChildren_lua(const IDisplayObject* obj);

    bool hasChild_lua(const IDisplayObject* obj, DisplayHandle child);          
    DisplayHandle getParent_lua(const IDisplayObject* obj);                     
    void setParent_lua(IDisplayObject* obj, const DisplayHandle& parent);       
    // Ancestor/Descendant helpers
    bool isAncestorOf_lua(IDisplayObject* obj, DisplayHandle descendant);       
    bool isAncestorOf_lua(IDisplayObject* obj, const std::string& name);        
    bool isDescendantOf_lua(IDisplayObject* obj, DisplayHandle ancestor);       
    bool isDescendantOf_lua(IDisplayObject* obj, const std::string& name);      
    // Remove this object from its parent (convenience). Returns true if removed.
    bool removeFromParent_lua(IDisplayObject* obj);                             
    // Recursive descendant removal: search depth-first and remove first match. Returns true if removed.
    bool removeDescendant_lua(IDisplayObject* obj, DisplayHandle descendant);   
    bool removeDescendant_lua(IDisplayObject* obj, const std::string& descendantName);

    // --- ✅ Type & Property Access --- //
    std::string getName_lua(const IDisplayObject* obj);                        
    void setName_lua(IDisplayObject* obj, const std::string& newName);    
    std::string getType_lua(const IDisplayObject* obj);                        
    Bounds getBounds_lua(const IDisplayObject* obj);                           
    // Accept either a Bounds userdata or a Lua table describing bounds
    void setBounds_lua(IDisplayObject* obj, const sol::object& bobj);          
    SDL_Color getColor_lua(const IDisplayObject* obj);                         
    // Accept either an SDL_Color userdata or a Lua table describing color
    void setColor_lua(IDisplayObject* obj, const sol::object& colorObj);       
    // Per-color helpers (foreground/background/border/outline/dropshadow)
    SDL_Color getForegroundColor_lua(const IDisplayObject* obj);
    void setForegroundColor_lua(IDisplayObject* obj, const sol::object& colorObj);
    SDL_Color getBackgroundColor_lua(const IDisplayObject* obj);
    void setBackgroundColor_lua(IDisplayObject* obj, const sol::object& colorObj);
    SDL_Color getBorderColor_lua(const IDisplayObject* obj);
    void setBorderColor_lua(IDisplayObject* obj, const sol::object& colorObj);
    SDL_Color getOutlineColor_lua(const IDisplayObject* obj);
    void setOutlineColor_lua(IDisplayObject* obj, const sol::object& colorObj);
    SDL_Color getDropshadowColor_lua(const IDisplayObject* obj);
    void setDropshadowColor_lua(IDisplayObject* obj, const sol::object& colorObj);
    // boolean properties for border/background
    bool hasBorder_lua(const IDisplayObject* obj); 
    bool hasBackground_lua(const IDisplayObject* obj);
    void setBorder_lua(IDisplayObject* obj, bool hasBorder);
    void setBackground_lua(IDisplayObject* obj, bool hasBackground);


    // --- ✅ Priority & Z-Order --- //
    int getMaxPriority_lua(const IDisplayObject* obj);              
    int getMinPriority_lua(const IDisplayObject* obj);              
    int getPriority_lua(const IDisplayObject* obj);                 
    void setToHighestPriority_lua(IDisplayObject* obj);             
    void setToLowestPriority_lua(IDisplayObject* obj);    
    void setToHighestPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor);
    void setToLowestPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor);
    void sortChildrenByPriority_lua(IDisplayObject* obj);           
    void setPriority_lua(IDisplayObject* obj, int priority);        
    void setPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor);
    void setPriority_lua_target(IDisplayObject* obj, const sol::object& descriptor, int value);
    std::vector<int> getChildrenPriorities_lua(const IDisplayObject* obj);      
    void moveToTop_lua(IDisplayObject* obj);
    void moveToTop_lua_any(IDisplayObject* obj, const sol::object& descriptor);
    void moveToBottom_lua(IDisplayObject* obj);                                 
    void moveToBottom_lua_any(IDisplayObject* obj, const sol::object& descriptor);
    void bringToFront_lua(IDisplayObject* obj);                                  
    void bringToFront_lua_any(IDisplayObject* obj, const sol::object& descriptor);
    void sendToBack_lua(IDisplayObject* obj);                                   
    void sendToBack_lua_any(IDisplayObject* obj, const sol::object& descriptor);
    void sendToBackAfter_lua(IDisplayObject* obj, const IDisplayObject* limitObj);
    void sendToBackAfter_lua_any(IDisplayObject* obj, const sol::object& descriptor, const IDisplayObject* limitObj);
    int getZOrder_lua(const IDisplayObject* obj);                               
    void setZOrder_lua(IDisplayObject* obj, int z_order);                       
    void setZOrder_lua_any(IDisplayObject* obj, const sol::object& descriptor);

    // --- ✅ Object Focus and Interactivity --- //
    void setKeyboardFocus_lua(IDisplayObject* obj);                 
    bool isKeyboardFocused_lua(const IDisplayObject* obj);
    bool isMouseHovered_lua(const IDisplayObject* obj);             
    bool isClickable_lua(const IDisplayObject* obj);                
    void setClickable_lua(IDisplayObject* obj, bool clickable);     
    bool isEnabled_lua(const IDisplayObject* obj);                  
    void setEnabled_lua(IDisplayObject* obj, bool enabled);         
    bool isHidden_lua(const IDisplayObject* obj);                   
    void setHidden_lua(IDisplayObject* obj, bool hidden);           
    bool isVisible_lua(const IDisplayObject* obj);                  
    void setVisible_lua(IDisplayObject* obj, bool visible);         

    // --- 🔄 Tab Management --- //
    int getTabPriority_lua(const IDisplayObject* obj);              
    void setTabPriority_lua(IDisplayObject* obj, int index);        
    bool isTabEnabled_lua(const IDisplayObject* obj);               
    void setTabEnabled_lua(IDisplayObject* obj, bool enabled);      

    // --- ☐ Geometry & Layout --- //
    int getX_lua(const IDisplayObject* obj);                        
    int getY_lua(const IDisplayObject* obj);                        
    int getWidth_lua(const IDisplayObject* obj);                    
    int getHeight_lua(const IDisplayObject* obj);                   
    void setX_lua(IDisplayObject* obj, int p_x);                    
    void setY_lua(IDisplayObject* obj, int p_y);                    
    void setWidth_lua(IDisplayObject* obj, int width);              
    void setHeight_lua(IDisplayObject* obj, int height);            

    // --- ☐ Edge Anchors --- //
    AnchorPoint getAnchorTop_lua(const IDisplayObject* obj);        
    AnchorPoint getAnchorLeft_lua(const IDisplayObject* obj);       
    AnchorPoint getAnchorBottom_lua(const IDisplayObject* obj);     
    AnchorPoint getAnchorRight_lua(const IDisplayObject* obj);      
    void setAnchorTop_lua(IDisplayObject* obj, AnchorPoint ap);     
    void setAnchorLeft_lua(IDisplayObject* obj, AnchorPoint ap);    
    void setAnchorBottom_lua(IDisplayObject* obj, AnchorPoint ap);  
    void setAnchorRight_lua(IDisplayObject* obj, AnchorPoint ap);   

    // --- ☐ World Edge Positions --- //
    float getLeft_lua(const IDisplayObject* obj);                   
    float getRight_lua(const IDisplayObject* obj);                  
    float getTop_lua(const IDisplayObject* obj);                    
    float getBottom_lua(const IDisplayObject* obj);                 
    void setLeft_lua(IDisplayObject* obj, float p_left);            
    void setRight_lua(IDisplayObject* obj, float p_right);          
    void setTop_lua(IDisplayObject* obj, float p_top);              
    void setBottom_lua(IDisplayObject* obj, float p_bottom);        

    // --- ☐ Local Edge Positions --- //
    float getLocalLeft_lua(const IDisplayObject* obj);              
    float getLocalRight_lua(const IDisplayObject* obj);             
    float getLocalTop_lua(const IDisplayObject* obj);               
    float getLocalBottom_lua(const IDisplayObject* obj);            
    void setLocalLeft_lua(IDisplayObject* obj, float p_left);       
    void setLocalRight_lua(IDisplayObject* obj, float p_right);     
    void setLocalTop_lua(IDisplayObject* obj, float p_top);         
    void setLocalBottom_lua(IDisplayObject* obj, float p_bottom);  

    // --- ☐ Orphan Retention Policy --- //
    IDisplayObject::OrphanRetentionPolicy orphanPolicyFromString_lua(IDisplayObject* obj, const std::string& s);
    std::string orphanPolicyToString_lua(IDisplayObject* obj, IDisplayObject::OrphanRetentionPolicy p);
    void setOrphanRetentionPolicy_lua(IDisplayObject* obj, const std::string& policyStr);
    std::string getOrphanRetentionPolicyString_lua(IDisplayObject* obj);

    // ☐ Lua-accessible accessors for orphan grace (milliseconds)
    int getOrphanGrace_lua(const IDisplayObject* obj);                              
    void setOrphanGrace_lua(IDisplayObject* obj, std::chrono::milliseconds grace);  


} // END: namespace SDOM