// lua_IDisplayObject.hpp
#pragma once

#include <sol/sol.hpp>
// #include <SDOM/SDOM.hpp>
// #include <SDOM/SDOM_IDisplayObject.hpp>

namespace SDOM 
{
    class IDisplayObject;
    class EventType;
    class DisplayHandle;
    struct Bounds;

    // --- Dirty/State Management --- //
    void cleanAll_lua(IDisplayObject* obj);              // TESTED
    bool getDirty_lua(const IDisplayObject* obj);        // TESTED
    void setDirty_lua(IDisplayObject* obj);              // TESTED
    bool isDirty_lua(const IDisplayObject* obj);         // TESTED

    // --- Debug/Utility --- //
    void printTree_lua(const IDisplayObject* obj);       // TESTED (used in listener_callbacks.lua)

    // --- Event Handling --- //
    void addEventListener_lua(IDisplayObject* obj, EventType& type, sol::function listener, bool useCapture, int priority);     // TESTED (typed form used by event tests)
    void removeEventListener_lua(IDisplayObject* obj, EventType& type, sol::function listener, bool useCapture);                // TESTED (typed form used by event tests)
    // Flexible variants that accept a Lua table descriptor or multiple-arg form
    void addEventListener_lua_any(IDisplayObject* obj, const sol::object& descriptor, const sol::object& maybe_listener, const sol::object& maybe_useCapture, const sol::object& maybe_priority); // UNTESTED
    void removeEventListener_lua_any(IDisplayObject* obj, const sol::object& descriptor, const sol::object& maybe_listener, const sol::object& maybe_useCapture); // UNTESTED
    // Short variants accepting only the descriptor table (common colon-call)
    void addEventListener_lua_any_short(IDisplayObject* obj, const sol::object& descriptor);  // UNTESTED
    void removeEventListener_lua_any_short(IDisplayObject* obj, const sol::object& descriptor); // UNTESTED

    // --- Hierarchy Management --- //
    void addChild_lua(IDisplayObject* obj, DisplayHandle child);                // TESTED (GC tests use addChild)
    DisplayHandle getChild_lua(const IDisplayObject* obj, std::string name);    // TESTED
    bool removeChild_lua(IDisplayObject* obj, DisplayHandle child);             // TESTED (GC tests use removeChild by handle)
    bool removeChild_lua(IDisplayObject* obj, const std::string& name);         // TESTED (removeChild by name used in tests)
    bool hasChild_lua(const IDisplayObject* obj, DisplayHandle child);          // TESTED (GC tests check hasChild)
    DisplayHandle getParent_lua(const IDisplayObject* obj);                     // TESTED
    void setParent_lua(IDisplayObject* obj, const DisplayHandle& parent);       // TESTED (parent manipulation exercised)

    // Ancestor/Descendant helpers
    bool isAncestorOf_lua(IDisplayObject* obj, DisplayHandle descendant);       // TESTED
    bool isAncestorOf_lua(IDisplayObject* obj, const std::string& name);        // TESTED
    bool isDescendantOf_lua(IDisplayObject* obj, DisplayHandle ancestor);       // TESTED
    bool isDescendantOf_lua(IDisplayObject* obj, const std::string& name);      // TESTED
    // Remove this object from its parent (convenience). Returns true if removed.
    bool removeFromParent_lua(IDisplayObject* obj);                             // TESTED
    // Recursive descendant removal: search depth-first and remove first match. Returns true if removed.
    bool removeDescendant_lua(IDisplayObject* obj, DisplayHandle descendant);   // TESTED
    bool removeDescendant_lua(IDisplayObject* obj, const std::string& descendantName); // TESTED

    // --- Type & Property Access --- //
    std::string getName_lua(const IDisplayObject* obj);                        // TESTED (used in GC & event tests)
    void setName_lua(IDisplayObject* obj, const std::string& newName);         // TESTED

    std::string getType_lua(const IDisplayObject* obj);                        // TESTED
    void setType_lua(IDisplayObject* obj, const std::string& newType);         // TESTED
    Bounds getBounds_lua(const IDisplayObject* obj);                           // TESTED
    // Accept either a Bounds userdata or a Lua table describing bounds
    void setBounds_lua(IDisplayObject* obj, const sol::object& bobj);          // TESTED
    SDL_Color getColor_lua(const IDisplayObject* obj);                         // TESTED
    // Accept either an SDL_Color userdata or a Lua table describing color
    void setColor_lua(IDisplayObject* obj, const sol::object& colorObj);       // TESTED
    // Handle-aware name getter: if underlying object is missing, return the handle's cached name
    std::string getName_handle_lua(DisplayHandle& self);                       // TESTED (used to avoid invalid handle errors)

    // --- Priority & Z-Order --- //
    int getMaxPriority_lua(const IDisplayObject* obj);              // TESTED
    int getMinPriority_lua(const IDisplayObject* obj);              // TESTED
    int getPriority_lua(const IDisplayObject* obj);                 // TESTED
    void setToHighestPriority_lua(IDisplayObject* obj);             // TESTED
    void setToLowestPriority_lua(IDisplayObject* obj);              // TESTED
    void sortChildrenByPriority_lua(IDisplayObject* obj);           // TESTED
    void setPriority_lua(IDisplayObject* obj, int priority);        // TESTED
    // Flexible overloads for priorities (descriptor/targeted forms)
    void setPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor);          // TESTED (overloads used by tests)
    void setPriority_lua_target(IDisplayObject* obj, const sol::object& descriptor, int value); // TESTED
    std::vector<int> getChildrenPriorities_lua(const IDisplayObject* obj);      // TESTED
    void moveToTop_lua(IDisplayObject* obj);                                    // TESTED
    void moveToTop_lua_any(IDisplayObject* obj, const sol::object& descriptor); // TESTED
    int getZOrder_lua(const IDisplayObject* obj);                               // TESTED
    void setZOrder_lua(IDisplayObject* obj, int z_order);                       // TESTED
    void setZOrder_lua_any(IDisplayObject* obj, const sol::object& descriptor); // TESTED

    bool getBorder_lua(const IDisplayObject* obj);
    bool getBackground_lua(const IDisplayObject* obj);
    void setBorder_lua(IDisplayObject* obj, bool hasBorder);
    void setBackground_lua(IDisplayObject* obj, bool hasBackground);

    // Descriptor forms for highest/lowest via parent
    void setToHighestPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor); // TESTED
    void setToLowestPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor);  // TESTED

    // --- Focus & Interactivity --- //
    void setKeyboardFocus_lua(IDisplayObject* obj);                 // TESTED
    bool isKeyboardFocused_lua(const IDisplayObject* obj);          // TESTED
    bool isMouseHovered_lua(const IDisplayObject* obj);             // TESTED
    bool isClickable_lua(const IDisplayObject* obj);                // TESTED
    void setClickable_lua(IDisplayObject* obj, bool clickable);     // TESTED
    bool isEnabled_lua(const IDisplayObject* obj);                  // TESTED
    void setEnabled_lua(IDisplayObject* obj, bool enabled);         // TESTED
    bool isHidden_lua(const IDisplayObject* obj);                   // TESTED
    void setHidden_lua(IDisplayObject* obj, bool hidden);           // TESTED
    bool isVisible_lua(const IDisplayObject* obj);                  // TESTED
    void setVisible_lua(IDisplayObject* obj, bool visible);         // TESTED

    // --- Tab Management --- //
    int getTabPriority_lua(const IDisplayObject* obj);              // TESTED
    void setTabPriority_lua(IDisplayObject* obj, int index);        // TESTED
    bool isTabEnabled_lua(const IDisplayObject* obj);               // TESTED
    void setTabEnabled_lua(IDisplayObject* obj, bool enabled);      // TESTED

    // --- Geometry & Layout --- //
    int getX_lua(const IDisplayObject* obj);                        // TESTED
    int getY_lua(const IDisplayObject* obj);                        // TESTED
    int getWidth_lua(const IDisplayObject* obj);                    // TESTED
    int getHeight_lua(const IDisplayObject* obj);                   // TESTED
    void setX_lua(IDisplayObject* obj, int p_x);                    // TESTED
    void setY_lua(IDisplayObject* obj, int p_y);                    // TESTED
    void setWidth_lua(IDisplayObject* obj, int width);              // TESTED
    void setHeight_lua(IDisplayObject* obj, int height);            // TESTED

    // --- Edge Anchors --- //
    AnchorPoint getAnchorTop_lua(const IDisplayObject* obj);        // TESTED
    AnchorPoint getAnchorLeft_lua(const IDisplayObject* obj);       // TESTED
    AnchorPoint getAnchorBottom_lua(const IDisplayObject* obj);     // TESTED
    AnchorPoint getAnchorRight_lua(const IDisplayObject* obj);      // TESTED
    void setAnchorTop_lua(IDisplayObject* obj, AnchorPoint ap);     // TESTED
    void setAnchorLeft_lua(IDisplayObject* obj, AnchorPoint ap);    // TESTED
    void setAnchorBottom_lua(IDisplayObject* obj, AnchorPoint ap);  // TESTED
    void setAnchorRight_lua(IDisplayObject* obj, AnchorPoint ap);   // TESTED

    // --- World Edge Positions --- //
    float getLeft_lua(const IDisplayObject* obj);                   // TESTED
    float getRight_lua(const IDisplayObject* obj);                  // TESTED
    float getTop_lua(const IDisplayObject* obj);                    // TESTED
    float getBottom_lua(const IDisplayObject* obj);                 // TESTED
    void setLeft_lua(IDisplayObject* obj, float p_left);            // TESTED
    void setRight_lua(IDisplayObject* obj, float p_right);          // TESTED
    void setTop_lua(IDisplayObject* obj, float p_top);              // TESTED
    void setBottom_lua(IDisplayObject* obj, float p_bottom);        // TESTED

    // --- Local Edge Positions --- //
    float getLocalLeft_lua(const IDisplayObject* obj);              
    float getLocalRight_lua(const IDisplayObject* obj);             
    float getLocalTop_lua(const IDisplayObject* obj);               
    float getLocalBottom_lua(const IDisplayObject* obj);            
    void setLocalLeft_lua(IDisplayObject* obj, float p_left);       
    void setLocalRight_lua(IDisplayObject* obj, float p_right);     
    void setLocalTop_lua(IDisplayObject* obj, float p_top);         
    void setLocalBottom_lua(IDisplayObject* obj, float p_bottom);   



    // --- Orphan Retention Policy --- //
    IDisplayObject::OrphanRetentionPolicy orphanPolicyFromString_lua(IDisplayObject* obj, const std::string& s);    // TESTED
    std::string orphanPolicyToString_lua(IDisplayObject* obj, IDisplayObject::OrphanRetentionPolicy p);             // TESTED
    void setOrphanRetentionPolicy_lua(IDisplayObject* obj, const std::string& policyStr);                           // TESTED (GC tests use string helper)
    std::string getOrphanRetentionPolicyString_lua(IDisplayObject* obj);                                            // TESTED

    // Lua-accessible accessors for orphan grace (milliseconds)
    int getOrphanGrace_lua(const IDisplayObject* obj);                              // TESTED (GC #4 uses getOrphanGrace)
    void setOrphanGrace_lua(IDisplayObject* obj, std::chrono::milliseconds grace);  // TESTED (GC #4 sets grace)

}