// lua_BindHelpers.cpp --- Lua binding helpers implementation

#include <SDOM/lua_BindHelpers.hpp>

namespace SDOM
{
	// Implementations mirror the per-signature lambdas previously defined
	// inline inside Core::_registerDisplayObject(). These helpers centralize
	// the three-place binding pattern: usertype method, CoreForward table
	// entry, and a best-effort global alias.

	void bind_noarg(const std::string& name, std::function<void()> func,
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

	void bind_table(const std::string& name, std::function<void(const sol::table&)> func,
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

	void bind_string(const std::string& name, std::function<void(const std::string&)> func,
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

	void bind_bool_arg(const std::string& name, std::function<void(bool)> func,
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

	void bind_do_arg(const std::string& name, std::function<void(const DisplayObject&)> func,
					sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		auto resolve_displayobject = [](const sol::object& maybeArg) -> DisplayObject {
			if (!maybeArg.valid()) return DisplayObject();
			try {
				if (maybeArg.is<DisplayObject>()) return maybeArg.as<DisplayObject>();
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
			return DisplayObject();
		};

		objHandleType[name] = [fcopy, resolve_displayobject](Core& /*core*/, const sol::object& maybeArg) {
			DisplayObject h = resolve_displayobject(maybeArg);
			if (h.isValid()) fcopy(h);
			return true;
		};
		coreTable.set_function(name, [fcopy, resolve_displayobject](sol::this_state ts, sol::object /*self*/, sol::object maybeArg) {
			sol::state_view sv = ts;
			DisplayObject h = resolve_displayobject(maybeArg);
			if (h.isValid()) fcopy(h);
			return sol::make_object(sv, sol::nil);
		});
		try {
			lua[name] = [fcopy, resolve_displayobject](sol::this_state ts, sol::object maybeArg) {
				sol::state_view sv = ts;
				DisplayObject h = resolve_displayobject(maybeArg);
				if (h.isValid()) fcopy(h);
				return sol::make_object(sv, sol::nil);
			};
		} catch(...) {}
	}

	void bind_object_arg(const std::string& name, std::function<void(const sol::object&)> func,
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

	void bind_return_displayobject(const std::string& name, std::function<DisplayObject()> func,
								sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/) -> DisplayObject { return fcopy(); };
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/) {
			sol::state_view sv = ts;
			DisplayObject h = fcopy();
			if (h.isValid() && h.get()) return sol::make_object(sv, h);
			return sol::make_object(sv, DisplayObject());
		});
		try {
			lua[name] = [fcopy](sol::this_state ts) {
				sol::state_view sv = ts;
				DisplayObject h = fcopy();
				if (h.isValid() && h.get()) return sol::make_object(sv, h);
				return sol::make_object(sv, DisplayObject());
			};
		} catch(...) {}
	}

	void bind_return_bool(const std::string& name, std::function<bool()> func,
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

	void bind_return_string(const std::string& name, std::function<std::string()> func,
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

	void bind_return_float(const std::string& name, std::function<float()> func,
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

	void bind_return_int(const std::string& name, std::function<int()> func,
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

	void bind_return_vector_do(const std::string& name, std::function<std::vector<DisplayObject>()> func,
							sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/) -> std::vector<DisplayObject> { return fcopy(); };
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

	void bind_return_vector_string(const std::string& name, std::function<std::vector<std::string>()> func,
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

	void bind_string_table_return_do(const std::string& name, std::function<DisplayObject(const std::string&, const sol::table&)> func,
									sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/, const std::string& typeName, const sol::table& cfg) -> DisplayObject {
			return fcopy(typeName, cfg);
		};
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/, const std::string& typeName, const sol::table& cfg) {
			sol::state_view sv = ts;
			DisplayObject h = fcopy(typeName, cfg);
			if (h.isValid() && h.get()) return sol::make_object(sv, h);
			return sol::make_object(sv, DisplayObject());
		});
		try {
			lua[name] = [fcopy](sol::this_state ts, const std::string& typeName, const sol::table& cfg) {
				sol::state_view sv = ts;
				DisplayObject h = fcopy(typeName, cfg);
				if (h.isValid() && h.get()) return sol::make_object(sv, h);
				return sol::make_object(sv, DisplayObject());
			};
		} catch(...) {}
	}

	void bind_string_return_do(const std::string& name, std::function<DisplayObject(const std::string&)> func,
							sol::usertype<Core>& objHandleType, sol::table& coreTable, sol::state_view lua)
	{
		auto fcopy = func;
		objHandleType[name] = [fcopy](Core& /*core*/, const std::string& s) -> DisplayObject { return fcopy(s); };
		coreTable.set_function(name, [fcopy](sol::this_state ts, sol::object /*self*/, const std::string& s) {
			sol::state_view sv = ts;
			DisplayObject h = fcopy(s);
			if (h.isValid() && h.get()) return sol::make_object(sv, h);
			return sol::make_object(sv, DisplayObject());
		});
		try {
			lua[name] = [fcopy](sol::this_state ts, const std::string& s) {
				sol::state_view sv = ts;
				DisplayObject h = fcopy(s);
				if (h.isValid() && h.get()) return sol::make_object(sv, h);
				return sol::make_object(sv, DisplayObject());
			};
		} catch(...) {}
	}

	void bind_string_return_bool(const std::string& name, std::function<bool(const std::string&)> func,
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

	void bind_callback_bool(const std::string& name, std::function<void(std::function<bool()>)> registrar,
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

	void bind_callback_void(const std::string& name, std::function<void(std::function<void()>)> registrar,
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

	void bind_callback_update(const std::string& name, std::function<void(std::function<void(float)>)> registrar,
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

	void bind_callback_event(const std::string& name, std::function<void(std::function<void(const Event&)>)> registrar,
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

	void bind_callback_resize(const std::string& name, std::function<void(std::function<void(int,int)>)> registrar,
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

	void bind_string_function_forwarder(const std::string& name, std::function<void(const std::string&, const sol::function&)> func,
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

	void bind_name_or_handle(const std::string& name, std::function<void(const std::string&)> nameFunc, std::function<void(const DisplayObject&)> handleFunc,
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
			if (maybeArg.is<DisplayObject>()) {
				DisplayObject h = maybeArg.as<DisplayObject>();
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
				if (maybeArg.is<DisplayObject>()) { DisplayObject h = maybeArg.as<DisplayObject>(); hfc(h); return sol::make_object(sv, sol::nil); }
				if (maybeArg.is<sol::table>()) { sol::table t = maybeArg.as<sol::table>(); if (t["name"].valid()) { try { std::string nm = t.get<std::string>("name"); nfc(nm); return sol::make_object(sv, sol::nil); } catch(...) {} } }
				return sol::make_object(sv, sol::nil);
			};
		} catch(...) {}
	}
} // namespace SDOM