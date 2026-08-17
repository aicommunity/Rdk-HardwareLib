#ifndef HARDWAREGUIHELPERS_H
#define HARDWAREGUIHELPERS_H

#include <QString>
#include <QVector>

#include "../../../../../Rdk/GUI/Qt/UComponentGuiContext.h"

class QComboBox;
class QFormLayout;
class QLayout;
class QPlainTextEdit;
class QSerialPortInfo;
class QWidget;

namespace HardwareGuiHelpers {

/** Decode property bytes from MModel (system locale on Windows, UTF-8 elsewhere). */
QString propertyStringFromEngine(const char* value);
/** Encode property text for MModel (must match propertyStringFromEngine). */
QByteArray propertyStringToEngine(const QString& value);

/** Application font suitable for Cyrillic and other Unicode in Hardware forms. */
void applyUnicodeFriendlyFont(QWidget* widget);

void applyCompactLayout(QLayout* layout, int margin = 4, int spacing = 4);
void applyCompactForm(QFormLayout* form);
void configureExpandingCombo(QComboBox* combo, int minChars);

/** maxH <= 0 leaves the widget without a maximum height (Monitor stretch). */
QPlainTextEdit* createStatusLogWidget(QWidget* parent, int minH = 48, int maxH = 120);
void setStatusLogText(QPlainTextEdit* widget, const QString& text);

QString serialPortDevicePath(const QSerialPortInfo& info);
QStringList listSerialPortDevicePaths();
void populateSerialPortCombo(QComboBox* combo, const QString& select_device_path = QString());
QString selectedSerialPortPath(const QComboBox* combo);
void selectSerialPortInCombo(QComboBox* combo, const QString& device_path);

QString getProp(const UComponentGuiContext& ctx, const char* name);
bool setProp(const UComponentGuiContext& ctx, const char* name, const QString& value);
int getPropInt(const UComponentGuiContext& ctx, const char* name, int default_value = 0);
bool getPropBool(const UComponentGuiContext& ctx, const char* name, bool default_value = false);
bool getMatrixPreview(const UComponentGuiContext& ctx,
                      const char* property_name,
                      int max_rows,
                      int max_cols,
                      QVector<QVector<double>>* out_rows);
void envReset(const UComponentGuiContext& ctx);
void envCalculate(const UComponentGuiContext& ctx);
void pulseEdge(const UComponentGuiContext& ctx, const char* edge_name);

} // namespace HardwareGuiHelpers

#endif
