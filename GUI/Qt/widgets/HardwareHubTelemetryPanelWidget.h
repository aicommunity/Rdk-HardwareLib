#ifndef HARDWAREHUBTELEMETRYPANELWIDGET_H
#define HARDWAREHUBTELEMETRYPANELWIDGET_H

#include "HardwareGuiHelpers.h"

#include <QWidget>

class QLabel;
class QTableWidget;
class QPlainTextEdit;

/** Shared NamedValuesJson table (+ optional matrix preview) for hub hosts. */
class HardwareHubTelemetryPanelWidget : public QWidget {
    Q_OBJECT
public:
    explicit HardwareHubTelemetryPanelWidget(QWidget* parent = nullptr);
    void setContext(const UComponentGuiContext& context);
    void refreshFromModel();
    /** When NamedValuesJson empty, try this prop (wheeled LastFeedbackJson). */
    void setFallbackJsonProp(const char* propName);

private:
    void populateNamedValues(const QString& jsonText);

    UComponentGuiContext Context;
    const char* FallbackJsonProp = nullptr;
    QTableWidget* Table = nullptr;
    QPlainTextEdit* MatrixPreview = nullptr;
    QLabel* Status = nullptr;
};

#endif
