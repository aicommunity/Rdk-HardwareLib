#ifndef UARDUINOBOARDDIAGRAMWIDGET_H
#define UARDUINOBOARDDIAGRAMWIDGET_H

#include <QMap>
#include <QStringList>
#include <QWidget>

#include "UArduinoPinOverlay.h"

class QLabel;
class QPlainTextEdit;
class QResizeEvent;

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

    static int firmataPinFromLabel(const QString& pinId, int boardProfile);

signals:
    void pinClicked(const QString& pinId);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateSvg();
    void reloadPinLayout();
    void layoutDiagram();
    QSizeF diagramViewBoxSize() const;
    QString svgResourceForProfile() const;
    QString pinsResourceForProfile() const;

    int m_boardProfile = 0;
    int m_connectionState = 0;
    QStringList m_highlightedPins;
    QMap<QString, QString> m_pinRoles;
    QString m_selectedPinId;
    bool m_interactive = false;

    QWidget* m_diagramHost = nullptr;
#if HARDWARELIB_HAS_QTSVG
    QSvgWidget* m_svg = nullptr;
#else
    QLabel* m_svgPlaceholder = nullptr;
#endif
    UArduinoPinOverlay* m_overlay = nullptr;
    QPlainTextEdit* m_statusLog = nullptr;
};

#endif
