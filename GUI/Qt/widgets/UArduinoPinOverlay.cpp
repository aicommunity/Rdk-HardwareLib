#include "UArduinoPinOverlay.h"

#include <QMouseEvent>
#include <QPainter>

UArduinoPinOverlay::UArduinoPinOverlay(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_TranslucentBackground);
}

void UArduinoPinOverlay::setPins(const QVector<PinRegion>& pins)
{
    Pins = pins;
    update();
}

void UArduinoPinOverlay::setPinRoles(const QMap<QString, QString>& roles)
{
    Roles = roles;
    update();
}

void UArduinoPinOverlay::setHighlightedIds(const QStringList& ids)
{
    HighlightedIds = ids;
    update();
}

void UArduinoPinOverlay::setSelectedId(const QString& id)
{
    SelectedId = id;
    update();
}

void UArduinoPinOverlay::setInteractive(bool interactive)
{
    Interactive = interactive;
    setCursor(interactive ? Qt::PointingHandCursor : Qt::ArrowCursor);
}

QString UArduinoPinOverlay::pinAt(const QPoint& pos) const
{
    const qreal w = width();
    const qreal h = height();
    if (w <= 0 || h <= 0)
        return {};

    for (const PinRegion& pin : Pins) {
        const QRectF r(pin.NormalizedRect.x() * w,
                       pin.NormalizedRect.y() * h,
                       pin.NormalizedRect.width() * w,
                       pin.NormalizedRect.height() * h);
        if (r.contains(pos))
            return pin.Id;
    }
    return {};
}

void UArduinoPinOverlay::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    if (Pins.isEmpty())
        return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const qreal w = width();
    const qreal h = height();
    for (const PinRegion& pin : Pins) {
        const QRectF r(pin.NormalizedRect.x() * w,
                       pin.NormalizedRect.y() * h,
                       pin.NormalizedRect.width() * w,
                       pin.NormalizedRect.height() * h);

        QColor fill(80, 140, 220, 90);
        if (HighlightedIds.contains(pin.Id))
            fill = QColor(255, 180, 40, 140);
        if (pin.Id == SelectedId)
            fill = QColor(60, 200, 90, 160);

        p.setPen(QPen(fill.darker(130), 1.5));
        p.setBrush(fill);
        p.drawRoundedRect(r, 3, 3);

        const QString role = Roles.value(pin.Id);
        const QString text =
            role.isEmpty() ? pin.Label : QStringLiteral("%1\n%2").arg(pin.Label, role);
        p.setPen(Qt::white);
        p.drawText(r.adjusted(2, 2, -2, -2), Qt::AlignCenter, text);
    }
}

void UArduinoPinOverlay::mousePressEvent(QMouseEvent* event)
{
    if (!Interactive || event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }
    const QString id = pinAt(event->pos());
    if (!id.isEmpty())
        emit pinClicked(id);
    QWidget::mousePressEvent(event);
}
