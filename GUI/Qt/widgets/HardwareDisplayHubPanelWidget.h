#ifndef HARDWAREDISPLAYHUBPANELWIDGET_H
#define HARDWAREDISPLAYHUBPANELWIDGET_H

#include <QWidget>

class QLineEdit;
class QSpinBox;

class HardwareDisplayHubPanelWidget : public QWidget {
    Q_OBJECT
public:
    explicit HardwareDisplayHubPanelWidget(QWidget* parent = nullptr);

signals:
    void commandRequested(const QString& line);

private:
    QSpinBox* RowSpin = nullptr;
    QSpinBox* ColSpin = nullptr;
    QLineEdit* TextEdit = nullptr;
};

#endif
