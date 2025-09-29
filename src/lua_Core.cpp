// lua_Core.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/lua_Core.hpp>

namespace SDOM
{

	// --- Main Loop & Event Dispatch --- //
	void quit_lua(Core& core) { core.quit(); }
	void shutdown_lua(Core& core) { core.shutdown(); }

	// --- Stage/Root Node Management --- //
	void setRootNodeByName_lua(Core& core, const std::string& name) { core.setRootNode(name); }
	void setRootNodeByHandle_lua(Core& core, const DomHandle& handle) { core.setRootNode(handle); }
	void setStage_lua(Core& core, const std::string& name) { core.setStage(name); }
	IDisplayObject* getRootNodePtr_lua(Core& core) { return core.getRootNodePtr(); }
	DomHandle getRootNode_lua(Core& core) { return core.getRootNode(); }
	DomHandle getStageHandle_lua(Core& core) { return core.getStageHandle(); }

	// --- Factory & EventManager Access --- //
	bool getIsTraversing_lua(const Core& core) { return core.getIsTraversing(); }
	Core& setIsTraversing_lua(Core& core, bool traversing) { core.setIsTraversing(traversing); return core; }

    // --- Object Creation --- //
    DomHandle createDisplayObject_lua(const std::string& typeName, const sol::table& config) {
        return Core::getInstance().createDisplayObject(typeName, config);
    }

	// --- Focus & Hover Management --- //
	void handleTabKeyPress_lua(Core& core) { core.handleTabKeyPress(); }
	void handleTabKeyPressReverse_lua(Core& core) { core.handleTabKeyPressReverse(); }
	void setKeyboardFocusedObject_lua(Core& core, DomHandle obj) { core.setKeyboardFocusedObject(obj); }
	DomHandle getKeyboardFocusedObject_lua(const Core& core) { return core.getKeyboardFocusedObject(); }
	void setMouseHoveredObject_lua(Core& core, DomHandle obj) { core.setMouseHoveredObject(obj); }
	DomHandle getMouseHoveredObject_lua(const Core& core) { return core.getMouseHoveredObject(); }

	// --- Window Title & Timing --- //
	std::string getWindowTitle_lua(const Core& core) { return core.getWindowTitle(); }
	Core& setWindowTitle_lua(Core& core, const std::string& title) { core.setWindowTitle(title); return core; }
	float getElapsedTime_lua(const Core& core) { return core.getElapsedTime(); }

	// --- Object Lookup --- //
	DomHandle getDisplayHandle_lua(Core& core, const std::string& name) { return core.getDisplayHandle(name); }
	DomHandle getFactoryStageHandle_lua(Core& core) { return core.getFactoryStageHandle(); }
	bool hasDisplayObject_lua(const Core& core, const std::string& name) { return core.hasDisplayObject(name); }

	// --- Orphan / Future Child Management --- //
	void destroyDisplayObject_lua(Core& core, const std::string& name) { core.destroyDisplayObject(name); }
	int countOrphanedDisplayObjects_lua(const Core& core) { return core.countOrphanedDisplayObjects(); }
	std::vector<DomHandle> getOrphanedDisplayObjects_lua(Core& core) { return core.getOrphanedDisplayObjects(); }
	void destroyOrphanedDisplayObjects_lua(Core& core) { core.destroyOrphanedDisplayObjects(); }

	// --- Utility Methods --- //
	std::vector<std::string> listDisplayObjectNames_lua(const Core& core) { return core.listDisplayObjectNames(); }
	void clearFactory_lua(Core& core) { core.clearFactory(); }
	void printObjectRegistry_lua(Core& core) { core.printObjectRegistry(); }

	// --- New Factory Methods --- //
	std::vector<std::string> getPropertyNamesForType_lua(const Core& core, const std::string& typeName) {
		return core.getFactory().getPropertyNamesForType(typeName); }
	std::vector<std::string> getCommandNamesForType_lua(const Core& core, const std::string& typeName) {
		return core.getFactory().getCommandNamesForType(typeName); }
	std::vector<std::string> getFunctionNamesForType_lua(const Core& core, const std::string& typeName) {
		return core.getFactory().getFunctionNamesForType(typeName); }

} // end namespace SDOM