-- luacheck: globals Core CLR Core_UnitTests Factory_UnitTests IDisplayObject_UnitTests Stage_UnitTests Box_UnitTests LUA_UnitTests DomHandle_UnitTests
---@diagnostic disable: undefined-global
-- The globals above are provided by the host (C++). Declaring them here
-- silences editor/linter "undefined global" warnings while keeping the
-- runtime behavior unchanged.


-- The C++ host calls `core.run("lua/config.lua")` (or `Core.run("lua/config.lua")`) which will
-- load this file inside the embedded Lua state. By design `Core::run(const sol::table&)`
-- calls `configureFromLua()` internally, so simply returning a `config` table from
-- this file is enough to configure the engine. You do NOT need to call
-- `Core:configure(config)` from Lua when the table is supplied to `core.run()` —
-- the existence of the returned table is all the host needs.
--
-- Notes / examples:
--   -- Called from C++ as a filename (examples/test working dir):
--   Core::getInstance().run("lua/config.lua");
--
--   -- From Lua, if you have a `config` table and want to run the engine from Lua:
--   -- Core.run(config)  -- will configure (via Core::run(table)) and then start the engine
--
--   -- If you prefer to configure without running immediately, call:
--   -- Core.configure(config)  -- this only applies the configuration and does not start the main loop
--
-- Returning the table below keeps the file usable both as a module (`require "lua/config"`)
-- and as a direct script loaded by the host.
local config = {
    Core = {
        windowWidth = 1200,
        windowHeight = 800,
        pixelWidth = 2,
        pixelHeight = 2,
        allowTextureResize = true,
        preserveAspectRatio = true,
        rendererLogicalPresentation = "SDL_LOGICAL_PRESENTATION_LETTERBOX",
        windowFlags = "SDL_WINDOW_RESIZABLE",
        pixelFormat = "SDL_PIXELFORMAT_RGBA8888",
        color = { r = 0, g = 0, b = 0, a = 255 },
        rootStage = "mainStage",
        children = {
            {
                rootStage = "mainStage",
                type = "Stage",
                name = "mainStage",
                color = { r = 32, g = 8, b = 4, a = 255 },
                children = {
                    {
                        type = "Box",
                        name = "redishBox",
                        x = 100.0,
                        y = 100,
                        width = 120,
                        height = 80,
                        color = { r = 200, g = 50, b = 50, a = 255 }
                    },
                    {
                        type = "Box",
                        name = "greenishBox",
                        x = 150,
                        y = 150,
                        width = 80,
                        height = 120,
                        color = { r = 50, g = 200, b = 50, a = 255 }
                    },
                    {
                        type = "Box",
                        name = "blueishBox",
                        x = 240,
                        y = 70,
                        width = 250,
                        height = 225,
                        color = { r = 50, g = 50, b = 200, a = 255 },
                        children = {
                            {
                                type = "Label",
                                name = "blueishBoxLabel",

                                anchorLeft = "left",
                                x = 245,
                                anchorTop = "top",
                                y = 75,
                                anchorRight = "right",
                                width = 240,
                                anchorBottom = "bottom",
                                height = 215,

                                text = "Hello, Label!",
                                resourceName = "default_bmp_font_8x8",
                                border = true,
                                borderColor = { r = 255, g = 255, b = 255, a = 64 },
                                fontSize = 8,
                                fontWidth = 8,
                                fontHeight = 8,
                                alignment = "middle_center",
                                foregroundColor = { r = 255, g = 255, b = 255, a = 255 },
                                maxWidth = 230
                            }
                        }
                    },
                    {
                        type = "Box",
                        name = "orangishBox",
                        x = 40,
                        y = 200,
                        width = 100,
                        height = 75,
                        color = { r = 200, g = 100, b = 50, a = 255 }
                    }
                }
            },
            {
                type = "Stage",
                name = "stageTwo",
                color = { r = 16, g = 32, b = 8, a = 255 }
            },
            {
                type = "Stage",
                name = "stageThree",
                color = { r = 8, g = 16, b = 32, a = 255 }
            }
        }
    }
}

-- The example binary runs with working directory `examples/test`, so use
-- project-relative paths from there (./lua/...). This avoids duplicating
-- the examples/test prefix when running from the example folder.
package.path = (package.path or "") .. ";./lua/?.lua;./lua/?/init.lua"

local callbacks = {}
local function require_callback(module_name)
    local ok, res = pcall(require, module_name)
    if not ok then
        error(string.format("Failed to require callback module '%s': %s", module_name, tostring(res)))
    end
    if type(res) ~= "table" then
        error(string.format("Callback module '%s' must return a table, got %s", module_name, type(res)))
    end
    return res
end

-- Load all callback modules. Each must return a table with the expected function(s).
callbacks.init = require_callback("callbacks.init")
callbacks.quit = require_callback("callbacks.quit")
callbacks.event = require_callback("callbacks.event")
callbacks.update = require_callback("callbacks.update")
callbacks.render = require_callback("callbacks.render")
callbacks.unittest = require_callback("callbacks.unittest")
callbacks.window_resize = require_callback("callbacks.window_resize")

-- Use the generic registerOn helper exposed by the C++ bindings.
-- This accepts the short name (Init, Update, Event, etc.) and a Lua function.
registerOn("Init", callbacks.init.on_init)
registerOn("Quit", callbacks.quit.on_quit)
registerOn("Event", callbacks.event.on_event)
registerOn("Update", callbacks.update.on_update)
registerOn("Render", callbacks.render.on_render)
registerOn("UnitTest", callbacks.unittest.on_unit_test)
registerOn("WindowResize", callbacks.window_resize.on_window_resize)

return config
