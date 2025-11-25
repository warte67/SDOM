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

std::string emitCallArgForParameter(const ParameterInfo& param)
{
    const std::string trimmedType = trim(param.type);
    if (trimmedType.empty()) {
        return {};
    }

    if (isCStringType(trimmedType)) {
        return "SDOM::CAPI::CallArg::makeCString(" + param.name + ")";
    }

    const bool isPointer = trimmedType.find('*') != std::string::npos;
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

    static const std::unordered_set<std::string> kUnsignedTypes = {
        "std::uint8_t", "std::uint16_t", "std::uint32_t", "std::uint64_t",
        "uint8_t", "uint16_t", "uint32_t", "uint64_t",
        "unsigned", "unsigned int", "unsigned long", "unsigned long long",
        "size_t"
    };

    if (kUnsignedTypes.count(normalized)) {
        return "SDOM::CAPI::CallArg::makeUInt(static_cast<std::uint64_t>(" + param.name + "))";
    }

    static const std::unordered_set<std::string> kSignedTypes = {
        "std::int8_t", "std::int16_t", "std::int32_t", "std::int64_t",
        "int8_t", "int16_t", "int32_t", "int64_t",
        "int", "long", "long long"
    };

    if (kSignedTypes.count(normalized)) {
        return "SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(" + param.name + "))";
    }

    if (normalized == "float" || normalized == "double") {
        return "SDOM::CAPI::CallArg::makeDouble(static_cast<double>(" + param.name + "))";
    }

    return "SDOM::CAPI::CallArg::makeInt(static_cast<std::int64_t>(" + param.name + "))";
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
                            const ReturnMetadata& meta)
{
    const bool hasArgs = !params.empty();
    if (hasArgs) {
        out << "    std::vector<SDOM::CAPI::CallArg> args;\n";
        out << "    args.reserve(" << params.size() << ");\n";
        for (const auto& param : params) {
            out << "    args.push_back(" << emitCallArgForParameter(param) << ");\n";
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
    } else if (meta.is_enum) {
        out << "    const auto callResult = " << callExpr << ";\n";
        out << "    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {\n";
        out << "        return static_cast<" << meta.normalized << ">(callResult.v.u);\n";
        out << "    }\n";
        out << "    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {\n";
        out << "        return static_cast<" << meta.normalized << ">(callResult.v.i);\n";
        out << "    }\n";
        out << "    return static_cast<" << meta.normalized << ">(0);\n";
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

    if (moduleNeedsCstdint(module)) {
        out << "#include <cstdint> // For uint64_t\n\n";
    }

    emitEnums(out, module);

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
    out << "#include <SDOM/SDOM_CAPI.h>\n";
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

    emitInvokeCallableCore(out, fn, params, returnMeta);
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

    emitInvokeCallableCore(out, fn, params, returnMeta);
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

    emitInvokeCallableCore(out, fn, params, returnMeta);
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

    emitInvokeCallableCore(out, fn, params, returnMeta);
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