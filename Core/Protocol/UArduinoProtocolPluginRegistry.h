#ifndef UARDUINOPROTOCOLPLUGINREGISTRY_H
#define UARDUINOPROTOCOLPLUGINREGISTRY_H

#include "IArduinoProtocolPlugin.h"

namespace RDK {

void registerArduinoProtocolPlugin(IArduinoProtocolPlugin* plugin);
IArduinoProtocolPlugin* findArduinoProtocolPlugin(const QString& hostPluginOrFirmwareId);
QStringList registeredArduinoProtocolPluginIds();
void registerBuiltinArduinoProtocolPlugins();

} // namespace RDK

#endif
