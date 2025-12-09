#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <utility>
#include <unordered_map>
#include <unordered_set>

#include <SDOM/SDOM_CAPI_BindGenerator.hpp>
#include <SDOM/SDOM_DataRegistry.hpp>

namespace SDOM {

namespace {

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
        { "SDOM_Event", "#include <SDOM/CAPI/SDOM_CAPI_Event.h>" },
        { "SDOM_Variant", "#include <SDOM/CAPI/SDOM_CAPI_Variant.h>" }
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

    out << "#pragma once\n";
    out << "// Auto-generated SDOM C API module: " << module.file_stem << "\n\n";

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
    if (moduleNeedsCstdint(module) || module.file_stem == "Handles") {
        addInclude("#include <stdint.h>");
    }
    if (module.file_stem == "Core") {
        addInclude("#include <SDL3/SDL.h>");
    }
    if (module.file_stem == "Event") {
        addInclude("#include <SDOM/CAPI/SDOM_CAPI_Variant.h>");
    }
    if (module.file_stem == "Variant") {
        addInclude("#include <SDOM/CAPI/SDOM_CAPI_Handles.h>");
    }

    const auto dependencyIncludes = collectDependencyIncludes(module);
    for (const auto& include_line : dependencyIncludes) {
        if (module.file_stem == "Handles") {
            // Avoid recursive self-include and keep the header lightweight to
            // prevent circular dependence with Variant.
            if (include_line.find("SDOM_CAPI_Handles.h") != std::string::npos) {
                continue;
            }
            if (include_line.find("SDOM_CAPI_Variant.h") != std::string::npos) {
                continue;
            }
        }
        addInclude(include_line);
    }

    if (!includes.empty()) {
        for (const auto& include_line : includes) {
            out << include_line << '\n';
        }
        out << '\n';
    }

    // Special-case Variant: emit the VariantType constants up front so both
    // struct definitions and function stubs can reference them without
    // duplicating fallback enums in sources.
    if (module.file_stem == "Variant") {
        out << "typedef enum SDOM_VariantType {\n";
        out << "    SDOM_VARIANT_TYPE_NULL   = 0,\n";
        out << "    SDOM_VARIANT_TYPE_BOOL   = 1,\n";
        out << "    SDOM_VARIANT_TYPE_INT    = 2,\n";
        out << "    SDOM_VARIANT_TYPE_FLOAT  = 3,\n";
        out << "    SDOM_VARIANT_TYPE_STRING = 4,\n";
        out << "    SDOM_VARIANT_TYPE_ARRAY  = 5,\n";
        out << "    SDOM_VARIANT_TYPE_OBJECT = 6,\n";
        out << "    SDOM_VARIANT_TYPE_DYNAMIC= 7,\n";
        out << "    SDOM_VARIANT_TYPE_DISPLAY_HANDLE = 8,\n";
        out << "    SDOM_VARIANT_TYPE_ASSET_HANDLE   = 9,\n";
        out << "    SDOM_VARIANT_TYPE_EVENT          = 10\n";
        out << "} SDOM_VariantType;\n\n";
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

    const bool needsExtern = moduleHasFunctions(module) || !module.structs.empty();
    if (needsExtern) {
        out << "#ifdef __cplusplus\n";
        out << "extern \"C\" {\n";
        out << "#endif\n\n";
    }

    if (module.file_stem == "Handles") {
        out << "typedef struct SDOM_DisplayHandle {\n";
        out << "    uint64_t object_id;\n";
        out << "    const char* name;\n";
        out << "    const char* type;\n";
        out << "} SDOM_DisplayHandle;\n\n";

        out << "typedef struct SDOM_AssetHandle {\n";
        out << "    uint64_t object_id;\n";
        out << "    const char* name;\n";
        out << "    const char* type;\n";
        out << "} SDOM_AssetHandle;\n\n";

        out << "typedef struct SDOM_Variant SDOM_Variant;\n";
        out << "typedef SDOM_Variant SDOM_Handle_Variant;\n\n";

        out << "SDOM_Variant SDOM_MakeDisplayHandle(const SDOM_DisplayHandle* handle);\n";
        out << "SDOM_Variant SDOM_MakeAssetHandle(const SDOM_AssetHandle* handle);\n";
        out << "bool SDOM_Handle_IsValid(const SDOM_Handle_Variant* handle);\n";
        out << "bool SDOM_Handle_IsDisplay(const SDOM_Handle_Variant* handle);\n";
        out << "bool SDOM_Handle_IsAsset(const SDOM_Handle_Variant* handle);\n\n";
    }

    emitStructs(out, module);

    if (moduleHasFunctions(module)) {
        emitFunctionPrototypes(out, module);
    }

    if (needsExtern) {
        out << "#ifdef __cplusplus\n";
        out << "} // extern \"C\"\n";
        out << "#endif\n";
    }
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

    out << "#include <SDOM/CAPI/SDOM_CAPI_" << module.file_stem << ".h>\n";
    out << "#include <SDOM/CAPI/SDOM_CAPI_Core.h>\n";
    if (module.file_stem == "Handles") {
        out << "#include <SDOM/CAPI/SDOM_CAPI_Variant.h>\n";
    }
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

        if (!type->doc.empty()) {
            std::string docLine = "// " + type->doc;
            std::replace(docLine.begin(), docLine.end(), '\n', ' ');
            out << docLine << '\n';
        }

        out << "typedef struct " << export_name << " {\n";

        if (type->properties.empty()) {
            out << "    void* _opaque; ///< Placeholder for opaque struct.\n";
        } else {
            struct FieldLine {
                std::string line;
                std::string comment;
            };
            std::vector<FieldLine> rendered;
            rendered.reserve(type->properties.size());

            std::size_t longest = 0;

            for (const auto& prop : type->properties) {
                std::string raw_cpp_type = trim(prop.cpp_type);
                std::string array_suffix;
                const auto bracket_pos = raw_cpp_type.find('[');
                if (bracket_pos != std::string::npos && raw_cpp_type.back() == ']') {
                    array_suffix = raw_cpp_type.substr(bracket_pos);
                    raw_cpp_type = trim(raw_cpp_type.substr(0, bracket_pos));
                }

                const std::string field_type = cFriendlyType(raw_cpp_type);
                std::string line = "    " + field_type + " " + prop.name;
                if (!array_suffix.empty()) {
                    line += array_suffix;
                }

                if (!prop.default_value || prop.default_value->empty()) {
                    line += ";";
                } else {
                    line += " = " + *prop.default_value + ";";
                }

                std::string comment;
                if (!prop.doc.empty() || field_type == "void*" || field_type == "const void*") {
                    comment = prop.doc;
                    if (comment.empty() && prop.cpp_type.find("SDOM::") != std::string::npos) {
                        comment = "Pointer to underlying C++ type (" + prop.cpp_type + ")";
                    }
                    std::replace(comment.begin(), comment.end(), '\n', ' ');
                }

                longest = std::max(longest, line.size());
                rendered.push_back(FieldLine{std::move(line), std::move(comment)});
            }

            // Align comments to the next tab stop after (longest + 2)
            constexpr std::size_t kTabStop = 4;
            const std::size_t baseWidth = longest + 2;
            const std::size_t alignmentWidth = ((baseWidth + kTabStop - 1) / kTabStop) * kTabStop;

            for (const auto& fld : rendered) {
                if (fld.comment.empty()) {
                    out << fld.line << '\n';
                    continue;
                }

                std::string padded = fld.line;
                if (padded.size() < alignmentWidth) {
                    padded.append(alignmentWidth - padded.size(), ' ');
                } else {
                    padded.push_back(' ');
                }

                out << padded << "///< " << fld.comment << '\n';
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
