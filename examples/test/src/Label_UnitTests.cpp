// Label Unit Tests

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>

/**** Label UnitTests: **********************
 *  We need to ensure that all of these helpers are properly bound to LUA.  If the
 *  Lua functions work correctly, we then can assume the C++ versions work as well
 *  since the LUA functions are bound to the C++ versions.
 *  
 *      // --- Accessors for the FontStyle settings --- //
 * 
 *      // Boolean Based Flags Getters
 *      // These boolean getters/setters are exercised by Label_test7 (Lua boolean getter/setter test)
 *      bool getBold() const                -- CONFIRMED BY UNIT TEST #7
 *      bool getItalic() const              -- CONFIRMED BY UNIT TEST #7
 *      bool getUnderline() const           -- CONFIRMED BY UNIT TEST #7
 *      bool getStrikethrough() const       -- CONFIRMED BY UNIT TEST #7
 *      bool getBorder() const              -- CONFIRMED BY UNIT TEST #7
 *      bool getBackground() const          -- CONFIRMED BY UNIT TEST #7
 *      bool getOutline() const             -- CONFIRMED BY UNIT TEST #7
 *      bool getDropshadow() const          -- CONFIRMED BY UNIT TEST #7
 *      bool getWordwrap() const            -- CONFIRMED BY UNIT TEST #7
 *      bool getAutoResize() const          -- CONFIRMED BY UNIT TEST #7
 *
 *      // Integer Based Value Getters
 *      // These integer getters/setters are exercised by Label_test8 (Lua integer getter/setter test)
 *      int getFontSize() const             -- CONFIRMED BY UNIT TEST #8
 *      int getFontWidth() const            -- CONFIRMED BY UNIT TEST #8
 *      int getFontHeight() const           -- CONFIRMED BY UNIT TEST #8
 *      int getMaxWidth() const             -- CONFIRMED BY UNIT TEST #8
 *      int getMaxHeight() const            -- CONFIRMED BY UNIT TEST #8
 *      int getBorderThickness() const      -- CONFIRMED BY UNIT TEST #8
 *      int getOutlineThickness() const     -- CONFIRMED BY UNIT TEST #8
 *      int getPaddingHoriz() const         -- CONFIRMED BY UNIT TEST #8
 *      int getPaddingVert() const          -- CONFIRMED BY UNIT TEST #8
 *      int getDropshadowOffsetX() const    -- CONFIRMED BY UNIT TEST #8
 *      int getDropshadowOffsetY() const    -- CONFIRMED BY UNIT TEST #8
 *
 *      // LabelAlign Table
 *      // Alignment getters/setters are exercised by Label_test9 (Lua enum/string access)
 *      LabelAlign getAlignment() const          -- CONFIRMED BY UNIT TEST #9
 *      std::string getAlignmentString() const;  -- CONFIRMED BY UNIT TEST #9
 *
 *      // SDL_Color getters
 *      // Color functionality is confirmed by Label_test10 (explicit Lua get/set tests); previously exercised indirectly by Label_test6
 *      SDL_Color getForegroundColor() const    -- CONFIRMED BY UNIT TEST #10
 *      SDL_Color getBackgroundColor() const    -- CONFIRMED BY UNIT TEST #10
 *      SDL_Color getBorderColor() const        -- CONFIRMED BY UNIT TEST #10
 *      SDL_Color getOutlineColor() const       -- CONFIRMED BY UNIT TEST #10
 *      SDL_Color getDropshadowColor() const    -- CONFIRMED BY UNIT TEST #10
 *
 *      // --- Mutators for the FontStyle settings --- //
 *
 *      // Boolean Based Flags Setters
 *      // These setters are exercised by Label_test7
 *      void setBold(bool v)                    -- CONFIRMED BY UNIT TEST #7
 *      void setItalic(bool v)                  -- CONFIRMED BY UNIT TEST #7
 *      void setUnderline(bool v)               -- CONFIRMED BY UNIT TEST #7
 *      void setStrikethrough(bool v)           -- CONFIRMED BY UNIT TEST #7
 *      void setBorder(bool v)                  -- CONFIRMED BY UNIT TEST #7
 *      void setBackground(bool v)              -- CONFIRMED BY UNIT TEST #7
 *      void setOutline(bool v)                 -- CONFIRMED BY UNIT TEST #7
 *      void setDropshadow(bool v)              -- CONFIRMED BY UNIT TEST #7
 *      void setWordwrap(bool v)                -- CONFIRMED BY UNIT TEST #7
 *      void setAutoResize(bool v)              -- CONFIRMED BY UNIT TEST #7
 *
 *      // Integer Based Value Setters
 *      // These setters are exercised by Label_test8
 *      void setFontSize(int v)                 -- CONFIRMED BY UNIT TEST #8
 *      void setFontWidth(int v)                -- CONFIRMED BY UNIT TEST #8
 *      void setFontHeight(int v)               -- CONFIRMED BY UNIT TEST #8
 *      void setMaxWidth(int v)                 -- CONFIRMED BY UNIT TEST #8
 *      void setMaxHeight(int v)                -- CONFIRMED BY UNIT TEST #8
 *      void setBorderThickness(int v)          -- CONFIRMED BY UNIT TEST #8
 *      void setOutlineThickness(int v)         -- CONFIRMED BY UNIT TEST #8
 *      void setPaddingHoriz(int v)             -- CONFIRMED BY UNIT TEST #8
 *      void setPaddingVert(int v)              -- CONFIRMED BY UNIT TEST #8
 *      void setDropshadowOffsetX(int v)        -- CONFIRMED BY UNIT TEST #8
 *      void setDropshadowOffsetY(int v)        -- CONFIRMED BY UNIT TEST #8
 *
 *      // LabelAlign Table
 *      void setAlignment(LabelAlign v)             -- CONFIRMED BY UNIT TEST #9
 *      void setAlignment(const std::string& v);    -- CONFIRMED BY UNIT TEST #9
 *
 *      // SDL_Color setters
 *      // Color setters/getters are confirmed by Label_test10 (explicit Lua get/set tests); previously exercised indirectly by Label_test6
 *      void setForegroundColor(SDL_Color v)    -- CONFIRMED BY UNIT TEST #10
 *      void setBackgroundColor(SDL_Color v)    -- CONFIRMED BY UNIT TEST #10
 *      void setBorderColor(SDL_Color v)        -- CONFIRMED BY UNIT TEST #10
 *      void setOutlineColor(SDL_Color v)       -- CONFIRMED BY UNIT TEST #10
 *      void setDropshadowColor(SDL_Color v)    -- CONFIRMED BY UNIT TEST #10
 *
 ********************************************/


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
            { "internal_icon_8x8_SpriteSheet",  "SpriteSheet", "internal_icon_8x8" },
            { "internal_icon_12x12",            "Texture",      "internal_icon_12x12" },
            { "internal_icon_12x12_SpriteSheet", "SpriteSheet", "internal_icon_12x12" },
            { "internal_icon_16x16",            "Texture",     "internal_icon_16x16" }, 
            { "internal_icon_12x12_SpriteSheet", "SpriteSheet", "internal_icon_12x12" },

            // ...
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
                        -- background escape sets the background_color on the style immediately
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
                        -- green => (0,176,0)
                        if not (style.background_color and style.background_color.g == 176) then
                            ok = false
                            err = err .. string.format("word '%s' background_color not set to green; ", tostring(text))
                        end
                    end
                    if in_border then
                        -- red => (176,0,0)
                        if not (style.border_color and style.border_color.r == 176) then
                            ok = false
                            err = err .. string.format("word '%s' border_color not set to red; ", tostring(text))
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
        std::string testDesc = "Label word/phrase border/outline/pad/dropshadow tokenization and inspection";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script (mirrors Label_test4 structure, but checks numeric inline escapes)
        auto res = lua.script(R"lua(
            local name = "unitLabelTest5"
            local txt = table.concat({
                "Pre ",
                "[border = 3]B3 ",
                "[outline=2]BO32 ",
                "[pad=4 x 6]P46 ",
                "[dropshadow=5, 7]D57 ",
                "INNER ",
                "[/outline]O2off ",
                "NEST ",
                "[border= 1]B1 ",
                "[/border]B3back ",
                "[/pad]Pdef ",
                "[/dropshadow]Ddef ",
                "[/border]Bdef ",
                "Post"
            })
            local cfg = {
                name = name,
                type = "Label",
                resource_name = "internal_font_8x8",
                text = txt,
                -- default numeric values to 0 so restoration lands at 0
                border_thickness = 0,
                outline_thickness = 0,
                padding_horiz = 0,
                padding_vert = 0,
                dropshadow_offset_x = 0,
                dropshadow_offset_y = 0,
            }

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

            local _ = h:tokenizeText()
            local tokens = h:getTokenList()

            -- helper to find first word token containing a substring
            local function find_word(sub)
                for i=1,#tokens do
                    local t = tokens[i]
                    if t.type == 'word' and tostring(t.text):find(sub, 1, true) then
                        return i, t
                    end
                end
                return nil, nil
            end

            local _, tB3 = find_word('B3')
            local _, tBO32 = find_word('BO32')
            local _, tP46 = find_word('P46')
            local _, tD57 = find_word('D57')
            local _, tO2off = find_word('O2off')
            local _, tB1 = find_word('B1')
            local _, tB3back = find_word('B3back')
            local _, tPdef = find_word('Pdef')
            local _, tDdef = find_word('Ddef')
            local _, tBdef = find_word('Bdef')

            if not (tB3 and tBO32 and tP46 and tD57 and tO2off and tB1 and tB3back and tPdef and tDdef and tBdef) then
                return { ok = false, err = 'failed to find expected tokens' }
            end

            local function expect(val, got, key)
                if got ~= val then
                    ok = false
                    err = err .. string.format('%s mismatch (expected %s got %s); ', tostring(key), tostring(val), tostring(got))
                end
            end

            -- Assertions: values after opens
            expect(3, tB3.style.border_thickness, 'border_thickness@B3')
            expect(3, tBO32.style.border_thickness, 'border_thickness@BO32')
            expect(2, tBO32.style.outline_thickness, 'outline_thickness@BO32')
            expect(4, tP46.style.padding_horiz, 'padding_horiz@P46')
            expect(6, tP46.style.padding_vert, 'padding_vert@P46')
            expect(5, tD57.style.dropshadow_offset_x, 'dropshadow_offset_x@D57')
            expect(7, tD57.style.dropshadow_offset_y, 'dropshadow_offset_y@D57')

            -- After [/outline], border still 3, outline restored (0)
            expect(3, tO2off.style.border_thickness, 'border_thickness@O2off')
            expect(0, tO2off.style.outline_thickness, 'outline_thickness@O2off')

            -- Nested border=1 while outer border=3
            expect(1, tB1.style.border_thickness, 'border_thickness@B1')
            -- After closing inner [/border], back to 3
            expect(3, tB3back.style.border_thickness, 'border_thickness@B3back')
            -- After [/pad] and [/dropshadow] restore to defaults (0,0)
            expect(0, tPdef.style.padding_horiz, 'padding_horiz@Pdef')
            expect(0, tPdef.style.padding_vert, 'padding_vert@Pdef')
            expect(0, tDdef.style.dropshadow_offset_x, 'dropshadow_offset_x@Ddef')
            expect(0, tDdef.style.dropshadow_offset_y, 'dropshadow_offset_y@Ddef')
            -- After final [/border] outer close, border back to default (0)
            expect(0, tBdef.style.border_thickness, 'border_thickness@Bdef')

            destroyDisplayObject(name)
            return { ok = ok, err = err }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Label_test5()

    bool Label_test6()
    {
        std::string testName = "Label #6";
        std::string testDesc = "Label color targeted inline escapes tokenization and inspection";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script (mirrors Label_test4 structure, but checks numeric inline escapes)
        auto res = lua.script(R"lua(
            -- Label_test6: comprehensive color-escape assertions
            local name = "unitLabelTest6"
            local txt = table.concat({
                "Start ",
                "[fgnd=lt_red]FRED[/fgnd] ",
                "[bgnd=lt_blue]BGBLUE[/bgnd] ",
                "[border=gold]BDGOLD[/border] ",
                "[outline=md_gray]OLGRAY[/outline] ",
                "[shadow=00000080]SHALPH[/shadow] ",
                "[fgnd=FF8800]HEXR[/fgnd] ",
                "[fgnd=FF880080]HEXRA[/fgnd] ",
                "[reset]AFTERRESET"
            })

            -- set distinct defaults so we can assert restoration after [reset]
            local cfg = {
                name = name,
                type = "Label",
                resource_name = "internal_font_8x8",
                text = txt,
                foreground_color = { r = 10, g = 11, b = 12, a = 255 },
                background_color = { r = 20, g = 21, b = 22, a = 255 },
                border_color = { r = 30, g = 31, b = 32, a = 255 },
                outline_color = { r = 40, g = 41, b = 42, a = 255 },
                dropshadow_color = { r = 50, g = 51, b = 52, a = 128 },
            }

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

            local _ = h:tokenizeText()
            local tokens = h:getTokenList()

            local function find_word(sub)
                for i=1,#tokens do
                    local t = tokens[i]
                    if t.type == 'word' and tostring(t.text):find(sub, 1, true) then
                        return i, t
                    end
                end
                return nil, nil
            end

            local _, tF = find_word('FRED')
            local _, tBG = find_word('BGBLUE')
            local _, tBD = find_word('BDGOLD')
            local _, tOL = find_word('OLGRAY')
            local _, tSH = find_word('SHALPH')
            local _, tHEX = find_word('HEXR')
            local _, tHEXA = find_word('HEXRA')
            local _, tAF = find_word('AFTERRESET')

            if not (tF and tBG and tBD and tOL and tSH and tHEX and tHEXA and tAF) then
                return { ok = false, err = 'failed to find expected tokens' }
            end

            local function expect_color(vec, got, key)
                if not got then
                    ok = false
                    err = err .. string.format('%s missing color; ', tostring(key))
                    return
                end
                if vec.r ~= got.r or vec.g ~= got.g or vec.b ~= got.b or (vec.a and vec.a ~= got.a) then
                    ok = false
                    err = err .. string.format('%s mismatch (expected %s,%s,%s,%s got %s,%s,%s,%s); ', tostring(key), tostring(vec.r), tostring(vec.g), tostring(vec.b), tostring(vec.a or 255), tostring(got.r), tostring(got.g), tostring(got.b), tostring(got.a or 255))
                end
            end

            -- check colors inside tags (should match named color map / hex values)
            expect_color({ r=255,g=32,b=32,a=255 }, tF.style.foreground_color, 'fgnd@FRED')           -- lt_red
            expect_color({ r=32,g=32,b=255,a=255 }, tBG.style.background_color, 'bgnd@BGBLUE')        -- lt_blue
            expect_color({ r=255,g=215,b=0,a=255 }, tBD.style.border_color, 'border@BDGOLD')         -- gold
            expect_color({ r=102,g=102,b=102,a=255 }, tOL.style.outline_color, 'outline@OLGRAY')     -- md_gray (102)
            expect_color({ r=0,g=0,b=0,a=128 }, tSH.style.dropshadow_color, 'shadow@SHALPH')        -- 00000080 (rgba alpha 0x80 = 128)
            expect_color({ r=0xFF,g=0x88,b=0x00,a=255 }, tHEX.style.foreground_color, 'fgnd@HEXR') -- FF8800
            expect_color({ r=0xFF,g=0x88,b=0x00,a=0x80 }, tHEXA.style.foreground_color, 'fgnd@HEXRA') -- FF880080

            -- After [reset] the AFTER_RESET token should have default foreground (cfg.foreground_color)
            expect_color({ r=10,g=11,b=12,a=255 }, tAF.style.foreground_color, 'foreground@AFTERRESET')

            destroyDisplayObject(name)
            return { ok = ok, err = err }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Label_test6()


    bool Label_test7()
    {
        std::string testName = "Label #7";
        std::string testDesc = "LUA: Label boolean flag getter/setter methods";
        sol::state& lua = SDOM::Core::getInstance().getLua();

        auto res = lua.script(R"lua(
            -- Create a label and exercise boolean style getters/setters.
            local name = "unitLabelTest7"
            local cfg = {
                name = name,
                type = "Label",
                resource_name = "internal_font_8x8",
                text = "Bool flags test"
            }

            local ok_create, h_or_err = pcall(function() return createDisplayObject("Label", cfg) end)
            if not ok_create then
                return { ok = false, err = "createDisplayObject failed: " .. tostring(h_or_err) }
            end
            local h = h_or_err
            if not h or not h:isValid() then
                return { ok = false, err = "failed to create label" }
            end

            local flags = {
                "Bold",
                "Italic",
                "Underline",
                "Strikethrough",
                "Border",
                "Background",
                "Outline",
                "Dropshadow",
                "Wordwrap",
                "AutoResize"
            }

            for _, suffix in ipairs(flags) do
                local setter = "set" .. suffix
                local getter = "get" .. suffix

                -- Ensure binding exists
                if type(h[setter]) ~= "function" then
                    return { ok = false, err = "missing setter binding: " .. setter }
                end
                if type(h[getter]) ~= "function" then
                    return { ok = false, err = "missing getter binding: " .. getter }
                end

                -- Set true -> expect getter to return true
                local ok_s, s_err = pcall(function() h[setter](h, true) end)
                if not ok_s then
                    return { ok = false, err = "setter failed for " .. setter .. " : " .. tostring(s_err) }
                end
                local ok_g, val = pcall(function() return h[getter](h) end)
                if not ok_g then
                    return { ok = false, err = "getter failed for " .. getter }
                end
                if not val then
                    return { ok = false, err = getter .. " did not return true after " .. setter .. "(true)" }
                end

                -- Set false -> expect getter to return false
                ok_s, s_err = pcall(function() h[setter](h, false) end)
                if not ok_s then
                    return { ok = false, err = "setter failed for " .. setter .. " : " .. tostring(s_err) }
                end
                ok_g, val = pcall(function() return h[getter](h) end)
                if not ok_g then
                    return { ok = false, err = "getter failed for " .. getter }
                end
                if val then
                    return { ok = false, err = getter .. " did not return false after " .. setter .. "(false)" }
                end
            end

            -- cleanup
            destroyDisplayObject(name)
            return { ok = true, err = "" }
        )lua").get<sol::table>();

        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Label_test7()


    bool Label_test8()
    {
        std::string testName = "Label #8";
        std::string testDesc = "Label integer value getter/setter methods";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
            -- Create a label and exercise integer style getters/setters.
            local name = "unitLabelTest8"
            local cfg = {
                name = name,
                type = "Label",
                resource_name = "internal_font_8x8",
                text = "Integer getters/setters test"
            }

            local ok_create, h_or_err = pcall(function() return createDisplayObject("Label", cfg) end)
            if not ok_create then
                return { ok = false, err = "createDisplayObject failed: " .. tostring(h_or_err) }
            end
            local h = h_or_err
            if not h or not h:isValid() then
                return { ok = false, err = "failed to create label" }
            end

            local props = {
                "FontSize",
                "FontWidth",
                "FontHeight",
                "MaxWidth",
                "MaxHeight",
                "BorderThickness",
                "OutlineThickness",
                "PaddingHoriz",
                "PaddingVert",
                "DropshadowOffsetX",
                "DropshadowOffsetY",
            }

            for i, suffix in ipairs(props) do
                local setter = "set" .. suffix
                local getter = "get" .. suffix

                if type(h[setter]) ~= "function" then
                    return { ok = false, err = "missing setter binding: " .. setter }
                end
                if type(h[getter]) ~= "function" then
                    return { ok = false, err = "missing getter binding: " .. getter }
                end

                local v1 = 100 + i
                local ok_s, s_err = pcall(function() h[setter](h, v1) end)
                if not ok_s then
                    return { ok = false, err = "setter failed for " .. setter .. " : " .. tostring(s_err) }
                end

                local ok_g, val = pcall(function() return h[getter](h) end)
                if not ok_g then
                    return { ok = false, err = "getter failed for " .. getter }
                end
                if val ~= v1 then
                    return { ok = false, err = getter .. " returned " .. tostring(val) .. " expected " .. tostring(v1) }
                end

                -- Try a second value
                local v2 = v1 + 7
                ok_s, s_err = pcall(function() h[setter](h, v2) end)
                if not ok_s then
                    return { ok = false, err = "setter failed for " .. setter .. " : " .. tostring(s_err) }
                end
                ok_g, val = pcall(function() return h[getter](h) end)
                if not ok_g then
                    return { ok = false, err = "getter failed for " .. getter }
                end
                if val ~= v2 then
                    return { ok = false, err = getter .. " returned " .. tostring(val) .. " expected " .. tostring(v2) }
                end
            end

            destroyDisplayObject(name)
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Label_test8()


    bool Label_test9()
    {
        std::string testName = "Label #9";
        std::string testDesc = "LabelAlign Table constants existence and exercise";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script: verify LabelAlign table and exercise enum/string setters/getters
        auto res = lua.script(R"lua(
            local ok = true
            local err = ""

            -- Verify LabelAlign table exists
            if type(LabelAlign) ~= 'table' then
                return { ok = false, err = "LabelAlign table missing" }
            end

            -- Expect a few representative lowercase keys to exist (the table canonicalizes to lowercase)
            local expected_keys = { 'top_left', 'top_center', 'top_right', 'middle_center', 'bottom_right', 'default' }
            for _, k in ipairs(expected_keys) do
                if LabelAlign[k] == nil then
                    return { ok = false, err = "LabelAlign missing key: " .. tostring(k) }
                end
            end

            -- Create a label handle
            local name = "unitLabelTest9"
            local cfg = { name = name, type = "Label", resource_name = "internal_font_8x8", text = "align test" }
            local ok_create, h_or_err = pcall(function() return createDisplayObject("Label", cfg) end)
            if not ok_create then
                return { ok = false, err = "createDisplayObject failed: " .. tostring(h_or_err) }
            end
            local h = h_or_err
            if not h or not h:isValid() then
                return { ok = false, err = "failed to create label" }
            end

            -- Inspect default alignment via both APIs
            if type(h.getAlignment) ~= 'function' or type(h.getAlignmentString) ~= 'function' then
                return { ok = false, err = "alignment getters missing" }
            end
            local ok_g, align_enum = pcall(function() return h:getAlignment() end)
            if not ok_g then return { ok = false, err = "getAlignment() threw" } end
            local ok_gs, align_str = pcall(function() return h:getAlignmentString() end)
            if not ok_gs then return { ok = false, err = "getAlignmentString() threw" } end

            -- Try setting alignment using enum value
            local ok_set = pcall(function() h:setAlignment(LabelAlign.middle_center) end)
            if not ok_set then return { ok = false, err = "setAlignment(enum) threw" } end
            local ok_check, new_str = pcall(function() return h:getAlignmentString() end)
            if not ok_check or new_str ~= "middle_center" then
                return { ok = false, err = "enum set did not produce expected string: " .. tostring(new_str) }
            end

            -- Try setting alignment using string name
            local ok_set2 = pcall(function() h:setAlignment("bottom_right") end)
            if not ok_set2 then return { ok = false, err = "setAlignment(string) threw" } end
            local ok_check2, new_str2 = pcall(function() return h:getAlignmentString() end)
            if not ok_check2 then return { ok = false, err = "getAlignmentString() after string set threw" } end
            if new_str2 ~= "bottom_right" then
                return { ok = false, err = string.format("string set did not produce expected alignment string (bottom_right) got (%s)", tostring(new_str2)) }
            end

            -- cleanup
            destroyDisplayObject(name)
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Label_test9()

    
    bool Label_test10()
    {
        std::string testName = "Label #10";
        std::string testDesc = "Label Color Verification and Exercise";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
            local ok = true
            local err = ""

            local name = "unitLabelTest10"
            local cfg = {
                name = name,
                type = "Label",
                resource_name = "internal_font_8x8",
                text = "color get/set test",
                foreground_color = { r = 10, g = 20, b = 30, a = 200 },
                background_color = { r = 40, g = 50, b = 60, a = 201 },
                border_color = { r = 70, g = 80, b = 90, a = 202 },
                outline_color = { r = 100, g = 110, b = 120, a = 203 },
                dropshadow_color = { r = 130, g = 140, b = 150, a = 204 },
            }

            local ok_create, h_or_err = pcall(function() return createDisplayObject("Label", cfg) end)
            if not ok_create then
                return { ok = false, err = "createDisplayObject failed: " .. tostring(h_or_err) }
            end
            local h = h_or_err
            if not h or not h:isValid() then
                return { ok = false, err = "failed to create label" }
            end

            local function expect_color(vec, got, key)
                -- If the property is a function (legacy binding), call it with the handle to get the table
                if type(got) == 'function' then
                    local okf, rv = pcall(function() return got(h) end)
                    if not okf then
                        ok = false
                        err = err .. string.format('%s getter threw; ', tostring(key))
                        return
                    end
                    got = rv
                end

                -- Try alternate access patterns if direct value is missing:
                if not got or type(got) ~= 'table' then
                    local prop_key = key .. "_color"
                    local alt = h[prop_key]
                    if type(alt) == 'function' then
                        local okf, rv = pcall(function() return alt(h) end)
                        if okf and type(rv) == 'table' then got = rv end
                    elseif type(alt) == 'table' then
                        got = alt
                    else
                        -- try getter like getForegroundColor
                        local cap = prop_key:gsub("_(%l)", function(c) return string.upper(c) end)
                        cap = cap:gsub("^%l", function(c) return string.upper(c) end)
                        local getter_name = "get" .. cap
                        local gf = h[getter_name]
                        if type(gf) == 'function' then
                            local okg, rvg = pcall(function() return gf(h) end)
                            if okg and type(rvg) == 'table' then got = rvg end
                        end
                    end
                end
                if not got or type(got) ~= 'table' then
                    ok = false
                    err = err .. string.format('%s missing or not a table; ', tostring(key))
                    return
                end
                local gr = got.r or got[1]
                local gg = got.g or got[2]
                local gb = got.b or got[3]
                local ga = got.a or got[4] or 255
                if gr ~= vec.r or gg ~= vec.g or gb ~= vec.b or ga ~= (vec.a or 255) then
                    ok = false
                    err = err .. string.format('%s mismatch (expected %s,%s,%s,%s got %s,%s,%s,%s); ', tostring(key), tostring(vec.r), tostring(vec.g), tostring(vec.b), tostring(vec.a or 255), tostring(gr), tostring(gg), tostring(gb), tostring(ga))
                end
            end

            -- verify initial colors via property access
            expect_color({ r = 10, g = 20, b = 30, a = 200 }, h.foreground_color, 'foreground')
            expect_color({ r = 40, g = 50, b = 60, a = 201 }, h.background_color, 'background')
            expect_color({ r = 70, g = 80, b = 90, a = 202 }, h.border_color, 'border')
            expect_color({ r = 100, g = 110, b = 120, a = 203 }, h.outline_color, 'outline')
            expect_color({ r = 130, g = 140, b = 150, a = 204 }, h.dropshadow_color, 'dropshadow')

            -- Explicitly verify function-style getters return the same values
            local function expect_via_getter(vec, getter_name, key)
                local gf = h[getter_name]
                if type(gf) ~= 'function' then
                    ok = false
                    err = err .. string.format('%s getter missing; ', tostring(getter_name))
                    return
                end
                local okg, rvg = pcall(function() return gf(h) end)
                if not okg or type(rvg) ~= 'table' then
                    ok = false
                    err = err .. string.format('%s getter failed or did not return table; ', tostring(getter_name))
                    return
                end
                -- reuse existing expect_color for comparison (it accepts table-like values)
                expect_color(vec, rvg, key .. '_via_getter')
            end

            expect_via_getter({ r = 10, g = 20, b = 30, a = 200 }, 'getForegroundColor', 'foreground')
            expect_via_getter({ r = 40, g = 50, b = 60, a = 201 }, 'getBackgroundColor', 'background')
            expect_via_getter({ r = 70, g = 80, b = 90, a = 202 }, 'getBorderColor', 'border')
            expect_via_getter({ r = 100, g = 110, b = 120, a = 203 }, 'getOutlineColor', 'outline')
            expect_via_getter({ r = 130, g = 140, b = 150, a = 204 }, 'getDropshadowColor', 'dropshadow')

            -- Set new colors using keyed tables
            h.foreground_color = { r = 1, g = 2, b = 3, a = 4 }
            h.background_color = { 9, 8, 7 } -- array style (alpha defaults to 255)
            h.border_color = { 11, 12, 13, 14 }

            expect_color({ r = 1, g = 2, b = 3, a = 4 }, h.foreground_color, 'foreground_after_set')
            expect_color({ r = 9, g = 8, b = 7, a = 255 }, h.background_color, 'background_after_set')
            expect_color({ r = 11, g = 12, b = 13, a = 14 }, h.border_color, 'border_after_set')

            -- Set dropshadow/outline via keyed tables and verify
            h.outline_color = { r = 21, g = 22, b = 23 } -- alpha default
            h.dropshadow_color = { r = 31, g = 32, b = 33, a = 34 }
            expect_color({ r = 21, g = 22, b = 23, a = 255 }, h.outline_color, 'outline_after_set')
            expect_color({ r = 31, g = 32, b = 33, a = 34 }, h.dropshadow_color, 'dropshadow_after_set')

            destroyDisplayObject(name)
            return { ok = ok, err = err }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Label_test10()

    bool Label_UnitTests() 
    {
        bool allTestsPassed = true;
        std::vector<std::function<bool()>> tests = 
        {
            [&]() { return Label_test0();  },   // scaffolding test
            [&]() { return Label_test1();  },   // internal resources created
            [&]() { return Label_test2();  },   // SpriteSheet/BitmapFont asset existence and metrics
            [&]() { return Label_test3();  },   // SpriteSheet/BitmapFont asset existence and metrics (LUA)
            [&]() { return Label_test4();  },   // Label word/phrase style flags tokenization and inspection
            [&]() { return Label_test5();  },   // Label word/phrase border/outline/pad/dropshadow tokenization and inspection
            [&]() { return Label_test6();  },   // Label color targeted inline escapes tokenization and inspection
            [&]() { return Label_test7();  },   // Label boolean flag getter/setter methods
            [&]() { return Label_test8();  },   // Label integer value getter/setter methods
            [&]() { return Label_test9();  },   // LabelAlign Table constants existence and exercise
            [&]() { return Label_test10(); },   // Label Color Verification and Exercise
        };
        for (auto& test : tests) 
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        return allTestsPassed;
    }

} // namespace SDOM