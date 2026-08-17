#ifndef UARDUINOBOARDDIAGRAMWIDGET_H
#define UARDUINOBOARDDIAGRAMWIDGET_H

#include <QMap>
#include <QStringList>
#include <QWidget>

#include "UArduinoPinOverlay.h"

class QLabel;
class QPlainTextEdit;
class QResizeEvent;
class QShowEvent;
class QEvent;

#if __has_include(<QSvgWidget>)
#include <QSvgWidget>
#define HARDWARELIB_HAS_QTSVG 1
#else
#define HARDWARELIB_HAS_QTSVG 0
class QSvgWidget;
#endif

class UArduinoBoardDiagramWidget : public QWidget {
    Q_OBJECT
public:
    explicit UArduinoBoardDiagramWidget(QWidget* parent = nullptr);

    void setBoardProfile(int profile);
    void setConnectionState(int state);
    void setHighlightedPins(const QStringList& pins);
    void setPinRoles(const QMap<QString, QString>& roles);
    void setSelectedPinId(const QString& pinId);
    void setInteractive(bool interactive);
    void applyPinStatusJson(const QString& json);

    static int firmataPinFromLabel(const QString& pinId, int board_profile);

signals:
    void pinClicked(const QString& pinId);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void updateSvg();
    void reloadPinLayout();
    void layoutDiagram();
    QSizeF diagramViewBoxSize() const;
    QString svgResourceForProfile() const;
    QString pinsResourceForProfile() const;

    int BoardProfileValue = 0;
    int ConnectionStateValue = 0;
    QStringList HighlightedPins;
    QMap<QString, QString> PinRoles;
    QString SelectedPinId;
    bool Interactive = false;

    QWidget* DiagramHost = nullptr;
#if HARDWARELIB_HAS_QTSVG
    QSvgWidget* SvgWidget = nullptr;
#else
    QLabel* SvgPlaceholder = nullptr;
#endif
    UArduinoPinOverlay* Overlay = nullptr;
    QPlainTextEdit* StatusLog = nullptr;
};

#endif
