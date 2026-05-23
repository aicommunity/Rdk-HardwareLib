#ifndef HARDWAREARDUINOPINCONSOLEWIDGET_H
#define HARDWAREARDUINOPINCONSOLEWIDGET_H

#include <QComboBox>
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

private slots:
    void onRowSelectionChanged();
    void onApplySelected();
    void onSetPinMode();
    void onWriteDigital();
    void onReadAnalog();
    void onMonitorAll();

private:
    void rebuildTable(int profile);
    void syncSelectedRowToProps();

    UComponentGuiContext Context;
    QTableWidget* Table = nullptr;
    QComboBox* PresetCombo = nullptr;
    int LastProfile = -1;
};

#endif
