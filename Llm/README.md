# Rdk-HardwareLib — LLM tools

Post-MVP library-scoped tools registered via `NmsdkLlmProjectContext::registerExtraTools`.

| Tool | Kind | Purpose |
|------|------|---------|
| `search_hardware_docs` | read | Keyword search under `Libraries/Rdk-HardwareLib/Docs` |
| `list_hardware_component_classes` | read | Arduino / Firmata component catalog |

**Write:** there is no separate HardwareLib domain write API. Use core graph tools (`add_component`, `set_property`, …) with hardware class names from `list_hardware_component_classes`, or `list_registered_classes` after the project is loaded.

Core net/component tools remain in `Rdk/LLM/Core/Tools/RegisterCoreRdkTools.cpp`.
