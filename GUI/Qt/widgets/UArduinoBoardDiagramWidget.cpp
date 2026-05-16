#include "UArduinoBoardDiagramWidget.h"

#include <QLabel>
#include <QVBoxLayout>

#if HARDWARELIB_HAS_QTSVG
#include <QSvgWidget>
#endif

UArduinoBoardDiagramWidget::UArduinoBoardDiagramWidget(QWidget* parent)
    : QWidget(parent)
{
#if HARDWARELIB_HAS_QTSVG
    m_svg = new QSvgWidget(this);
#else
    m_svgPlaceholder = new QLabel(tr("Board diagram (install Qt Svg)"), this);
    m_svgPlaceholder->setAlignment(Qt::AlignCenter);
    m_svgPlaceholder->setMinimumHeight(160);
#endif
    m_statusLabel = new QLabel(this);
    m_statusLabel->setWordWrap(true);

    auto* layout = new QVBoxLayout(this);
#if HARDWARELIB_HAS_QTSVG
    layout->addWidget(m_svg, 1);
#else
    layout->addWidget(m_svgPlaceholder, 1);
#endif
    layout->addWidget(m_statusLabel);
    setMinimumHeight(220);
    updateSvg();
}

QString UArduinoBoardDiagramWidget::svgResourceForProfile() const
{
    return m_boardProfile == 1 ? QStringLiteral(":/boards/arduino_mega2560_pinout.svg")
                               : QStringLiteral(":/boards/arduino_uno_pinout.svg");
}

void UArduinoBoardDiagramWidget::updateSvg()
{
#if HARDWARELIB_HAS_QTSVG
    m_svg->load(svgResourceForProfile());
#endif

    QString status;
    switch (m_connectionState) {
    case 1: status = tr("Opening…"); break;
    case 2: status = tr("Connected"); break;
    case 3: status = tr("Error"); break;
    default: status = tr("Disconnected"); break;
    }

    const QString board = m_boardProfile == 1 ? QStringLiteral("Mega 2560") : QStringLiteral("Uno");
    QString pinInfo;
    if (!m_highlightedPins.isEmpty())
        pinInfo = tr("Pins: %1").arg(m_highlightedPins.join(QStringLiteral(", ")));

    m_statusLabel->setText(QStringLiteral("%1 — %2%3%4")
                               .arg(board,
                                    status,
                                    pinInfo.isEmpty() ? QString() : QStringLiteral("\n"),
                                    pinInfo));
}

void UArduinoBoardDiagramWidget::setBoardProfile(int profile)
{
    m_boardProfile = profile;
    updateSvg();
}

void UArduinoBoardDiagramWidget::setConnectionState(int state)
{
    m_connectionState = state;
    updateSvg();
}

void UArduinoBoardDiagramWidget::setHighlightedPins(const QStringList& pins)
{
    m_highlightedPins = pins;
    updateSvg();
}
