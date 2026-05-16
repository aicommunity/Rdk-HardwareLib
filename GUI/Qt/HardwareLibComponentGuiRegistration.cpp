#include "HardwareLibComponentGuiRegistration.h"

#include "../../../../Rdk/GUI/Qt/UComponentFormRegistry.h"
#include "HardwareArduinoBoardControllerWidget.h"
#include "HardwareArduinoFirmataControllerWidget.h"
#include "HardwareArduinoSensorSketchControllerWidget.h"

namespace
{
UComponentFormDescriptor MakeDescriptor(const QString& id,
                                      const QString& title,
                                      const std::function<UVisualControllerWidget*(RDK::UApplication*)>& factory)
{
    UComponentFormDescriptor descriptor;
    descriptor.formId = id;
    descriptor.title = title;
    descriptor.singleInstance = true;
    descriptor.factory = factory;
    return descriptor;
}
}

void RegisterHardwareLibComponentGuiForms()
{
    UComponentFormRegistry& registry = UComponentFormRegistry::instance();

    registry.registerFormFactory(
        QStringLiteral("ArduinoBoard"),
        MakeDescriptor(QStringLiteral("hw.arduino.board"), QStringLiteral("Arduino Board"),
                       [](RDK::UApplication* app) {
                           return new HardwareArduinoBoardControllerWidget(nullptr, app);
                       }));

    registry.registerFormFactory(
        QStringLiteral("ArduinoSensorSketch"),
        MakeDescriptor(QStringLiteral("hw.arduino.sensor_sketch"),
                       QStringLiteral("Arduino Sensor Sketch"),
                       [](RDK::UApplication* app) {
                           return new HardwareArduinoSensorSketchControllerWidget(nullptr, app);
                       }));

    registry.registerFormFactory(
        QStringLiteral("ArduinoFirmata"),
        MakeDescriptor(QStringLiteral("hw.arduino.firmata"), QStringLiteral("Arduino Firmata"),
                       [](RDK::UApplication* app) {
                           return new HardwareArduinoFirmataControllerWidget(nullptr, app);
                       }));
}
