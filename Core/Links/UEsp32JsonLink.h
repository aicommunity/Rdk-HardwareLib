#ifndef UESP32JSONLINK_H
#define UESP32JSONLINK_H

#include "Board/UEsp32Board.h"

#include <QByteArray>
#include <QQueue>
#include <QString>

namespace RDK {

/** Line-oriented JSON UART link on ESP32 (WaveRover / Waveshare UGV). */
class RDK_LIB_TYPE UEsp32JsonLink : public UEsp32Board {
public:
    UProperty<string, UEsp32JsonLink, ptPubParameter | ptInput> Command;
    UProperty<bool, UEsp32JsonLink, ptPubParameter | ptInput> SendCommand;
    UProperty<string, UEsp32JsonLink, ptPubState> LastTxJson;
    UProperty<string, UEsp32JsonLink, ptPubState> LastRxJson;
    UProperty<string, UEsp32JsonLink, ptPubState> LastFeedbackJson;
    UProperty<int, UEsp32JsonLink, ptPubState> TxCount;
    UProperty<int, UEsp32JsonLink, ptPubState> RxCount;

    UEsp32JsonLink();
    virtual ~UEsp32JsonLink();
    virtual UEsp32JsonLink* New() = 0;

protected:
    virtual bool ADefault() override;
    virtual bool ACalculate() override;
    virtual void OnBoardCalculate() override;
    virtual void OnJsonLine(const QString& line);

    void EnqueueJson(const QString& json);
    void FlushTxQueue();
    void ProcessIncomingLines();
    void ProcessJsonEdges();

    QQueue<QByteArray> TxQueue;
    QByteArray LineBuffer;
};

} // namespace RDK

#endif
