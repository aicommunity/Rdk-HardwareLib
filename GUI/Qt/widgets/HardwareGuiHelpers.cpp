#include "HardwareGuiHelpers.h"

#include "../../../../../Rdk/Deploy/Include/rdk_init.h"

namespace HardwareGuiHelpers {

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

void envReset(const UComponentGuiContext& ctx)
{
    MEnv_Reset(ctx.channelIndex, ctx.componentLongName.toUtf8().constData());
}

void envCalculate(const UComponentGuiContext& ctx)
{
    MEnv_Calculate(ctx.channelIndex, ctx.componentLongName.toUtf8().constData());
}

} // namespace HardwareGuiHelpers
