#ifndef UHARDWARESETUP_H
#define UHARDWARESETUP_H

#include "UHardwareCatalogTypes.h"

#include <QByteArray>
#include <QSet>
#include <QString>
#include <QVector>

namespace RDK {

class UHardwareCatalog;

class UHardwareSetup {
public:
    bool loadFromFile(const QString& path, QString* error = nullptr);
    bool loadFromJson(const QByteArray& json, QString* error = nullptr);
    QByteArray toJson(bool compact = true) const;
    bool saveToFile(const QString& path, QString* error = nullptr) const;
    void setDocument(const UHwSetupDocument& doc) { Doc = doc; }
    bool validate(const UHardwareCatalog& catalog, QVector<UHwIssue>* issues) const;
    QSet<QString> occupiedPins(const UHardwareCatalog& catalog) const;
    const UHwSetupDocument& document() const { return Doc; }
    static UHwSetupDocument fromFirmwareDefaults(const UHwFirmwareInfo& firmware, const QString& boardId);

private:
    UHwSetupDocument Doc;
};

} // namespace RDK

#endif
