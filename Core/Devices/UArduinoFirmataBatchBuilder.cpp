#include "UArduinoFirmataBatchBuilder.h"

namespace RDK {

void UArduinoFirmataBatchBuilder::clearPinConfig(MDMatrix<double>& batch)
{
    batch.Assign(0, 2, 0.0);
}

void UArduinoFirmataBatchBuilder::appendPinMode(MDMatrix<double>& batch, int pin, int mode)
{
    if (pin < 0)
        return;
    const int row = batch.GetRows();
    batch.Resize(row + 1, 2, 0.0);
    batch(row, 0) = pin;
    batch(row, 1) = mode;
}

void UArduinoFirmataBatchBuilder::appendDigital(MDMatrix<double>& batch, int pin, int value)
{
    if (pin < 0)
        return;
    const int row = batch.GetRows();
    batch.Resize(row + 1, 2, 0.0);
    batch(row, 0) = pin;
    batch(row, 1) = value ? 1.0 : 0.0;
}

void UArduinoFirmataBatchBuilder::appendAnalog(MDMatrix<double>& batch, int pin, int value)
{
    if (pin < 0)
        return;
    const int row = batch.GetRows();
    batch.Resize(row + 1, 2, 0.0);
    batch(row, 0) = pin;
    batch(row, 1) = value;
}

} // namespace RDK
