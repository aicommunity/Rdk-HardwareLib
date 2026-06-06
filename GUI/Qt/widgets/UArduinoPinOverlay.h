#ifndef UARDUINOPINOVERLAY_H
#define UARDUINOPINOVERLAY_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <QWidget>

class UArduinoPinOverlay : public QWidget {
    Q_OBJECT
public:
    struct PinRegion {
        QString Id;
        QString Label;
        QRectF NormalizedRect;
        QString Role;
    };

    enum class PinModeVisual { Unknown, Input, Output, Analog, Pwm };

    struct PinVisualState {
        PinModeVisual Mode = PinModeVisual::Unknown;
        int Digital = -1;
        int Analog = -1;
        bool Supported = true;
    };

    explicit UArduinoPinOverlay(QWidget* parent = nullptr);

    void setPins(const QVector<PinRegion>& pins);
    void setPinRoles(const QMap<QString, QString>& roles);
    void setPinStates(const QMap<QString, PinVisualState>& states);
    void setHighlightedIds(const QStringList& ids);
    void setSelectedId(const QString& id);
    void setInteractive(bool interactive);

signals:
    void pinClicked(const QString& pinId);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    QColor fillColorForPin(const PinRegion& pin) const;
    QString pinAt(const QPoint& pos) const;

    QVector<PinRegion> Pins;
    QMap<QString, QString> Roles;
    QMap<QString, PinVisualState> VisualStates;
    QStringList HighlightedIds;
    QString SelectedId;
    bool Interactive = false;
};

#endif
