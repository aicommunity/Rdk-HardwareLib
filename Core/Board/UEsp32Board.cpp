#include "UEsp32Board.h"

#include "Transport/UArduinoSerialSession.h"
#include "UArduinoPropertyString.h"

namespace RDK {

UEsp32Board::UEsp32Board()
    : ChipTarget("ChipTarget", this)
    , FlashBaud("FlashBaud", this)
{
}

UEsp32Board::~UEsp32Board() = default;

UEsp32Board* UEsp32Board::New()
{
    return new UEsp32Board;
}

int UEsp32Board::DefaultBaudRate() const
{
    return 115200;
}

void UEsp32Board::ConfigureSerialSession(UArduinoSerialSession& session)
{
    // ESP32 USB-UART: avoid DTR/RTS reset pulse used for AVR boards.
    session.ToggleDtrOnOpen = false;
    session.DtrOnOpen = false;
    session.RtsOnOpen = false;
}

bool UEsp32Board::ADefault()
{
    UMcuSerialBoard::ADefault();
    BaudRate = DefaultBaudRate();
    ChipTarget = "esp32";
    FlashBaud = 921600;
    BundledFirmwareId = "";
    FirmwarePath = "";
    SyncDerivedStates();
    return true;
}

void UEsp32Board::RunUploadBlocking()
{
    UploadProgress = 0;
    UploadLastResult = UArduinoPropertyString::toStdProperty(QStringLiteral(
        "ESP32 flash via IDE esptool not wired yet — use flash_download_tool / esptool.py; Connect-only in P0"));
    LastError = *UploadLastResult;
    SyncDerivedStates();
}

void UEsp32Board::startUploadAsync()
{
    RunUploadBlocking();
}

} // namespace RDK
