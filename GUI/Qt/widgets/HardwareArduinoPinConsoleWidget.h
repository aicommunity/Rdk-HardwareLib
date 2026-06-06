#ifndef HARDWAREARDUINOPINCONSOLEWIDGET_H
#define HARDWAREARDUINOPINCONSOLEWIDGET_H

#include <QComboBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QWidget>

#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"

class HardwareArduinoPinConsoleWidget : public QWidget {
    Q_OBJECT
public:
    explicit HardwareArduinoPinConsoleWidget(QWidget* parent = nullptr);

    void setContext(const UComponentGuiContext& context);
    void refreshFromModel();
    void applyToModel();

signals:
    void calculateRequested();
    void selectedPinChanged(int firmata_pin, int board_profile);

private slots:
    void onRowSelectionChanged();
    void onApplySelected();
    void onSetPinMode();
    void onWriteDigital();
    void onReadAnalog();
    void onWritePwm();
    void onMonitorAll();

private:
    void rebuildTable(int profile);
    void syncSelectedRowToProps();
    void emitSelectedPin();

    UComponentGuiContext Context;
    QTableWidget* Table = nullptr;
    QComboBox* PresetCombo = nullptr;
    QSpinBox* DigitalSpin = nullptr;
    QSpinBox* PwmSpin = nullptr;
    QSpinBox* ServoAngleSpin = nullptr;
    int LastProfile = -1;
};

#endif
