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

-- -- create a TTF backing asset and a TruetypeFont wrapper (place this BEFORE configure(config))
-- createAsset("TTFAsset", 
-- {
--     name = "varela_ttf_asset",
--     type = "TTFAsset",
--     filename = "/home/jay/Documents/GitHub/SDOM/examples/test/assets/VarelaRound.ttf", -- absolute path
--     -- filename = "./assets/VarelaRound.ttf", -- relative path
--     internalFontSize = 12
-- })

-- -- create the TruetypeFont wrapper that Labels will reference
-- createAsset("truetype", {
--     name     = "VarelaRound",        -- resource name used by Labels
--     type = "truetype",
--     filename = "varela_ttf_asset",   -- points to the backing TTFAsset by name
--     fontSize = 12
-- })

-- -- Debug: verify assets exist in factory
-- print("has asset varela_ttf_asset:", hasAssetObject("varela_ttf_asset"))
-- print("has asset VarelaRound:", hasAssetObject("VarelaRound"))

local config = {
    resources = 
    {
        { name = "VarelaRound16", type = "TruetypeFont", filename = "/home/jay/Documents/GitHub/SDOM/examples/test/assets/VarelaRound.ttf", font_size = 16 },
        { name = "VarelaRound32", type = "TruetypeFont", filename = "/home/jay/Documents/GitHub/SDOM/examples/test/assets/VarelaRound.ttf", font_size = 32 },
        { name = "external_bmp_8x8", type = "BitmapFont", filename = "/home/jay/Documents/GitHub/SDOM/examples/test/assets/font_8x8.png", font_width = 8, font_height = 8 }
    },
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
                    color = { r = 128, g = 16, b = 16, a = 255 },

                    -- add children with a Label using the new TruetypeFont
                    children = {
                        {
                            type = "Label",
                            name = "redishBoxLabel",
                            anchor_left = "left",       
                            x = 105,                    
                            anchor_top = "top",         
                            y = 105,                     
                            anchor_right = "right",     
                            width = 110,                
                            anchor_bottom = "bottom",   
                            height = 70,               
                            outline = true,
                            wordwrap = false,
                            auto_resize = true,
                            text = "Hello VarelaRound",
                            resource_name = "VarelaRound32",   -- use the TruetypeFont created above
                            font_size = 32,
                            alignment = "center",
                            foreground_color = { r = 255, g = 255, b = 255, a = 255 },
                            border = true,
                            border_color = { r = 255, g = 255, b = 255, a = 64 }
                        }
                    }
                },
                {
                    type = "Box",
                    name = "greenishBox",
                    x = 150,
                    y = 150,
                    width = 80,
                    height = 120,
                    color = { r = 32, g = 128, b = 32, a = 255 },
                    -- add children with a Label using the new TruetypeFont
                    children = {
                        {
                            type = "Label",
                            name = "greenishBoxLabel",
                            anchor_left = "left",       
                            x = 155,                    
                            anchor_top = "top",         
                            y = 155,                     
                            anchor_right = "right",     
                            width = 70,                
                            anchor_bottom = "bottom",   
                            height = 110,               
                            outline = true,
                            wordwrap = true,
                            auto_resize = true,
                            text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat",
                            resource_name = "external_bmp_8x8",   -- use the BitmapFontcreated above
                            -- font_size = 8,
                            font_width = 8,
                            font_height = 8,
                            alignment = "center",
                            foreground_color = { r = 255, g = 255, b = 255, a = 255 },
                            border = true,
                            border_color = { r = 255, g = 255, b = 255, a = 64 }
                        }
                    }
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

                            anchor_left = "left",       -- verified
                            x = 245,                    -- verified (world coordinates)
                            -- left = 5,                -- not working because the Label does not have a parent
                            anchor_top = "top",         -- verified
                            y = 75,                     -- verified (world coordinates)
                            -- top = 5,                 -- not working because the Label does not have a parent  
                            anchor_right = "right",     -- verified
                            width = 240,                -- verified (right - left = 240)
                            -- right = -5,              -- not working because the Label does not have a parent
                            anchor_bottom = "bottom",   -- verified
                            height = 215,               -- verified (bottom - top = 215)
                            -- bottom = -5,             -- not working because the Label does not have a parent

                            -- isClickable = true,

                            wordwrap = true,            -- verified
                            auto_resize = false,        -- 

                            outline = true,             -- verified
                            -- outline_thickness = 1,   -- verified

                            text = "The [bold][color=red]quick brown[color=white][/bold] fox jumps over the lazy dog by the river, sunlight catching on ripples as children laugh in the distance. A small breeze stirs the pages of an open book left on the bench, and a bell rings from the town square.",
                            -- resource_name = "default_bmp_8x8",                      -- verified
                            resource_name = "default_ttf",  -- internal ttf asset name
                            border = true,                                          -- verified
                            border_color = { r = 255, g = 255, b = 255, a = 64 },   -- verified

                            -- Note: `font_width` and `font_height` only affect BitmapFont rendering.
                            -- For BitmapFonts, these properties allow per-axis stretching of glyphs.
                            -- For TrueType fonts, only `font_size` is used; `font_width` and `font_height` are ignored.
                            -- If you need non-uniform scaling for TrueType fonts, use a bitmap font or create a custom asset.   
                            
                            font_size = 12,                                          -- verified

                            font_width = 8,         -- verified working now
                            font_height = 12,        -- verified working now
                            alignment = "center",       -- verified, leading and trailing spaces are now properly trimmed
                            foreground_color = { r = 255, g = 255, b = 255, a = 255 },  -- verified
                            max_width = 500             -- verified
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
} -- Closing brace for the config table

-- Configure the test application
configure(config)

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
