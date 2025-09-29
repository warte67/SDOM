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
	void handleTabKeyPress_lua(Core& core);							// **REMOVE**
	void handleTabKeyPressReverse_lua(Core& core);					// **REMOVE**	
	void setKeyboardFocusedObject_lua(Core& core, DomHandle obj);
	DomHandle getKeyboardFocusedObject_lua(const Core& core);
	void setMouseHoveredObject_lua(Core& core, DomHandle obj);
	DomHandle getMouseHoveredObject_lua(const Core& core);

	// --- Window Title & Timing --- //
	std::string getWindowTitle_lua(const Core& core);
	Core& setWindowTitle_lua(Core& core, const std::string& title);
	float getElapsedTime_lua(const Core& core);

	// --- Event helpers (exposed to Lua) --- //
	void pumpEventsOnce_lua(Core& core);
	void pushMouseEvent_lua(Core& core, const sol::object& args);

	// synthesize keyboard events from Lua: expects table { key=<SDLK_* int>, type="down"|"up", mod=<modifier mask optional> }
	void pushKeyboardEvent_lua(Core& core, const sol::object& args);

	// --- Object Lookup --- //
	DomHandle getDisplayHandle_lua(Core& core, const std::string& name);
	DomHandle getFactoryStageHandle_lua(Core& core);
	bool hasDisplayObject_lua(const Core& core, const std::string& name);

	// --- Orphan / Future Child Management --- //
	void destroyDisplayObject_lua(Core& core, const std::string& name); // **verified**
	int countOrphanedDisplayObjects_lua(const Core& core); // **verified**
	std::vector<DomHandle> getOrphanedDisplayObjects_lua(Core& core); // **verified**
	void destroyOrphanedDisplayObjects_lua(Core& core); // **verified**

    // --- Utility Methods --- //
	std::vector<std::string> listDisplayObjectNames_lua(const Core& core);
	void clearFactory_lua(Core& core);
	void printObjectRegistry_lua(Core& core);

	// --- New Factory Methods --- //
	std::vector<std::string> getPropertyNamesForType_lua(const Core& core, const std::string& typeName);
	std::vector<std::string> getCommandNamesForType_lua(const Core& core, const std::string& typeName);
	std::vector<std::string> getFunctionNamesForType_lua(const Core& core, const std::string& typeName);

} // end namespace SDOM