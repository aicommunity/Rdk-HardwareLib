#include "UArduinoBoardDiagramWidget.h"

#include "HardwareGuiHelpers.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPlainTextEdit>
#include <QResizeEvent>
#include <QVBoxLayout>

#if HARDWARELIB_HAS_QTSVG
#include <QSvgWidget>
#endif

namespace {

QString resolveBundledPath(const QString& qrcPath, const QString& resourcesRelativePath)
{
    if (QFile::exists(qrcPath))
        return qrcPath;

    const QByteArray sdkRoot = qgetenv("NMSDK_ROOT");
    if (!sdkRoot.isEmpty()) {
        const QString devPath =
            QString::fromLocal8Bit(sdkRoot)
            + QStringLiteral("/Libraries/Rdk-HardwareLib/GUI/Qt/Resources/")
            + resourcesRelativePath;
        if (QFile::exists(devPath))
            return devPath;
    }
    return qrcPath;
}

QVector<UArduinoPinOverlay::PinRegion> loadPinsFromResource(const QString& resourcePath,
                                                            const QString& resourcesRelativePath)
{
    const QString path = resolveBundledPath(resourcePath, resourcesRelativePath);
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return {};

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    const QJsonObject root = doc.object();
    const QJsonArray pins = root.value(QStringLiteral("pins")).toArray();

    QVector<UArduinoPinOverlay::PinRegion> result;
    result.reserve(pins.size());
    for (const QJsonValue& v : pins) {
        const QJsonObject o = v.toObject();
        const QJsonArray rect = o.value(QStringLiteral("rect")).toArray();
        if (rect.size() < 4)
            continue;
        UArduinoPinOverlay::PinRegion region;
        region.id = o.value(QStringLiteral("id")).toString();
        region.label = o.value(QStringLiteral("label")).toString(region.id);
        region.normalizedRect = QRectF(rect.at(0).toDouble(),
                                       rect.at(1).toDouble(),
                                       rect.at(2).toDouble(),
                                       rect.at(3).toDouble());
        result.append(region);
    }
    return result;
}

} // namespace

UArduinoBoardDiagramWidget::UArduinoBoardDiagramWidget(QWidget* parent)
    : QWidget(parent)
{
    m_diagramHost = new QWidget(this);
    m_diagramHost->setMinimumHeight(160);

#if HARDWARELIB_HAS_QTSVG
    m_svg = new QSvgWidget(m_diagramHost);
#else
    m_svgPlaceholder = new QLabel(tr("Board diagram (install Qt Svg)"), m_diagramHost);
    m_svgPlaceholder->setAlignment(Qt::AlignCenter);
#endif

    m_overlay = new UArduinoPinOverlay(m_diagramHost);
    connect(m_overlay, &UArduinoPinOverlay::pinClicked, this, &UArduinoBoardDiagramWidget::pinClicked);

    m_statusLog = HardwareGuiHelpers::createStatusLogWidget(this);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_diagramHost, 1);
    layout->addWidget(m_statusLog);
    setMinimumHeight(220);
    reloadPinLayout();
    updateSvg();
    layoutDiagram();
}

QString UArduinoBoardDiagramWidget::svgResourceForProfile() const
{
    return m_boardProfile == 1 ? QStringLiteral(":/boards/arduino_mega2560_pinout.svg")
                               : QStringLiteral(":/boards/arduino_uno_pinout.svg");
}

QString UArduinoBoardDiagramWidget::pinsResourceForProfile() const
{
    return m_boardProfile == 1 ? QStringLiteral(":/boards/mega2560_pins.json")
                               : QStringLiteral(":/boards/uno_pins.json");
}

void UArduinoBoardDiagramWidget::reloadPinLayout()
{
    const QString pinsRel = m_boardProfile == 1 ? QStringLiteral("boards/mega2560_pins.json")
                                                : QStringLiteral("boards/uno_pins.json");
    m_overlay->setPins(loadPinsFromResource(pinsResourceForProfile(), pinsRel));
    m_overlay->setPinRoles(m_pinRoles);
    m_overlay->setHighlightedIds(m_highlightedPins);
    m_overlay->setSelectedId(m_selectedPinId);
    m_overlay->setInteractive(m_interactive);
}

void UArduinoBoardDiagramWidget::updateSvg()
{
#if HARDWARELIB_HAS_QTSVG
    const QString svgRel = m_boardProfile == 1 ? QStringLiteral("boards/arduino_mega2560_pinout.svg")
                                               : QStringLiteral("boards/arduino_uno_pinout.svg");
    m_svg->load(resolveBundledPath(svgResourceForProfile(), svgRel));
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
    if (!m_selectedPinId.isEmpty())
        pinInfo += (pinInfo.isEmpty() ? QString() : QStringLiteral("; "))
                   + tr("Selected: %1").arg(m_selectedPinId);

    HardwareGuiHelpers::setStatusLogText(
        m_statusLog,
        QStringLiteral("%1 — %2%3%4").arg(board,
                                          status,
                                          pinInfo.isEmpty() ? QString() : QStringLiteral("\n"),
                                          pinInfo));
    layoutDiagram();
}

QSizeF UArduinoBoardDiagramWidget::diagramViewBoxSize() const
{
    return m_boardProfile == 1 ? QSizeF(500.0, 200.0) : QSizeF(400.0, 200.0);
}

void UArduinoBoardDiagramWidget::layoutDiagram()
{
    if (!m_diagramHost)
        return;

    const QRect host = m_diagramHost->rect();
    const QSizeF scaled = diagramViewBoxSize().scaled(host.size(), Qt::KeepAspectRatio);
    QRect geom(QPoint(0, 0), scaled.toSize());
    geom.moveCenter(host.center());

#if HARDWARELIB_HAS_QTSVG
    if (m_svg)
        m_svg->setGeometry(geom);
#else
    if (m_svgPlaceholder)
        m_svgPlaceholder->setGeometry(geom);
#endif
    if (m_overlay)
        m_overlay->setGeometry(geom);
}

void UArduinoBoardDiagramWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    layoutDiagram();
}

int UArduinoBoardDiagramWidget::firmataPinFromLabel(const QString& pinId, int boardProfile)
{
    Q_UNUSED(boardProfile);
    if (pinId.startsWith(QLatin1String("D"), Qt::CaseInsensitive)) {
        bool ok = false;
        const int n = pinId.mid(1).toInt(&ok);
        return ok ? n : -1;
    }
    if (pinId.startsWith(QLatin1String("A"), Qt::CaseInsensitive)) {
        bool ok = false;
        const int n = pinId.mid(1).toInt(&ok);
        return ok ? (14 + n) : -1;
    }
    return -1;
}

void UArduinoBoardDiagramWidget::setBoardProfile(int profile)
{
    m_boardProfile = profile;
    reloadPinLayout();
    updateSvg();
    layoutDiagram();
}

void UArduinoBoardDiagramWidget::setConnectionState(int state)
{
    m_connectionState = state;
    updateSvg();
}

void UArduinoBoardDiagramWidget::setHighlightedPins(const QStringList& pins)
{
    m_highlightedPins = pins;
    m_overlay->setHighlightedIds(m_highlightedPins);
    updateSvg();
}

void UArduinoBoardDiagramWidget::setPinRoles(const QMap<QString, QString>& roles)
{
    m_pinRoles = roles;
    m_overlay->setPinRoles(m_pinRoles);
}

void UArduinoBoardDiagramWidget::setSelectedPinId(const QString& pinId)
{
    m_selectedPinId = pinId;
    m_overlay->setSelectedId(m_selectedPinId);
    updateSvg();
}

void UArduinoBoardDiagramWidget::setInteractive(bool interactive)
{
    m_interactive = interactive;
    m_overlay->setInteractive(interactive);
}
