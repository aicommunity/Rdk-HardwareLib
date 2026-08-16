#ifndef UARDUINOASSEMBLYSCENEBUILDER_H
#define UARDUINOASSEMBLYSCENEBUILDER_H

#include "UHardwareCatalogTypes.h"

#include <QRectF>
#include <QString>
#include <QVector>

namespace RDK {

class UHardwareCatalog;

struct UAssemblyNode {
    enum Kind { Board, Shield, Module, PortAnchor };
    Kind kind = Board;
    QString id;
    QString label;
    QRectF normRect;
    QString assetPath;
    QStringList issueCodes;
};

struct UAssemblyWire {
    QString fromNodeId;
    QString toPortId;
    QStringList conductors;
    bool conflict = false;
};

struct UAssemblySceneModel {
    QVector<UAssemblyNode> nodes;
    QVector<UAssemblyWire> wires;
    int boardProfile = 0;
    QStringList occupiedPins;
};

class UArduinoAssemblySceneBuilder {
public:
    static UAssemblySceneModel build(const UHardwareCatalog& catalog,
                                     const UHwSetupDocument& setup,
                                     const QVector<UHwIssue>& issues);
};

} // namespace RDK

#endif
