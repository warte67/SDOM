// lua_Core.hpp
#pragma once

#include <sol/sol.hpp>
#include <SDOM/SDOM.hpp>

namespace SDOM
{
	class Core;
	class Event;

	// --- Main Loop & Event Dispatch --- //
	void quit_lua(Core& core); // **verified**
	void shutdown_lua(Core& core); // **verified**
	void run_lua(Core& core); // **verified**

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
	void setRootNodeByName_lua(Core& core, const std::string& name); // **verified**
	void setRootNodeByHandle_lua(Core& core, const DomHandle& handle); // **verified**
	void setStage_lua(Core& core, const std::string& name); // **verified**
	IDisplayObject* getRootNodePtr_lua(Core& core); // **verified**
	DomHandle getRootNode_lua(Core& core); // **verified**
	DomHandle getStageHandle_lua(Core& core); // **verified**

	// --- Factory & EventManager Access --- //
	bool getIsTraversing_lua(const Core& core); // **verified**
	Core& setIsTraversing_lua(Core& core, bool traversing); // **verified**

    // --- Object Creation --- //
	DomHandle createDisplayObject_lua(const std::string& typeName, const sol::table& config);    // **verified**

	// --- Focus & Hover Management --- //
	void handleTabKeyPress_lua(Core& core);					// **verified**
	void handleTabKeyPressReverse_lua(Core& core);			// **verified**
	void setKeyboardFocusedObject_lua(Core& core, DomHandle obj);  // **verified**
	DomHandle getKeyboardFocusedObject_lua(const Core& core); // **verified**
	void setMouseHoveredObject_lua(Core& core, DomHandle obj);	// **verified**
	DomHandle getMouseHoveredObject_lua(const Core& core);	// **verified**

	// --- Window Title & Timing --- //
	std::string getWindowTitle_lua(const Core& core);	// **verified**
	Core& setWindowTitle_lua(Core& core, const std::string& title);	// **verified**
	float getElapsedTime_lua(const Core& core);	// **verified**

	// --- Event helpers (exposed to Lua) --- //
	void pumpEventsOnce_lua(Core& core); // **verified**
	void pushMouseEvent_lua(Core& core, const sol::object& args); // **verified**

	// synthesize keyboard events from Lua: expects table { key=<SDLK_* int>, type="down"|"up", mod=<modifier mask optional> }
	void pushKeyboardEvent_lua(Core& core, const sol::object& args); // **verified**

	// --- Object Lookup --- //
	DomHandle getDisplayObjectHandle_lua(Core& core, const std::string& name); // **verified**
	DomHandle getFactoryStageHandle_lua(Core& core); // **verified**
	bool hasDisplayObject_lua(const Core& core, const std::string& name); // **verified**

	// --- Orphan / Future Child Management --- //
	void destroyDisplayObject_lua(Core& core, const std::string& name); // **verified**
	int countOrphanedDisplayObjects_lua(const Core& core); // **verified**
	std::vector<DomHandle> getOrphanedDisplayObjects_lua(Core& core); // **verified**
	void destroyOrphanedDisplayObjects_lua(Core& core); // **verified**

    // --- Utility Methods --- //
	std::vector<std::string> listDisplayObjectNames_lua(const Core& core); // **verified**
	void printObjectRegistry_lua(Core& core);	// **verified**

	// --- New Factory Methods --- //
	std::vector<std::string> getPropertyNamesForType_lua(const Core& core, const std::string& typeName); // **verified**
	std::vector<std::string> getCommandNamesForType_lua(const Core& core, const std::string& typeName); // **verified**
	std::vector<std::string> getFunctionNamesForType_lua(const Core& core, const std::string& typeName); // **verified**

} // end namespace SDOM