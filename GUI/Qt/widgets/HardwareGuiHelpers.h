#ifndef HARDWAREGUIHELPERS_H
#define HARDWAREGUIHELPERS_H

#include <QString>
#include <QVector>

#include "../../../../../Rdk/GUI/Qt/UComponentGuiContext.h"

namespace HardwareGuiHelpers {

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
