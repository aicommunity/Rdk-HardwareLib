#include "RegisterHardwareLibLlmTools.h"

#include <filesystem>

#include "../../../Rdk/LLM/Core/Context/ILLMProjectContextProvider.h"
#include "../../../Rdk/LLM/Core/Context/UDocSearchIndex.h"
#include "../../../Rdk/LLM/Core/Tools/ULLMToolRegistry.h"

namespace fs = std::filesystem;

namespace {

RDK::LLM::LLMToolDefinition makeDef(const std::string& name, const std::string& desc,
                                    nlohmann::json input)
{
    RDK::LLM::LLMToolDefinition d;
    d.name = name;
    d.kind = RDK::LLM::LLMToolKind::Read;
    d.description = desc;
    d.input_schema = std::move(input);
    d.output_schema = {{"type", "object"}};
    return d;
}

fs::path hardwareDocsRoot(RDK::LLM::ILLMProjectContextProvider* ctx)
{
    if(!ctx)
        return {};
    return ctx->paths().repository_root / "Libraries" / "Rdk-HardwareLib" / "Docs";
}

} // namespace

void RegisterHardwareLibLlmTools(RDK::LLM::ULLMToolRegistry& registry,
                                 RDK::LLM::ILLMProjectContextProvider* project_context)
{
    registry.registerTool(
        makeDef("search_hardware_docs",
                "Search Rdk-HardwareLib documentation (Arduino, Firmata, firmware)",
                {{"type", "object"},
                 {"required", {"query"}},
                 {"properties",
                  {{"query", {{"type", "string"}}}, {"top_k", {{"type", "integer"}}}}},
                 {"additionalProperties", false}}),
        [project_context](const nlohmann::json& args) -> RDK::LLM::ToolGatewayResult {
            RDK::LLM::ToolGatewayResult r;
            const std::string query = args.at("query").get<std::string>();
            const int top_k = args.value("top_k", 5);
            const fs::path root = hardwareDocsRoot(project_context);
            std::vector<fs::path> roots;
            if(!root.empty())
                roots.push_back(root);
            auto snippets = RDK::LLM::searchDocsWithIndex(roots, query, top_k);
            r.result["snippets"] = nlohmann::json::array();
            r.result["library"] = "Rdk-HardwareLib";
            for(const auto& s : snippets)
            {
                r.result["snippets"].push_back(
                    {{"path", s.path}, {"title", s.title}, {"excerpt", s.excerpt}, {"score", s.score}});
            }
            r.ok = true;
            return r;
        });

    registry.registerTool(
        makeDef("list_hardware_component_classes",
                "Lists hardware library component class names and short descriptions",
                {{"type", "object"}, {"additionalProperties", false}}),
        [](const nlohmann::json& args) -> RDK::LLM::ToolGatewayResult {
            (void)args;
            RDK::LLM::ToolGatewayResult r;
            r.result["classes"] = nlohmann::json::array({
                {{"class_name", "ArduinoBoard"}, {"summary", "Arduino board transport node"}},
                {{"class_name", "ArduinoSensorSketch"}, {"summary", "Custom sensor sketch link"}},
                {{"class_name", "ArduinoFirmata"}, {"summary", "Standard Firmata protocol"}},
                {{"class_name", "ArduinoAdc"}, {"summary", "ADC via linked Firmata"}},
                {{"class_name", "ArduinoDcDemo"}, {"summary", "DC demo (deprecated sketch link)"}},
            });
            r.result["docs_hint"] = "Libraries/Rdk-HardwareLib/Docs/Component-Catalog.md";
            r.ok = true;
            return r;
        });
}
