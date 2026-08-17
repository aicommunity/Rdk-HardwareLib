#ifndef UARDUINOPINOVERLAY_H
#define UARDUINOPINOVERLAY_H

#include <QMap>
#include <QRectF>
#include <QSizeF>
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
    /** Letterbox pin 0..1 coords into this viewBox aspect (matches SVG). Empty = fill widget. */
    void setViewBoxSize(const QSizeF& size);

signals:
    void pinClicked(const QString& pinId);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QColor fillColorForPin(const PinRegion& pin) const;
    QString pinAt(const QPoint& pos) const;
    QRectF contentRect() const;
    QRectF mappedPinRect(const PinRegion& pin) const;

    QVector<PinRegion> Pins;
    QSizeF ViewBox;
    QMap<QString, QString> Roles;
    QMap<QString, PinVisualState> VisualStates;
    QStringList HighlightedIds;
    QString SelectedId;
    QString HoverId;
    bool Interactive = false;
};

#endif
