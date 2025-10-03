// lua_Core.hpp
#pragma once

#include <sol/sol.hpp>
#include <SDOM/SDOM.hpp>

namespace SDOM
{
	class Core;
	class Event;

	// --- Main Loop & Event Dispatch --- //
	void quit_lua(); // **verified**
	void shutdown_lua(); // **verified**
	void run_lua(); // **verified**

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
	void registerOn_lua(const std::string& name, const sol::function& f);

	// --- Stage/Root Node Management --- //
	void setRootNodeByName_lua(const std::string& name); // **verified**
	void setRootNodeByHandle_lua(const DisplayObject& handle); // **verified**
	void setStage_lua(const std::string& name); // **verified**
	IDisplayObject* getRootNodePtr_lua(); // **verified**
	DisplayObject getRootNode_lua(); // **verified**
	DisplayObject getStageHandle_lua(); // **verified**

	// --- Factory & EventManager Access --- //
	bool getIsTraversing_lua(); // **verified**
	Core* setIsTraversing_lua(bool traversing); // **verified**

    // --- Object Creation --- //
	DisplayObject createDisplayObject_lua(const std::string& typeName, const sol::table& config);    // **verified**

	// --- Focus & Hover Management --- //
	void handleTabKeyPress_lua(); 			// **verified**
	void handleTabKeyPressReverse_lua(); 			// **verified**
	void setKeyboardFocusedObject_lua(DisplayObject obj);  // **verified**
	DisplayObject getKeyboardFocusedObject_lua(); // **verified**
	void setMouseHoveredObject_lua(DisplayObject obj);    // **verified**
	DisplayObject getMouseHoveredObject_lua();     // **verified**

	// --- Window Title & Timing --- //
	std::string getWindowTitle_lua();	// **verified**
	Core* setWindowTitle_lua(const std::string& title); 	// **verified**
	float getElapsedTime_lua();	// **verified**

	// --- Event helpers (exposed to Lua) --- //
	void pumpEventsOnce_lua(); // **verified**
	void pushMouseEvent_lua(const sol::object& args); // **verified**

	// synthesize keyboard events from Lua: expects table { key=<SDLK_* int>, type="down"|"up", mod=<modifier mask optional> }
	void pushKeyboardEvent_lua(const sol::object& args); // **verified**

	// --- Object Lookup --- //
	DisplayObject getDisplayObjectHandle_lua(const std::string& name); // **verified**
	DisplayObject getFactoryStageHandle_lua(); // **verified**
	bool hasDisplayObject_lua(const std::string& name); // **verified**

	// --- Orphan / Future Child Management --- //
	void destroyDisplayObject_lua(const std::string& name); // **verified**
	int countOrphanedDisplayObjects_lua(); // **verified**
	std::vector<DisplayObject> getOrphanedDisplayObjects_lua(); // **verified**
	void destroyOrphanedDisplayObjects_lua(); // **verified**

	// --- Utility Methods --- //
	std::vector<std::string> listDisplayObjectNames_lua(); // **verified**
	void printObjectRegistry_lua();	// **verified**

	// --- New Factory Methods --- //
	std::vector<std::string> getPropertyNamesForType_lua(const std::string& typeName); // **verified**
	std::vector<std::string> getCommandNamesForType_lua(const std::string& typeName); // **verified**
	std::vector<std::string> getFunctionNamesForType_lua(const std::string& typeName); // **verified**

} // end namespace SDOM