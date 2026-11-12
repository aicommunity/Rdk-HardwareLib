#include "UEsp32JsonLink.h"

#include "Transport/UArduinoSerialSession.h"
#include "UArduinoPropertyString.h"

namespace RDK {

UEsp32JsonLink::UEsp32JsonLink()
    : Command("Command", this)
    , SendCommand("SendCommand", this)
    , LastTxJson("LastTxJson", this)
    , LastRxJson("LastRxJson", this)
    , LastFeedbackJson("LastFeedbackJson", this)
    , TxCount("TxCount", this)
    , RxCount("RxCount", this)
{
}

UEsp32JsonLink::~UEsp32JsonLink() = default;

bool UEsp32JsonLink::ADefault()
{
    UEsp32Board::ADefault();
    Command = "";
    SendCommand = false;
    LastTxJson = "";
    LastRxJson = "";
    LastFeedbackJson = "";
    TxCount = 0;
    RxCount = 0;
    BaudRate = 115200;
    return true;
}

void UEsp32JsonLink::EnqueueJson(const QString& json)
{
    if (json.isEmpty())
        return;
    QByteArray line = json.toUtf8();
    if (!line.endsWith('\n'))
        line.append('\n');
    TxQueue.enqueue(line);
}

void UEsp32JsonLink::FlushTxQueue()
{
    if (!Session || !Session->isOpen())
        return;
    while (!TxQueue.isEmpty()) {
        if (Session->bytesToWrite() > 0)
            return;
        const QByteArray line = TxQueue.dequeue();
        Session->write(line);
        LastTxJson = UArduinoPropertyString::toStdProperty(QString::fromUtf8(line).trimmed());
        TxCount = TxCount + 1;
        TouchActivity();
    }
}

void UEsp32JsonLink::ProcessIncomingLines()
{
    if (!Session)
        return;
    const QByteArray data = Session->takeReceivedBytes();
    if (data.isEmpty())
        return;
    TouchActivity();
    LastHealthResponseMs = LastActivityMs;
    LineBuffer.append(data);
    while (true) {
        const int nl = LineBuffer.indexOf('\n');
        if (nl < 0)
            break;
        QByteArray one = LineBuffer.left(nl);
        LineBuffer.remove(0, nl + 1);
        if (one.endsWith('\r'))
            one.chop(1);
        const QString line = QString::fromUtf8(one).trimmed();
        if (line.isEmpty())
            continue;
        LastRxJson = UArduinoPropertyString::toStdProperty(line);
        RxCount = RxCount + 1;
        OnJsonLine(line);
    }
}

void UEsp32JsonLink::ProcessJsonEdges()
{
    if (SendCommand) {
        if (!Command->empty())
            EnqueueJson(UArduinoPropertyString::fromStdProperty(*Command));
        ResetEdge(SendCommand);
    }
}

void UEsp32JsonLink::OnJsonLine(const QString& line)
{
    LastFeedbackJson = UArduinoPropertyString::toStdProperty(line);
}

void UEsp32JsonLink::OnBoardCalculate()
{
    ProcessIncomingLines();
    FlushTxQueue();
}

bool UEsp32JsonLink::ACalculate()
{
    ProcessJsonEdges();
    if (!UEsp32Board::ACalculate())
        return false;
    ProcessIncomingLines();
    FlushTxQueue();
    return true;
}

} // namespace RDK
