# Rdk-HardwareLib — LLM tools

Post-MVP library-scoped read tools registered via `NmsdkLlmProjectContext::registerExtraTools`.

| Tool | Purpose |
|------|---------|
| `search_hardware_docs` | Keyword search under `Libraries/Rdk-HardwareLib/Docs` |
| `list_hardware_component_classes` | Arduino / Firmata component catalog |

Core net/component tools remain in `Rdk/LLM/Core/Tools/RegisterCoreRdkTools.cpp`.
