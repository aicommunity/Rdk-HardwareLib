#ifndef UHARDWARECATALOG_H
#define UHARDWARECATALOG_H

#include "UHardwareCatalogTypes.h"

namespace RDK {

class UHardwareCatalog {
public:
    static UHardwareCatalog& instance();

    bool load(QString* error = nullptr);
    bool reload(QString* error = nullptr);
    bool isLoaded() const { return Loaded; }
    void unload();

    const UHwBoardInfo* board(const QString& id) const;
    const UHwShieldInfo* shield(const QString& id) const;
    const UHwModuleInfo* module(const QString& id) const;
    const UHwFirmwareInfo* firmware(const QString& id) const;

    QStringList boardIds() const;
    QStringList shieldIds() const;
    QStringList moduleIds() const;
    QStringList firmwareIds(bool onlyAvailable = true) const;
    QStringList firmwaresForShield(const QString& shieldId, bool onlyAvailable = true) const;

    QString rootPath() const { return RootPath; }

private:
    UHardwareCatalog() = default;
    bool loadFromRoot(const QString& root, QString* error);

    bool Loaded = false;
    QString RootPath;
    QMap<QString, UHwBoardInfo> Boards;
    QMap<QString, UHwShieldInfo> Shields;
    QMap<QString, UHwModuleInfo> Modules;
    QMap<QString, UHwFirmwareInfo> Firmwares;
};

} // namespace RDK

#endif
