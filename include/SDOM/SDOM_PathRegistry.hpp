#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace SDOM {

enum class PathType {
    Fonts,
    Images,
    Lua,
    Themes,
    EngineConfig,
    VersionConfig,
    CacheDir,
    UserSettings
};

class PathRegistry {
public:
    static PathRegistry& get();

    bool loadFromJson(const std::string& filename);

    std::string resolve(const std::string& name, PathType type) const;

    const std::vector<std::string>& fontPaths() const { return fontPaths_; }
    const std::vector<std::string>& imagePaths() const { return imagePaths_; }
    const std::vector<std::string>& luaPaths() const { return luaPaths_; }
    const std::vector<std::string>& themePaths() const { return themePaths_; }

    const std::string& engineConfigPath() const { return engineConfig_; }
    const std::string& versionConfigPath() const { return versionConfig_; }
    const std::string& cacheDirectory() const { return cacheDir_; }
    const std::string& userSettingsPath() const { return userSettings_; }

    const std::filesystem::path& configFilePath() const { return configFile_; }

private:
    PathRegistry();

    void seedDefaults();
    void ensureConfigFile();
    void writeDefaultConfig(const std::filesystem::path& file) const;

    const std::vector<std::string>& pathsFor(PathType type) const;
    std::filesystem::path normalizePath(const std::string& raw, const std::filesystem::path& baseDir) const;

    static std::filesystem::path executableDirectory();
    static std::filesystem::path systemShareDirectory();
    static std::filesystem::path homeDirectory();
    static std::filesystem::path userConfigDirectory();
    static std::filesystem::path userCacheDirectory();

private:
    std::filesystem::path exeDir_{};
    std::filesystem::path systemDir_{};
    std::filesystem::path configFile_{};

    std::vector<std::string> fontPaths_;
    std::vector<std::string> imagePaths_;
    std::vector<std::string> luaPaths_;
    std::vector<std::string> themePaths_;

    std::string engineConfig_;
    std::string versionConfig_;
    std::string cacheDir_;
    std::string userSettings_;
};

} // namespace SDOM
