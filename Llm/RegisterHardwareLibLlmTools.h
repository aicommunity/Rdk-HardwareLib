#ifndef RDK_HARDWARE_LIB_REGISTER_LLM_TOOLS_H
#define RDK_HARDWARE_LIB_REGISTER_LLM_TOOLS_H

namespace RDK::LLM {
class ILLMProjectContextProvider;
class URdkDomainAccess;
class ULLMToolRegistry;
} // namespace RDK::LLM

void RegisterHardwareLibLlmTools(RDK::LLM::ULLMToolRegistry& registry,
                                 RDK::LLM::ILLMProjectContextProvider* project_context,
                                 RDK::LLM::URdkDomainAccess& domain);

#endif
