#include "UArduinoCustomLink.h"

#include "Transport/UArduinoSerialSession.h"
#include "UArduinoPropertyString.h"

namespace RDK {

UArduinoCustomLink::UArduinoCustomLink()
    : Command("Command", this)
    , SendCommandFlag("SendCommandFlag", this)
    , SentCommand("SentCommand", this)
    , InputCommand("InputCommand", this)
    , ProtocolVersion("ProtocolVersion", this)
    , RxFrameCount("RxFrameCount", this)
    , TxCommandCount("TxCommandCount", this)
    , SendCommand("SendCommand", this)
    , RequestGetStatus("RequestGetStatus", this)
    , RequestProtocolNegotiate("RequestProtocolNegotiate", this)
    , IsProtocolReady("IsProtocolReady", this)
    , HasPendingCommands("HasPendingCommands", this)
    , LastSentCommand("LastSentCommand", this)
{
}

UArduinoCustomLink::~UArduinoCustomLink() = default;

bool UArduinoCustomLink::ADefault()
{
    UArduinoBoard::ADefault();
    Command = "";
    SendCommandFlag = false;
    SentCommand = "";
    InputCommand = "";
    ProtocolVersion = 1;
    RxFrameCount = 0;
    TxCommandCount = 0;
    SendCommand = false;
    RequestGetStatus = false;
    RequestProtocolNegotiate = false;
    LastSentCommand = "";
    Parser.setProtocolVersion(1);
    ProtocolNegotiated = false;
    SyncCustomLinkStates();
    return true;
}

void UArduinoCustomLink::SyncCustomLinkStates()
{
    IsProtocolReady = ProtocolNegotiated && IsConnected;
    bool pending = !CommandQueue.isEmpty();
    if (Session && Session->isOpen() && Session->bytesToWrite() > 0)
        pending = true;
    HasPendingCommands = pending;
    if (!SentCommand->empty())
        LastSentCommand = SentCommand;
}

void UArduinoCustomLink::ProcessCustomLinkEdges()
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

    if (InputCommand.IsConnected() && InputCommand.IsNewData() && !InputCommand->empty()) {
        EnqueueCommand(InputCommand);
        SentCommand = InputCommand;
    }

    if (SendCommandFlag) {
        if (!Command->empty()) {
            EnqueueCommand(Command);
            SentCommand = Command;
        }
        SendCommandFlag = false;
    }
}

void UArduinoCustomLink::NegotiateProtocol()
{
    if (ProtocolNegotiated || ConnectionState != ArduinoConnected)
        return;

    if (ProtocolVersion >= 2)
        EnqueueCommand("PROTO 2");

    ProtocolNegotiated = true;
    Parser.setProtocolVersion(ProtocolVersion);
}

void UArduinoCustomLink::EnqueueCommand(const string& command)
{
    if (command.empty())
        return;
    const QByteArray line_bytes = UArduinoPropertyString::fromStdProperty(command).toUtf8();
    QByteArray line(line_bytes);
    if (!line.endsWith('\n'))
        line.append('\n');
    CommandQueue.enqueue(line);
}

void UArduinoCustomLink::FlushCommandQueue()
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

void UArduinoCustomLink::ProcessIncoming()
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

void UArduinoCustomLink::OnHealthCheck()
{
    UArduinoBoard::OnHealthCheck();
    EnqueueCommand("GET STATUS");
    FlushCommandQueue();
}

void UArduinoCustomLink::OnBoardCalculate()
{
    if (ConnectionState != ArduinoConnected)
        ProtocolNegotiated = false;
    else
        NegotiateProtocol();
    ProcessIncoming();
    FlushCommandQueue();
}

bool UArduinoCustomLink::ACalculate()
{
    ProcessCustomLinkEdges();

    if (!UArduinoBoard::ACalculate())
        return false;

    if (ConnectionState == ArduinoConnected)
        NegotiateProtocol();
    ProcessIncoming();
    FlushCommandQueue();
    SyncCustomLinkStates();
    return true;
}

} // namespace RDK
