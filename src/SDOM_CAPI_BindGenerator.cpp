#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <vector>
#include <utility>
#include <unordered_map>
#include <unordered_set>

#include <SDOM/SDOM_CAPI_BindGenerator.hpp>
#include <SDOM/SDOM_DataRegistry.hpp>
#include <SDOM/SDOM_Version.hpp>

namespace SDOM {

namespace {

constexpr const char* kLicenseEnvVar = "SDOM_GENERATED_LICENSE_PATH";

std::string semanticVersion()
{
    std::ostringstream oss;
    oss << SDOM_VERSION_MAJOR << '.'
        << SDOM_VERSION_MINOR << '.'
        << SDOM_VERSION_PATCH;
    return oss.str();
}

const std::string& licenseBanner()
{
    static const std::string banner = [] {
        std::vector<std::string> candidates;
        if (const char* env_path = std::getenv(kLicenseEnvVar)) {
            if (*env_path != '\0') {
                candidates.emplace_back(env_path);
            }
        }
        candidates.emplace_back("LICENSE");
        candidates.emplace_back("../LICENSE");
        candidates.emplace_back("../../LICENSE");

        for (const auto& path : candidates) {
            std::ifstream in(path);
            if (!in) {
                continue;
            }

            std::ostringstream oss;
            std::string line;
            while (std::getline(in, line)) {
                if (line.empty()) {
                    oss << "//\n";
                } else {
                    oss << "// " << line << '\n';
                }
            }

            const std::string text = oss.str();
            if (!text.empty()) {
                return text;
            }
        }

        return std::string("// License notice unavailable; see LICENSE in the repository root.\n");
    }();

    return banner;
}

std::string moduleBriefFor(const CAPI_BindGenerator::BindModule& module);
void emitWrappedComment(std::ofstream& out,
                        const std::string& text,
                        const std::string& prefix,
                        std::size_t maxWidth = 80);

void emitFileBanner(std::ofstream& out,
                    const CAPI_BindGenerator::BindModule& module,
                    bool isHeader,
                    const std::string& filepath)
{
    const std::string version = semanticVersion();
    const std::string brief = moduleBriefFor(module);
    std::string filename = filepath;
    const auto last_slash = filename.find_last_of("/\\");
    if (last_slash != std::string::npos) {
        filename = filename.substr(last_slash + 1);
    }

    const char* border = "// =============================================================================\n";
    out << border;
    out << "//  SDOM C API binding \xE2\x80\x94 AUTO-GENERATED FILE. DO NOT EDIT.\n";
    out << "//\n";
    out << "//  File: " << filename << '\n';
    out << "//  Module: " << module.file_stem << '\n';
    out << "//\n";
    out << "//  Brief:\n";
    const std::string briefText = brief.empty()
        ? std::string("Auto-generated SDOM C bindings.")
        : brief;
    emitWrappedComment(out, briefText, "//    ", 80);
    out << border;
    out << "//\n";
    out << "//  Authors:\n";
    out << "//    Jay Faries (warte67) - Primary architect of SDOM\n";
    out << "//\n";
    out << "//  File Type: " << (isHeader ? "Header" : "Source") << '\n';
    out << "//  SDOM Version: " << version << " (" << SDOM_VERSION_CODENAME << ")\n";
    out << "//  Build Identifier: " << SDOM_VERSION_BUILD << '\n';
    out << "//  Commit: " << SDOM_VERSION_COMMIT << " on branch " << SDOM_BUILD_BRANCH << '\n';
    out << "//  Compiler: " << SDOM_BUILD_COMPILER << '\n';
    out << "//  Platform: " << SDOM_BUILD_PLATFORM << '\n';
    out << "//  Generated: " << SDOM_VERSION_BUILD_DATE << '\n';
    out << "//  Generator: sdom_generate_bindings\n";
    out << "//\n";
    out << "//  License Notice:\n";

    const std::string& banner = licenseBanner();
    out << banner;
    if (!banner.empty() && banner.back() != '\n') {
        out << '\n';
    }

    out << border << '\n';
}

std::string jsonEscape(const std::string& value)
{
    std::ostringstream oss;
    for (char ch : value) {
        switch (ch) {
            case '\\': oss << "\\\\"; break;
            case '"': oss << "\\\""; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (static_cast<unsigned char>(ch) < 0x20) {
                    oss << "\\u"
                        << std::hex << std::uppercase << std::setw(4) << std::setfill('0')
                        << static_cast<int>(static_cast<unsigned char>(ch))
                        << std::nouppercase << std::dec;
                    oss << std::setfill(' ');
                } else {
                    oss << ch;
                }
                break;
        }
    }
    return oss.str();
}

std::string trim(std::string value) {
    const auto is_space = [](unsigned char ch) { return std::isspace(ch) != 0; };

    value.erase(value.begin(), std::find_if(value.begin(), value.end(), [&](unsigned char ch) {
        return !is_space(ch);
    }));
    value.erase(std::find_if(value.rbegin(), value.rend(), [&](unsigned char ch) {
        return !is_space(ch);
    }).base(), value.end());

    return value;
}

std::string collapseWhitespace(std::string value)
{
    for (char& ch : value) {
        if (ch == '\n' || ch == '\r' || ch == '\t') {
            ch = ' ';
        }
    }

    std::string out;
    out.reserve(value.size());
    bool lastSpace = false;
    for (char ch : value) {
        if (ch == ' ') {
            if (lastSpace) {
                continue;
            }
            lastSpace = true;
        } else {
            lastSpace = false;
        }
        out.push_back(ch);
    }

    return trim(out);
}

std::string synthesizeModuleBrief(const CAPI_BindGenerator::BindModule& module)
{
    auto collectNames = [](const std::vector<const TypeInfo*>& bucket,
                           std::vector<std::string>& target) {
        for (const TypeInfo* type : bucket) {
            if (!type || type->name.empty()) {
                continue;
            }
            target.push_back(type->name);
        }
    };

    std::vector<std::string> names;
    names.reserve(module.objects.size() + module.structs.size());
    collectNames(module.objects, names);
    collectNames(module.structs, names);
    collectNames(module.globals, names);
    collectNames(module.functions, names);

    if (names.empty()) {
        collectNames(module.enums, names);
        collectNames(module.aliases, names);
    }

    if (names.empty()) {
        return module.file_stem.empty()
            ? std::string("Auto-generated C bindings for SDOM.")
            : std::string("Auto-generated C bindings for module ") + module.file_stem + '.';
    }

    constexpr std::size_t kMaxMentions = 3;
    const std::size_t mentionCount = std::min(kMaxMentions, names.size());

    std::ostringstream oss;
    oss << "C bindings for ";
    for (std::size_t i = 0; i < mentionCount; ++i) {
        if (i > 0) {
            oss << (i + 1 == mentionCount ? " and " : ", ");
        }
        oss << names[i];
    }

    if (names.size() > mentionCount) {
        oss << " plus " << (names.size() - mentionCount) << " more types";
    }
    oss << '.';

    return oss.str();
}

std::string moduleBriefFor(const CAPI_BindGenerator::BindModule& module)
{
    std::string brief = module.module_brief;
    if (brief.empty()) {
        brief = synthesizeModuleBrief(module);
    }
    return collapseWhitespace(brief);
}

void emitWrappedComment(std::ofstream& out,
                        const std::string& text,
                        const std::string& prefix,
                        std::size_t maxWidth)
{
    if (text.empty()) {
        return;
    }

    const std::size_t usableWidth = maxWidth > prefix.size()
        ? maxWidth - prefix.size()
        : 0;

    if (usableWidth == 0) {
        out << prefix << text << '\n';
        return;
    }

    std::istringstream words(text);
    std::string word;
    std::string line;
    while (words >> word) {
        if (line.empty()) {
            line = word;
            continue;
        }

        if (line.size() + 1 + word.size() > usableWidth) {
            out << prefix << line << '\n';
            line = word;
        } else {
            line += ' ';
            line += word;
        }
    }

    if (!line.empty()) {
        out << prefix << line << '\n';
    }
}

std::string normalizeTypeToken(std::string type);
bool isUnsignedIntegralToken(const std::string& token);
bool isSignedIntegralToken(const std::string& token);

std::string sanitizeEnumToken(std::string value) {
    for (char& ch : value) {
        if (std::isalnum(static_cast<unsigned char>(ch)) || ch == '_') {
            continue;
        }

        if (ch == ':') {
            ch = '_';
        } else {
            ch = '_';
        }
    }

    if (!value.empty() && std::isdigit(static_cast<unsigned char>(value.front()))) {
        value.insert(value.begin(), '_');
    }

    return value;
}

std::string formatEnumValue(std::uint32_t value) {
    std::ostringstream oss;
    oss << "0x" << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << value;
    return oss.str();
}

constexpr std::size_t kEnumCommentColumn = 52;

std::string cFriendlyType(const std::string& cppType) {
    std::string cleaned = trim(cppType);
    if (cleaned.empty()) {
        return "void*";
    }

    const bool isConst = cleaned.rfind("const ", 0) == 0;
    if (isConst) {
        cleaned = trim(cleaned.substr(6));
    }

    if (cleaned.find("SDOM::") == std::string::npos) {
        return (isConst ? std::string("const ") + cleaned : cleaned);
    }

    return isConst ? "const void*" : "void*";
}

std::string deduceCReturnType(const FunctionInfo& fn) {
    if (!fn.c_signature.empty() && !fn.c_name.empty()) {
        const auto pos = fn.c_signature.find(fn.c_name);
        if (pos != std::string::npos) {
            return trim(fn.c_signature.substr(0, pos));
        }
    }

    if (!fn.return_type.empty()) {
        return trim(fn.return_type);
    }

    return "void";
}

std::string extractParamIdentifier(std::string param) {
    param = trim(std::move(param));
    if (param.empty() || param == "void") {
        return {};
    }

    std::size_t end = param.size();
    while (end > 0) {
        const unsigned char ch = static_cast<unsigned char>(param[end - 1]);
        if (std::isalnum(ch) || ch == '_') {
            break;
        }
        --end;
    }

    if (end == 0) {
        return {};
    }

    std::size_t start = end;
    while (start > 0) {
        const unsigned char ch = static_cast<unsigned char>(param[start - 1]);
        if (!std::isalnum(ch) && ch != '_') {
            break;
        }
        --start;
    }

    if (start == end) {
        return {};
    }

    return param.substr(start, end - start);
}

struct ParameterInfo {
    std::string name;
    std::string type;
};

std::vector<ParameterInfo> extractParameters(const FunctionInfo& fn)
{
    std::vector<ParameterInfo> params;

    if (fn.c_signature.empty() || fn.c_name.empty()) {
        return params;
    }

    const auto name_pos = fn.c_signature.find(fn.c_name);
    if (name_pos == std::string::npos) {
        return params;
    }

    const auto open_paren = fn.c_signature.find('(', name_pos + fn.c_name.size());
    const auto close_paren = fn.c_signature.rfind(')');
    if (open_paren == std::string::npos || close_paren == std::string::npos || close_paren <= open_paren) {
        return params;
    }

    const std::string params_str = fn.c_signature.substr(open_paren + 1, close_paren - open_paren - 1);
    std::stringstream ss(params_str);
    std::string param;
    while (std::getline(ss, param, ',')) {
        std::string trimmed = trim(param);
        if (trimmed.empty() || trimmed == "void") {
            continue;
        }

        const auto name = extractParamIdentifier(trimmed);
        if (name.empty()) {
            continue;
        }

        const std::size_t param_name_pos = trimmed.rfind(name);
        if (param_name_pos == std::string::npos) {
            continue;
        }

        std::string type_part = trim(trimmed.substr(0, param_name_pos));
        if (type_part.empty()) {
            continue;
        }

        params.push_back(ParameterInfo{ name, type_part });
    }

    return params;
}


bool isCStringReturnType(const std::string& type)
{
    const std::string trimmed = trim(type);
    return trimmed == "const char*" || trimmed == "const char *" || trimmed == "char const*" || trimmed == "char const *";
}

bool isCStringType(const std::string& type)
{
    if (isCStringReturnType(type)) {
        return true;
    }

    const std::string trimmed = trim(type);
    return trimmed == "char*" || trimmed == "char *";
}

// Detect SDOM_Event handle pointers (but not SDOM_EventType, etc.)
bool isEventHandlePointerType(const std::string& trimmedType)
{
    if (trimmedType.find('*') == std::string::npos) {
        return false;
    }

    std::string base = trimmedType;
    // strip pointers, const, and whitespace
    base.erase(std::remove_if(base.begin(), base.end(), [](char c) {
        return c == '*' || c == ' ' || c == '\t';
    }), base.end());

    const std::string kConst = "const";
    if (base.rfind(kConst, 0) == 0) {
        base = base.substr(kConst.size());
    }

    return base == "SDOM_Event";
}

std::string emitCallArgForParameter(const ParameterInfo& param,
                                    bool wrapEventHandles)
{
    const std::string trimmedType = trim(param.type);
    if (trimmedType.empty()) {
        return {};
    }

    if (isCStringType(trimmedType)) {
        return "SDOM::CAPI::CallArg::makeCString(" + param.name + ")";
    }

    const bool isPointer = trimmedType.find('*') != std::string::npos;
    if (isPointer && wrapEventHandles && isEventHandlePointerType(trimmedType)) {
        return "SDOM::CAPI::CallArg::makePtr(eventImplPtr(" + param.name + "))";
    }
    if (isPointer) {
        const bool isConstPtr = trimmedType.find("const") != std::string::npos;
        if (isConstPtr) {
            return "SDOM::CAPI::CallArg::makePtr(const_cast<void*>(static_cast<const void*>(" + param.name + ")))";
        }
        return "SDOM::CAPI::CallArg::makePtr(reinterpret_cast<void*>(" + param.name + "))";
    }

    const std::string normalized = normalizeTypeToken(trimmedType);
    if (normalized == "bool") {
        return "SDOM::CAPI::CallArg::makeBool(" + param.name + ")";
    }

    if (isUnsignedIntegralToken(normalized)) {
        return "SDOM::CAPI::CallArg::makeUInt(static_cast<std::uint64_t>(" + param.name + "))";
    }

    if (isSignedIntegralToken(normalized)) {
        return "SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(" + param.name + "))";
    }

    if (normalized == "float" || normalized == "double") {
        return "SDOM::CAPI::CallArg::makeDouble(static_cast<double>(" + param.name + "))";
    }

    return "SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(" + param.name + "))";
}

void emitDocComment(std::ofstream& out, const std::string& doc)
{
    if (doc.empty()) {
        return;
    }

    std::string oneLine = doc;
    std::replace(oneLine.begin(), oneLine.end(), '\n', ' ');
    out << "/** " << oneLine << " */\n";
}

void emitDoxygenForPrototype(std::ofstream& out, const FunctionInfo& fn)
{
    // Build brief
    std::string brief = fn.doc;
    std::replace(brief.begin(), brief.end(), '\n', ' ');

    out << "/**\n";
    out << " * @brief " << brief << "\n";
    out << " *\n";
    out << " * C++:   " << fn.cpp_signature << "\n";
    out << " * C API: " << fn.c_signature << "\n";
    out << " *\n";

    const auto params = extractParameters(fn);
    for (const auto& p : params) {
        if (trim(p.type).find('*') != std::string::npos) {
            out << " * @param " << p.name << " Pointer parameter.\n";
        }
    }

    out << " * @return " << fn.return_type << "; check SDOM_GetError() for details on failure.\n";
    out << " */\n";
}

struct ReturnMetadata {
    std::string c_type;
    std::string normalized;
    bool is_enum = false;
};

using BindModule = CAPI_BindGenerator::BindModule;

bool moduleDeclaresEnum(const std::string& normalizedType, const BindModule& module)
{
    if (normalizedType.empty()) {
        return false;
    }

    for (const TypeInfo* entry : module.enums) {
        if (!entry) {
            continue;
        }

        const std::string export_name = entry->export_name.empty() ? entry->name : entry->export_name;
        if (export_name == normalizedType) {
            return true;
        }
    }

    return false;
}

ReturnMetadata analyzeReturnMetadata(const FunctionInfo& fn, const BindModule& module)
{
    ReturnMetadata meta;
    meta.c_type = deduceCReturnType(fn);
    meta.normalized = normalizeTypeToken(meta.c_type);
    meta.is_enum = moduleDeclaresEnum(meta.normalized, module);
    return meta;
}

bool functionMentionsToken(const FunctionInfo& fn, const std::string& token)
{
    if (token.empty()) {
        return false;
    }

    if (!fn.c_signature.empty() && fn.c_signature.find(token) != std::string::npos) {
        return true;
    }
    if (!fn.return_type.empty() && fn.return_type.find(token) != std::string::npos) {
        return true;
    }

    for (const auto& param_type : fn.param_types) {
        if (param_type.find(token) != std::string::npos) {
            return true;
        }
    }

    return false;
}

bool typeMentionsToken(const TypeInfo* type, const std::string& token)
{
    if (!type || token.empty()) {
        return false;
    }

    for (const auto& prop : type->properties) {
        if (prop.cpp_type.find(token) != std::string::npos) {
            return true;
        }
    }

    for (const auto& fn : type->functions) {
        if (functionMentionsToken(fn, token)) {
            return true;
        }
    }

    return false;
}

bool moduleUsesTypeToken(const BindModule& module, const std::string& token)
{
    if (token.empty()) {
        return false;
    }

    const auto bucketUsesToken = [&](const std::vector<const TypeInfo*>& bucket) {
        for (const TypeInfo* type : bucket) {
            if (typeMentionsToken(type, token)) {
                return true;
            }
        }
        return false;
    };

    return bucketUsesToken(module.objects)
        || bucketUsesToken(module.structs)
        || bucketUsesToken(module.globals)
        || bucketUsesToken(module.functions)
        || bucketUsesToken(module.aliases);
}

std::vector<std::string> collectDependencyIncludes(const BindModule& module)
{
    struct TokenInclude {
        const char* token;
        const char* include_line;
    };

    static constexpr TokenInclude requirements[] = {
        { "SDOM_AssetHandle", "#include <SDOM/CAPI/SDOM_CAPI_Handles.h>" },
        { "SDOM_DisplayHandle", "#include <SDOM/CAPI/SDOM_CAPI_Handles.h>" },
        { "SDOM_Event", "#include <SDOM/CAPI/SDOM_CAPI_Event.h>" }
    };

    std::vector<std::string> includes;
    for (const auto& requirement : requirements) {
        if (moduleUsesTypeToken(module, requirement.token)) {
            if (std::find(includes.begin(), includes.end(), requirement.include_line) == includes.end()) {
                includes.emplace_back(requirement.include_line);
            }
        }
    }

    return includes;
}

bool isPointerReturnType(const std::string& type)
{
    const std::string trimmed = trim(type);
    return trimmed.find('*') != std::string::npos;
}

const ParameterInfo* findSubjectParameter(const std::vector<ParameterInfo>& params)
{
    for (const auto& param : params) {
        const std::string trimmed = trim(param.type);
        if (trimmed.empty()) {
            continue;
        }

        if (trimmed.find("SDOM_") == std::string::npos) {
            continue;
        }

        if (trimmed.find('*') == std::string::npos) {
            continue;
        }

        if (isCStringType(trimmed)) {
            continue;
        }

        return &param;
    }

    return nullptr;
}

void emitGuardReturn(std::ofstream& out, const ReturnMetadata& meta)
{
    const std::string trimmed = trim(meta.c_type);

    if (trimmed == "void") {
        out << "        return;\n";
        return;
    }

    if (trimmed == "bool") {
        out << "        return false;\n";
        return;
    }

    if (isCStringReturnType(trimmed) || isPointerReturnType(trimmed)) {
        out << "        return nullptr;\n";
        return;
    }

    if (meta.is_enum && !meta.normalized.empty()) {
        out << "        return static_cast<" << meta.normalized << ">(0);\n";
        return;
    }

    out << "        return {};\n";
}

void emitSubjectGuard(std::ofstream& out,
                      const FunctionInfo& fn,
                      const ParameterInfo& subjectParam,
                      const ReturnMetadata& meta)
{
    const std::string guardName = subjectParam.name.empty() ? std::string("subject") : subjectParam.name;
    const std::string fnLabel = fn.c_name.empty() ? fn.name : fn.c_name;

    out << "    if (!" << guardName << ") {\n";
    out << "        SDOM_SetError(\"" << fnLabel << ": subject '" << guardName << "' is null\");\n";
    emitGuardReturn(out, meta);
    out << "    }\n\n";
}

void emitInvokeCallableCore(std::ofstream& out,
                            const FunctionInfo& fn,
                            const std::vector<ParameterInfo>& params,
                            const ReturnMetadata& meta,
                            bool wrapEventHandles)
{
    const bool hasArgs = !params.empty();
    if (hasArgs) {
        out << "    std::vector<SDOM::CAPI::CallArg> args;\n";
        out << "    args.reserve(" << params.size() << ");\n";
        for (const auto& param : params) {
            out << "    args.push_back(" << emitCallArgForParameter(param, wrapEventHandles) << ");\n";
        }
        out << '\n';
    }

    const std::string callTarget = fn.c_name.empty() ? fn.name : fn.c_name;
    const std::string callExpr = "SDOM::CAPI::invokeCallable(\"" + callTarget + "\", "
        + (hasArgs ? std::string("args") : std::string("{}")) + ")";

    const std::string trimmedReturn = trim(meta.c_type);

    if (trimmedReturn == "void") {
        out << "    (void)" << callExpr << ";\n";
        out << "    return;\n";
    } else if (trimmedReturn == "bool") {
        out << "    const auto callResult = " << callExpr << ";\n";
        out << "    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {\n";
        out << "        return false;\n";
        out << "    }\n";
        out << "    return callResult.v.b;\n";
    } else if (isCStringReturnType(trimmedReturn)) {
        out << "    static thread_local std::string s_result;\n";
        out << "    const auto callResult = " << callExpr << ";\n";
        out << "    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {\n";
        out << "        s_result.clear();\n";
        out << "        return s_result.c_str();\n";
        out << "    }\n";
        out << "    s_result = callResult.s;\n";
        out << "    return s_result.c_str();\n";
    } else if (trimmedReturn == "float" || trimmedReturn == "double") {
        out << "    const auto callResult = " << callExpr << ";\n";
        out << "    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {\n";
        out << "        return static_cast<" << trimmedReturn << ">(callResult.v.d);\n";
        out << "    }\n";
        out << "    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {\n";
        out << "        return static_cast<" << trimmedReturn << ">(callResult.v.u);\n";
        out << "    }\n";
        out << "    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {\n";
        out << "        return static_cast<" << trimmedReturn << ">(callResult.v.i);\n";
        out << "    }\n";
        out << "    return static_cast<" << trimmedReturn << ">(0);\n";
    } else if (meta.is_enum) {
        out << "    const auto callResult = " << callExpr << ";\n";
        out << "    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {\n";
        out << "        return static_cast<" << meta.normalized << ">(callResult.v.u);\n";
        out << "    }\n";
        out << "    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {\n";
        out << "        return static_cast<" << meta.normalized << ">(callResult.v.i);\n";
        out << "    }\n";
        out << "    return static_cast<" << meta.normalized << ">(0);\n";
    } else if (isSignedIntegralToken(meta.normalized)) {
        out << "    const auto callResult = " << callExpr << ";\n";
        out << "    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {\n";
        out << "        return static_cast<" << trimmedReturn << ">(callResult.v.i);\n";
        out << "    }\n";
        out << "    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {\n";
        out << "        return static_cast<" << trimmedReturn << ">(callResult.v.u);\n";
        out << "    }\n";
        out << "    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {\n";
        out << "        return static_cast<" << trimmedReturn << ">(callResult.v.d);\n";
        out << "    }\n";
        out << "    return static_cast<" << trimmedReturn << ">(0);\n";
    } else if (isUnsignedIntegralToken(meta.normalized)) {
        out << "    const auto callResult = " << callExpr << ";\n";
        out << "    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {\n";
        out << "        return static_cast<" << trimmedReturn << ">(callResult.v.u);\n";
        out << "    }\n";
        out << "    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {\n";
        out << "        const auto value = callResult.v.i < 0 ? std::int64_t{0} : callResult.v.i;\n";
        out << "        return static_cast<" << trimmedReturn << ">(value);\n";
        out << "    }\n";
        out << "    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Double) {\n";
        out << "        const auto value = callResult.v.d < 0.0 ? 0.0 : callResult.v.d;\n";
        out << "        return static_cast<" << trimmedReturn << ">(value);\n";
        out << "    }\n";
        out << "    return static_cast<" << trimmedReturn << ">(0);\n";
    } else {
        out << "    // TODO: marshal return type '" << meta.c_type << "'.\n";
        out << "    (void)" << callExpr << ";\n";
        out << "    return {}; // placeholder\n";
    }

    out << "}\n\n";
}

std::unordered_map<std::string, CAPI_BindGenerator::CustomEmitter>& customEmitterRegistry()
{
    static std::unordered_map<std::string, CAPI_BindGenerator::CustomEmitter> registry;
    return registry;
}

std::string normalizeTypeToken(std::string type)
{
    type = trim(std::move(type));
    if (type.rfind("const ", 0) == 0) {
        type = trim(type.substr(6));
    }

    if (!type.empty() && type.rfind("enum ", 0) == 0) {
        type = trim(type.substr(5));
    }

    while (!type.empty() && (type.back() == '&' || type.back() == '*')) {
        type.pop_back();
        type = trim(type);
    }

    return type;
}

bool isUnsignedIntegralToken(const std::string& token)
{
    static const std::unordered_set<std::string> kUnsignedTypes = {
        "std::uint8_t", "std::uint16_t", "std::uint32_t", "std::uint64_t",
        "uint8_t", "uint16_t", "uint32_t", "uint64_t",
        "unsigned", "unsigned int", "unsigned long", "unsigned long long",
        "unsigned short", "size_t", "std::size_t"
    };
    return kUnsignedTypes.count(token) > 0;
}

bool isSignedIntegralToken(const std::string& token)
{
    static const std::unordered_set<std::string> kSignedTypes = {
        "std::int8_t", "std::int16_t", "std::int32_t", "std::int64_t",
        "int8_t", "int16_t", "int32_t", "int64_t",
        "int", "long", "long long", "short"
    };
    return kSignedTypes.count(token) > 0;
}

} // namespace

bool CAPI_BindGenerator::generate(const std::unordered_map<std::string, TypeInfo>& types,
                                  const DataRegistry& registry)
{
    (void)registry;

    const BindModuleMap modules = buildModuleMap(types);

    latest_manifest_ = buildBindingManifest(types);
    type_descriptors_.clear();
    for (const auto& desc : latest_manifest_.type_bindings) {
        type_descriptors_.emplace(desc.name, desc);
    }

    generated_headers_.clear();
    generated_sources_.clear();

    emitBindingManifest(latest_manifest_);

    for (const auto& [_, module] : modules) {
        const bool has_functions = moduleHasFunctions(module);
        const bool has_header_content = !module.empty() || has_functions;

        if (has_header_content) {
            generateHeader(module);
        }

        if (has_functions) {
            generateSource(module);
        }
    }

    emitGeneratedFileIndex();

    return true;
}

void CAPI_BindGenerator::generateHeader(const BindModule& module)
{
    if (module.file_stem.empty()) {
        return;
    }

    const std::string filename = headerPathFor(header_dir_, module.file_stem);
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "[CAPI_BindGenerator] Failed to open header: " << filename << '\n';
        return;
    }

    std::cout << "[CAPI_BindGenerator] Generating header: " << filename << '\n';

    emitFileBanner(out, module, true, filename);
    out << "#pragma once\n\n";

    std::vector<std::string> includes;
    const auto addInclude = [&](const std::string& line) {
        if (line.empty()) {
            return;
        }
        if (std::find(includes.begin(), includes.end(), line) == includes.end()) {
            includes.emplace_back(line);
        }
    };

    addInclude("#include <stdbool.h>");
    if (moduleNeedsCstdint(module)) {
        addInclude("#include <stdint.h>");
    }
    if (module.file_stem == "Core") {
        addInclude("#include <SDL3/SDL.h>");
    }

    const auto dependencyIncludes = collectDependencyIncludes(module);
    for (const auto& include_line : dependencyIncludes) {
        addInclude(include_line);
    }

    if (!includes.empty()) {
        for (const auto& include_line : includes) {
            out << include_line << '\n';
        }
        out << '\n';
    }

    emitEnums(out, module);

    if (module.file_stem == "Core") {
        out << "\ntypedef struct SDOM_CoreConfig {\n";
        out << "    float windowWidth;\n";
        out << "    float windowHeight;\n";
        out << "    float pixelWidth;\n";
        out << "    float pixelHeight;\n";
        out << "    int preserveAspectRatio;\n";
        out << "    int allowTextureResize;\n";
        out << "    SDL_RendererLogicalPresentation rendererLogicalPresentation;\n";
        out << "    SDL_WindowFlags windowFlags;\n";
        out << "    SDL_PixelFormat pixelFormat;\n";
        out << "    SDL_Color color;\n";
        out << "} SDOM_CoreConfig;\n\n";
        out << "#define SDOM_CORECONFIG_DEFAULT  { \\\n";
        out << "    800.0f, 600.0f, \\\n";
        out << "    2.0f, 2.0f, \\\n";
        out << "    1, 0, \\\n";
        out << "    SDL_LOGICAL_PRESENTATION_LETTERBOX, \\\n";
        out << "    SDL_WINDOW_RESIZABLE, \\\n";
        out << "    SDL_PIXELFORMAT_RGBA8888, \\\n";
        out << "    { 32, 32, 32, 255 } \\\n";
        out << "}\n\n";
    }

    emitStructs(out, module);

    if (moduleHasFunctions(module)) {
        out << "#ifdef __cplusplus\n";
        out << "extern \"C\" {\n";
        out << "#endif\n\n";

        emitFunctionPrototypes(out, module);

        out << "#ifdef __cplusplus\n";
        out << "} // extern \"C\"\n";
        out << "#endif\n";
    }

    generated_headers_.push_back(filename);
}

void CAPI_BindGenerator::generateSource(const BindModule& module)
{
    if (module.file_stem.empty() || !moduleHasFunctions(module)) {
        return;
    }

    const std::string filename = sourcePathFor(source_dir_, module.file_stem);
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "[CAPI_BindGenerator] Failed to open source: " << filename << '\n';
        return;
    }

    std::cout << "[CAPI_BindGenerator] Generating source: " << filename << '\n';

    emitFileBanner(out, module, false, filename);
    out << "#include <SDOM/CAPI/SDOM_CAPI_" << module.file_stem << ".h>\n";
    out << "#include <SDOM/CAPI/SDOM_CAPI_Core.h>\n";
    out << "#include <SDOM/SDOM_DataRegistry.hpp>\n";
    out << "#include <string>\n";
    out << "#include <vector>\n\n";

    if (!moduleHasFunctions(module)) {
        out << "// No callable bindings recorded for this module.\n";
        return;
    }

    out << "#ifdef __cplusplus\n";
    out << "extern \"C\" {\n";
    out << "#endif\n\n";

    if (moduleUsesEventHandle(module)) {
        out << "static inline void* eventImplPtr(const SDOM_Event* evt)\n";
        out << "{\n";
        out << "    return evt ? evt->impl : nullptr;\n";
        out << "}\n\n";
    }

    emitFunctionBodies(out, module);

    out << "#ifdef __cplusplus\n";
    out << "} // extern \"C\"\n";
    out << "#endif\n";

    generated_sources_.push_back(filename);
}

void CAPI_BindGenerator::emitBindingManifest(const BindingManifest& manifest) const
{
    if (source_dir_.empty()) {
        return;
    }

    const std::string path = source_dir_ + "/bindings_manifest.json";
    std::ofstream out(path);
    if (!out) {
        std::cerr << "[CAPI_BindGenerator] Failed to write binding manifest: " << path << '\n';
        return;
    }

    out << manifestToJson(manifest, 2) << '\n';
    std::cout << "[CAPI_BindGenerator] Wrote binding manifest: " << path << '\n';
}

void CAPI_BindGenerator::emitGeneratedFileIndex() const
{
    if (generated_headers_.empty() && generated_sources_.empty()) {
        return;
    }

    const std::string out_dir = source_dir_.empty() ? header_dir_ : source_dir_;
    if (out_dir.empty()) {
        return;
    }

    const std::string path = out_dir + "/generated_files.json";
    std::ofstream out(path);
    if (!out) {
        std::cerr << "[CAPI_BindGenerator] Failed to write generated file index: " << path << '\n';
        return;
    }

    const auto writeArray = [&](const std::vector<std::string>& items) {
        out << "  [\n";
        for (std::size_t i = 0; i < items.size(); ++i) {
            out << "    \"" << jsonEscape(items[i]) << "\"";
            if (i + 1 < items.size()) {
                out << ',';
            }
            out << '\n';
        }
        out << "  ]";
    };

    out << "{\n";
    out << "  \"version\": {\n";
    out << "    \"semantic\": \"" << semanticVersion() << "\",\n";
    out << "    \"codename\": \"" << jsonEscape(SDOM_VERSION_CODENAME) << "\",\n";
    out << "    \"build\": \"" << jsonEscape(SDOM_VERSION_BUILD) << "\",\n";
    out << "    \"commit\": \"" << jsonEscape(SDOM_VERSION_COMMIT) << "\",\n";
    out << "    \"branch\": \"" << jsonEscape(SDOM_BUILD_BRANCH) << "\",\n";
    out << "    \"build_date\": \"" << jsonEscape(SDOM_VERSION_BUILD_DATE) << "\",\n";
    out << "    \"compiler\": \"" << jsonEscape(SDOM_BUILD_COMPILER) << "\",\n";
    out << "    \"platform\": \"" << jsonEscape(SDOM_BUILD_PLATFORM) << "\"\n";
    out << "  },\n";

    out << "  \"generated_headers\":\n";
    writeArray(generated_headers_);
    out << ",\n";

    out << "  \"generated_sources\":\n";
    writeArray(generated_sources_);
    out << '\n';

    out << "}\n";

    std::cout << "[CAPI_BindGenerator] Wrote generated file index: " << path << '\n';
}

bool CAPI_BindGenerator::moduleNeedsCstdint(const BindModule& module)
{
    const auto requiresCstdint = [](const std::string& cppType) {
        const std::string normalized = normalizeTypeToken(cppType);
        static const std::set<std::string> fixedWidthTypes = {
            "std::uint8_t", "std::uint16_t", "std::uint32_t", "std::uint64_t",
            "std::int8_t",  "std::int16_t",  "std::int32_t",  "std::int64_t",
            "uint8_t", "uint16_t", "uint32_t", "uint64_t",
            "int8_t",  "int16_t",  "int32_t",  "int64_t"
        };

        return fixedWidthTypes.find(normalized) != fixedWidthTypes.end();
    };

    for (const TypeInfo* type : module.structs) {
        if (!type) {
            continue;
        }

        for (const auto& prop : type->properties) {
            if (requiresCstdint(prop.cpp_type)) {
                return true;
            }
        }
    }

    return false;
}

bool CAPI_BindGenerator::moduleUsesEventHandle(const BindModule& module)
{
    bool usesEventHandle = false;
    forEachCallableType(module, [&](const TypeInfo* type) {
        if (!type) {
            return;
        }
        for (const auto& fn : type->functions) {
            if (!fn.exported || fn.c_signature.empty()) {
                continue;
            }
            const auto params = extractParameters(fn);
            for (const auto& p : params) {
                if (isEventHandlePointerType(trim(p.type))) {
                    usesEventHandle = true;
                    return;
                }
            }
            if (usesEventHandle) {
                return;
            }
        }
    });
    return usesEventHandle;
}

void CAPI_BindGenerator::emitStructs(std::ofstream& out, const BindModule& module) const
{
    if (module.structs.empty()) {
        return;
    }

    for (const TypeInfo* type : module.structs) {
        if (!type) {
            continue;
        }

        const std::string export_name = type->export_name.empty() ? type->name : type->export_name;
        if (export_name.empty()) {
            continue;
        }

        out << "typedef struct " << export_name << " {\n";

        if (type->properties.empty()) {
            out << "    void* _opaque; ///< Placeholder for opaque struct.\n";
        } else {
            for (const auto& prop : type->properties) {
                const std::string field_type = cFriendlyType(prop.cpp_type);
                std::string line = "    " + field_type + " " + prop.name;
                if (!prop.default_value || prop.default_value->empty()) {
                    line += ";";
                } else {
                    line += " = " + *prop.default_value + ";";
                }

                if (!prop.doc.empty() || field_type == "void*" || field_type == "const void*") {
                    if (line.size() < 52) {
                        line.append(52 - line.size(), ' ');
                    } else {
                        line.push_back(' ');
                    }

                    std::string comment = prop.doc;
                    if (comment.empty() && prop.cpp_type.find("SDOM::") != std::string::npos) {
                        comment = "Pointer to underlying C++ type (" + prop.cpp_type + ")";
                    }

                    if (!comment.empty()) {
                        std::replace(comment.begin(), comment.end(), '\n', ' ');
                        line += "///< " + comment;
                    }
                }

                out << line << '\n';
            }
        }

        out << "} " << export_name << ";\n\n";
    }

}

void CAPI_BindGenerator::emitEnums(std::ofstream& out, const BindModule& module) const
{
    if (module.enums.empty()) {
        return;
    }

    std::map<std::string, std::vector<const TypeInfo*>> grouped;
    for (const TypeInfo* entry : module.enums) {
        if (!entry) {
            continue;
        }

        std::string export_name = entry->export_name.empty()
                                 ? "SDOM_CAPI_" + module.file_stem + "_Enum"
                                 : entry->export_name;
        grouped[export_name].push_back(entry);
    }

    for (auto& [export_name, entries] : grouped) {
        if (entries.empty()) {
            continue;
        }

        std::sort(entries.begin(), entries.end(), [](const TypeInfo* lhs, const TypeInfo* rhs) {
            static const std::string empty;

            const bool lhs_has_value = lhs && lhs->enum_value.has_value();
            const bool rhs_has_value = rhs && rhs->enum_value.has_value();

            if (lhs_has_value && rhs_has_value) {
                const auto lhs_val = lhs->enum_value.value();
                const auto rhs_val = rhs->enum_value.value();
                if (lhs_val != rhs_val) {
                    return lhs_val < rhs_val;
                }
            } else if (lhs_has_value != rhs_has_value) {
                return lhs_has_value; // explicit ids before implicit ones
            }

            const std::string& lhs_cat = (lhs && !lhs->category.empty()) ? lhs->category : empty;
            const std::string& rhs_cat = (rhs && !rhs->category.empty()) ? rhs->category : empty;
            if (lhs_cat != rhs_cat) {
                if (lhs_cat.empty()) {
                    return false;
                }
                if (rhs_cat.empty()) {
                    return true;
                }
                return lhs_cat < rhs_cat;
            }

            const std::string lhs_name = lhs ? lhs->name : std::string();
            const std::string rhs_name = rhs ? rhs->name : std::string();
            return lhs_name < rhs_name;
        });

        out << "typedef enum " << export_name << " {\n";

        std::string last_category;
        bool have_category = false;

        for (std::size_t i = 0; i < entries.size(); ++i) {
            const TypeInfo* entry = entries[i];
            std::string token = !entry->cpp_type_id.empty() ? entry->cpp_type_id : entry->name;
            token = sanitizeEnumToken(std::move(token));

            std::string category_label;
            if (entry && !entry->category.empty()) {
                category_label = entry->category;
            }

            if (category_label != last_category) {
                if (!category_label.empty()) {
                    if (have_category) {
                        out << '\n';
                    }
                    out << "    /* " << category_label << " events */\n";
                    last_category = category_label;
                    have_category = true;
                }
            }

            std::string line = "    " + export_name + "_" + token;

            if (entry && entry->enum_value.has_value()) {
                line += " = " + formatEnumValue(entry->enum_value.value());
            }

            if (i + 1 < entries.size()) {
                line += ",";
            }

            const bool has_doc = entry && !entry->doc.empty();
            if (has_doc) {
                std::string doc_line = entry->doc;
                std::replace(doc_line.begin(), doc_line.end(), '\n', ' ');
                if (line.size() < kEnumCommentColumn) {
                    line.append(kEnumCommentColumn - line.size(), ' ');
                } else {
                    line.push_back(' ');
                }
                line += "///< " + doc_line;
            }

            out << line << '\n';
        }

        out << "} " << export_name << ";\n\n";
    }
}

void CAPI_BindGenerator::emitFunctionPrototypes(std::ofstream& out, const BindModule& module) const
{
    bool wrote_any = false;
    std::unordered_set<std::string> emitted_names;

    forEachCallableType(module, [&](const TypeInfo* type) {
        for (const auto& fn : type->functions) {
            if (!fn.exported || fn.c_signature.empty()) {
                continue;
            }

            const std::string key = fn.c_name.empty() ? fn.c_signature : fn.c_name;
            if (!emitted_names.insert(key).second) {
                continue; // Skip duplicate metadata entries
            }

            if (!wrote_any) {
                out << "// Callable bindings\n";
                wrote_any = true;
            }

            // Separate comment blocks for readability
            out << "\n";
            emitDoxygenForPrototype(out, fn);
            out << fn.c_signature << ";\n";
        }
    });

    if (wrote_any) {
        out << '\n';
    }
}

void CAPI_BindGenerator::emitFunctionBodies(std::ofstream& out, const BindModule& module) const
{
    bool wrote_any = false;
    std::unordered_set<std::string> emitted_names;

    forEachCallableType(module, [&](const TypeInfo* type) {
        const SubjectTypeDescriptor* descriptor = type ? descriptorFor(type->name) : nullptr;

        for (const auto& fn : type->functions) {
            if (!fn.exported || fn.c_signature.empty()) {
                continue;
            }

            const std::string key = fn.c_name.empty() ? fn.c_signature : fn.c_name;
            if (!emitted_names.insert(key).second) {
                continue; // Avoid emitting duplicate bodies
            }

            bool handled = false;

            if (descriptor) {
                switch (descriptor->dispatch_family) {
                case SubjectDispatchFamily::MethodTable:
                    emitMethodTableFunction(out, fn, module, descriptor);
                    handled = true;
                    break;
                case SubjectDispatchFamily::Singleton:
                    emitSingletonFunction(out, fn, module, descriptor);
                    handled = true;
                    break;
                case SubjectDispatchFamily::EventRouter:
                    emitEventRouterFunction(out, fn, module, descriptor);
                    handled = true;
                    break;
                case SubjectDispatchFamily::Custom:
                    emitCustomFunction(out, fn, module, descriptor);
                    handled = true;
                    break;
                default:
                    break;
                }
            }

            if (!handled) {
                emitLegacyCallableBody(out, fn, module);
            }

            wrote_any = true;
        }
    });

    if (!wrote_any) {
        out << "// No callable bindings recorded for this module.\n";
    }
}

const SubjectTypeDescriptor* CAPI_BindGenerator::descriptorFor(const std::string& typeName) const
{
    if (typeName.empty()) {
        return nullptr;
    }

    const auto it = type_descriptors_.find(typeName);
    if (it != type_descriptors_.end()) {
        return &it->second;
    }

    return nullptr;
}

void CAPI_BindGenerator::registerCustomEmitter(const std::string& subjectKind,
                                               CustomEmitter emitter)
{
    if (subjectKind.empty() || !emitter) {
        return;
    }

    customEmitterRegistry()[subjectKind] = std::move(emitter);
}

void CAPI_BindGenerator::emitMethodTableFunction(std::ofstream& out,
                                                 const FunctionInfo& fn,
                                                 const BindModule& module,
                                                 const SubjectTypeDescriptor* descriptor) const
{
    if (!descriptor) {
        emitLegacyCallableBody(out, fn, module);
        return;
    }

    const auto params = extractParameters(fn);
    const auto returnMeta = analyzeReturnMetadata(fn, module);

    out << fn.c_signature << " {\n";
    out << "    // Dispatch family: method_table (" << descriptor->subject_kind << ")\n";

    if (const ParameterInfo* subject = findSubjectParameter(params)) {
        emitSubjectGuard(out, fn, *subject, returnMeta);
    }

    emitInvokeCallableCore(out, fn, params, returnMeta, /*wrapEventHandles=*/false);
}

void CAPI_BindGenerator::emitSingletonFunction(std::ofstream& out,
                                                const FunctionInfo& fn,
                                                const BindModule& module,
                                                const SubjectTypeDescriptor* descriptor) const
{
    const auto params = extractParameters(fn);
    const auto returnMeta = analyzeReturnMetadata(fn, module);

    out << fn.c_signature << " {\n";
    if (descriptor) {
        out << "    // Dispatch family: singleton (" << descriptor->subject_kind << ")\n";
    }

    if (const ParameterInfo* subject = findSubjectParameter(params)) {
        emitSubjectGuard(out, fn, *subject, returnMeta);
    }

    emitInvokeCallableCore(out, fn, params, returnMeta, /*wrapEventHandles=*/false);
}

void CAPI_BindGenerator::emitEventRouterFunction(std::ofstream& out,
                                                 const FunctionInfo& fn,
                                                 const BindModule& module,
                                                 const SubjectTypeDescriptor* descriptor) const
{
    const auto params = extractParameters(fn);
    const auto returnMeta = analyzeReturnMetadata(fn, module);

    out << fn.c_signature << " {\n";
    if (descriptor) {
        out << "    // Dispatch family: event_router (" << descriptor->subject_kind << ")\n";
    }

    if (const ParameterInfo* subject = findSubjectParameter(params)) {
        emitSubjectGuard(out, fn, *subject, returnMeta);
    }

    emitInvokeCallableCore(out, fn, params, returnMeta, /*wrapEventHandles=*/true);
}

void CAPI_BindGenerator::emitCustomFunction(std::ofstream& out,
                                            const FunctionInfo& fn,
                                            const BindModule& module,
                                            const SubjectTypeDescriptor* descriptor) const
{
    if (descriptor) {
        auto it = customEmitterRegistry().find(descriptor->subject_kind);
        if (it != customEmitterRegistry().end() && it->second) {
            it->second(out, fn, module, descriptor);
            return;
        }
    }

    emitLegacyCallableBody(out, fn, module);
}

void CAPI_BindGenerator::emitLegacyCallableBody(std::ofstream& out,
                                                const FunctionInfo& fn,
                                                const BindModule& module) const
{
    out << fn.c_signature << " {\n";

    const auto params = extractParameters(fn);
    const auto returnMeta = analyzeReturnMetadata(fn, module);

    emitInvokeCallableCore(out, fn, params, returnMeta, /*wrapEventHandles=*/false);
}

std::string CAPI_BindGenerator::headerPathFor(const std::string& dir, const std::string& stem)
{
    return dir + "/SDOM_CAPI_" + stem + ".h";
}

std::string CAPI_BindGenerator::sourcePathFor(const std::string& dir, const std::string& stem)
{
    return dir + "/SDOM_CAPI_" + stem + ".cpp";
}

void CAPI_BindGenerator::forEachCallableType(
    const BindModule& module,
    const std::function<void(const TypeInfo*)>& fn)
{
    if (!fn) {
        return;
    }

    auto visit = [&](const std::vector<const TypeInfo*>& bucket) {
        for (const TypeInfo* type : bucket) {
            if (!type || type->functions.empty()) {
                continue;
            }

            fn(type);
        }
    };

    visit(module.objects);
    visit(module.structs);
    visit(module.globals);
    visit(module.functions);
    visit(module.aliases);
}

bool CAPI_BindGenerator::isEnumReturnType(const std::string& normalizedType, const BindModule& module)
{
    return moduleDeclaresEnum(normalizedType, module);
}

} // namespace SDOM
