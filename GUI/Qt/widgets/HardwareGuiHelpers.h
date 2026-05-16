#ifndef HARDWAREGUIHELPERS_H
#define HARDWAREGUIHELPERS_H

#include <QString>
#include <QVector>

#include "../../../../../Rdk/GUI/Qt/UComponentGuiContext.h"

class QComboBox;
class QPlainTextEdit;
class QSerialPortInfo;
class QWidget;

namespace HardwareGuiHelpers {

QPlainTextEdit* createStatusLogWidget(QWidget* parent);
void setStatusLogText(QPlainTextEdit* widget, const QString& text);

QString serialPortDevicePath(const QSerialPortInfo& info);
QStringList listSerialPortDevicePaths();
void populateSerialPortCombo(QComboBox* combo, const QString& selectDevicePath = QString());
QString selectedSerialPortPath(const QComboBox* combo);
void selectSerialPortInCombo(QComboBox* combo, const QString& devicePath);

QString getProp(const UComponentGuiContext& ctx, const char* name);
bool setProp(const UComponentGuiContext& ctx, const char* name, const QString& value);
int getPropInt(const UComponentGuiContext& ctx, const char* name, int defaultValue = 0);
bool getPropBool(const UComponentGuiContext& ctx, const char* name, bool defaultValue = false);
bool getMatrixPreview(const UComponentGuiContext& ctx,
                      const char* propertyName,
                      int maxRows,
                      int maxCols,
                      QVector<QVector<double>>* outRows);
void envReset(const UComponentGuiContext& ctx);
void envCalculate(const UComponentGuiContext& ctx);

} // namespace HardwareGuiHelpers

#endif
