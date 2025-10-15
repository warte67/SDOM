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
        { name = "external_font_8x8", type = "BitmapFont", filename = "/home/jay/Documents/GitHub/SDOM/examples/test/assets/font_8x8.png", font_width = 8, font_height = 8 },
        { name = "external_font_8x12", type = "BitmapFont", filename = "/home/jay/Documents/GitHub/SDOM/examples/test/assets/font_8x12.png", font_width = 8, font_height = 12 },
        { name = "external_icon_8x8", type = "SpriteSheet", filename = "/home/jay/Documents/GitHub/SDOM/examples/test/assets/icon_8x8.png", sprite_width = 8, sprite_height = 8 }
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
                    name = "rightMainFrame",
                    type = "Frame",
                    x = 300,
                    y = 5,
                    width = 295,
                    height = 390,
                    -- icon_resource = "external_icon_8x8", -- test icon resource name resolution
                    color = { r = 64, g = 32, b = 32, a = 255 },
                    children = {
                        {
                            name = "mainFrameGroup",
                            type = "Group",
                            x = 450,
                            y = 10,
                            width = 140,
                            height = 70,
                            text = "Main Group",

                            -- font_resource = "external_font_8x12",
                            -- font_resource = "VarelaRound16",
                            font_resource = "internal_ttf",
                            font_size = 12,  -- these should be default values from the externally loaded BitmapFont asset
                            -- font_width = 12,
                            -- font_height = 12, -- if not specified, should use the backing SpriteSheet's spriteHeight

                            color = { r = 255, g = 255, b = 255, a = 96 },
                            label_color = { r = 224, g = 192, b = 192, a = 255 },
                            children = {
                                {
                                    name = "mainFrameGroup_radiobox_1",
                                    type = "Radiobox",
                                    x = 460,
                                    y = 24,
                                    width = 120,    -- w: should work but doesnt
                                    height = 16,    -- h: should work but doesnt
                                    color = { r = 255, g = 255, b = 255, a = 255 },
                                    label_color = { r = 255, g = 255, b = 255, a = 255 },
                                    font_resource = "internal_ttf",
                                    font_size = 10,
                                    text = "First",
                                    -- border = true
                                },
                                {
                                    name = "mainFrameGroup_radiobox_2",
                                    type = "Radiobox",
                                    x = 460,
                                    y = 40,
                                    width = 120,    -- w: should work but doesnt
                                    height = 16,    -- h: should work but doesnt
                                    color = { r = 255, g = 255, b = 255, a = 255 },
                                    label_color = { r = 255, g = 255, b = 255, a = 255 },
                                    font_resource = "VarelaRound16",
                                    font_size = 10,
                                    text = "Second",
                                    checked = true;
                                    -- border = true
                                },
                                {
                                    name = "mainFrameGroup_radiobox_3",
                                    type = "Radiobox",
                                    x = 460,
                                    y = 56,
                                    width = 120,    -- w: should work but doesnt
                                    height = 16,    -- h: should work but doesnt
                                    color = { r = 255, g = 255, b = 255, a = 255 },
                                    label_color = { r = 255, g = 255, b = 255, a = 255 },
                                    text = "Third",
                                    -- border = true
                                }                                   
                            }
                        },
                        {
                            name = "mainFrame_checkbox_1",
                            type = "Checkbox",
                            x = 305,
                            y = 10,
                            width = 145,    -- w: should work but doesnt
                            height = 16,    -- h: should work but doesnt
                            color = { r = 255, g = 255, b = 255, a = 255 },
                            label_color = { r = 255, g = 255, b = 255, a = 255 },
                            font_resource = "internal_ttf",
                            font_size = 10,
                            text = "internal_ttf Checkbox",
                            -- border = true
                        },
                        {
                            name = "mainFrame_checkbox_2",
                            type = "Checkbox",
                            x = 305,
                            y = 26,
                            width = 145,    -- w: should work but doesnt
                            height = 16,    -- h: should work but doesnt
                            color = { r = 255, g = 255, b = 255, a = 255 },
                            label_color = { r = 255, g = 255, b = 255, a = 255 },
                            font_resource = "VarelaRound16",
                            font_size = 10,
                            text = "VarelaRound16 Checkbox",
                            -- border = true
                        },
                        {
                            name = "mainFrame_checkbox_3",
                            type = "Checkbox",
                            x = 305,
                            y = 42,
                            width = 145,    -- w: should work but doesnt
                            height = 16,    -- h: should work but doesnt
                            color = { r = 255, g = 255, b = 255, a = 255 },
                            label_color = { r = 255, g = 255, b = 255, a = 255 },
                            text = "font_8x8 Checkbox",
                            -- border = true
                        },                             
                        {
                            name = "mainFrame_radiobox_1",
                            type = "Radiobox",
                            x = 305,
                            y = 56,
                            width = 145,    -- w: should work but doesnt
                            height = 16,    -- h: should work but doesnt
                            color = { r = 255, g = 255, b = 255, a = 255 },
                            label_color = { r = 255, g = 255, b = 255, a = 255 },
                            font_resource = "internal_ttf",
                            font_size = 10,
                            text = "internal_ttf Radiobox",
                            selected = true;
                            -- border = true
                        },
                        {
                            name = "mainFrame_radiobox_2",
                            type = "Radiobox",
                            x = 305,
                            y = 72,
                            width = 145,    -- w: should work but doesnt
                            height = 16,    -- h: should work but doesnt
                            color = { r = 255, g = 255, b = 255, a = 255 },
                            label_color = { r = 255, g = 255, b = 255, a = 255 },
                            font_resource = "VarelaRound16",
                            font_size = 10,
                            text = "VarelaRound16 Radiobox",
                            -- border = true
                        },
                        {
                            name = "mainFrame_radiobox_3",
                            type = "Radiobox",
                            x = 305,
                            y = 88,
                            width = 145,    -- w: should work but doesnt
                            height = 16,    -- h: should work but doesnt
                            color = { r = 255, g = 255, b = 255, a = 255 },
                            label_color = { r = 255, g = 255, b = 255, a = 255 },
                            text = "font_8x8 Radiobox",
                            -- border = true
                        },
                        {
                            name = "mainFrame_tristate_1",
                            -- type = "TriStateCheckbox",
                            type = "TristateButton",
                            x = 305,
                            y = 104,
                            width = 145,    -- w: should work but doesnt
                            height = 16,    -- h: should work but doesnt
                            color = { r = 255, g = 255, b = 255, a = 255 },
                            label_color = { r = 255, g = 255, b = 255, a = 255 },
                            font_resource = "VarelaRound16",
                            font_size = 10,
                            text = "VarelaRound16 TriStateCheckbox",
                            -- border = true
                        },                        
                    }
                },
                {
                    name = "main_stage_button",
                    type = "Button",
                    x = 5,
                    y = 370,
                    width = 150,
                    height = 25,
                    text = "[color=white]Go to [color=yellow]S[color=white]tage Two",
                    font_resource = "VarelaRound16", -- test font resource name resolution
                    font_size = 16,
                    -- icon_resource = "external_icon_8x8", -- test icon resource name resolution
                    label_color = { r = 255, g = 255, b = 255, a = 255 },
                    color = { r = 96, g = 32, b = 16, a = 255 }                    
                },
                {
                    name = "main_stage_hamburger_iconbutton",
                    type = "IconButton",
                    x = 150,
                    y = 8,
                    width = 8;
                    height = 8;
                    color = { r = 255, g = 255, b = 255, a = 255 },
                    icon_resource = "internal_icon_8x8", -- test icon resource name resolution
                    -- icon_index = 54 -- this works
                    -- icon_index = "hamburger" -- this works
                    icon_index = IconIndex.Hamburger -- use dot form for numeric constant
                },               
                {
                    name = "redishBox",
                    type = "Box",
                    x = 100.0,
                    y = 100,
                    width = 120,
                    height = 80,
                    color = { r = 128, g = 16, b = 16, a = 255 },

                    -- add children with a Label using the new TruetypeFont
                    children = {
                        {
                            name = "redishBoxLabel",
                            type = "Label",
                            anchor_left = "left",       
                            x = 105,                    
                            anchor_top = "top",         
                            y = 105,                     
                            anchor_right = "right",     
                            width = 110,                
                            anchor_bottom = "bottom",   
                            height = 70,               
                            outline = true,
                            wordwrap = true,
                            auto_resize = true,
                            text = "[color=white]Hello [bold][dropshadow]VarelaRound[/dropshadow][/bold], this is a 32 point font size.",
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
                    name = "greenishBox",
                    type = "Box",
                    x = 150,
                    y = 150,
                    width = 180,
                    height = 120,
                    color = { r = 16, g = 64, b = 16, a = 255 },
                    -- add children with a Label using the new TruetypeFont
                    children = {
                        {
                            name = "greenishBoxLabel",
                            type = "Label",
                            anchor_left = "left",       
                            x = 155,                    
                            anchor_top = "top",         
                            y = 155,                     
                            -- anchor_right = "left",     
                            anchor_right = "right",     
                            width = 170,                
                            -- anchor_bottom = "top",   
                            anchor_bottom = "bottom",   
                            height = 110,               
                            outline = true,
                            wordwrap = true,
                            -- auto_resize = true,
                            auto_resize = false,
                            max_width = 400,
                            text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat",
                            resource_name = "internal_font_8x8",   -- use an internally created BitmapFont
                            -- resource_name = "external_font_8x8",   -- use the BitmapFont created above
                            -- font_size = 8,
                            -- font_width = 8,  -- if not specified, will use the backing SpriteSheet's spriteWidth
                            -- font_height = 8, -- if not specified, will use the backing SpriteSheet's spriteHeight
                            alignment = "center",
                            foreground_color = { r = 255, g = 255, b = 255, a = 255 },
                            border = true,
                            border_color = { r = 255, g = 255, b = 255, a = 64 }
                        }
                    }
                },
                {
                    name = "blueishBox",
                    type = "Box",
                    x = 180,
                    y = 70,
                    width = 250,
                    height = 225,
                    color = { r = 50, g = 50, b = 200, a = 255 },
                    children = {
                        {
                            name = "blueishBoxLabel",
                            type = "Label",

                            anchor_left = "left",       -- verified
                            x = 185,                    -- verified (world coordinates)
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

                            text = "The [bold][color=lt_red]quick brown[color=white][/bold] fox jumps over the lazy dog by the river, sunlight catching on ripples as children laugh in the distance. A small breeze stirs the pages of an open book left on the bench, and a bell rings from the town square.",
                            -- resource_name = "internal_font_8x8",
                            resource_name = "internal_ttf",  -- internal ttf asset name
                            border = true,
                            border_color = { r = 255, g = 255, b = 255, a = 64 },

                            -- Note: `font_width` and `font_height` only affect BitmapFont rendering.
                            -- For BitmapFonts, these properties allow per-axis stretching of glyphs.
                            -- For TrueType fonts, only `font_size` is used; `font_width` and `font_height` are ignored.
                            -- If you need non-uniform scaling for TrueType fonts, use a bitmap font or create a custom asset.   
                            
                            font_size = 16,
                            -- font_width = 8,
                            -- font_height = 12,
                            alignment = "center",       -- verified, leading and trailing spaces are now properly trimmed
                            foreground_color = { r = 255, g = 255, b = 255, a = 255 },  -- verified
                            max_width = 500             -- verified
                        }
                    }
                },
                {
                    name = "orangishBox",
                    type = "Box",
                    x = 40,
                    y = 200,
                    width = 100,
                    height = 75,
                    color = { r = 128, g = 64, b = 25, a = 255 },
                    -- add children with a Label using the new TruetypeFont
                    children = 
                    {
                        {
                            name = "orangeishBoxLabel",
                            type = "Label",
                            anchor_left = "left",       
                            x = 45,                    
                            anchor_top = "top",         
                            y = 205,                     
                            -- anchor_right = "left",     
                            anchor_right = "right",     
                            width = 90,                
                            -- anchor_bottom = "top",   
                            anchor_bottom = "bottom",   
                            height = 65,               
                            outline = true,
                            wordwrap = true,
                            auto_resize = false,
                            -- auto_resize = true,
                            max_width = 400,
                            text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat",
                            resource_name = "internal_font_8x12",   -- use an internally created BitmapFont
                            -- resource_name = "external_font_8x12",   -- use the BitmapFont created above
                            -- font_size = 12,
                            -- font_width = 8,      -- if not specified, will use the backing SpriteSheet's spriteWidth
                            -- font_height = 12,    -- if not specified, will use the backing SpriteSheet's spriteHeight
                            alignment = "center",
                            foreground_color = { r = 255, g = 255, b = 255, a = 255 },
                            border = true,
                            border_color = { r = 255, g = 255, b = 255, a = 64 }
                        }
                    }
                }
            }
        },
        {
            name = "stageTwo",
            type = "Stage",
            color = { r = 16, g = 32, b = 8, a = 255 },
            children = {
                {
                    name = "rightStage2Frame",
                    type = "Frame",
                    x = 300,
                    y = 5,
                    width = 295,
                    height = 390,
                    -- icon_resource = "external_icon_8x8", -- test icon resource name resolution
                    color = { r = 32, g = 64, b = 32, a = 255 }
                },
                {
                    name = "stage2_button",
                    type = "Button",
                    x = 5, y = 370, width = 150, height = 25,
                    text = "[color=white]Go to [color=yellow]S[color=white]tage Three",
                    font_resource = "VarelaRound16",
                    font_size = 16,
                    color = { r = 32, g = 96, b = 16, a = 255 }
                }
            }
        },
        {
            name = "stageThree",
            type = "Stage",
            color = { r = 8, g = 16, b = 32, a = 255 },
            children = {
                {
                    name = "rightStage3Frame",
                    type = "Frame",
                    x = 300,
                    y = 5,
                    width = 295,
                    height = 390,
                    -- icon_resource = "external_icon_8x8", -- test icon resource name resolution
                    color = { r = 32, g = 32, b = 64, a = 255 }
                }, 
                {
                    name = "stage3_button",
                    type = "Button",
                    x = 5, y = 370, width = 150, height = 25,
                    text = "[color=white]Go to Main [color=yellow]S[color=white]tage",
                    font_resource = "VarelaRound16",
                    font_size = 16,
                    color = { r = 16, g = 32, b = 96, a = 255 }
                }
            }
        }
    },
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


-- Add an event listener that switches the root stage when the main button is activated.
local function on_main_stage_button_click(evnt)
    -- print("Button clicked! Changing root stage to 'stageTwo'")
    setRoot("stageTwo") -- switch to stageTwo when button is clicked
end
local function on_stage2_button_click(evnt)
    -- print("Button clicked! Changing 'stageTwo' stage to 'stageThree'")
    setRoot("stageThree") -- switch to stageThree when button is clicked
end
local function on_stage3_button_click(evnt)
    -- print("Button clicked! Changing 'stageThree' stage to 'mainStage'")
    setRoot("mainStage") -- switch to mainStage when button is clicked
end

-- Get the button display objects by name
local btnObj_1 = getDisplayObject("main_stage_button")
local btnObj_2 = getDisplayObject("stage2_button")
local btnObj_3 = getDisplayObject("stage3_button")

-- use named fields (type, listener)
btnObj_1:addEventListener({ type = EventType.MouseClick, listener = on_main_stage_button_click })
btnObj_2:addEventListener({ type = EventType.MouseClick, listener = on_stage2_button_click })
btnObj_3:addEventListener({ type = EventType.MouseClick, listener = on_stage3_button_click })

-- At this point we want runtime errors if this doesnt go right, so no pcall here.
-- -- safe listener registration (checks existence and avoids runtime errors)
-- if btnObj_1 then
--     pcall(function()
--         btnObj_1:addEventListener({ type = EventType.MouseClick, listener = on_main_stage_button_click })
--     end)
-- end
-- if btnObj_2 then
--     pcall(function()
--         btnObj_2:addEventListener({ type = EventType.MouseClick, listener = on_stage2_button_click })
--     end)
-- end
-- if btnObj_3 then
--     pcall(function()
--         btnObj_3:addEventListener({ type = EventType.MouseClick, listener = on_stage3_button_click })
--     end)
-- end

return config
