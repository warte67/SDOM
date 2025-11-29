#include <SDOM/SDOM_PathRegistry.hpp>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <system_error>

#include <json.hpp>

#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#else
#include <unistd.h>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif
#endif

namespace SDOM {
namespace {

std::filesystem::path execPathFromPlatform()
{
#if defined(_WIN32)
    std::wstring buffer(MAX_PATH, L'\0');
    DWORD len = ::GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    if (len == 0) {
        return std::filesystem::current_path();
    }
    buffer.resize(len);
    return std::filesystem::path(buffer);
#elif defined(__APPLE__)
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::string buf(size, '\0');
    if (_NSGetExecutablePath(buf.data(), &size) == 0) {
        return std::filesystem::path(buf).lexically_normal();
    }
    return std::filesystem::current_path();
#else
    std::array<char, 4096> buffer{};
    ssize_t len = ::readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
    if (len > 0) {
        return std::filesystem::path(std::string(buffer.data(), len)).lexically_normal();
    }
    return std::filesystem::current_path();
#endif
}

std::filesystem::path envPath(const char* name)
{
    if (const char* value = std::getenv(name)) {
        if (*value) {
            return std::filesystem::path(value);
        }
    }
    return {};
}

} // namespace

PathRegistry& PathRegistry::get()
{
    static PathRegistry instance;
    return instance;
}

PathRegistry::PathRegistry()
{
    seedDefaults();
    ensureConfigFile();
    loadFromJson(configFile_.string());
}

bool PathRegistry::loadFromJson(const std::string& filename)
{
    std::ifstream in(filename);
    if (!in) {
        return false;
    }

    nlohmann::json doc;
    try {
        in >> doc;
    } catch (const std::exception&) {
        return false;
    }

    std::filesystem::path configDir = std::filesystem::path(filename).parent_path();
    if (configDir.empty()) {
        configDir = exeDir_;
    }

    auto loadArray = [&](const nlohmann::json& arr, std::vector<std::string>& target) {
        if (!arr.is_array()) {
            return;
        }
        target.clear();
        for (const auto& value : arr) {
            if (!value.is_string()) {
                continue;
            }
            auto normalized = normalizePath(value.get<std::string>(), configDir);
            if (!normalized.empty()) {
                target.push_back(normalized.string());
            }
        }
    };

    if (const auto assetsIt = doc.find("assets"); assetsIt != doc.end() && assetsIt->is_object()) {
        if (const auto fontsIt = assetsIt->find("fonts"); fontsIt != assetsIt->end()) {
            loadArray(*fontsIt, fontPaths_);
        }
        if (const auto imagesIt = assetsIt->find("images"); imagesIt != assetsIt->end()) {
            loadArray(*imagesIt, imagePaths_);
        }
        if (const auto luaIt = assetsIt->find("lua"); luaIt != assetsIt->end()) {
            loadArray(*luaIt, luaPaths_);
        }
        if (const auto themesIt = assetsIt->find("themes"); themesIt != assetsIt->end()) {
            loadArray(*themesIt, themePaths_);
        }
    }

    if (const auto configIt = doc.find("config"); configIt != doc.end() && configIt->is_object()) {
        if (const auto engineIt = configIt->find("engine"); engineIt != configIt->end() && engineIt->is_string()) {
            engineConfig_ = normalizePath(engineIt->get<std::string>(), configDir).string();
        }
        if (const auto versionIt = configIt->find("version"); versionIt != configIt->end() && versionIt->is_string()) {
            versionConfig_ = normalizePath(versionIt->get<std::string>(), configDir).string();
        }
    }

    if (const auto runtimeIt = doc.find("runtime"); runtimeIt != doc.end() && runtimeIt->is_object()) {
        if (const auto cacheIt = runtimeIt->find("cache"); cacheIt != runtimeIt->end() && cacheIt->is_string()) {
            cacheDir_ = normalizePath(cacheIt->get<std::string>(), configDir).string();
        }
        if (const auto settingsIt = runtimeIt->find("settings"); settingsIt != runtimeIt->end() && settingsIt->is_string()) {
            userSettings_ = normalizePath(settingsIt->get<std::string>(), configDir).string();
        }
    }

    return true;
}

std::string PathRegistry::resolve(const std::string& name, PathType type) const
{
    std::filesystem::path candidate{name};
    if (candidate.is_absolute()) {
        if (std::filesystem::exists(candidate)) {
            return candidate.string();
        }
    }

    if (type == PathType::EngineConfig) {
        std::filesystem::path path(engineConfig_);
        if (!engineConfig_.empty() && std::filesystem::exists(path)) {
            return engineConfig_;
        }
        return {};
    }
    if (type == PathType::VersionConfig) {
        std::filesystem::path path(versionConfig_);
        if (!versionConfig_.empty() && std::filesystem::exists(path)) {
            return versionConfig_;
        }
        return {};
    }
    if (type == PathType::CacheDir) {
        if (!cacheDir_.empty()) {
            std::error_code ec;
            std::filesystem::create_directories(std::filesystem::path(cacheDir_), ec);
        }
        return cacheDir_;
    }
    if (type == PathType::UserSettings) {
        if (!userSettings_.empty()) {
            std::error_code ec;
            auto parent = std::filesystem::path(userSettings_).parent_path();
            if (!parent.empty()) {
                std::filesystem::create_directories(parent, ec);
            }
        }
        return userSettings_;
    }

    const auto& roots = pathsFor(type);
    for (const auto& base : roots) {
        if (base.empty()) {
            continue;
        }
        std::filesystem::path full = std::filesystem::path(base);
        if (!name.empty()) {
            full /= name;
        }
        if (std::filesystem::exists(full)) {
            return full.string();
        }
    }
    return {};
}

void PathRegistry::seedDefaults()
{
    exeDir_ = executableDirectory();
    systemDir_ = systemShareDirectory();
    configFile_ = exeDir_ / "sdom_paths.json";

    auto appendIfUnique = [](std::vector<std::string>& list, const std::filesystem::path& value) {
        if (value.empty()) return;
        const auto str = value.lexically_normal().string();
        if (str.empty()) return;
        if (std::find(list.begin(), list.end(), str) == list.end()) {
            list.push_back(str);
        }
    };

    fontPaths_.clear();
    appendIfUnique(fontPaths_, exeDir_ / "assets/fonts");
    appendIfUnique(fontPaths_, exeDir_ / "assets");
    appendIfUnique(fontPaths_, systemDir_ / "fonts");

    imagePaths_.clear();
    appendIfUnique(imagePaths_, exeDir_ / "assets/images");
    appendIfUnique(imagePaths_, exeDir_ / "assets");
    appendIfUnique(imagePaths_, systemDir_ / "images");

    luaPaths_.clear();
    appendIfUnique(luaPaths_, exeDir_);
    appendIfUnique(luaPaths_, exeDir_ / "lua");
    appendIfUnique(luaPaths_, exeDir_ / "assets/lua");
    appendIfUnique(luaPaths_, systemDir_ / "lua");

    themePaths_.clear();
    appendIfUnique(themePaths_, exeDir_ / "assets/themes");
    appendIfUnique(themePaths_, systemDir_ / "themes");

    engineConfig_ = (exeDir_ / "config/engine.json").string();
    versionConfig_ = (exeDir_ / "config/version.json").string();
    cacheDir_ = userCacheDirectory().string();
    userSettings_ = (userConfigDirectory() / "user.json").string();
}

void PathRegistry::ensureConfigFile()
{
    if (std::filesystem::exists(configFile_)) {
        return;
    }

    writeDefaultConfig(configFile_);
}

void PathRegistry::writeDefaultConfig(const std::filesystem::path& file) const
{
    nlohmann::json doc;
    doc["assets"]["fonts"] = fontPaths_;
    doc["assets"]["images"] = imagePaths_;
    doc["assets"]["lua"] = luaPaths_;
    doc["assets"]["themes"] = themePaths_;
    doc["config"]["engine"] = engineConfig_;
    doc["config"]["version"] = versionConfig_;
    doc["runtime"]["cache"] = cacheDir_;
    doc["runtime"]["settings"] = userSettings_;

    std::error_code ec;
    std::filesystem::create_directories(file.parent_path(), ec);
    if (!cacheDir_.empty()) {
        std::filesystem::create_directories(std::filesystem::path(cacheDir_), ec);
    }
    if (!userSettings_.empty()) {
        auto parent = std::filesystem::path(userSettings_).parent_path();
        if (!parent.empty()) {
            std::filesystem::create_directories(parent, ec);
        }
    }

    std::ofstream out(file);
    if (!out) {
        return;
    }
    out << doc.dump(4) << '\n';
}

const std::vector<std::string>& PathRegistry::pathsFor(PathType type) const
{
    switch (type) {
        case PathType::Fonts:  return fontPaths_;
        case PathType::Images: return imagePaths_;
        case PathType::Lua:    return luaPaths_;
        case PathType::Themes: return themePaths_;
        default:               return luaPaths_; // Should not happen for config types
    }
}

std::filesystem::path PathRegistry::normalizePath(const std::string& raw, const std::filesystem::path& baseDir) const
{
    if (raw.empty()) {
        return {};
    }

    std::string value = raw;
    if (value == "~") {
        value = homeDirectory().string();
    } else if (value.rfind("~/", 0) == 0) {
        auto home = homeDirectory();
        value = (home / value.substr(2)).string();
    }

    std::filesystem::path pathValue{value};
    if (!pathValue.is_absolute()) {
        pathValue = baseDir / pathValue;
    }
    return pathValue.lexically_normal();
}

std::filesystem::path PathRegistry::executableDirectory()
{
    auto exec = execPathFromPlatform();
    if (std::filesystem::is_regular_file(exec)) {
        return exec.parent_path();
    }
    return exec;
}

std::filesystem::path PathRegistry::systemShareDirectory()
{
    if (auto env = envPath("SDOM_SYSTEM_DATA"); !env.empty()) {
        return env;
    }
#if defined(_WIN32)
    if (auto programData = envPath("PROGRAMDATA"); !programData.empty()) {
        return programData / "SDOM";
    }
    return std::filesystem::path("C:/ProgramData/SDOM");
#else
    return std::filesystem::path("/usr/local/share/sdom");
#endif
}

std::filesystem::path PathRegistry::homeDirectory()
{
#if defined(_WIN32)
    if (auto home = envPath("USERPROFILE"); !home.empty()) {
        return home;
    }
    if (auto drive = envPath("HOMEDRIVE"); !drive.empty()) {
        if (auto path = envPath("HOMEPATH"); !path.empty()) {
            return drive / path;
        }
    }
    return std::filesystem::current_path();
#else
    if (auto home = envPath("HOME"); !home.empty()) {
        return home;
    }
    return std::filesystem::current_path();
#endif
}

std::filesystem::path PathRegistry::userConfigDirectory()
{
#if defined(_WIN32)
    if (auto appdata = envPath("APPDATA"); !appdata.empty()) {
        return appdata / "SDOM";
    }
    return homeDirectory() / "AppData/Roaming/SDOM";
#else
    if (auto xdg = envPath("XDG_CONFIG_HOME"); !xdg.empty()) {
        return xdg / "sdom";
    }
    return homeDirectory() / ".config/sdom";
#endif
}

std::filesystem::path PathRegistry::userCacheDirectory()
{
#if defined(_WIN32)
    if (auto local = envPath("LOCALAPPDATA"); !local.empty()) {
        return local / "SDOM/Cache";
    }
    return homeDirectory() / "AppData/Local/SDOM/Cache";
#else
    if (auto xdg = envPath("XDG_CACHE_HOME"); !xdg.empty()) {
        return xdg / "sdom";
    }
    return homeDirectory() / ".cache/sdom";
#endif
}

} // namespace SDOM
