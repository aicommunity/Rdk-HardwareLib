#include "HardwareGuiHelpers.h"

#include "../../../../../Rdk/Core/Math/MDMatrix.h"
#include "../../../../../Rdk/Deploy/Include/rdk_init.h"
#include "../../../Core/Transport/UArduinoSerialPortUtil.h"

#include <QtSerialPort/QSerialPortInfo>

namespace HardwareGuiHelpers {

QString serialPortDevicePath(const QSerialPortInfo& info)
{
#if defined(Q_OS_WIN)
    return info.portName();
#else
    if (!info.systemLocation().isEmpty())
        return info.systemLocation();
    return RDK::UArduinoSerialPortUtil::normalizeDevicePath(info.portName());
#endif
}

QStringList listSerialPortDevicePaths()
{
    return RDK::UArduinoSerialPortUtil::listAvailableDevicePaths();
}

QString getProp(const UComponentGuiContext& ctx, const char* name)
{
    const char* v = MModel_GetComponentPropertyValue(ctx.channelIndex,
                                                     ctx.componentLongName.toUtf8().constData(),
                                                     name);
    return v ? QString::fromUtf8(v) : QString();
}

bool setProp(const UComponentGuiContext& ctx, const char* name, const QString& value)
{
    return MModel_SetComponentPropertyValue(ctx.channelIndex,
                                            ctx.componentLongName.toUtf8().constData(),
                                            name,
                                            value.toUtf8().constData()) == 0;
}

int getPropInt(const UComponentGuiContext& ctx, const char* name, int defaultValue)
{
    bool ok = false;
    const int v = getProp(ctx, name).toInt(&ok);
    return ok ? v : defaultValue;
}

bool getPropBool(const UComponentGuiContext& ctx, const char* name, bool defaultValue)
{
    const QString v = getProp(ctx, name);
    if (v.isEmpty())
        return defaultValue;
    return v == QLatin1String("1") || v.compare(QLatin1String("true"), Qt::CaseInsensitive) == 0;
}

bool getMatrixPreview(const UComponentGuiContext& ctx,
                      const char* propertyName,
                      const int maxRows,
                      const int maxCols,
                      QVector<QVector<double>>* outRows)
{
    if (!outRows || ctx.componentLongName.isEmpty())
        return false;
    outRows->clear();

    const void* raw = MModel_GetComponentPropertyData(ctx.channelIndex,
                                                     ctx.componentLongName.toUtf8().constData(),
                                                     propertyName);
    const auto* matrix = static_cast<const RDK::MDMatrix<double>*>(raw);
    if (!matrix || matrix->GetRows() <= 0 || matrix->GetCols() <= 0)
        return false;

    const int rows = qMin(matrix->GetRows(), maxRows);
    const int cols = qMin(matrix->GetCols(), maxCols);
    outRows->reserve(rows);
    for (int r = 0; r < rows; ++r) {
        QVector<double> row;
        row.reserve(cols);
        for (int c = 0; c < cols; ++c)
            row.append(matrix->operator()(r, c));
        outRows->append(row);
    }
    return true;
}

void envReset(const UComponentGuiContext& ctx)
{
    MEnv_Reset(ctx.channelIndex, ctx.componentLongName.toUtf8().constData());
}

void envCalculate(const UComponentGuiContext& ctx)
{
    MEnv_Calculate(ctx.channelIndex, ctx.componentLongName.toUtf8().constData());
}

} // namespace HardwareGuiHelpers
