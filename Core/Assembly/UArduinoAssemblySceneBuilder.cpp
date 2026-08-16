#include "UArduinoAssemblySceneBuilder.h"

#include "Catalog/UHardwareCatalog.h"

namespace RDK {

UAssemblySceneModel UArduinoAssemblySceneBuilder::build(const UHardwareCatalog& catalog,
                                                        const UHwSetupDocument& setup,
                                                        const QVector<UHwIssue>& issues)
{
    UAssemblySceneModel model;
    const UHwBoardInfo* board = catalog.board(setup.board);
    model.boardProfile = board ? board->boardProfileInt : 0;

    UAssemblyNode boardNode;
    boardNode.kind = UAssemblyNode::Board;
    boardNode.id = QStringLiteral("board");
    boardNode.label = board ? board->title : setup.board;
    boardNode.normRect = QRectF(0.05, 0.45, 0.55, 0.45);
    if (board)
        boardNode.assetPath = board->pinoutSvg;
    model.nodes.append(boardNode);

    qreal shield_y = 0.08;
    for (const QString& shieldId : setup.stack) {
        const UHwShieldInfo* shield = catalog.shield(shieldId);
        UAssemblyNode node;
        node.kind = UAssemblyNode::Shield;
        node.id = QStringLiteral("shield:") + shieldId;
        node.label = shield ? shield->title : shieldId;
        node.normRect = QRectF(0.08, shield_y, 0.5, 0.28);
        if (shield)
            node.assetPath = shield->layoutAsset;
        model.nodes.append(node);
        shield_y += 0.06;
    }

    const int n = setup.devices.size();
    for (int i = 0; i < n; ++i) {
        const UHwSetupDevice& device = setup.devices.at(i);
        const UHwModuleInfo* mod = catalog.module(device.module);
        UAssemblyNode node;
        node.kind = UAssemblyNode::Module;
        node.id = QStringLiteral("device:") + device.id;
        node.label = mod ? mod->title : device.module;
        const qreal y = 0.08 + i * (0.8 / qMax(1, n));
        node.normRect = QRectF(0.65, y, 0.3, 0.12);
        if (mod)
            node.assetPath = mod->asset;
        for (const UHwIssue& issue : issues) {
            if (issue.message.contains(device.id) || issue.message.contains(device.port)
                || issue.message.contains(device.module))
                node.issueCodes.append(issue.code);
        }
        model.nodes.append(node);

        UAssemblyWire wire;
        wire.fromNodeId = node.id;
        wire.toPortId = device.port.isEmpty() ? (QStringLiteral("channel:") + device.channel)
                                             : device.port;
        wire.conductors = mod ? mod->wires : QStringList{QStringLiteral("S")};
        wire.conflict = node.issueCodes.contains(QStringLiteral("PinConflict"));
        model.wires.append(wire);
        if (!device.port.isEmpty())
            model.occupiedPins.append(device.port);
    }

    return model;
}

} // namespace RDK
