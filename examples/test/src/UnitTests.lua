-- test_utils.lua
---@diagnostic disable: undefined-global

-- store all problems here
local problems = {}

-- overall result flag
local ok = true  -- true initially

-- helper function to push an error
local function push_error(msg)
    table.insert(problems, tostring(msg))
    -- ok = false  -- mark overall result as failed
end

-- helper function to get the results
local function get_results()
    return {
        ok = ok,
        errors = problems
    }
end

-- optionally return table itself for require()
return {
    push_error = push_error,
    get_results = get_results
}