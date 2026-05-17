#ifndef UARDUINOCUSTOMLINK_H
#define UARDUINOCUSTOMLINK_H

#include "UArduinoBoard.h"
#include "Protocol/UArduinoBinaryStreamParser.h"

#include <QByteArray>
#include <QQueue>
#include <QString>

namespace RDK {

class RDK_LIB_TYPE UArduinoCustomLink : public UArduinoBoard {
public:
    UProperty<string, UArduinoCustomLink, ptPubParameter | ptInput> Command;
    UProperty<bool, UArduinoCustomLink, ptPubState> SendCommandFlag;
    UProperty<string, UArduinoCustomLink, ptPubState> SentCommand;
    UProperty<string, UArduinoCustomLink, ptPubState | ptInput> InputCommand;
    UProperty<int, UArduinoCustomLink, ptPubParameter> ProtocolVersion;
    UProperty<int, UArduinoCustomLink, ptPubState> RxFrameCount;
    UProperty<int, UArduinoCustomLink, ptPubState> TxCommandCount;

    UProperty<bool, UArduinoCustomLink, ptPubParameter | ptInput> SendCommand;
    UProperty<bool, UArduinoCustomLink, ptPubParameter | ptInput> RequestGetStatus;
    UProperty<bool, UArduinoCustomLink, ptPubParameter | ptInput> RequestProtocolNegotiate;

    UProperty<bool, UArduinoCustomLink, ptPubState> IsProtocolReady;
    UProperty<bool, UArduinoCustomLink, ptPubState> HasPendingCommands;
    UProperty<string, UArduinoCustomLink, ptPubState> LastSentCommand;

    UArduinoCustomLink();
    virtual ~UArduinoCustomLink();

    virtual UArduinoCustomLink* New() = 0;

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
    void NegotiateProtocol();

    UArduinoBinaryStreamParser Parser;
    QQueue<QByteArray> CommandQueue;
    bool ProtocolNegotiated = false;
};

} // namespace RDK

#endif
