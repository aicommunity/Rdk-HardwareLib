#include "UArduinoAssemblyViewWidget.h"

#include "Catalog/UHardwareCatalog.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QSvgGenerator>

UArduinoAssemblyViewWidget::UArduinoAssemblyViewWidget(QWidget* parent)
    : QGraphicsView(parent)
{
    setScene(new QGraphicsScene(this));
    setRenderHint(QPainter::Antialiasing, true);
    setBackgroundBrush(QColor(245, 245, 248));
    setMinimumHeight(220);
}

void UArduinoAssemblyViewWidget::setCatalog(const RDK::UHardwareCatalog* catalog)
{
    Catalog = catalog;
}

void UArduinoAssemblyViewWidget::setSetup(const RDK::UHwSetupDocument& doc)
{
    Setup = doc;
}

void UArduinoAssemblyViewWidget::setIssues(const QVector<RDK::UHwIssue>& issues)
{
    Issues = issues;
}

void UArduinoAssemblyViewWidget::rebuild()
{
    scene()->clear();
    if (!Catalog)
        return;
    Model = RDK::UArduinoAssemblySceneBuilder::build(*Catalog, Setup, Issues);
    const QRectF canvas(0, 0, 800, 480);
    scene()->setSceneRect(canvas);
    for (const RDK::UAssemblyNode& node : Model.nodes) {
        const QRectF r(node.normRect.x() * canvas.width(),
                       node.normRect.y() * canvas.height(),
                       node.normRect.width() * canvas.width(),
                       node.normRect.height() * canvas.height());
        auto* item = scene()->addRect(r, QPen(Qt::darkGray), QBrush(QColor(255, 255, 255, 220)));
        if (!node.issueCodes.isEmpty())
            item->setBrush(QBrush(QColor(255, 220, 220)));
        auto* text = scene()->addSimpleText(node.label);
        text->setPos(r.topLeft() + QPointF(8, 8));
    }
    for (const RDK::UAssemblyWire& wire : Model.wires) {
        QPointF from(650, 80);
        QPointF to(400, 200);
        for (const RDK::UAssemblyNode& node : Model.nodes) {
            if (node.id == wire.fromNodeId) {
                from = QPointF(node.normRect.center().x() * canvas.width(),
                               node.normRect.center().y() * canvas.height());
            }
            if (node.kind == RDK::UAssemblyNode::Board) {
                to = QPointF(node.normRect.center().x() * canvas.width(),
                             node.normRect.top() * canvas.height() + 20);
            }
        }
        QPen pen(wire.conflict ? Qt::red : QColor(40, 90, 200), wire.conflict ? 3 : 2);
        scene()->addLine(QLineF(from, to), pen);
    }
    fitInView(canvas, Qt::KeepAspectRatio);
}

bool UArduinoAssemblyViewWidget::exportSvg(const QString& path, QString* error) const
{
    QSvgGenerator generator;
    generator.setFileName(path);
    generator.setSize(QSize(800, 480));
    generator.setViewBox(QRect(0, 0, 800, 480));
    generator.setTitle(QStringLiteral("Nmsdk assembly"));
    QPainter painter;
    if (!painter.begin(&generator)) {
        if (error)
            *error = QStringLiteral("Cannot write SVG");
        return false;
    }
    scene()->render(&painter);
    painter.end();
    return true;
}
