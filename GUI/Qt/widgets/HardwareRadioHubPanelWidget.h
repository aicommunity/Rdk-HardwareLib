#ifndef HARDWARERADIOHUBPANELWIDGET_H
#define HARDWARERADIOHUBPANELWIDGET_H

#include "HardwareGuiHelpers.h"

#include <QWidget>

class QCheckBox;
class QLabel;
class QLineEdit;

class HardwareRadioHubPanelWidget : public QWidget {
    Q_OBJECT
public:
    explicit HardwareRadioHubPanelWidget(QWidget* parent = nullptr);
    void setContext(const UComponentGuiContext& context);
    void refreshFromModel();
    bool esp32BoardMode() const;

signals:
    void commandRequested(const QString& line);
    void esp32ModeChanged(bool enabled);

private:
    UComponentGuiContext Context;
    QLineEdit* HexEdit = nullptr;
    QLabel* LastRx = nullptr;
    QLabel* UidLabel = nullptr;
    QCheckBox* Esp32ModeCheck = nullptr;
};

#endif
