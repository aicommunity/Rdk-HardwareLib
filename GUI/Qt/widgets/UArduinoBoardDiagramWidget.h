#ifndef UARDUINOBOARDDIAGRAMWIDGET_H
#define UARDUINOBOARDDIAGRAMWIDGET_H

#include <QStringList>
#include <QWidget>

class QLabel;

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

private:
    void updateSvg();
    QString svgResourceForProfile() const;

    int m_boardProfile = 0;
    int m_connectionState = 0;
    QStringList m_highlightedPins;
#if HARDWARELIB_HAS_QTSVG
    QSvgWidget* m_svg = nullptr;
#else
    QLabel* m_svgPlaceholder = nullptr;
#endif
    QLabel* m_statusLabel = nullptr;
};

#endif
