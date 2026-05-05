#include "../../../../Rdk/GUI/Qt/UComponentFormRegistry.h"
#include "../../../../Rdk/GUI/Qt/UGenericComponentControllerWidget.h"

namespace
{
UComponentFormDescriptor MakeHardwareDescriptor(const QString& id, const QString& title)
{
    UComponentFormDescriptor descriptor;
    descriptor.formId = id;
    descriptor.title = title;
    descriptor.singleInstance = true;
    descriptor.factory = [id, title](RDK::UApplication* app) -> UVisualControllerWidget*
    {
        return new UGenericComponentControllerWidget(id, title, nullptr, app);
    };
    return descriptor;
}
}

void RegisterHardwareLibComponentGuiForms()
{
    UComponentFormRegistry& registry = UComponentFormRegistry::instance();
    registry.registerFormFactory("NHardwareDevice", MakeHardwareDescriptor("hw.device.panel", "HardwareLib: Device Panel"));
}
