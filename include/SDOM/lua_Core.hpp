// lua_Core.hpp
#pragma once

#include <sol/sol.hpp>
// #include <SDOM/SDOM.hpp>

namespace SDOM
{
    class Core;
    class Event;

/*

    Renaming / Adding Commands Checklist:

    change 	setRootNodeByHandle_lua  		to 	setRootNode_lua					
    change	setStage_lua 					to 	setStageByName_lua				
    change	getRootNode_lua					to	getRoot_lua						
    change	getStageHandle_lua				to	getStage_lua					
    change 	handleTabKeyPress_lua			to	doTabKeyPressForward_lua		
    change 	handleTabKeyPressReverse_lua	to	doTabKeyPressReverse_lua		

    add		setStage_lua (const DisplayObject& handle)							

    remove 	getRootNodePtr_lua // no need to expose raw pointer to Lua			
    remove 	getFactoryStageHandle_lua // redundant and obscure					

*/

    // --- Main Loop & Event Dispatch --- //
    void quit_lua();		// TESTED (manually from listener_callbacks.lua)
    void shutdown_lua();	// TESTED (manually from listener_callbacks.lua)
    void run_lua();         // UNTESTED

    void configure_lua(const sol::table& config);            // UNTESTED
    void configureFromFile_lua(const std::string& filename); // UNTESTED

    // --- Callback/Hook Registration --- //
    void registerOnInit_lua(std::function<bool()> fn);            	// UNTESTED
    void registerOnQuit_lua(std::function<void()> fn);              // UNTESTED
    void registerOnUpdate_lua(std::function<void(float)> fn);       // UNTESTED
    void registerOnEvent_lua(std::function<void(const Event&)> fn); // UNTESTED
    void registerOnRender_lua(std::function<void()> fn);            // UNTESTED
    void registerOnUnitTest_lua(std::function<bool()> fn);          // TESTED (unit test harness registers callbacks)
    void registerOnWindowResize_lua(std::function<void(int, int)> fn); // UNTESTED

    // Generic registration helper: registerOn("Init"|"Update"|...) from Lua
    void registerOn_lua(const std::string& name, const sol::function& f);	// TESTED (used to register test callbacks)

    // --- Stage/Root Node Management --- //
    void setRootNodeByName_lua(const std::string& name); 	// UNTESTED
    void setRootNode_lua(const DisplayObject& handle); 		// UNTESTED
    void setStageByName_lua(const std::string& name);       // UNTESTED
    void setStage_lua(const DisplayObject& handle);         // UNTESTED
    DisplayObject getRoot_lua();  							// UNTESTED
    DisplayObject getStage_lua(); 							// UNTESTED

    // --- Factory & EventManager Access --- //
    bool getIsTraversing_lua();     				// UNTESTED
    void setIsTraversing_lua(bool traversing); 		// UNTESTED

    // --- Object Creation and Lookup--- //
    DisplayObject createDisplayObject_lua(const std::string& typeName, const sol::table& config);    // TESTED (GC and other tests create objects)
    DisplayObject getDisplayObject_lua(const std::string& name); 	// UNTESTED
    bool hasDisplayObject_lua(const std::string& name);          	// TESTED (GC tests check existence)

    // --- Focus & Hover Management --- //
    void doTabKeyPressForward_lua();             					// UNTESTED
    void doTabKeyPressReverse_lua();            					// UNTESTED
    void setKeyboardFocusedObject_lua(const DisplayObject& handle); // UNTESTED
    DisplayObject getKeyboardFocusedObject_lua();                   // UNTESTED
    void setMouseHoveredObject_lua(const DisplayObject& handle);    // UNTESTED
    DisplayObject getMouseHoveredObject_lua();                      // UNTESTED

    // --- Window Title & Timing --- //
    std::string getWindowTitle_lua();	// UNTESTED
    void setWindowTitle_lua(const std::string& title); 				// UNTESTED
    float getElapsedTime_lua();	 // alias getDeltaTime() —			// UNTESTED

    // --- Event helpers (exposed to Lua) --- //
void pumpEventsOnce_lua();                 						// UNTESTED
    void pushMouseEvent_lua(const sol::object& args);   		// UNTESTED
    void pushKeyboardEvent_lua(const sol::object& args);		// UNTESTED

    // --- Orphan / Future Child Management --- //
    void destroyDisplayObject_lua(const std::string& name);     // TESTED
    int countOrphanedDisplayObjects_lua();                      // TESTED
    std::vector<DisplayObject> getOrphanedDisplayObjects_lua(); // TESTED
    void destroyOrphanedDisplayObjects_lua();  // aliases:  "destroyOrphanedObjects" and "destroyOrphans" // UNTESTED
    void collectGarbage_lua();                                  // TESTED

    // --- Utility Methods --- //
    std::vector<std::string> listDisplayObjectNames_lua(); 		// UNTESTED
    void printObjectRegistry_lua();                        		// UNTESTED	


} // end namespace SDOM