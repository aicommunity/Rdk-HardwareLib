#include "UArduinoProtocolPluginRegistry.h"

#include "UNmsdkMotorHubProtocolPlugin.h"
#include "UNmsdkSensorHubProtocolPlugin.h"
#include "USensorLabProtocolPlugin.h"

#include <QMap>
#include <QMutex>

namespace RDK {

namespace {

QMutex& registryMutex()
{
    static QMutex m;
    return m;
}

QMap<QString, IArduinoProtocolPlugin*>& pluginMap()
{
    static QMap<QString, IArduinoProtocolPlugin*> map;
    return map;
}

} // namespace

void registerArduinoProtocolPlugin(IArduinoProtocolPlugin* plugin)
{
    if (!plugin)
        return;
    QMutexLocker lock(&registryMutex());
    pluginMap().insert(plugin->id(), plugin);
    for (const QString& alias : plugin->protocolIds())
        pluginMap().insert(alias, plugin);
}

IArduinoProtocolPlugin* findArduinoProtocolPlugin(const QString& hostPluginOrFirmwareId)
{
    QMutexLocker lock(&registryMutex());
    return pluginMap().value(hostPluginOrFirmwareId, nullptr);
}

QStringList registeredArduinoProtocolPluginIds()
{
    QMutexLocker lock(&registryMutex());
    QStringList ids;
    for (auto it = pluginMap().constBegin(); it != pluginMap().constEnd(); ++it) {
        if (it.key() == it.value()->id() && !ids.contains(it.key()))
            ids.append(it.key());
    }
    return ids;
}

void registerBuiltinArduinoProtocolPlugins()
{
    static bool done = false;
    if (done)
        return;
    done = true;
    registerArduinoProtocolPlugin(new USensorLabProtocolPlugin);
    registerArduinoProtocolPlugin(new UNmsdkSensorHubProtocolPlugin);
    registerArduinoProtocolPlugin(new UNmsdkMotorHubProtocolPlugin);
}

} // namespace RDK
