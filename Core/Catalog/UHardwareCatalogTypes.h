#ifndef UHARDWARECATALOGTYPES_H
#define UHARDWARECATALOGTYPES_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <QVector>

namespace RDK {

enum class UHwIssueSeverity { Error, Warning };

struct UHwIssue {
    UHwIssueSeverity severity = UHwIssueSeverity::Error;
    QString code;
    QString message;
};

struct UHwBoardInfo {
    QString id;
    QString title;
    int boardProfileInt = 0;
    QString mcu;
    QString protocol;
    int uploadBaud = 115200;
    int runtimeBaudDefault = 57600;
    int firmataPinCount = 20;
    int analogBase = 14;
    QString pinoutSvg;
    QString pinsJson;
};

struct UHwShieldPort {
    QString signalPin;
    QString header;
    QString powerRail;
    QString reserved;
};

struct UHwMotorChannel {
    QString dir;
    QString dir2;
    QString pwm;
    QString brake;
    QString sense;
    QString enable;
};

struct UHwShieldInfo {
    QString id;
    QString title;
    QStringList compatibleBoards;
    QString defaultFirmware;
    QString layoutAsset;
    QMap<QString, UHwShieldPort> ports;
    QMap<QString, UHwMotorChannel> channels;
    QStringList occupiesPins;
    QStringList compatibleModules;
    QStringList warnings;
};

struct UHwModuleInfo {
    QString id;
    QString title;
    QString signalType;
    QStringList wires;
    QStringList roles;
    QString portKind;
    QStringList channelIds;
    bool requiresMcuTiming = false;
    QStringList requiredCapabilities;
    QStringList preferredFirmware;
    QString asset;
};

struct UHwFirmwareInfo {
    QString id;
    QString title;
    QString tier;
    QString transport;
    bool available = true;
    int runtimeBaud = 57600;
    QStringList boards;
    QStringList compatibleShields;
    QStringList capabilities;
    QMap<QString, QString> hex; // boardKey -> relative hex
    QString hostPlugin;
    QString protocolId;
    QMap<QString, QString> defaultPins;
    QString legacyManifestId;
};

struct UHwSetupDevice {
    QString id;
    QString module;
    QString port;
    QString channel;
    QString role;
};

struct UHwSetupDocument {
    int schemaVersion = 1;
    QString board;
    QString firmwareId;
    QStringList stack;
    QVector<UHwSetupDevice> devices;
};

} // namespace RDK

#endif
