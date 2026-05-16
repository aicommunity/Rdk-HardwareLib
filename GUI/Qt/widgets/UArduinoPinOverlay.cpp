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
    m_pins = pins;
    update();
}

void UArduinoPinOverlay::setPinRoles(const QMap<QString, QString>& roles)
{
    m_roles = roles;
    update();
}

void UArduinoPinOverlay::setHighlightedIds(const QStringList& ids)
{
    m_highlighted = ids;
    update();
}

void UArduinoPinOverlay::setSelectedId(const QString& id)
{
    m_selectedId = id;
    update();
}

void UArduinoPinOverlay::setInteractive(bool interactive)
{
    m_interactive = interactive;
    setCursor(interactive ? Qt::PointingHandCursor : Qt::ArrowCursor);
}

QString UArduinoPinOverlay::pinAt(const QPoint& pos) const
{
    const qreal w = width();
    const qreal h = height();
    if (w <= 0 || h <= 0)
        return {};

    for (const PinRegion& pin : m_pins) {
        const QRectF r(pin.normalizedRect.x() * w,
                       pin.normalizedRect.y() * h,
                       pin.normalizedRect.width() * w,
                       pin.normalizedRect.height() * h);
        if (r.contains(pos))
            return pin.id;
    }
    return {};
}

void UArduinoPinOverlay::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    if (m_pins.isEmpty())
        return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const qreal w = width();
    const qreal h = height();
    for (const PinRegion& pin : m_pins) {
        const QRectF r(pin.normalizedRect.x() * w,
                       pin.normalizedRect.y() * h,
                       pin.normalizedRect.width() * w,
                       pin.normalizedRect.height() * h);

        QColor fill(80, 140, 220, 90);
        if (m_highlighted.contains(pin.id))
            fill = QColor(255, 180, 40, 140);
        if (pin.id == m_selectedId)
            fill = QColor(60, 200, 90, 160);

        p.setPen(QPen(fill.darker(130), 1.5));
        p.setBrush(fill);
        p.drawRoundedRect(r, 3, 3);

        const QString role = m_roles.value(pin.id);
        const QString text = role.isEmpty() ? pin.label : QStringLiteral("%1\n%2").arg(pin.label, role);
        p.setPen(Qt::white);
        p.drawText(r.adjusted(2, 2, -2, -2), Qt::AlignCenter, text);
    }
}

void UArduinoPinOverlay::mousePressEvent(QMouseEvent* event)
{
    if (!m_interactive || event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }
    const QString id = pinAt(event->pos());
    if (!id.isEmpty())
        emit pinClicked(id);
    QWidget::mousePressEvent(event);
}
