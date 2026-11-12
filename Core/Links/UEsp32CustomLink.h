#ifndef UESP32CUSTOMLINK_H
#define UESP32CUSTOMLINK_H

#include "Board/UEsp32Board.h"
#include "Protocol/UArduinoBinaryStreamParser.h"

#include <QByteArray>
#include <QQueue>
#include <QString>

namespace RDK {

/** Framed-v2 custom link on ESP32 serial board (mirror of UArduinoCustomLink). */
class RDK_LIB_TYPE UEsp32CustomLink : public UEsp32Board {
public:
    UProperty<string, UEsp32CustomLink, ptPubParameter | ptInput> Command;
    UProperty<bool, UEsp32CustomLink, ptPubState> SendCommandFlag;
    UProperty<string, UEsp32CustomLink, ptPubState> SentCommand;
    UProperty<int, UEsp32CustomLink, ptPubParameter> ProtocolVersion;
    UProperty<int, UEsp32CustomLink, ptPubState> RxFrameCount;
    UProperty<int, UEsp32CustomLink, ptPubState> TxCommandCount;
    UProperty<bool, UEsp32CustomLink, ptPubParameter | ptInput> SendCommand;
    UProperty<bool, UEsp32CustomLink, ptPubParameter | ptInput> RequestGetStatus;
    UProperty<bool, UEsp32CustomLink, ptPubParameter | ptInput> RequestProtocolNegotiate;
    UProperty<bool, UEsp32CustomLink, ptPubState> IsProtocolReady;
    UProperty<bool, UEsp32CustomLink, ptPubState> HasPendingCommands;

    UEsp32CustomLink();
    virtual ~UEsp32CustomLink();
    virtual UEsp32CustomLink* New() = 0;

protected:
    virtual bool ADefault() override;
    virtual bool ACalculate() override;
    virtual void OnBoardCalculate() override;
    virtual void OnHealthCheck() override;
    virtual void OnBinaryFrame(uint8_t type, const QByteArray& payload) = 0;

    void ProcessCustomLinkEdges();
    void SyncCustomLinkStates();
    void EnqueueCommand(const string& command);
    void FlushCommandQueue();
    void ProcessIncoming();
    virtual void NegotiateProtocol();

    UArduinoBinaryStreamParser Parser;
    QQueue<QByteArray> CommandQueue;
    bool ProtocolNegotiated = false;
};

} // namespace RDK

#endif
