// lua_Core.hpp
#pragma once

#include <sol/sol.hpp>
#include <SDOM/SDOM.hpp>

namespace SDOM
{
	class Core;
	class Event;

/*

	Renaming / Adding Commands Checklist:

	change 	setRootNodeByHandle_lua  		to 	setRootNode_lua					**CHECK**
	change	setStage_lua 					to 	setStageByName_lua				**CHECK**
	change	getRootNode_lua					to	getRoot_lua						**CHECK**
	change	getStageHandle_lua				to	getStage_lua					**CHECK**
	change 	handleTabKeyPress_lua			to	doTabKeyPressForward_lua		**CHECK**
	change 	handleTabKeyPressReverse_lua	to	doTabKeyPressReverse_lua		**CHECK**

	add		setStage_lua (const DisplayObject& handle)							**CHECK**

	remove 	getRootNodePtr_lua // no need to expose raw pointer to Lua			**CHECK**
	remove 	getFactoryStageHandle_lua // redundant and obscure					**CHECK**

*/

	// --- Main Loop & Event Dispatch --- //
	void quit_lua();		// **GLOBAL LUA**
	void shutdown_lua();	// **GLOBAL LUA**
	void run_lua(); 

	void configure_lua(const sol::table& config);
	void configureFromFile_lua(const std::string& filename);

	// --- Callback/Hook Registration --- //
	void registerOnInit_lua(std::function<bool()> fn);
	void registerOnQuit_lua(std::function<void()> fn);
	void registerOnUpdate_lua(std::function<void(float)> fn);
	void registerOnEvent_lua(std::function<void(const Event&)> fn);
	void registerOnRender_lua(std::function<void()> fn);
	void registerOnUnitTest_lua(std::function<bool()> fn);
	void registerOnWindowResize_lua(std::function<void(int, int)> fn);

	// Generic registration helper: registerOn("Init"|"Update"|...) from Lua
	void registerOn_lua(const std::string& name, const sol::function& f);	// **GLOBAL LUA**

	// --- Stage/Root Node Management --- //
	void setRootNodeByName_lua(const std::string& name); 
	void setRootNode_lua(const DisplayObject& handle); 
	void setStageByName_lua(const std::string& name); 
	void setStage_lua(const DisplayObject& handle); 
	DisplayObject getRoot_lua(); 
	DisplayObject getStage_lua(); 

	// --- Factory & EventManager Access --- //
	bool getIsTraversing_lua(); 
	Core* setIsTraversing_lua(bool traversing); 

    // --- Object Creation --- //
	DisplayObject createDisplayObject_lua(const std::string& typeName, const sol::table& config);    

	// --- Focus & Hover Management --- //
	void doTabKeyPressForward_lua();             
	void doTabKeyPressReverse_lua();             
	void setKeyboardFocusedObject_lua(DisplayObject obj);  
	DisplayObject getKeyboardFocusedObject_lua(); 
	void setMouseHoveredObject_lua(DisplayObject obj);    
	DisplayObject getMouseHoveredObject_lua();     

	// --- Window Title & Timing --- //
	std::string getWindowTitle_lua();	
	Core* setWindowTitle_lua(const std::string& title); 	
	float getElapsedTime_lua();	

	// --- Event helpers (exposed to Lua) --- //
	void pumpEventsOnce_lua(); 
	void pushMouseEvent_lua(const sol::object& args); 

	// synthesize keyboard events from Lua: expects table { key=<SDLK_* int>, type="down"|"up", mod=<modifier mask optional> }
	void pushKeyboardEvent_lua(const sol::object& args); 

	// --- Object Lookup --- //
	DisplayObject getDisplayObject_lua(const std::string& name); 
	bool hasDisplayObject_lua(const std::string& name); 

	// --- Orphan / Future Child Management --- //
	void destroyDisplayObject_lua(const std::string& name); 
	int countOrphanedDisplayObjects_lua(); 
	std::vector<DisplayObject> getOrphanedDisplayObjects_lua(); 
	void destroyOrphanedDisplayObjects_lua(); 

	// --- Utility Methods --- //
	std::vector<std::string> listDisplayObjectNames_lua(); 
	void printObjectRegistry_lua();	


} // end namespace SDOM