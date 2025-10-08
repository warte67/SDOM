// lua_Core.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/lua_Core.hpp>

namespace SDOM
{

	// --- Event helpers exposed to Lua --- //
	void pumpEventsOnce_lua() { Core::getInstance().pumpEventsOnce(); }

	void pushMouseEvent_lua(const sol::object& args) 
	{
		Core* c = &Core::getInstance();
		// Expect a table with { x=<stage-x>, y=<stage-y>, type="down"|"up", button=<int> }
		if (!args.is<sol::table>()) return;
		sol::table t = args.as<sol::table>();
		if (!t["x"].valid() || !t["y"].valid()) return;
		float sx = t["x"].get<float>();
		float sy = t["y"].get<float>();
		std::string type = "down";
		if (t["type"].valid()) type = t["type"].get<std::string>();
		int button = 1;
		if (t["button"].valid()) button = t["button"].get<int>();

		// Convert stage/render coords to window coords using SDL_RenderCoordinatesToWindow
		float winX = 0.0f, winY = 0.0f;
		SDL_Renderer* renderer = c->getRenderer();
		if (renderer) 
		{
			SDL_RenderCoordinatesToWindow(renderer, sx, sy, &winX, &winY);
			LUA_INFO("pushMouseEvent_lua: SDL_RenderCoordinatesToWindow stage:(" << sx << "," << sy << ") -> window:(" << winX << "," << winY << ") type:" << type << " button:" << button);
		} 
		else 
		{
			// Fallback: simple scaling (may fail in tiled/letterboxed)
			const Core::CoreConfig& cfg = c->getConfig();
			winX = sx * cfg.pixelWidth;
			winY = sy * cfg.pixelHeight;
			LUA_INFO("pushMouseEvent_lua: Fallback scaling stage:(" << sx << "," << sy << ") -> window:(" << winX << "," << winY << ") type:" << type << " button:" << button);
		}

		// Debug logging for synthetic mouse events
		LUA_INFO("[pushMouseEvent_lua] stage:(" << sx << "," << sy << ") -> window:(" << winX << "," << winY << ") type:" << type << " button:" << button);

	Uint32 winID = 0;
	if (c->getWindow()) winID = SDL_GetWindowID(c->getWindow());

		SDL_Event ev;
		std::memset(&ev, 0, sizeof(ev));
		if (type == "up") {
			ev.type = SDL_EVENT_MOUSE_BUTTON_UP;
			ev.button.windowID = winID;
			ev.button.which = 0;
			ev.button.button = button;
			ev.button.clicks = 1;
			ev.button.x = winX;
			ev.button.y = winY;
			ev.motion.windowID = winID;
			ev.motion.which = 0;
			ev.motion.x = winX;
			ev.motion.y = winY;
		} else if (type == "motion") {
			ev.type = SDL_EVENT_MOUSE_MOTION;
			ev.motion.windowID = winID;
			ev.motion.which = 0;
			ev.motion.x = winX;
			ev.motion.y = winY;
		} else {
			ev.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
			ev.button.windowID = winID;
			ev.button.which = 0;
			ev.button.button = button;
			ev.button.clicks = 1;
			ev.button.x = winX;
			ev.button.y = winY;
			ev.motion.windowID = winID;
			ev.motion.which = 0;
			ev.motion.x = winX;
			ev.motion.y = winY;
		}

		// SDL_PushEvent(&ev);
		c->getEventManager().Queue_SDL_Event(ev);        
	}

	void pushKeyboardEvent_lua(const sol::object& args) {
		Core* c = &Core::getInstance();
		if (!args.is<sol::table>()) return;
		sol::table t = args.as<sol::table>();
		if (!t["key"].valid()) return;
		int key = t["key"].get<int>();
		std::string type = "down";
		if (t["type"].valid()) type = t["type"].get<std::string>();
		int mod = 0;
		if (t["mod"].valid()) mod = t["mod"].get<int>();

		SDL_Event ev;
		std::memset(&ev, 0, sizeof(ev));
		if (type == "up") ev.type = SDL_EVENT_KEY_UP;
		else ev.type = SDL_EVENT_KEY_DOWN;

		ev.key.windowID = c->getWindow() ? SDL_GetWindowID(c->getWindow()) : 0;
		ev.key.timestamp = SDL_GetTicks();
		ev.key.repeat = 0;
		ev.key.mod = mod;
		ev.key.key = key;

		// SDL_PushEvent(&ev);
		c->getEventManager().Queue_SDL_Event(ev);
	}


	// --- Main Loop & Event Dispatch --- //
	void quit_lua() { Core::getInstance().quit(); }
	void shutdown_lua() { Core::getInstance().shutdown(); }

	// Start the main loop from Lua. This calls Core::run() on the singleton
	void run_lua() { Core::getInstance().run(); }

	// Configuration from Lua table
	void configure_lua(const sol::table& config) {
		Core::getInstance().configureFromLua(config);
	}
	// Configuration from Lua file
	void configureFromFile_lua(const std::string& filename) {
		Core::getInstance().configureFromLuaFile(filename);
	}

	// --- Callback/Hook Registration --- //
	void registerOnInit_lua(std::function<bool()> fn) { getCore()._fnOnInit(fn); }
	void registerOnQuit_lua(std::function<void()> fn) { getCore()._fnOnQuit(fn); }
	void registerOnUpdate_lua(std::function<void(float)> fn) { getCore()._fnOnUpdate(fn); }
	void registerOnEvent_lua(std::function<void(const Event&)> fn) { getCore()._fnOnEvent(fn); }
	void registerOnRender_lua(std::function<void()> fn) { getCore()._fnOnRender(fn); }
	void registerOnUnitTest_lua(std::function<bool()> fn) { getCore()._fnOnUnitTest(fn); }
	void registerOnWindowResize_lua(std::function<void(int, int)> fn) { getCore()._fnOnWindowResize(fn); }

	// Generic registration helper so Lua can call: Core:registerOn("Update", function(dt) ... end)
	void registerOn_lua(const std::string& name, const sol::function& f)
	{
		// Wrap the lua function into a protected function for safety
		sol::protected_function pf = f;
		if (name == "Init") {
			registerOnInit_lua([pf]() -> bool {
				sol::protected_function_result r = pf();
				if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnInit error: ") + err.what()); return false; }
				try { return r.get<bool>(); } catch (...) { return false; }
			});
		} else if (name == "Quit") {
			registerOnQuit_lua([pf]() { sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnQuit error: ") + err.what()); } });
		} else if (name == "Update") {
			registerOnUpdate_lua([pf](float dt) { sol::protected_function_result r = pf(dt); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnUpdate error: ") + err.what()); } });
		} else if (name == "Event") {
			registerOnEvent_lua([pf](const Event& e) { sol::protected_function_result r = pf(e); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnEvent error: ") + err.what()); } });
		} else if (name == "Render") {
			registerOnRender_lua([pf]() { sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnRender error: ") + err.what()); } });
		} else if (name == "UnitTest") {
			registerOnUnitTest_lua([pf]() -> bool { sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnUnitTest error: ") + err.what()); return false; } try { return r.get<bool>(); } catch (...) { return false; } });
		} else if (name == "WindowResize") {
			registerOnWindowResize_lua([pf](int w, int h) { sol::protected_function_result r = pf(w, h); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnWindowResize error: ") + err.what()); } });
		} else {
			ERROR("Unknown registerOn name: " + name);
		}
	}

	// --- Stage/Root Node Management --- //
	void setRootNodeByName_lua(const std::string& name) { Core::getInstance().setRootNode(name); }
	void setRootNode_lua(const DisplayObject& handle) { Core::getInstance().setRootNode(handle); }
	void setStageByName_lua(const std::string& name) { Core::getInstance().setStage(name); }
	DisplayObject getRoot_lua() { return Core::getInstance().getRootNode(); }
	DisplayObject getStage_lua() { return Core::getInstance().getStageHandle(); }
	void setStage_lua(const DisplayObject& handle) { Core::getInstance().setRootNode(handle); }

	// --- Factory & EventManager Access --- //
	bool getIsTraversing_lua() { return Core::getInstance().getIsTraversing(); }
	void setIsTraversing_lua(bool traversing) { Core::getInstance().setIsTraversing(traversing); }

	// --- Object Creation --- //
	DisplayObject createDisplayObject_lua(const std::string& typeName, const sol::table& config) {
		return Core::getInstance().createDisplayObject(typeName, config);
	}

	// --- Focus & Hover Management --- //
	void doTabKeyPressForward_lua() { Core::getInstance().handleTabKeyPress(); }
	void doTabKeyPressReverse_lua() { Core::getInstance().handleTabKeyPressReverse(); }
	void setKeyboardFocusedObject_lua(const DisplayObject& handle) { Core::getInstance().setKeyboardFocusedObject(handle); }
	DisplayObject getKeyboardFocusedObject_lua() { return Core::getInstance().getKeyboardFocusedObject(); }
	void setMouseHoveredObject_lua(const DisplayObject& handle) { Core::getInstance().setMouseHoveredObject(handle); }
	DisplayObject getMouseHoveredObject_lua() { return Core::getInstance().getMouseHoveredObject(); }

	// --- Window Title & Timing --- //
	std::string getWindowTitle_lua() { return Core::getInstance().getWindowTitle(); }
	void setWindowTitle_lua(const std::string& title) { Core::getInstance().setWindowTitle(title); }
	float getElapsedTime_lua() { return Core::getInstance().getElapsedTime(); }

	// --- Object Lookup --- //
	DisplayObject getDisplayObject_lua(const std::string& name) {
		DisplayObject h = Core::getInstance().getDisplayObject(name);
		return h;
	}
	bool hasDisplayObject_lua(const std::string& name) { return Core::getInstance().hasDisplayObject(name); }

    AssetObject createAssetObject_lua(const std::string& typeName, const sol::table& config) {
        return getCore().createAssetObject(typeName, config);
    }
    AssetObject getAssetObject_lua(const std::string& name) {
        return getCore().getAssetObject(name);
    }
    bool hasAssetObject_lua(const std::string& name) {
        return getCore().hasAssetObject(name);
    }


	// --- Orphan / Future Child Management --- //
	void destroyDisplayObject_lua(const std::string& name) { Core::getInstance().destroyDisplayObject(name); }
	int countOrphanedDisplayObjects_lua() { return Core::getInstance().countOrphanedDisplayObjects(); }
	std::vector<DisplayObject> getOrphanedDisplayObjects_lua() { return Core::getInstance().getOrphanedDisplayObjects(); }
	void destroyOrphanedDisplayObjects_lua() { Core::getInstance().destroyOrphanedDisplayObjects(); }
	void collectGarbage_lua() { Core::getInstance().getFactory().collectGarbage(); }

	// --- Utility Methods --- //
	std::vector<std::string> listDisplayObjectNames_lua() { return Core::getInstance().listDisplayObjectNames(); }
	void printObjectRegistry_lua() { Core::getInstance().printObjectRegistry(); }


} // end namespace SDOM