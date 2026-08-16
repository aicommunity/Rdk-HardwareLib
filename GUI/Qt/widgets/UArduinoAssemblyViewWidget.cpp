#include "UArduinoAssemblyViewWidget.h"

#include "Catalog/UHardwareCatalog.h"
#include "Catalog/UHardwareCatalogPaths.h"

#include <QDir>
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsSvgItem>
#include <QPainter>
#include <QSvgGenerator>
#include <QSvgRenderer>

namespace {

QString resolveAssemblyAsset(const QString& assetPath)
{
    const QString trimmed = assetPath.trimmed();
    if (trimmed.isEmpty())
        return {};
    if (trimmed.startsWith(QStringLiteral(":/")))
        return trimmed;
    if (QDir::isAbsolutePath(trimmed) && QFileInfo::exists(trimmed))
        return trimmed;
    const QString fromCatalog = QDir(RDK::UHardwareCatalogPaths::catalogRoot()).filePath(trimmed);
    if (QFileInfo::exists(fromCatalog))
        return fromCatalog;
    return trimmed;
}

void addNodeVisual(QGraphicsScene* scene, const RDK::UAssemblyNode& node, const QRectF& r)
{
    const QString asset = resolveAssemblyAsset(node.assetPath);
    bool drewAsset = false;
    if (!asset.isEmpty()) {
        if (asset.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive)
            || asset.startsWith(QStringLiteral(":/"))) {
            auto* svgItem = new QGraphicsSvgItem(asset);
            QSvgRenderer* renderer = svgItem->renderer();
            if (renderer && renderer->isValid()) {
                const QRectF bounds = svgItem->boundingRect();
                if (bounds.width() > 1.0 && bounds.height() > 1.0) {
                    const qreal sx = r.width() / bounds.width();
                    const qreal sy = r.height() / bounds.height();
                    const qreal s = qMin(sx, sy);
                    svgItem->setScale(s);
                    svgItem->setPos(r.topLeft());
                    svgItem->setZValue(0);
                    scene->addItem(svgItem);
                    drewAsset = true;
                } else {
                    delete svgItem;
                }
            } else {
                delete svgItem;
            }
        } else {
            QPixmap pm(asset);
            if (!pm.isNull()) {
                auto* pix = scene->addPixmap(
                    pm.scaled(r.size().toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                pix->setPos(r.topLeft());
                pix->setZValue(0);
                drewAsset = true;
            }
        }
    }

    QColor fill = drewAsset ? QColor(255, 255, 255, 40) : QColor(255, 255, 255, 220);
    if (!node.issueCodes.isEmpty())
        fill = QColor(255, 220, 220, drewAsset ? 120 : 220);
    auto* frame = scene->addRect(r, QPen(Qt::darkGray), QBrush(fill));
    frame->setZValue(1);
    auto* text = scene->addSimpleText(node.label);
    text->setPos(r.topLeft() + QPointF(8, 8));
    text->setZValue(2);
}

} // namespace

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
        addNodeVisual(scene(), node, r);
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
        auto* line = scene()->addLine(QLineF(from, to), pen);
        line->setZValue(3);
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
