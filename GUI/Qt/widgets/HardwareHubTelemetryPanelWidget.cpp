#include "HardwareHubTelemetryPanelWidget.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLabel>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QVBoxLayout>

HardwareHubTelemetryPanelWidget::HardwareHubTelemetryPanelWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    HardwareGuiHelpers::applyCompactLayout(root);
    Status = new QLabel(tr("Named values"), this);
    root->addWidget(Status);
    Table = new QTableWidget(0, 2, this);
    Table->setHorizontalHeaderLabels({tr("Key"), tr("Value")});
    Table->horizontalHeader()->setStretchLastSection(true);
    Table->verticalHeader()->setVisible(false);
    Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    Table->setSelectionBehavior(QAbstractItemView::SelectRows);
    root->addWidget(Table, 2);
    MatrixPreview = new QPlainTextEdit(this);
    MatrixPreview->setReadOnly(true);
    MatrixPreview->setPlaceholderText(tr("DoubleMatrixReadings (last rows)…"));
    MatrixPreview->setMaximumHeight(96);
    root->addWidget(MatrixPreview, 1);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}

void HardwareHubTelemetryPanelWidget::setFallbackJsonProp(const char* propName)
{
    FallbackJsonProp = propName;
}

void HardwareHubTelemetryPanelWidget::setContext(const UComponentGuiContext& context)
{
    Context = context;
    refreshFromModel();
}

void HardwareHubTelemetryPanelWidget::populateNamedValues(const QString& jsonText)
{
    Table->setRowCount(0);
    if (jsonText.trimmed().isEmpty() || jsonText.trimmed() == QLatin1String("{}")) {
        Status->setText(tr("Named values (empty)"));
        return;
    }
    const QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8());
    if (!doc.isObject()) {
        Status->setText(tr("Named values (not JSON object)"));
        Table->setRowCount(1);
        Table->setItem(0, 0, new QTableWidgetItem(QStringLiteral("raw")));
        Table->setItem(0, 1, new QTableWidgetItem(jsonText));
        return;
    }
    const QJsonObject obj = doc.object();
    Status->setText(tr("Named values (%1 keys)").arg(obj.size()));
    const QStringList keys = obj.keys();
    Table->setRowCount(keys.size());
    for (int i = 0; i < keys.size(); ++i) {
        Table->setItem(i, 0, new QTableWidgetItem(keys[i]));
        const QJsonValue v = obj.value(keys[i]);
        QString cell;
        if (v.isDouble())
            cell = QString::number(v.toDouble(), 'g', 8);
        else if (v.isString())
            cell = v.toString();
        else if (v.isBool())
            cell = v.toBool() ? QStringLiteral("true") : QStringLiteral("false");
        else if (v.isNull())
            cell = QStringLiteral("null");
        else
            cell = QString::fromUtf8(QJsonDocument::fromVariant(v.toVariant()).toJson(QJsonDocument::Compact));
        Table->setItem(i, 1, new QTableWidgetItem(cell));
    }
}

void HardwareHubTelemetryPanelWidget::refreshFromModel()
{
    if (Context.componentLongName.isEmpty())
        return;
    QString text = HardwareGuiHelpers::getProp(Context, "NamedValuesJson");
    if (text.isEmpty() && FallbackJsonProp)
        text = HardwareGuiHelpers::getProp(Context, FallbackJsonProp);
    populateNamedValues(text);

    QVector<QVector<double>> rows;
    if (HardwareGuiHelpers::getMatrixPreview(Context, "DoubleMatrixReadings", 8, 10, &rows)
        && !rows.isEmpty()) {
        QStringList lines;
        for (const auto& row : rows) {
            QStringList cells;
            for (double v : row)
                cells << QString::number(v, 'g', 6);
            lines << cells.join(QLatin1Char(' '));
        }
        MatrixPreview->setPlainText(lines.join(QLatin1Char('\n')));
    } else {
        MatrixPreview->clear();
    }
}
