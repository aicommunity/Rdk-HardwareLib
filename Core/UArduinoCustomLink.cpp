#include "UArduinoCustomLink.h"

#include "Transport/UArduinoSerialSession.h"

namespace RDK {

UArduinoCustomLink::UArduinoCustomLink()
    : Command("Command", this)
    , SendCommandFlag("SendCommandFlag", this)
    , SentCommand("SentCommand", this)
    , InputCommand("InputCommand", this)
    , ProtocolVersion("ProtocolVersion", this)
    , RxFrameCount("RxFrameCount", this)
    , TxCommandCount("TxCommandCount", this)
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
    Parser.setProtocolVersion(1);
    ProtocolNegotiated = false;
    return true;
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
    QByteArray line = QByteArray::fromStdString(command);
    if (!line.endsWith('\n'))
        line.append('\n');
    CommandQueue.enqueue(line);
}

void UArduinoCustomLink::FlushCommandQueue()
{
    if (!Session || !Session->isOpen())
        return;

    while (!CommandQueue.isEmpty()) {
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
    if (!UArduinoBoard::ACalculate())
        return false;

    if (InputCommand.IsConnected() && InputCommand.IsNewData() && !InputCommand->empty()) {
        EnqueueCommand(InputCommand);
        SentCommand = InputCommand;
    }

    if (SendCommandFlag) {
        EnqueueCommand(Command);
        SentCommand = Command;
        SendCommandFlag = false;
    }

    if (ConnectionState == ArduinoConnected)
        NegotiateProtocol();
    ProcessIncoming();
    FlushCommandQueue();
    return true;
}

} // namespace RDK
