#ifndef HARDWAREPIXELHUBPANELWIDGET_H
#define HARDWAREPIXELHUBPANELWIDGET_H

#include <QWidget>

class QLineEdit;
class QSpinBox;

class HardwarePixelHubPanelWidget : public QWidget {
    Q_OBJECT
public:
    explicit HardwarePixelHubPanelWidget(QWidget* parent = nullptr);

signals:
    void commandRequested(const QString& line);

private:
    QSpinBox* LedIndex = nullptr;
    QSpinBox* RSpin = nullptr;
    QSpinBox* GSpin = nullptr;
    QSpinBox* BSpin = nullptr;
    QSpinBox* Color565 = nullptr;
    QLineEdit* TextEdit = nullptr;
};

#endif
