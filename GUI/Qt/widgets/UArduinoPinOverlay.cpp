#include "UArduinoPinOverlay.h"

#include <QEvent>
#include <QFont>
#include <QFontMetricsF>
#include <QMouseEvent>
#include <QPainter>

UArduinoPinOverlay::UArduinoPinOverlay(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
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

void UArduinoPinOverlay::setPinStates(const QMap<QString, PinVisualState>& states)
{
    VisualStates = states;
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

void UArduinoPinOverlay::setViewBoxSize(const QSizeF& size)
{
    if (ViewBox == size)
        return;
    ViewBox = size;
    update();
}

QRectF UArduinoPinOverlay::contentRect() const
{
    const QRectF host(rect());
    if (ViewBox.width() <= 0.0 || ViewBox.height() <= 0.0)
        return host;
    const QSizeF scaled = ViewBox.scaled(host.size(), Qt::KeepAspectRatio);
    QRectF r(QPointF(0, 0), scaled);
    r.moveCenter(host.center());
    return r;
}

QRectF UArduinoPinOverlay::mappedPinRect(const PinRegion& pin) const
{
    const QRectF c = contentRect();
    return QRectF(c.x() + pin.NormalizedRect.x() * c.width(),
                  c.y() + pin.NormalizedRect.y() * c.height(),
                  pin.NormalizedRect.width() * c.width(),
                  pin.NormalizedRect.height() * c.height());
}

QString UArduinoPinOverlay::pinAt(const QPoint& pos) const
{
    if (width() <= 0 || height() <= 0)
        return {};

    for (const PinRegion& pin : Pins) {
        if (mappedPinRect(pin).contains(pos))
            return pin.Id;
    }
    return {};
}

QColor UArduinoPinOverlay::fillColorForPin(const PinRegion& pin) const
{
    const PinVisualState vs = VisualStates.value(pin.Id);
    if (!vs.Supported)
        return QColor(QStringLiteral("#9e9e9e"));

    if (vs.Mode == PinModeVisual::Analog && vs.Analog >= 0) {
        const int alpha = 80 + (vs.Analog * 120) / 1023;
        QColor c(QStringLiteral("#2196f3"));
        c.setAlpha(alpha);
        return c;
    }
    if (vs.Mode == PinModeVisual::Output && vs.Digital >= 0)
        return vs.Digital ? QColor(QStringLiteral("#4caf50"))
                          : QColor(QStringLiteral("#388e3c"));
    if (vs.Mode == PinModeVisual::Input && vs.Digital >= 0)
        return vs.Digital ? QColor(QStringLiteral("#ffc107"))
                          : QColor(QStringLiteral("#827717"));
    if (vs.Mode == PinModeVisual::Pwm)
        return QColor(QStringLiteral("#7e57c2"));

    QColor fill(255, 255, 255, 20);
    if (HighlightedIds.contains(pin.Id))
        fill = QColor(255, 180, 40, 150);
    if (pin.Id == HoverId)
        fill = QColor(255, 255, 255, 90);
    return fill;
}

void UArduinoPinOverlay::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    if (Pins.isEmpty())
        return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    for (const PinRegion& pin : Pins) {
        const QRectF r = mappedPinRect(pin);

        QColor fill = fillColorForPin(pin);
        if (pin.Id == SelectedId)
            fill = QColor(60, 200, 90, 160);

        QPen pen = QPen(fill.darker(130), 1.5);
        if (pin.Id == SelectedId)
            pen = QPen(QColor(QStringLiteral("#00e676")), 3.0);

        p.setPen(pen);
        p.setBrush(fill);
        p.drawRoundedRect(r, 2, 2);

        const bool labeled = pin.Id == SelectedId || pin.Id == HoverId
            || HighlightedIds.contains(pin.Id) || !Roles.value(pin.Id).isEmpty();
        if (!labeled)
            continue;

        const QString role = Roles.value(pin.Id);
        const QString text = role.isEmpty() ? pin.Label
                                           : QStringLiteral("%1  %2").arg(pin.Label, role);
        QFont font = p.font();
        font.setPixelSize(10);
        font.setBold(true);
        p.setFont(font);
        const QRectF br = QFontMetricsF(font).boundingRect(text);
        QRectF bubble(0, 0, br.width() + 10, br.height() + 6);
        bubble.moveCenter(QPointF(r.center().x(), r.top() - bubble.height() * 0.5 - 2));
        if (bubble.top() < 0)
            bubble.moveTop(r.bottom() + 2);
        if (bubble.left() < 0)
            bubble.moveLeft(0);
        if (bubble.right() > width())
            bubble.moveRight(width());
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 0, 0, 180));
        p.drawRoundedRect(bubble, 3, 3);
        p.setPen(Qt::white);
        p.drawText(bubble, Qt::AlignCenter, text);
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

void UArduinoPinOverlay::mouseMoveEvent(QMouseEvent* event)
{
    const QString id = pinAt(event->pos());
    if (id != HoverId) {
        HoverId = id;
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void UArduinoPinOverlay::leaveEvent(QEvent* event)
{
    if (!HoverId.isEmpty()) {
        HoverId.clear();
        update();
    }
    QWidget::leaveEvent(event);
}
