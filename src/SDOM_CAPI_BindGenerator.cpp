#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>

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

std::vector<std::string> extractParameterNames(const FunctionInfo& fn) {
    std::vector<std::string> names;

    if (fn.c_signature.empty() || fn.c_name.empty()) {
        return names;
    }

    const auto name_pos = fn.c_signature.find(fn.c_name);
    if (name_pos == std::string::npos) {
        return names;
    }

    const auto open_paren = fn.c_signature.find('(', name_pos + fn.c_name.size());
    const auto close_paren = fn.c_signature.rfind(')');
    if (open_paren == std::string::npos || close_paren == std::string::npos || close_paren <= open_paren) {
        return names;
    }

    const std::string params = fn.c_signature.substr(open_paren + 1, close_paren - open_paren - 1);
    std::stringstream ss(params);
    std::string param;
    while (std::getline(ss, param, ',')) {
        auto name = extractParamIdentifier(param);
        if (!name.empty()) {
            names.push_back(std::move(name));
        }
    }

    return names;
}

bool isCStringReturnType(const std::string& type)
{
    const std::string trimmed = trim(type);
    return trimmed == "const char*" || trimmed == "const char *" || trimmed == "char const*" || trimmed == "char const *";
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
    out << "#include <SDOM/SDOM_DataRegistry.hpp>\n";
    out << "#include <string>\n\n";

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
                    if (line.size() < 60) {
                        line.append(60 - line.size(), ' ');
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

    forEachCallableType(module, [&](const TypeInfo* type) {
        for (const auto& fn : type->functions) {
            if (!fn.exported || fn.c_signature.empty()) {
                continue;
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

    forEachCallableType(module, [&](const TypeInfo* type) {
        for (const auto& fn : type->functions) {
            if (!fn.exported || fn.c_signature.empty()) {
                continue;
            }

            out << fn.c_signature << " {\n";

            const auto paramNames = extractParameterNames(fn);
            if (!paramNames.empty()) {
                for (const auto& name : paramNames) {
                    out << "    (void)" << name << ";\n";
                }
                out << '\n';
            }

            const std::string cReturnType = deduceCReturnType(fn);
            const std::string normalizedReturnType = normalizeTypeToken(cReturnType);
            const bool returnIsEnum = isEnumReturnType(normalizedReturnType, module);
            const std::string callExpr = "SDOM::CAPI::invokeCallable(\"" + fn.c_name + "\", {})";

            if (cReturnType == "void") {
                out << "    (void)" << callExpr << ";\n";
                out << "    return;\n";
            } else if (cReturnType == "bool") {
                out << "    const auto callResult = " << callExpr << ";\n";
                out << "    if (callResult.kind != SDOM::CAPI::CallArg::Kind::Bool) {\n";
                out << "        return false;\n";
                out << "    }\n";
                out << "    return callResult.v.b;\n";
            } else if (isCStringReturnType(cReturnType)) {
                out << "    static thread_local std::string s_result;\n";
                out << "    const auto callResult = " << callExpr << ";\n";
                out << "    if (callResult.kind != SDOM::CAPI::CallArg::Kind::CString) {\n";
                out << "        s_result.clear();\n";
                out << "        return s_result.c_str();\n";
                out << "    }\n";
                out << "    s_result = callResult.s;\n";
                out << "    return s_result.c_str();\n";
            } else if (returnIsEnum) {
                out << "    const auto callResult = " << callExpr << ";\n";
                out << "    if (callResult.kind == SDOM::CAPI::CallArg::Kind::UInt) {\n";
                out << "        return static_cast<" << normalizedReturnType << ">(callResult.v.u);\n";
                out << "    }\n";
                out << "    if (callResult.kind == SDOM::CAPI::CallArg::Kind::Int) {\n";
                out << "        return static_cast<" << normalizedReturnType << ">(callResult.v.i);\n";
                out << "    }\n";
                out << "    return static_cast<" << normalizedReturnType << ">(0);\n";
            } else {
                out << "    // TODO: marshal return type '" << cReturnType << "'.\n";
                out << "    (void)" << callExpr << ";\n";
                out << "    return {}; // placeholder\n";
            }

            out << "}\n\n";
            wrote_any = true;
        }
    });

    if (!wrote_any) {
        out << "// No callable bindings recorded for this module.\n";
    }
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

} // namespace SDOM