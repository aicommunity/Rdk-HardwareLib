#include "UEsp32CustomLink.h"

#include "Transport/UArduinoSerialSession.h"
#include "UArduinoPropertyString.h"

namespace RDK {

UEsp32CustomLink::UEsp32CustomLink()
    : Command("Command", this)
    , SendCommandFlag("SendCommandFlag", this)
    , SentCommand("SentCommand", this)
    , ProtocolVersion("ProtocolVersion", this)
    , RxFrameCount("RxFrameCount", this)
    , TxCommandCount("TxCommandCount", this)
    , SendCommand("SendCommand", this)
    , RequestGetStatus("RequestGetStatus", this)
    , RequestProtocolNegotiate("RequestProtocolNegotiate", this)
    , IsProtocolReady("IsProtocolReady", this)
    , HasPendingCommands("HasPendingCommands", this)
{
}

UEsp32CustomLink::~UEsp32CustomLink() = default;

bool UEsp32CustomLink::ADefault()
{
    UEsp32Board::ADefault();
    Command = "";
    SendCommandFlag = false;
    SentCommand = "";
    ProtocolVersion = 2;
    RxFrameCount = 0;
    TxCommandCount = 0;
    SendCommand = false;
    RequestGetStatus = false;
    RequestProtocolNegotiate = false;
    Parser.setProtocolVersion(2);
    ProtocolNegotiated = false;
    SyncCustomLinkStates();
    return true;
}

void UEsp32CustomLink::SyncCustomLinkStates()
{
    IsProtocolReady = ProtocolNegotiated && IsConnected;
    bool pending = !CommandQueue.isEmpty();
    if (Session && Session->isOpen() && Session->bytesToWrite() > 0)
        pending = true;
    HasPendingCommands = pending;
}

void UEsp32CustomLink::ProcessCustomLinkEdges()
{
    if (RequestProtocolNegotiate) {
        ProtocolNegotiated = false;
        ResetEdge(RequestProtocolNegotiate);
    }
    if (RequestGetStatus) {
        EnqueueCommand("GET STATUS");
        FlushCommandQueue();
        ResetEdge(RequestGetStatus);
    }
    if (SendCommand) {
        if (!Command->empty()) {
            EnqueueCommand(Command);
            SentCommand = Command;
            FlushCommandQueue();
        }
        ResetEdge(SendCommand);
    }
    if (SendCommandFlag) {
        if (!Command->empty()) {
            EnqueueCommand(Command);
            SentCommand = Command;
        }
        SendCommandFlag = false;
    }
}

void UEsp32CustomLink::NegotiateProtocol()
{
    if (ProtocolNegotiated || ConnectionState != ArduinoConnected)
        return;
    if (ProtocolVersion >= 2)
        EnqueueCommand("PROTO 2");
    ProtocolNegotiated = true;
    Parser.setProtocolVersion(ProtocolVersion);
}

void UEsp32CustomLink::EnqueueCommand(const string& command)
{
    if (command.empty())
        return;
    QByteArray line = UArduinoPropertyString::fromStdProperty(command).toUtf8();
    if (!line.endsWith('\n'))
        line.append('\n');
    CommandQueue.enqueue(line);
}

void UEsp32CustomLink::FlushCommandQueue()
{
    if (!Session || !Session->isOpen())
        return;
    while (!CommandQueue.isEmpty()) {
        if (Session->bytesToWrite() > 0)
            return;
        const QByteArray line = CommandQueue.dequeue();
        Session->write(line);
        TxCommandCount = TxCommandCount + 1;
        TouchActivity();
    }
}

void UEsp32CustomLink::ProcessIncoming()
{
    if (!Session)
        return;
    const QByteArray data = Session->takeReceivedBytes();
    if (data.isEmpty())
        return;
    TouchActivity();
    LastHealthResponseMs = LastActivityMs;
    Parser.setDebug(ShowDebug);
    Parser.setProtocolVersion(ProtocolVersion);
    Parser.feed(data, [this](uint8_t type, const QByteArray& payload) {
        RxFrameCount = RxFrameCount + 1;
        OnBinaryFrame(type, payload);
    });
}

void UEsp32CustomLink::OnHealthCheck()
{
    UEsp32Board::OnHealthCheck();
    EnqueueCommand("GET STATUS");
    FlushCommandQueue();
}

void UEsp32CustomLink::OnBoardCalculate()
{
    if (ConnectionState != ArduinoConnected)
        ProtocolNegotiated = false;
    else
        NegotiateProtocol();
    ProcessIncoming();
    FlushCommandQueue();
}

bool UEsp32CustomLink::ACalculate()
{
    ProcessCustomLinkEdges();
    if (!UEsp32Board::ACalculate())
        return false;
    if (ConnectionState == ArduinoConnected)
        NegotiateProtocol();
    ProcessIncoming();
    FlushCommandQueue();
    SyncCustomLinkStates();
    return true;
}

} // namespace RDK
