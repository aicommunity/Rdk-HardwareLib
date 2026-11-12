#ifndef UESP32BOARD_H
#define UESP32BOARD_H

#include "UMcuSerialBoard.h"

namespace RDK {

class RDK_LIB_TYPE UEsp32Board : public UMcuSerialBoard {
public:
    UProperty<string, UEsp32Board, ptPubParameter> ChipTarget;
    UProperty<int, UEsp32Board, ptPubParameter> FlashBaud;

    UEsp32Board();
    virtual ~UEsp32Board();

    virtual UEsp32Board* New();

protected:
    virtual bool ADefault() override;
    virtual void ConfigureSerialSession(UArduinoSerialSession& session) override;
    virtual int DefaultBaudRate() const override;
    virtual void RunUploadBlocking() override;
    virtual void startUploadAsync() override;
};

} // namespace RDK

#endif
