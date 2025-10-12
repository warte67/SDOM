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

	// // Start the main loop from Lua. This calls Core::run() on the singleton
	// void run_lua() { Core::getInstance().run(); }

	// Configuration from Lua table
	void configure_lua(const sol::table& config) {
		// Preprocess a 'resources' array in the config so resources are registered
		// before display objects are constructed. This allows Labels and other
		// display objects to reference resources by name during onInit().
		try {
		// Ensure the Factory and its resource type registrations exist before
		// attempting to create assets. Factory::onInit() is now idempotent so
		// calling it here is safe and avoids ordering issues where resource
		// types haven't been registered yet.
		try {
			getCore().getFactory().onInit();
		} catch (...) {
			DEBUG_LOG("Factory::onInit() threw during configure_lua preprocessing");
		}
			if (config.valid() && config["resources"].valid()) {
				sol::table resTbl = config["resources"];
				for (std::size_t i = 1; i <= resTbl.size(); ++i) {
					try {
						sol::object obj = resTbl[i];
						if (!obj.valid() || !obj.is<sol::table>()) continue;
						sol::table r = obj.as<sol::table>();

						// Validate name and filename
						if (!r["name"].valid() || !r["filename"].valid()) {
							DEBUG_LOG("Skipping resource entry: missing name or filename");
							continue;
						}
						std::string name = r["name"].get<std::string>();
						std::string filename = r["filename"].get<std::string>();

						// Resolve type: accept explicit or infer from alias/filename
						std::string typeName;
						if (r["type"].valid()) {
							typeName = r["type"].get<std::string>();
						} else {
							// Basic inference by extension
							std::string lower = filename;
							std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
							if (lower.find(".ttf") != std::string::npos) typeName = "truetype";
							else if (lower.find(".png") != std::string::npos) typeName = "Texture";
							else {
								DEBUG_LOG("Cannot infer resource type for '" << name << "' (" << filename << ")");
								continue;
							}
						}

						// Normalize common aliases
						if (typeName == "TruetypeFont" || typeName == "truetypefont" || typeName == "Truetype") typeName = "truetype";
						if (typeName == "TTFAsset") typeName = "TTFAsset"; // keep as-is

						// Ensure the table has required keys for Factory::createAsset
						r["name"] = name;
						r["type"] = typeName;
						r["filename"] = filename;

						// Special-case TrueType: create an underlying TTFAsset first and
						// then create a TruetypeFont that references the TTFAsset by name.
						if (typeName == "truetype") {
							std::string ttfAssetName;
							if (r["ttfAssetName"].valid()) ttfAssetName = r["ttfAssetName"].get<std::string>();
							else ttfAssetName = name + std::string("_TTFAsset");

							// If the TTFAsset doesn't already exist, create it
							if (!getCore().hasAssetObject(ttfAssetName)) {
								sol::state_view lua = SDOM::getLua();
								sol::table ttfCfg = lua.create_table();
								ttfCfg["name"] = ttfAssetName;
								ttfCfg["type"] = std::string("TTFAsset");
								ttfCfg["filename"] = filename;
								// propagate font size if provided
								if (r["fontSize"].valid()) ttfCfg["internalFontSize"] = r["fontSize"].get<int>();
								else if (r["size"].valid()) ttfCfg["internalFontSize"] = r["size"].get<int>();

								std::cout << "[CONFIG] creating underlying TTFAsset name='" << ttfAssetName << "' filename='" << filename << "'\n";
								AssetHandle ttfh = getCore().createAssetObject(std::string("TTFAsset"), ttfCfg);
								std::cout << "[CONFIG] createAsset(TTFAsset) returned isValid=" << (ttfh.isValid() ? "true" : "false") << " for name='" << ttfAssetName << "'\n";
								if (!ttfh.isValid()) {
									std::cout << "[CONFIG] FAILED to create underlying TTFAsset: " << ttfAssetName << "\n";
								}
							} else {
								std::cout << "[CONFIG] underlying TTFAsset already exists: " << ttfAssetName << "\n";
							}

							// Now create the public TruetypeFont that references the TTFAsset
							r["filename"] = ttfAssetName; // TruetypeFont expects filename to be a TTFAsset name
							r["type"] = std::string("truetype");

							std::cout << "[CONFIG] creating resource name='" << name << "' type='" << std::string("truetype") << "' filename='" << r["filename"].get<std::string>() << "'\n";
							AssetHandle h = getCore().createAssetObject(std::string("truetype"), r);
							std::cout << "[CONFIG] createAsset(truetype) returned isValid=" << (h.isValid() ? "true" : "false") << " for name='" << name << "'\n";
							if (!h.isValid()) {
								std::cout << "[CONFIG] FAILED to create resource: " << name << " (type=truetype)\n";
							} else {
								std::cout << "[CONFIG] Registered resource: " << name << " (type=truetype)\n";
							}
						} else {
							std::cout << "[CONFIG] creating resource name='" << name << "' type='" << typeName << "' filename='" << filename << "'\n";
							AssetHandle h = getCore().createAssetObject(typeName, r);
							std::cout << "[CONFIG] createAsset returned isValid=" << (h.isValid() ? "true" : "false") << " for name='" << name << "'\n";
							if (!h.isValid()) {
								std::cout << "[CONFIG] FAILED to create resource: " << name << " (type=" << typeName << ")\n";
							} else {
								std::cout << "[CONFIG] Registered resource: " << name << " (type=" << typeName << ")\n";
							}
						}

					} catch (const std::exception& e) {
						DEBUG_LOG("Exception while processing resource entry: " << e.what());
					}
				}
			}
		} catch(...) {
			DEBUG_LOG("Exception while preprocessing resources table (ignored)");
		}

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
	void setRootNode_lua(const DisplayHandle& handle) { Core::getInstance().setRootNode(handle); }
	void setStageByName_lua(const std::string& name) { Core::getInstance().setStage(name); }
	DisplayHandle getRoot_lua() { return Core::getInstance().getRootNode(); }
	DisplayHandle getStage_lua() { return Core::getInstance().getStageHandle(); }
	void setStage_lua(const DisplayHandle& handle) { Core::getInstance().setRootNode(handle); }

	// --- Factory & EventManager Access --- //
	bool getIsTraversing_lua() { return Core::getInstance().getIsTraversing(); }
	void setIsTraversing_lua(bool traversing) { Core::getInstance().setIsTraversing(traversing); }

	// --- Object Creation --- //
	DisplayHandle createDisplayObject_lua(const std::string& typeName, const sol::table& config) {
		return Core::getInstance().createDisplayObject(typeName, config);
	}

	// --- Focus & Hover Management --- //
	void doTabKeyPressForward_lua() { Core::getInstance().handleTabKeyPress(); }
	void doTabKeyPressReverse_lua() { Core::getInstance().handleTabKeyPressReverse(); }
	void setKeyboardFocusedObject_lua(const DisplayHandle& handle) { Core::getInstance().setKeyboardFocusedObject(handle); }
	DisplayHandle getKeyboardFocusedObject_lua() { return Core::getInstance().getKeyboardFocusedObject(); }
	void setMouseHoveredObject_lua(const DisplayHandle& handle) { Core::getInstance().setMouseHoveredObject(handle); }
	DisplayHandle getMouseHoveredObject_lua() { return Core::getInstance().getMouseHoveredObject(); }

	// --- Window Title & Timing --- //
	std::string getWindowTitle_lua() { return Core::getInstance().getWindowTitle(); }
	void setWindowTitle_lua(const std::string& title) { Core::getInstance().setWindowTitle(title); }
	float getElapsedTime_lua() { return Core::getInstance().getElapsedTime(); }

	// --- Object Lookup --- //
	DisplayHandle getDisplayObject_lua(const std::string& name) {
		DisplayHandle h = Core::getInstance().getDisplayObject(name);
		return h;
	}
	bool hasDisplayObject_lua(const std::string& name) { return Core::getInstance().hasDisplayObject(name); }

    AssetHandle createAssetObject_lua(const std::string& typeName, const sol::table& config) {
        return getCore().createAssetObject(typeName, config);
    }
    AssetHandle getAssetObject_lua(const std::string& name) {
        return getCore().getAssetObject(name);
    }
    bool hasAssetObject_lua(const std::string& name) {
        return getCore().hasAssetObject(name);
    }


	// --- Orphan / Future Child Management --- //
	void destroyDisplayObject_lua(const std::string& name) { Core::getInstance().destroyDisplayObject(name); }
	int countOrphanedDisplayObjects_lua() { return Core::getInstance().countOrphanedDisplayObjects(); }
	std::vector<DisplayHandle> getOrphanedDisplayObjects_lua() { return Core::getInstance().getOrphanedDisplayObjects(); }
	void destroyOrphanedDisplayObjects_lua() { Core::getInstance().destroyOrphanedDisplayObjects(); }
	void collectGarbage_lua() { Core::getInstance().getFactory().collectGarbage(); }

	// --- Utility Methods --- //
	std::vector<std::string> listDisplayObjectNames_lua() { return Core::getInstance().listDisplayObjectNames(); }
	void printObjectRegistry_lua() { Core::getInstance().printObjectRegistry(); }


} // end namespace SDOM