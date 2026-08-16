#ifndef UARDUINOASSEMBLYVIEWWIDGET_H
#define UARDUINOASSEMBLYVIEWWIDGET_H

#include "Assembly/UArduinoAssemblySceneBuilder.h"

#include <QGraphicsView>

namespace RDK {
class UHardwareCatalog;
}

class UArduinoAssemblyViewWidget : public QGraphicsView {
    Q_OBJECT
public:
    explicit UArduinoAssemblyViewWidget(QWidget* parent = nullptr);

    void setCatalog(const RDK::UHardwareCatalog* catalog);
    void setSetup(const RDK::UHwSetupDocument& doc);
    void setIssues(const QVector<RDK::UHwIssue>& issues);
    void rebuild();
    bool exportSvg(const QString& path, QString* error = nullptr) const;

signals:
    void portClicked(const QString& portId);
    void deviceClicked(const QString& deviceId);

private:
    const RDK::UHardwareCatalog* Catalog = nullptr;
    RDK::UHwSetupDocument Setup;
    QVector<RDK::UHwIssue> Issues;
    RDK::UAssemblySceneModel Model;
};

#endif
