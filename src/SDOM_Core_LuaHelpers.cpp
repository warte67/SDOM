// lua_BindHelpers.cpp --- Lua binding helpers implementation

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Core_LuaHelpers.hpp>

namespace SDOM
{
	// Implementations mirror the per-signature lambdas previously defined
	// inline inside Core::_registerDisplayObject(). These helpers centralize
	// the three-place binding pattern: usertype method, CoreForward table
	// entry, and a best-effort global alias.

	void core_bind_noarg(const std::string& name, std::function<void()> func,
					sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/) {
			fcopy();
			return true;
		};
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/){
			sol::state_view sv = ts;
			fcopy();
			return sol::make_object(sv, true);
		});
		try {
			lua[name] = [fcopy](sol::this_state ts) {
				sol::state_view sv = ts;
				fcopy();
				return sol::make_object(sv, true);
			};
		} catch(...) {}
	}

	void core_bind_table(const std::string& name, std::function<void(const sol::table&)> func,
					sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/, const sol::table& cfg) {
			fcopy(cfg);
			return true;
		};
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/, const sol::table& cfg){
			sol::state_view sv = ts;
			fcopy(cfg);
			return sol::make_object(sv, sol::nil);
		});
		try {
			lua[name] = [fcopy](sol::this_state ts, const sol::table& cfg) {
				sol::state_view sv = ts;
				fcopy(cfg);
				return sol::make_object(sv, sol::nil);
			};
		} catch(...) {}
	}

	void core_bind_string(const std::string& name, std::function<void(const std::string&)> func,
					sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/, const std::string& s) {
			fcopy(s);
			return true;
		};
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/, const std::string& s){
			sol::state_view sv = ts;
			fcopy(s);
			return sol::make_object(sv, sol::nil);
		});
		try {
			lua[name] = [fcopy](sol::this_state ts, const std::string& s) {
				sol::state_view sv = ts;
				fcopy(s);
				return sol::make_object(sv, sol::nil);
			};
		} catch(...) {}
	}

	void core_bind_bool_arg(const std::string& name, std::function<void(bool)> func,
					sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/, bool v) {
			fcopy(v);
			return true;
		};
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/, bool v){
			sol::state_view sv = ts;
			fcopy(v);
			return sol::make_object(sv, sol::nil);
		});
		try {
			lua[name] = [fcopy](sol::this_state ts, bool v) {
				sol::state_view sv = ts;
				fcopy(v);
				return sol::make_object(sv, sol::nil);
			};
		} catch(...) {}
	}

	void core_bind_do_arg(const std::string& name, std::function<void(const DisplayHandle&)> func,
					sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		auto resolve_displayobject = [](const sol::object& maybeArg) -> DisplayHandle {
			if (!maybeArg.valid()) return DisplayHandle();
			try {
				if (maybeArg.is<DisplayHandle>()) return maybeArg.as<DisplayHandle>();
				if (maybeArg.is<std::string>()) {
					std::string nm = maybeArg.as<std::string>();
					return Core::getInstance().getDisplayObject(nm);
				}
				if (maybeArg.is<sol::table>()) {
					sol::table t = maybeArg.as<sol::table>();
					if (t["name"].valid()) {
						try { std::string nm = t.get<std::string>("name"); return Core::getInstance().getDisplayObject(nm); } catch(...) {}
					}
				}
			} catch(...) {}
			return DisplayHandle();
		};

		objHandleType[name] = [fcopy, resolve_displayobject](Core& /*core*/, const sol::object& maybeArg) {
			DisplayHandle h = resolve_displayobject(maybeArg);
			if (h.isValid()) fcopy(h);
			return true;
		};
		coreTable.set_function(name, [fcopy, resolve_displayobject](sol::this_state ts, sol::object /*self*/, sol::object maybeArg) {
			sol::state_view sv = ts;
			DisplayHandle h = resolve_displayobject(maybeArg);
			if (h.isValid()) fcopy(h);
			return sol::make_object(sv, sol::nil);
		});
		try {
			lua[name] = [fcopy, resolve_displayobject](sol::this_state ts, sol::object maybeArg) {
				sol::state_view sv = ts;
				DisplayHandle h = resolve_displayobject(maybeArg);
				if (h.isValid()) fcopy(h);
				return sol::make_object(sv, sol::nil);
			};
		} catch(...) {}
	}

	void core_bind_object_arg(const std::string& name, std::function<void(const sol::object&)> func,
						sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/, const sol::object& args) {
			fcopy(args);
			return true;
		};
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/, const sol::object& args) {
			sol::state_view sv = ts;
			fcopy(args);
			return sol::make_object(sv, sol::nil);
		});
		try {
			lua[name] = [fcopy](sol::this_state ts, const sol::object& args) {
				sol::state_view sv = ts;
				fcopy(args);
				return sol::make_object(sv, sol::nil);
			};
		} catch(...) {}
	}

	// void core_bind_return_displayobject(const std::string& name, std::function<DisplayHandle()> func,
	// 							sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	// {
	// 	auto fcopy = func;
	// 	objHandleType[name] = [fcopy](Core& /*core*/) -> DisplayHandle { return fcopy(); };
	// 	coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/) {
	// 		sol::state_view sv = ts;
	// 		DisplayHandle h = fcopy();
	// 		// Ensure DisplayHandle usertype exists before constructing userdata
	// 		try { SDOM::DisplayHandle::ensure_handle_table(sv); } catch(...) {}
	// 		if (h.isValid() && h.get()) return sol::make_object(sv, h);
	// 		return sol::make_object(sv, DisplayHandle());
	// 	});
	// 	try {
	// 		lua[name] = [fcopy](sol::this_state ts) {
	// 			sol::state_view sv = ts;
	// 			DisplayHandle h = fcopy();
	// 			if (h.isValid() && h.get()) return sol::make_object(sv, h);
	// 			return sol::make_object(sv, DisplayHandle());
	// 		};
	// 	} catch(...) {}
	// }

	void core_bind_return_bool(const std::string& name, std::function<bool()> func,
						sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/) -> bool { return fcopy(); };
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/) {
			sol::state_view sv = ts;
			bool v = fcopy();
			return sol::make_object(sv, v);
		});
		try {
			lua[name] = [fcopy](sol::this_state ts) {
				sol::state_view sv = ts;
				bool v = fcopy();
				return sol::make_object(sv, v);
			};
		} catch(...) {}
	}

	void core_bind_return_string(const std::string& name, std::function<std::string()> func,
							sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/) -> std::string { return fcopy(); };
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/) {
			sol::state_view sv = ts;
			std::string s = fcopy();
			return sol::make_object(sv, s);
		});
		try {
			lua[name] = [fcopy](sol::this_state ts) {
				sol::state_view sv = ts;
				std::string s = fcopy();
				return sol::make_object(sv, s);
			};
		} catch(...) {}
	}

	void core_bind_return_float(const std::string& name, std::function<float()> func,
						sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/) -> float { return fcopy(); };
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/) {
			sol::state_view sv = ts;
			float v = fcopy();
			return sol::make_object(sv, v);
		});
		try {
			lua[name] = [fcopy](sol::this_state ts) {
				sol::state_view sv = ts;
				float v = fcopy();
				return sol::make_object(sv, v);
			};
		} catch(...) {}
	}

	void core_bind_return_int(const std::string& name, std::function<int()> func,
						sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/) -> int { return fcopy(); };
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/) {
			sol::state_view sv = ts;
			int v = fcopy();
			return sol::make_object(sv, v);
		});
		try {
			lua[name] = [fcopy](sol::this_state ts) {
				sol::state_view sv = ts;
				int v = fcopy();
				return sol::make_object(sv, v);
			};
		} catch(...) {}
	}

	void core_bind_return_vector_do(const std::string& name, std::function<std::vector<DisplayHandle>()> func,
							sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/) -> std::vector<DisplayHandle> { return fcopy(); };
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/) {
			sol::state_view sv = ts;
			auto v = fcopy();
			return sol::make_object(sv, v);
		});
		try {
			lua[name] = [fcopy](sol::this_state ts) {
				sol::state_view sv = ts;
				auto v = fcopy();
				return sol::make_object(sv, v);
			};
		} catch(...) {}
	}

	void core_bind_return_vector_string(const std::string& name, std::function<std::vector<std::string>()> func,
								sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/) -> std::vector<std::string> { return fcopy(); };
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/) {
			sol::state_view sv = ts;
			auto v = fcopy();
			return sol::make_object(sv, v);
		});
		try {
			lua[name] = [fcopy](sol::this_state ts) {
				sol::state_view sv = ts;
				auto v = fcopy();
				return sol::make_object(sv, v);
			};
		} catch(...) {}
	}

	// void core_bind_string_table_return_do(const std::string& name, std::function<DisplayHandle(const std::string&, const sol::table&)> func,
	// 								sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	// {
	// 	auto fcopy = func;
	// 	objHandleType[name] = [fcopy](Core& /*core*/, const std::string& typeName, const sol::table& cfg) -> DisplayHandle {
	// 		return fcopy(typeName, cfg);
	// 	};
	// 	coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/, const std::string& typeName, const sol::table& cfg) {
	// 		sol::state_view sv = ts;
	// 		DisplayHandle h = fcopy(typeName, cfg);
	// 		// Ensure DisplayHandle usertype exists before constructing userdata
	// 		try { SDOM::DisplayHandle::ensure_handle_table(sv); } catch(...) {}
	// 		if (h.isValid() && h.get()) return sol::make_object(sv, h);
	// 		return sol::make_object(sv, DisplayHandle());
	// 	});
	// 	try {
	// 		lua[name] = [fcopy](sol::this_state ts, const std::string& typeName, const sol::table& cfg) {
	// 			sol::state_view sv = ts;
	// 			DisplayHandle h = fcopy(typeName, cfg);
	// 			if (h.isValid() && h.get()) return sol::make_object(sv, h);
	// 			return sol::make_object(sv, DisplayHandle());
	// 		};
	// 	} catch(...) {}
	// }

	void core_bind_string_table_return_asset(const std::string& name, std::function<AssetHandle(const std::string&, const sol::table&)> func,
									sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/, const std::string& typeName, const sol::table& cfg) -> AssetHandle {
			return fcopy(typeName, cfg);
		};
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/, const std::string& typeName, const sol::table& cfg) {
			sol::state_view sv = ts;
			AssetHandle h = fcopy(typeName, cfg);
			if (h.isValid() && h.get()) return sol::make_object(sv, h);
			return sol::make_object(sv, AssetHandle());
		});
		try {
			lua[name] = [fcopy](sol::this_state ts, const std::string& typeName, const sol::table& cfg) {
				sol::state_view sv = ts;
				AssetHandle h = fcopy(typeName, cfg);
				if (h.isValid() && h.get()) return sol::make_object(sv, h);
				return sol::make_object(sv, AssetHandle());
			};
		} catch(...) {}
	}

	// void core_bind_string_return_do(const std::string& name, std::function<DisplayHandle(const std::string&)> func,
	// 						sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	// {
	// 	auto fcopy = func;
	// 	objHandleType[name] = [fcopy](Core& /*core*/, const std::string& s) -> DisplayHandle { return fcopy(s); };
	// 	coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/, const std::string& s) {
	// 		sol::state_view sv = ts;
	// 		DisplayHandle h = fcopy(s);
	// 		// Ensure DisplayHandle usertype exists before constructing userdata
	// 		try { SDOM::DisplayHandle::ensure_handle_table(sv); } catch(...) {}
	// 		if (h.isValid() && h.get()) return sol::make_object(sv, h);
	// 		return sol::make_object(sv, DisplayHandle());
	// 	});
	// 	try {
	// 		lua[name] = [fcopy](sol::this_state ts, const std::string& s) {
	// 			sol::state_view sv = ts;
	// 			DisplayHandle h = fcopy(s);
	// 			if (h.isValid() && h.get()) return sol::make_object(sv, h);
	// 			return sol::make_object(sv, DisplayHandle());
	// 		};
	// 	} catch(...) {}
	// }

	void core_bind_string_return_asset(const std::string& name, std::function<AssetHandle(const std::string&)> func,
							sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/, const std::string& s) -> AssetHandle { return fcopy(s); };
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/, const std::string& s) {
			sol::state_view sv = ts;
			AssetHandle h = fcopy(s);
			if (h.isValid() && h.get()) return sol::make_object(sv, h);
			return sol::make_object(sv, AssetHandle());
		});
		try {
			lua[name] = [fcopy](sol::this_state ts, const std::string& s) {
				sol::state_view sv = ts;
				AssetHandle h = fcopy(s);
				if (h.isValid() && h.get()) return sol::make_object(sv, h);
				return sol::make_object(sv, AssetHandle());
			};
		} catch(...) {}
	}
	
	void core_bind_string_return_bool(const std::string& name, std::function<bool(const std::string&)> func,
								sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/, const std::string& s) -> bool { return fcopy(s); };
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/, const std::string& s) {
			sol::state_view sv = ts;
			bool v = fcopy(s);
			return sol::make_object(sv, v);
		});
		try {
			lua[name] = [fcopy](sol::this_state ts, const std::string& s) {
				sol::state_view sv = ts;
				bool v = fcopy(s);
				return sol::make_object(sv, v);
			};
		} catch(...) {}
	}

	void core_bind_callback_bool(const std::string& name, std::function<void(std::function<bool()>)> registrar,
							sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto reg = registrar;
		auto conv = [](const sol::function& f) -> std::function<bool()> {
			return [f]() -> bool {
				sol::protected_function pf = f;
				sol::protected_function_result r = pf();
				if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua callback error: ") + err.what()); return false; }
				try { return r.get<bool>(); } catch(...) { return false; }
			};
		};

		objHandleType[name] = [reg, conv](Core& /*core*/, const sol::function& f) {
			reg(conv(f));
			return true;
		};
		coreTable.set_function(name, [reg, conv](sol::this_state ts, sol::object /*self*/, const sol::function& f) {
			sol::state_view sv = ts;
			reg(conv(f));
			return sol::make_object(sv, sol::nil);
		});
		try {
			lua[name] = [reg, conv](sol::this_state ts, const sol::function& f) {
				sol::state_view sv = ts;
				reg(conv(f));
				return sol::make_object(sv, sol::nil);
			};
		} catch(...) {}
	}

	void core_bind_callback_void(const std::string& name, std::function<void(std::function<void()>)> registrar,
							sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto reg = registrar;
		auto conv = [](const sol::function& f) -> std::function<void()> {
			return [f]() {
				sol::protected_function pf = f;
				sol::protected_function_result r = pf();
				if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua callback error: ") + err.what()); }
			};
		};

		objHandleType[name] = [reg, conv](Core& /*core*/, const sol::function& f) {
			reg(conv(f));
			return true;
		};
		coreTable.set_function(name, [reg, conv](sol::this_state ts, sol::object /*self*/, const sol::function& f) {
			sol::state_view sv = ts;
			reg(conv(f));
			return sol::make_object(sv, sol::nil);
		});
		try {
			lua[name] = [reg, conv](sol::this_state ts, const sol::function& f) {
				sol::state_view sv = ts;
				reg(conv(f));
				return sol::make_object(sv, sol::nil);
			};
		} catch(...) {}
	}

	void core_bind_callback_update(const std::string& name, std::function<void(std::function<void(float)>)> registrar,
							sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto reg = registrar;
		auto conv = [](const sol::function& f) -> std::function<void(float)> {
			return [f](float dt) {
				sol::protected_function pf = f;
				sol::protected_function_result r = pf(dt);
				if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua callback error: ") + err.what()); }
			};
		};

		objHandleType[name] = [reg, conv](Core& /*core*/, const sol::function& f) {
			reg(conv(f));
			return true;
		};
		coreTable.set_function(name, [reg, conv](sol::this_state ts, sol::object /*self*/, const sol::function& f) {
			sol::state_view sv = ts;
			reg(conv(f));
			return sol::make_object(sv, sol::nil);
		});
		try {
			lua[name] = [reg, conv](sol::this_state ts, const sol::function& f) {
				sol::state_view sv = ts;
				reg(conv(f));
				return sol::make_object(sv, sol::nil);
			};
		} catch(...) {}
	}

	void core_bind_callback_event(const std::string& name, std::function<void(std::function<void(const Event&)>)> registrar,
							sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto reg = registrar;
		auto conv = [](const sol::function& f) -> std::function<void(const Event&)> {
			return [f](const Event& e) {
				sol::protected_function pf = f;
				sol::protected_function_result r = pf(e);
				if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua callback error: ") + err.what()); }
			};
		};

		objHandleType[name] = [reg, conv](Core& /*core*/, const sol::function& f) {
			reg(conv(f));
			return true;
		};
		coreTable.set_function(name, [reg, conv](sol::this_state ts, sol::object /*self*/, const sol::function& f) {
			sol::state_view sv = ts;
			reg(conv(f));
			return sol::make_object(sv, sol::nil);
		});
		try {
			lua[name] = [reg, conv](sol::this_state ts, const sol::function& f) {
				sol::state_view sv = ts;
				reg(conv(f));
				return sol::make_object(sv, sol::nil);
			};
		} catch(...) {}
	}

	void core_bind_callback_resize(const std::string& name, std::function<void(std::function<void(int,int)>)> registrar,
							sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto reg = registrar;
		auto conv = [](const sol::function& f) -> std::function<void(int,int)> {
			return [f](int w, int h) {
				sol::protected_function pf = f;
				sol::protected_function_result r = pf(w, h);
				if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua callback error: ") + err.what()); }
			};
		};

		objHandleType[name] = [reg, conv](Core& /*core*/, const sol::function& f) {
			reg(conv(f));
			return true;
		};
		coreTable.set_function(name, [reg, conv](sol::this_state ts, sol::object /*self*/, const sol::function& f) {
			sol::state_view sv = ts;
			reg(conv(f));
			return sol::make_object(sv, sol::nil);
		});
		try {
			lua[name] = [reg, conv](sol::this_state ts, const sol::function& f) {
				sol::state_view sv = ts;
				reg(conv(f));
				return sol::make_object(sv, sol::nil);
			};
		} catch(...) {}
	}

	void core_bind_string_function_forwarder(const std::string& name, std::function<void(const std::string&, const sol::function&)> func,
										sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/, const std::string& s, const sol::function& f) {
			fcopy(s, f);
			return true;
		};
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/, const std::string& s, const sol::function& f) {
			sol::state_view sv = ts;
			fcopy(s, f);
			return sol::make_object(sv, sol::nil);
		});
		try {
			lua[name] = [fcopy](sol::this_state ts, const std::string& s, const sol::function& f) {
				sol::state_view sv = ts;
				fcopy(s, f);
				return sol::make_object(sv, sol::nil);
			};
		} catch(...) {}
	}

	void core_bind_name_or_handle(const std::string& name, std::function<void(const std::string&)> nameFunc, std::function<void(const DisplayHandle&)> handleFunc,
							 sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto nfc = nameFunc;
		auto hfc = handleFunc;

		// only bind the CoreForward table entry and global alias here because
		// the usertype method variant is often hand-written; still provide all three
		coreTable.set_function(name, [nfc, hfc](sol::this_state ts, sol::object /*self*/, sol::object maybeArg) {
			sol::state_view sv = ts;
			if (!maybeArg.valid()) return sol::make_object(sv, sol::nil);
			if (maybeArg.is<std::string>()) {
				std::string nm = maybeArg.as<std::string>();
				nfc(nm);
				return sol::make_object(sv, sol::nil);
			}
			if (maybeArg.is<DisplayHandle>()) {
				DisplayHandle h = maybeArg.as<DisplayHandle>();
				hfc(h);
				return sol::make_object(sv, sol::nil);
			}
			if (maybeArg.is<sol::table>()) {
				sol::table t = maybeArg.as<sol::table>();
				if (t["name"].valid()) {
					try { std::string nm = t.get<std::string>("name"); nfc(nm); return sol::make_object(sv, sol::nil); } catch(...) {}
				}
			}
			return sol::make_object(sv, sol::nil);
		});
		try {
			lua[name] = [nfc, hfc](sol::this_state ts, sol::object maybeArg) {
				sol::state_view sv = ts;
				if (!maybeArg.valid()) return sol::make_object(sv, sol::nil);
				if (maybeArg.is<std::string>()) {
					std::string nm = maybeArg.as<std::string>(); nfc(nm); return sol::make_object(sv, sol::nil);
				}
				if (maybeArg.is<DisplayHandle>()) { DisplayHandle h = maybeArg.as<DisplayHandle>(); hfc(h); return sol::make_object(sv, sol::nil); }
				if (maybeArg.is<sol::table>()) { sol::table t = maybeArg.as<sol::table>(); if (t["name"].valid()) { try { std::string nm = t.get<std::string>("name"); nfc(nm); return sol::make_object(sv, sol::nil); } catch(...) {} } }
				return sol::make_object(sv, sol::nil);
			};
		} catch(...) {}
	}

	// Configuration from Lua table
	void core_configure_lua(const sol::table& config) {
		// Preprocess a 'resources' array in the config so resources are registered
		// before display objects are constructed. This allows Labels and other
		// display objects to reference resources by name during onInit().
		try {
		// Ensure SDL (window/renderer/texture) is initialized before
		// creating assets that depend on an SDL_Renderer (e.g., BitmapFont
		// -> SpriteSheet -> Texture). Build a minimal CoreConfig from the
		// provided table and call configure() — this does not create any
		// display objects.
		try {
			Core::CoreConfig preCfg = getCore().getConfig();
			if (config.valid()) {
				try { if (config["windowWidth"].valid())  preCfg.windowWidth  = config["windowWidth"].get<double>(); } catch(...) {}
				try { if (config["windowHeight"].valid()) preCfg.windowHeight = config["windowHeight"].get<double>(); } catch(...) {}
				try { if (config["pixelWidth"].valid())   preCfg.pixelWidth   = config["pixelWidth"].get<double>(); } catch(...) {}
				try { if (config["pixelHeight"].valid())  preCfg.pixelHeight  = config["pixelHeight"].get<double>(); } catch(...) {}
				try { if (config["preserveAspectRatio"].valid()) preCfg.preserveAspectRatio = config["preserveAspectRatio"].get<bool>(); } catch(...) {}
				try { if (config["allowTextureResize"].valid())  preCfg.allowTextureResize  = config["allowTextureResize"].get<bool>(); } catch(...) {}
			}
			getCore().configure(preCfg);
		} catch(...) {
			DEBUG_LOG("configure_lua: pre-initialization configure() failed");
		}
			bool logRes = false;
			try { if (config.valid() && config["debug"].valid()) { sol::table dbg = config["debug"]; if (dbg.valid() && dbg["logResourceCreation"].valid()) logRes = dbg["logResourceCreation"].get<bool>(); } } catch(...) {}
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
							// Normalize font size from any supported key
							int sizeVal = -1;
							try {
								if (r["font_size"].valid()) sizeVal = r["font_size"].get<int>();
								else if (r["fontSize"].valid()) sizeVal = r["fontSize"].get<int>();
								else if (r["size"].valid()) sizeVal = r["size"].get<int>();
							} catch(...) { sizeVal = -1; }
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
								if (sizeVal > 0) ttfCfg["internalFontSize"] = sizeVal;

								if (logRes) std::cout << "[CONFIG] creating underlying TTFAsset name='" << ttfAssetName << "' filename='" << filename << "'\n";
								AssetHandle ttfh = getCore().createAssetObject(std::string("TTFAsset"), ttfCfg);
								if (logRes) std::cout << "[CONFIG] createAsset(TTFAsset) returned isValid=" << (ttfh.isValid() ? "true" : "false") << " for name='" << ttfAssetName << "'\n";
								if (!ttfh.isValid()) {
									if (logRes) std::cout << "[CONFIG] FAILED to create underlying TTFAsset: " << ttfAssetName << "\n";
								}
							} else {
								if (logRes) std::cout << "[CONFIG] underlying TTFAsset already exists: " << ttfAssetName << "\n";
							}

							// Now create the public TruetypeFont that references the TTFAsset
							r["filename"] = ttfAssetName; // TruetypeFont expects filename to be a TTFAsset name
							r["type"] = std::string("truetype");
							if (sizeVal > 0) r["fontSize"] = sizeVal; // TruetypeFont expects fontSize/size

							if (logRes) std::cout << "[CONFIG] creating resource name='" << name << "' type='" << std::string("truetype") << "' filename='" << r["filename"].get<std::string>() << "'\n";
							AssetHandle h = getCore().createAssetObject(std::string("truetype"), r);
							if (logRes) std::cout << "[CONFIG] createAsset(truetype) returned isValid=" << (h.isValid() ? "true" : "false") << " for name='" << name << "'\n";
							if (!h.isValid()) {
								if (logRes) std::cout << "[CONFIG] FAILED to create resource: " << name << " (type=truetype)\n";
							} else {
								if (logRes) std::cout << "[CONFIG] Registered resource: " << name << " (type=truetype)\n";
							}
						} else {
							if (logRes) std::cout << "[CONFIG] creating resource name='" << name << "' type='" << typeName << "' filename='" << filename << "'\n";
							AssetHandle h = getCore().createAssetObject(typeName, r);
							if (logRes) std::cout << "[CONFIG] createAsset returned isValid=" << (h.isValid() ? "true" : "false") << " for name='" << name << "'\n";
							if (!h.isValid()) {
								if (logRes) std::cout << "[CONFIG] FAILED to create resource: " << name << " (type=" << typeName << ")\n";
							} else {
								if (logRes) std::cout << "[CONFIG] Registered resource: " << name << " (type=" << typeName << ")\n";
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
	} // END: core_configure_lua()


	// Configuration from Lua file
	void core_configureFromFile_lua(const std::string& filename) {
		Core::getInstance().configureFromLuaFile(filename);
	} // END: core_configureFromFile_lua()


	// --- Callback/Hook Registration --- //
	void core_registerOnInit_lua(std::function<bool()> fn) { getCore()._fnOnInit(fn); }
	void core_registerOnQuit_lua(std::function<void()> fn) { getCore()._fnOnQuit(fn); }
	void core_registerOnUpdate_lua(std::function<void(float)> fn) { getCore()._fnOnUpdate(fn); }
	void core_registerOnEvent_lua(std::function<void(const Event&)> fn) { getCore()._fnOnEvent(fn); }
	void core_registerOnRender_lua(std::function<void()> fn) { getCore()._fnOnRender(fn); }
	void core_registerOnUnitTest_lua(std::function<bool()> fn) { getCore()._fnOnUnitTest(fn); }
	void core_registerOnWindowResize_lua(std::function<void(int, int)> fn) { getCore()._fnOnWindowResize(fn); }


	// Generic registration helper so Lua can call: Core:registerOn("Update", function(dt) ... end)
	void core_registerOn_lua(const std::string& name, const sol::function& f)
	{
		// Wrap the lua function into a protected function for safety
		sol::protected_function pf = f;
		if (name == "Init") {
			core_registerOnInit_lua([pf]() -> bool {
				sol::protected_function_result r = pf();
				if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnInit error: ") + err.what()); return false; }
				// Ensure the function returned at least one value before attempting to get<bool>()
				try {
					if (r.return_count() <= 0) {
						ERROR(std::string("Lua registerOnInit error: no return value (expected boolean)"));
						return false;
					}
					return r.get<bool>();
				} catch (const std::exception& e) {
					ERROR(std::string("Lua registerOnInit error: bad return type: ") + e.what());
					return false;
				} catch (...) {
					return false;
				}
			});
		} else if (name == "Quit") {
			core_registerOnQuit_lua([pf]() { sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnQuit error: ") + err.what()); } });
		} else if (name == "Update") {
			core_registerOnUpdate_lua([pf](float dt) { sol::protected_function_result r = pf(dt); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnUpdate error: ") + err.what()); } });
		} else if (name == "Event") {
			core_registerOnEvent_lua([pf](const Event& e) { sol::protected_function_result r = pf(e); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnEvent error: ") + err.what()); } });
		} else if (name == "Render") {
			core_registerOnRender_lua([pf]() { sol::protected_function_result r = pf(); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnRender error: ") + err.what()); } });
		} else if (name == "UnitTest") {
			core_registerOnUnitTest_lua([pf]() -> bool {
				sol::protected_function_result r = pf();
				if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnUnitTest error: ") + err.what()); return false; }
				try {
					if (r.return_count() <= 0) {
						ERROR(std::string("Lua registerOnUnitTest error: no return value (expected boolean)"));
						return false;
					}
					return r.get<bool>();
				} catch (const std::exception& e) {
					ERROR(std::string("Lua registerOnUnitTest error: bad return type: ") + e.what());
					return false;
				} catch (...) {
					return false;
				}
			});
		} else if (name == "WindowResize") {
			core_registerOnWindowResize_lua([pf](int w, int h) { sol::protected_function_result r = pf(w, h); if (!r.valid()) { sol::error err = r; ERROR(std::string("Lua registerOnWindowResize error: ") + err.what()); } });
		} else {
			ERROR("Unknown registerOn name: " + name);
		}
	} // END: core_registerOn_lua()




} // namespace SDOM