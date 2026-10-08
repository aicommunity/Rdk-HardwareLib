param(
    [string]$RepoRoot = (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)),
    [string]$ArduinoCli = "",
    [string]$ArduinoData = "",
    [string]$ArduinoUser = "",
    [switch]$SkipLibraries
)

$ErrorActionPreference = "Stop"
$builder = Join-Path $RepoRoot "Scripts\build_arduino_firmware.ps1"
if (-not (Test-Path -LiteralPath $builder)) {
    throw "Repository firmware builder not found: $builder"
}

$arguments = @("-RepoRoot", $RepoRoot)
if ($ArduinoCli) { $arguments += @("-ArduinoCli", $ArduinoCli) }
if ($ArduinoData) { $arguments += @("-ArduinoData", $ArduinoData) }
if ($ArduinoUser) { $arguments += @("-ArduinoUser", $ArduinoUser) }
if ($SkipLibraries) { $arguments += "-SkipLibraries" }

& $builder @arguments
if ($LASTEXITCODE -ne 0) {
    throw "Repository firmware build failed (exit $LASTEXITCODE)."
}
