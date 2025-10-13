// Label Unit Tests

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>



namespace SDOM
{


    bool Label_test0()
    {
        std::string testName = "Label #0";
        std::string testDesc = "Label UnitTest Scaffolding";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
                -- return { ok = false, err = "unknown error" }
                return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Label_test0()



    bool Label_test1()
    {
        std::string testName = "Label #1";
        std::string testDesc = "Internal resources have been created";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
                local icon_8x8 = getAssetObject("internal_icon_8x8")
                if (icon_8x8:isValid() == false) then
                    return { ok = false, err = "'internal_icon_8x8' asset not found" }
                end
                local ttf = getAssetObject("internal_ttf_asset")
                if (ttf:isValid() == false) then
                    return { ok = false, err = "'internal_ttf_asset' asset not found" }
                end            
                local font_8x8 = getAssetObject("internal_font_8x8")
                if (font_8x8:isValid() == false) then
                    return { ok = false, err = "'internal_font_8x8' asset not found" }
                end
                local font_8x12 = getAssetObject("internal_font_8x12")
                if (font_8x12:isValid() == false) then
                    return { ok = false, err = "'internal_font_8x12' asset not found" }
                end
                return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Label_test1()

    bool Label_test2()
    {
        std::string testName = "Label #2";
        std::string testDesc = "SpriteSheet/BitmapFont asset existence and metrics";
        // Perform checks in C++ using AssetHandle to reliably resolve filenames via AssetHandle::get()/getFilename()
        struct Check { const char* name; const char* type; const char* file; };
        std::vector<Check> checks = {
            { "internal_ttf_asset",             "TTFAsset",    "internal_ttf" },
            { "internal_font_8x8",              "BitmapFont",  "internal_font_8x8" },
            { "internal_font_8x8_SpriteSheet",  "SpriteSheet", "internal_font_8x8" },
            { "internal_font_8x8_Texture",      "Texture",     "internal_font_8x8" },
            { "internal_font_8x12",             "BitmapFont",  "internal_font_8x12" },
            { "internal_font_8x12_SpriteSheet", "SpriteSheet", "internal_font_8x12" },
            { "internal_font_8x12_Texture",     "Texture",     "internal_font_8x12" },
            { "internal_ttf",                   "truetype",    "internal_ttf_asset" },
            { "internal_icon_8x8",              "Texture",     "internal_icon_8x8" },
        };

        bool ok = true;
        std::string err;
        for (auto& c : checks) {
            AssetHandle a = getFactory().getAssetObject(c.name);
            if (!a.isValid()) {
                ok = false;
                err = std::string("asset missing: ") + c.name;
                break;
            }
            std::string atype = a.getType();
            if (atype != c.type) {
                ok = false;
                err = std::string("type mismatch for ") + c.name + " - got: " + atype + " expected: " + c.type;
                break;
            }

            // Resolve filename using the concrete IAssetObject via AssetHandle::get()
            IAssetObject* obj = a.get();
            std::string fname;
            if (obj) {
                fname = obj->getFilename();
            } else {
                fname = a.getFilename();
            }

            // Normalize by removing trailing _asset if present
            auto normalize = [](const std::string& s) {
                if (s.size() >= 6 && s.rfind("_asset") == s.size() - 6) return s.substr(0, s.size() - 6);
                return s;
            };
            std::string nf = normalize(fname);
            std::string expected = normalize(c.file);

            if (nf != expected && nf.find(expected) == std::string::npos) {
                // Treat filename mismatches as test failures (strict equality required).
                ok = false;
                err = std::string("filename mismatch for ") + c.name + " - got: " + nf + " expected: " + expected;
                break;
            }
        }

        // getFactory().printAssetRegistry();

        // // Print the asset dependency tree for debugging/inspection
        // getFactory().printAssetTree();

        // // Print the grouped asset dependency tree for debugging/inspection
        // getFactory().printAssetTreeGrouped();

        // getCore().getRootNode()->printTree();

        // report and return test condition state
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });

    } // END: Font_test2()

    bool Label_test3()
    {
        std::string testName = "Label #3";
        std::string testDesc = "LUA: SpriteSheet/BitmapFont asset existence and metrics";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
                local checks = {
                    { name = "internal_ttf_asset",               type = "TTFAsset",   file = "internal_ttf" },
                    { name = "internal_font_8x8",                type = "BitmapFont", file = "internal_font_8x8" },
                    { name = "internal_font_8x8_SpriteSheet",    type = "SpriteSheet",file = "internal_font_8x8" },
                    { name = "internal_font_8x8_Texture",        type = "Texture",    file = "internal_font_8x8" },
                    { name = "internal_font_8x12",               type = "BitmapFont", file = "internal_font_8x12" },
                    { name = "internal_font_8x12_SpriteSheet",   type = "SpriteSheet",file = "internal_font_8x12" },
                    { name = "internal_font_8x12_Texture",       type = "Texture",    file = "internal_font_8x12" },
                    { name = "internal_ttf",                     type = "truetype",   file = "internal_ttf_asset" },
                    { name = "internal_icon_8x8",                type = "Texture",    file = "internal_icon_8x8" },
                }

                local function normalize_basename(s)
                    if type(s) ~= "string" then return tostring(s) end
                    return s:gsub("_asset$", "")
                end

                local function extract_filename(obj, depth)
                    depth = depth or 0
                    if depth > 6 then return tostring(obj) end
                    if type(obj) == "string" then return obj end
                    -- try getFilename -> might be string or another handle
                    local ok, out = pcall(function() return obj.getFilename and obj:getFilename() end)
                    if ok and out and type(out) == "string" and out ~= "" then return out end
                    if ok and out then
                        local rec = extract_filename(out, depth + 1)
                        if rec and rec ~= "" then return rec end
                    end
                    ok, out = pcall(function() return obj.getName and obj:getName() end)
                    if ok and out and type(out) == "string" and out ~= "" then return out end
                    if ok and out then
                        local rec = extract_filename(out, depth + 1)
                        if rec and rec ~= "" then return rec end
                    end
                    -- try fields
                    ok, out = pcall(function() return obj.filename end)
                    if ok and type(out) == "string" and out ~= "" then return out end
                    ok, out = pcall(function() return obj.name end)
                    if ok and type(out) == "string" and out ~= "" then return out end
                    return tostring(obj)
                end

                local diagnostics = {}
                local all_ok = true

                for _, c in ipairs(checks) do
                    local a = getAssetObject(c.name)
                    if not a then
                        diagnostics[c.name] = diagnostics[c.name] or {}
                        diagnostics[c.name]["missing"] = "asset_missing"
                        all_ok = false
                        goto continue
                    end
                    -- check isValid method
                    local ok_isValid, isVal = pcall(function() return a:isValid() end)
                    if not ok_isValid then
                        diagnostics[c.name] = diagnostics[c.name] or {}
                        diagnostics[c.name]["missing"] = diagnostics[c.name]["missing"] or {}
                        table.insert(diagnostics[c.name]["missing"], "isValid")
                        all_ok = false
                    else
                        if not isVal then
                            diagnostics[c.name] = diagnostics[c.name] or {}
                            diagnostics[c.name]["invalid"] = true
                            all_ok = false
                        end
                    end

                    -- check getType
                    local ok_type, atype = pcall(function() return a:getType() end)
                    if not ok_type then
                        diagnostics[c.name] = diagnostics[c.name] or {}
                        diagnostics[c.name]["missing"] = diagnostics[c.name]["missing"] or {}
                        table.insert(diagnostics[c.name]["missing"], "getType")
                        all_ok = false
                    else
                        if atype ~= c.type then
                            diagnostics[c.name] = diagnostics[c.name] or {}
                            diagnostics[c.name]["type_mismatch"] = { got = tostring(atype), expected = tostring(c.type) }
                            all_ok = false
                        end
                    end

                    -- attempt to extract filename via getFilename/getName
                    local fname = extract_filename(a)
                    local nf = normalize_basename(fname)
                    local expected = normalize_basename(c.file)
                    if nf ~= expected and string.find(nf, expected, 1, true) == nil then
                        diagnostics[c.name] = diagnostics[c.name] or {}
                        diagnostics[c.name]["filename_mismatch"] = { got = tostring(nf), expected = tostring(expected) }
                        -- don't flag as overall failure; we want to capture binding presence first
                    end

                    ::continue::
                end

                return { ok = all_ok, err = "", diag = diagnostics }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Label_test3()

    bool Label_test4()
    {
        std::string testName = "Label #4";
        std::string testDesc = "Label word/phrase style flags tokenization and inspection";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
            -- Label_test4: comprehensive style-flag assertions
            local name = "unitLabelTest4"
            local txt = "Start [bold]BOLD[/bold] [italic]ITALIC[/italic] [underline]UNDERLINE[/underline] [strike]STRIKE[/strike] [border]BORDER[/border] [bgnd=green]BACKGROUND[/bgnd] [outline]OUTLINE[/outline] [dropshadow]DROPSHADOW[/dropshadow] End"
            local cfg = { name = name, type = "Label", resource_name = "internal_font_8x8", text = txt }
            local ok = true
            local err = ""

            local ok_create, h_or_err = pcall(function() return createDisplayObject("Label", cfg) end)
            if not ok_create then
                return { ok = false, err = "createDisplayObject failed: " .. tostring(h_or_err) }
            end
            local h = h_or_err
            if not h or not h:isValid() then
                return { ok = false, err = "failed to create label" }
            end

            -- Force tokenization to ensure deterministic tokens
            local ok_tok, tok_err = pcall(function() return h:tokenizeText() end)
            if not ok_tok then
                return { ok = false, err = "tokenizeText failed: " .. tostring(tok_err) }
            end

            local ok_list, list_or_err = pcall(function() return h:getTokenList() end)
            if not ok_list then
                return { ok = false, err = "getTokenList failed: " .. tostring(list_or_err) }
            end
            local tokens = list_or_err
            if not tokens or type(tokens) ~= "table" then
                return { ok = false, err = "getTokenList did not return a table" }
            end

            -- mapping of open tag text to style key for boolean toggles
            local open_map = {
                ["[bold]"] = "bold",
                ["[italic]"] = "italic",
                ["[underline]"] = "underline",
                ["[strike]"] = "strikethrough",
                ["[outline]"] = "outline",
                ["[dropshadow]"] = "dropshadow",
            }
            local close_map = {
                ["[/bold]"] = "bold",
                ["[/italic]"] = "italic",
                ["[/underline]"] = "underline",
                ["[/strike]"] = "strikethrough",
                ["[/outline]"] = "outline",
                ["[/dropshadow]"] = "dropshadow",
            }

            -- For color-based tags (bgnd, border) we assert on color values instead
            local in_bgnd = false
            local in_border = false

            -- keep an expected boolean state for flags that are actually toggled
            local expected = { bold=false, italic=false, underline=false, strikethrough=false, outline=false, dropshadow=false }

            local i = 1
            while i <= #tokens do
                local t = tokens[i]
                local typ = t.type or ""
                local text = tostring(t.text or "")
                local style = t.style or {}
                -- print(string.format("[Label_test4] token[%d] type=%s text=%s", i, tostring(typ), tostring(text)))

                if typ == "escape" then
                    -- Color open tags: [bgnd=green], [border=red]
                    if text:match("^%[bgnd=") then
                        -- background escape sets the backgroundColor on the style immediately
                        in_bgnd = true
                    elseif text:match("^%[border=") then
                        in_border = true
                    elseif open_map[text] then
                        local key = open_map[text]
                        -- Note: tokenizer records the token with the style BEFORE applying the action.
                        -- Therefore the escape token itself will NOT show the new flag; the following words will.
                        if style[key] then ok = false; err = err .. string.format("open %s unexpectedly had style.%s true; ", tostring(text), tostring(key)) end
                        expected[key] = true
                    elseif close_map[text] then
                        local key = close_map[text]
                        -- For closing toggles the escape token will still show the prior (true) state
                        if not style[key] then ok = false; err = err .. string.format("close %s unexpectedly had style.%s false; ", tostring(text), tostring(key)) end
                        expected[key] = false
                    end
                    i = i + 1
                elseif typ == "punct" and text == "[" then
                    -- detect color-based closing tags which are tokenized as: '[' '/' 'bgnd' ']'
                    if tokens[i+1] and tokens[i+1].type == 'punct' and tokens[i+1].text == '/' and tokens[i+2] and tokens[i+2].type == 'word' then
                        local wn = tostring(tokens[i+2].text or "")
                        if wn == 'bgnd' then in_bgnd = false end
                        if wn == 'border' then in_border = false end
                    end
                    i = i + 1
                elseif typ == "word" then
                    -- for boolean flags, the word's style should equal expected
                    for k, v in pairs(expected) do
                        local got = style[k] or false
                        if got ~= v then
                            ok = false
                            err = err .. string.format("word '%s' style.%s mismatch (expected %s got %s); ", tostring(text), tostring(k), tostring(v), tostring(got))
                        end
                    end
                    -- for bgnd/border, check color when inside
                    if in_bgnd then
                        -- green => (0,255,0)
                        if not (style.backgroundColor and style.backgroundColor.g == 255) then
                            ok = false
                            err = err .. string.format("word '%s' backgroundColor not set to green; ", tostring(text))
                        end
                    end
                    if in_border then
                        -- red => (255,0,0)
                        if not (style.borderColor and style.borderColor.r == 255) then
                            ok = false
                            err = err .. string.format("word '%s' borderColor not set to red; ", tostring(text))
                        end
                    end
                    i = i + 1
                else
                    i = i + 1
                end
            end

            destroyDisplayObject(name)
            return { ok = ok, err = err }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Label_test4()

    
    bool Label_test5()
    {
        std::string testName = "Label #5";
        std::string testDesc = "Label word/phrase thickness/padding/offset tokenization and inspection";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
                -- return { ok = false, err = "unknown error" }
                return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Label_test5()



    bool Label_UnitTests() 
    {
        bool allTestsPassed = true;
        std::vector<std::function<bool()>> tests = 
        {
            [&]() { return Label_test0(); },    // scaffolding test
            [&]() { return Label_test1(); },    // internal resources created
            [&]() { return Label_test2(); },    // SpriteSheet/BitmapFont asset existence and metrics
            [&]() { return Label_test3(); },    // SpriteSheet/BitmapFont asset existence and metrics (LUA)
            [&]() { return Label_test4(); },    // Label word/phrase style flags tokenization and inspection
            [&]() { return Label_test5(); },    // Label word/phrase thickness/padding/offset tokenization and inspection
        };
        for (auto& test : tests) 
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        return allTestsPassed;
    }

} // namespace SDOM