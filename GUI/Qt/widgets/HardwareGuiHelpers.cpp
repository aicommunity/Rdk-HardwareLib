#include "HardwareGuiHelpers.h"

#include "../../../../../Rdk/Core/Math/MDMatrix.h"
#include "../../../../../Rdk/Deploy/Include/rdk_init.h"
#include "../../../Core/Transport/UArduinoSerialPortUtil.h"

#include <QColor>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QtSerialPort/QSerialPortInfo>

namespace HardwareGuiHelpers {

QPlainTextEdit* createStatusLogWidget(QWidget* parent)
{
    auto* edit = new QPlainTextEdit(parent);
    edit->setReadOnly(true);
    edit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    edit->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    edit->setTabChangesFocus(true);
    edit->setMinimumHeight(72);
    edit->setMaximumHeight(180);
    edit->setPlaceholderText(QObject::tr("Status and log messages (Ctrl+C to copy selection)"));
    return edit;
}

void setStatusLogText(QPlainTextEdit* widget, const QString& text)
{
    if (!widget)
        return;
    if (widget->toPlainText() == text)
        return;
    widget->setPlainText(text);
}

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

void populateSerialPortCombo(QComboBox* combo, const QString& select_device_path)
{
    if (!combo)
        return;

    QString selected = select_device_path;
    if (selected.isEmpty())
        selected = selectedSerialPortPath(combo);

    combo->clear();
    for (const RDK::UArduinoSerialPortEntry& entry : RDK::UArduinoSerialPortUtil::listPortsSorted()) {
        combo->addItem(entry.DisplayLabel, entry.DevicePath);
        const int idx = combo->count() - 1;
        if (!entry.LikelyAttachedDevice)
            combo->setItemData(idx, QColor(128, 128, 128), Qt::ForegroundRole);
    }
    selectSerialPortInCombo(combo, selected);
}

QString selectedSerialPortPath(const QComboBox* combo)
{
    if (!combo)
        return QString();
    const QVariant data = combo->currentData();
    if (data.isValid() && !data.toString().isEmpty())
        return data.toString();
    return RDK::UArduinoSerialPortUtil::normalizeDevicePath(combo->currentText());
}

void selectSerialPortInCombo(QComboBox* combo, const QString& device_path)
{
    if (!combo)
        return;
    if (device_path.isEmpty()) {
        if (combo->count() > 0)
            combo->setCurrentIndex(0);
        return;
    }

    const QString normalized = RDK::UArduinoSerialPortUtil::normalizeDevicePath(device_path);
    for (int i = 0; i < combo->count(); ++i) {
        const QString item_path = combo->itemData(i).toString();
        if (item_path == normalized || item_path == device_path) {
            combo->setCurrentIndex(i);
            return;
        }
    }

    combo->insertItem(0,
                      QObject::tr("%1 (not in list)").arg(normalized),
                      normalized);
    combo->setItemData(0, QColor(160, 120, 60), Qt::ForegroundRole);
    combo->setCurrentIndex(0);
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

int getPropInt(const UComponentGuiContext& ctx, const char* name, int default_value)
{
    bool ok = false;
    const int v = getProp(ctx, name).toInt(&ok);
    return ok ? v : default_value;
}

bool getPropBool(const UComponentGuiContext& ctx, const char* name, bool default_value)
{
    const QString v = getProp(ctx, name);
    if (v.isEmpty())
        return default_value;
    return v == QLatin1String("1") || v.compare(QLatin1String("true"), Qt::CaseInsensitive) == 0;
}

bool getMatrixPreview(const UComponentGuiContext& ctx,
                      const char* property_name,
                      const int max_rows,
                      const int max_cols,
                      QVector<QVector<double>>* out_rows)
{
    if (!out_rows || ctx.componentLongName.isEmpty())
        return false;
    out_rows->clear();

    const void* raw = MModel_GetComponentPropertyData(ctx.channelIndex,
                                                     ctx.componentLongName.toUtf8().constData(),
                                                     property_name);
    const auto* matrix = static_cast<const RDK::MDMatrix<double>*>(raw);
    if (!matrix || matrix->GetRows() <= 0 || matrix->GetCols() <= 0)
        return false;

    const int rows = qMin(matrix->GetRows(), max_rows);
    const int cols = qMin(matrix->GetCols(), max_cols);
    out_rows->reserve(rows);
    for (int r = 0; r < rows; ++r) {
        QVector<double> row;
        row.reserve(cols);
        for (int c = 0; c < cols; ++c)
            row.append(matrix->operator()(r, c));
        out_rows->append(row);
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

void pulseEdge(const UComponentGuiContext& ctx, const char* edge_name)
{
    setProp(ctx, edge_name, QStringLiteral("1"));
    envCalculate(ctx);
}

} // namespace HardwareGuiHelpers
