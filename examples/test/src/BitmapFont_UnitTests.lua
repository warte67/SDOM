-- BitmapFont_UnitTests.lua
---@diagnostic disable: undefined-global

local utils = require("src.UnitTests")
local function push(msg) utils.push_error(msg) end
local function assert_true(cond, msg) if not cond then push(msg) end end

local function run_all()
    -- Use default internal bitmap font created by Factory::onInit()
    local h = Core:getAssetObject("internal_font_8x8")
    assert_true(h and (h.isValid == nil or h:isValid()), "Missing or invalid handle: internal_font_8x8")
    if not (h and (h.isValid == nil or h:isValid())) then return end

    assert_true(h:getType() == "BitmapFont", "Type mismatch for internal_font_8x8")

    -- Initial metrics
    local w0 = h:getBitmapFontWidth()
    local h0 = h:getBitmapFontHeight()
    assert_true(w0 == 8, "Expected initial BitmapFont width 8, got " .. tostring(w0))
    assert_true(h0 == 8, "Expected initial BitmapFont height 8, got " .. tostring(h0))

    -- Underlying resource handle and SpriteSheet tile size
    local rs = h:getResourceHandle()
    assert_true(rs and (rs.isValid == nil or rs:isValid()), "getResourceHandle returned invalid handle")
    if rs and (rs.isValid == nil or rs:isValid()) then
        assert_true(rs:getType() == "SpriteSheet", "Resource type mismatch for internal_font_8x8_SpriteSheet")
        assert_true(rs:getSpriteWidth() == 8 and rs:getSpriteHeight() == 8,
            "SpriteSheet tile size mismatch for internal_font_8x8_SpriteSheet")
    end

    -- BitmapFont width/height are publicly immutable; they should match SpriteSheet tile size
    if rs and (rs.isValid == nil or rs:isValid()) then
        assert_true(h:getBitmapFontWidth() == rs:getSpriteWidth(),  "BitmapFont width != SpriteSheet tile width")
        assert_true(h:getBitmapFontHeight() == rs:getSpriteHeight(), "BitmapFont height != SpriteSheet tile height")
    end
end

local M = { _done = false }
function M.step()
    if M._done then return true end
    local ok, err = pcall(run_all)
    if not ok then push("Lua test runner threw: " .. tostring(err)) end
    M._done = true
    return true
end

local results = utils.get_results()
results.step = M.step
return results
