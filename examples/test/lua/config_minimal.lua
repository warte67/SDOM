-- Minimal config for headless unit-test runs
local config = {
    windowWidth = 64,
    windowHeight = 64,
    pixelWidth = 1,
    pixelHeight = 1,
    rootStage = "mainStage",
    children = {
        { name = "mainStage", type = "Stage", children = {} }
    }
}

-- Apply configuration and return the table for Core::run
configure(config)
return config
