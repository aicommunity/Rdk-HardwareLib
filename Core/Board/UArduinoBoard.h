#ifndef UARDUINOBOARD_H
#define UARDUINOBOARD_H

#include "UMcuSerialBoard.h"

namespace RDK {

class UArduinoFlasher;

class RDK_LIB_TYPE UArduinoBoard : public UMcuSerialBoard {
public:
    UProperty<int, UArduinoBoard, ptPubParameter> BoardProfile;

    UArduinoBoard();
    virtual ~UArduinoBoard();

    virtual UArduinoBoard* New();

protected:
    virtual bool ADefault() override;

    virtual void ConfigureSerialSession(UArduinoSerialSession& session) override;
    virtual QString ResolveFirmwarePath() const override;
    virtual int DefaultBaudRate() const override;
    virtual void RunUploadBlocking() override;
    virtual void startUploadAsync() override;
    virtual void requestCancelUpload() override;
    virtual void AUnInit() override;

    QString ResolveHexPath() const;

    UArduinoFlasher* Flasher = nullptr;
};

} // namespace RDK

#endif
