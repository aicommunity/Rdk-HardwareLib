#include "UArduinoBoardDiagramWidget.h"

#include "HardwareGuiHelpers.h"

#include <Transport/UArduinoPinMap.h>

#include <QDebug>
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

QString resolveBundledPath(const QString& qrc_path, const QString& resources_relative_path)
{
    if (QFile::exists(qrc_path))
        return qrc_path;

    const QByteArray sdk_root = qgetenv("NMSDK_ROOT");
    if (!sdk_root.isEmpty()) {
        const QString dev_path =
            QString::fromLocal8Bit(sdk_root)
            + QStringLiteral("/Libraries/Rdk-HardwareLib/GUI/Qt/Resources/")
            + resources_relative_path;
        if (QFile::exists(dev_path))
            return dev_path;
    }
    return qrc_path;
}

QVector<UArduinoPinOverlay::PinRegion> loadPinsFromResource(const QString& resource_path,
                                                            const QString& resources_relative_path,
                                                            int board_profile)
{
    const QString path = resolveBundledPath(resource_path, resources_relative_path);
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
        region.Id = o.value(QStringLiteral("id")).toString();
        region.Label = o.value(QStringLiteral("label")).toString(region.Id);
        if (o.contains(QStringLiteral("firmataPin"))) {
            const int json_pin = o.value(QStringLiteral("firmataPin")).toInt(-1);
            const int map_pin =
                RDK::UArduinoPinMap::firmataPinForLabel(region.Id, board_profile);
            if (map_pin >= 0 && json_pin >= 0 && map_pin != json_pin)
                qWarning("Pin %s: firmataPin %d != PinMap %d", qPrintable(region.Id), json_pin,
                         map_pin);
        }
        region.NormalizedRect = QRectF(rect.at(0).toDouble(),
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
    DiagramHost = new QWidget(this);
    DiagramHost->setMinimumHeight(160);

#if HARDWARELIB_HAS_QTSVG
    SvgWidget = new QSvgWidget(DiagramHost);
#else
    SvgPlaceholder = new QLabel(tr("Board diagram (install Qt Svg)"), DiagramHost);
    SvgPlaceholder->setAlignment(Qt::AlignCenter);
#endif

    Overlay = new UArduinoPinOverlay(DiagramHost);
    connect(Overlay, &UArduinoPinOverlay::pinClicked, this, &UArduinoBoardDiagramWidget::pinClicked);

    StatusLog = HardwareGuiHelpers::createStatusLogWidget(this);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(DiagramHost, 1);
    layout->addWidget(StatusLog);
    setMinimumHeight(220);
    reloadPinLayout();
    updateSvg();
    layoutDiagram();
}

QString UArduinoBoardDiagramWidget::svgResourceForProfile() const
{
    return BoardProfileValue == 1 ? QStringLiteral(":/boards/arduino_mega2560_pinout.svg")
                               : QStringLiteral(":/boards/arduino_uno_pinout.svg");
}

QString UArduinoBoardDiagramWidget::pinsResourceForProfile() const
{
    return BoardProfileValue == 1 ? QStringLiteral(":/boards/mega2560_pins.json")
                               : QStringLiteral(":/boards/uno_pins.json");
}

void UArduinoBoardDiagramWidget::reloadPinLayout()
{
    const QString pinsRel = BoardProfileValue == 1 ? QStringLiteral("boards/mega2560_pins.json")
                                                : QStringLiteral("boards/uno_pins.json");
    Overlay->setPins(loadPinsFromResource(pinsResourceForProfile(), pinsRel, BoardProfileValue));
    Overlay->setPinRoles(PinRoles);
    Overlay->setHighlightedIds(HighlightedPins);
    Overlay->setSelectedId(SelectedPinId);
    Overlay->setInteractive(Interactive);
}

void UArduinoBoardDiagramWidget::updateSvg()
{
#if HARDWARELIB_HAS_QTSVG
    const QString svgRel = BoardProfileValue == 1 ? QStringLiteral("boards/arduino_mega2560_pinout.svg")
                                               : QStringLiteral("boards/arduino_uno_pinout.svg");
    SvgWidget->load(resolveBundledPath(svgResourceForProfile(), svgRel));
#endif

    QString status;
    switch (ConnectionStateValue) {
    case 1: status = tr("Opening\u2026"); break;
    case 2: status = tr("Connected"); break;
    case 3: status = tr("Error"); break;
    default: status = tr("Disconnected"); break;
    }

    const QString board = BoardProfileValue == 1 ? QStringLiteral("Mega 2560") : QStringLiteral("Uno");
    QString pinInfo;
    if (!HighlightedPins.isEmpty())
        pinInfo = tr("Pins: %1").arg(HighlightedPins.join(QStringLiteral(", ")));
    if (!SelectedPinId.isEmpty())
        pinInfo += (pinInfo.isEmpty() ? QString() : QStringLiteral("; "))
                   + tr("Selected: %1").arg(SelectedPinId);

    HardwareGuiHelpers::setStatusLogText(
        StatusLog,
        QStringLiteral("%1 \u2014 %2%3%4").arg(board,
                                          status,
                                          pinInfo.isEmpty() ? QString() : QStringLiteral("\n"),
                                          pinInfo));
    layoutDiagram();
}

QSizeF UArduinoBoardDiagramWidget::diagramViewBoxSize() const
{
    return BoardProfileValue == 1 ? QSizeF(500.0, 200.0) : QSizeF(400.0, 200.0);
}

void UArduinoBoardDiagramWidget::layoutDiagram()
{
    if (!DiagramHost)
        return;

    const QRect host = DiagramHost->rect();
    const QSizeF scaled = diagramViewBoxSize().scaled(host.size(), Qt::KeepAspectRatio);
    QRect geom(QPoint(0, 0), scaled.toSize());
    geom.moveCenter(host.center());

#if HARDWARELIB_HAS_QTSVG
    if (SvgWidget)
        SvgWidget->setGeometry(geom);
#else
    if (SvgPlaceholder)
        SvgPlaceholder->setGeometry(geom);
#endif
    if (Overlay)
        Overlay->setGeometry(geom);
}

void UArduinoBoardDiagramWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    layoutDiagram();
}

int UArduinoBoardDiagramWidget::firmataPinFromLabel(const QString& pinId, int board_profile)
{
    return RDK::UArduinoPinMap::firmataPinForLabel(pinId, board_profile);
}

void UArduinoBoardDiagramWidget::setBoardProfile(int profile)
{
    BoardProfileValue = profile;
    reloadPinLayout();
    updateSvg();
    layoutDiagram();
}

void UArduinoBoardDiagramWidget::setConnectionState(int state)
{
    ConnectionStateValue = state;
    updateSvg();
}

void UArduinoBoardDiagramWidget::setHighlightedPins(const QStringList& pins)
{
    HighlightedPins = pins;
    Overlay->setHighlightedIds(HighlightedPins);
    updateSvg();
}

void UArduinoBoardDiagramWidget::setPinRoles(const QMap<QString, QString>& roles)
{
    PinRoles = roles;
    Overlay->setPinRoles(PinRoles);
}

void UArduinoBoardDiagramWidget::setSelectedPinId(const QString& pinId)
{
    SelectedPinId = pinId;
    Overlay->setSelectedId(SelectedPinId);
    updateSvg();
}

void UArduinoBoardDiagramWidget::setInteractive(bool interactive)
{
    Interactive = interactive;
    Overlay->setInteractive(interactive);
}

void UArduinoBoardDiagramWidget::applyPinStatusJson(const QString& json)
{
    if (!Overlay || json.isEmpty())
        return;

    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    const QJsonArray pins = doc.object().value(QStringLiteral("pins")).toArray();
    QMap<QString, UArduinoPinOverlay::PinVisualState> states;
    for (const QJsonValue& v : pins) {
        const QJsonObject o = v.toObject();
        const QString id = o.value(QStringLiteral("id")).toString();
        if (id.isEmpty())
            continue;
        UArduinoPinOverlay::PinVisualState vs;
        vs.Supported = o.value(QStringLiteral("supported")).toBool(true);
        const int host_mode = o.value(QStringLiteral("hostMode")).toInt(-1);
        switch (host_mode) {
        case 0: vs.Mode = UArduinoPinOverlay::PinModeVisual::Input; break;
        case 1: vs.Mode = UArduinoPinOverlay::PinModeVisual::Output; break;
        case 2: vs.Mode = UArduinoPinOverlay::PinModeVisual::Analog; break;
        case 3: vs.Mode = UArduinoPinOverlay::PinModeVisual::Pwm; break;
        default: vs.Mode = UArduinoPinOverlay::PinModeVisual::Unknown; break;
        }
        vs.Digital = o.value(QStringLiteral("digital")).toInt(-1);
        vs.Analog = o.value(QStringLiteral("analog")).toInt(-1);
        states.insert(id, vs);
    }
    Overlay->setPinStates(states);
}
