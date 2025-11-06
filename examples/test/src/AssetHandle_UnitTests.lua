-- AssetHandle_UnitTests.lua
---@diagnostic disable: undefined-global

local utils = require("src.UnitTests")
local function push(msg) utils.push_error(msg) end
local function assert_true(cond, msg) if not cond then push(msg) end end

-- Convenience: fetch an asset and assert basic invariants
local function expect_asset(name, expected_type)
    if not Core:hasAssetObject(name) then
        push("Missing asset: '" .. name .. "'")
        return nil
    end
    local h = Core:getAssetObject(name)
    if not (h and (h.isValid == nil or h:isValid())) then
        push("Invalid handle for asset: '" .. name .. "'")
        return nil
    end
    if expected_type and h:getType() ~= expected_type then
        push(string.format("Type mismatch for '%s': got '%s' expected '%s'",
            name, tostring(h:getType()), tostring(expected_type)))
    end
    return h
end

local function run_all()
    -- Internal textures
    expect_asset("internal_icon_8x8",  "Texture")
    expect_asset("internal_icon_12x12", "Texture")
    expect_asset("internal_icon_16x16", "Texture")

    -- SpriteSheet wrappers with correct tile size
    do
        local ss8 = expect_asset("internal_icon_8x8_SpriteSheet", "SpriteSheet")
        if ss8 then
            assert_true(ss8:getSpriteWidth() == 8 and ss8:getSpriteHeight() == 8,
                "SpriteSheet size mismatch for internal_icon_8x8_SpriteSheet")
        end
        local ss12 = expect_asset("internal_icon_12x12_SpriteSheet", "SpriteSheet")
        if ss12 then
            assert_true(ss12:getSpriteWidth() == 12 and ss12:getSpriteHeight() == 12,
                "SpriteSheet size mismatch for internal_icon_12x12_SpriteSheet")
        end
        local ss16 = expect_asset("internal_icon_16x16_SpriteSheet", "SpriteSheet")
        if ss16 then
            assert_true(ss16:getSpriteWidth() == 16 and ss16:getSpriteHeight() == 16,
                "SpriteSheet size mismatch for internal_icon_16x16_SpriteSheet")
        end
    end

    -- Default bitmap fonts and their SpriteSheets (Factory creates with type "BitmapFont")
    expect_asset("internal_font_8x8",  "BitmapFont")
    expect_asset("internal_font_8x12", "BitmapFont")
    do
        local s88 = expect_asset("internal_font_8x8_SpriteSheet", "SpriteSheet")
        if s88 then
            assert_true(s88:getSpriteWidth() == 8 and s88:getSpriteHeight() == 8,
                "SpriteSheet size mismatch for internal_font_8x8_SpriteSheet")
        end
        local s8x12 = expect_asset("internal_font_8x12_SpriteSheet", "SpriteSheet")
        if s8x12 then
            assert_true(s8x12:getSpriteWidth() == 8 and s8x12:getSpriteHeight() == 12,
                "SpriteSheet size mismatch for internal_font_8x12_SpriteSheet")
        end
    end

    -- Truetype chain present (if SDL_ttf active). We only assert presence/type.
    expect_asset("internal_ttf_asset", "TTFAsset")
    do
        local tf = Core:getAssetObject("internal_ttf")
        if tf and (tf.isValid == nil or tf:isValid()) then
            local ty = tf:getType()
            if not (ty == "truetype" or ty == "TruetypeFont") then
                push("Type mismatch for 'internal_ttf': got '" .. tostring(ty) .. "'")
            end
        end
    end
end

-- Re-entrant wrapper
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
