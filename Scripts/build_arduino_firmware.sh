#!/usr/bin/env bash
# Build bundled Arduino/ESP firmware HEX into Bin/ArduinoFirmware/.
# Thin entry under HardwareLib; delegates AVR sensor_lab/Firmata to repo root script,
# then builds nmsdk_motor_hub (Uno/Mega) when arduino-cli is available.
set -euo pipefail

HW_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REPO_ROOT="$(cd "${HW_ROOT}/../.." && pwd)"
SRC_FW="${HW_ROOT}/Firmware"
BIN_FW="${REPO_ROOT}/Bin/ArduinoFirmware"
CLI="${ARDUINO_CLI:-arduino-cli}"

if [[ -x "${REPO_ROOT}/Scripts/build_arduino_firmware.sh" ]]; then
  "${REPO_ROOT}/Scripts/build_arduino_firmware.sh" "$@"
fi

if ! command -v "${CLI}" >/dev/null 2>&1; then
  echo "arduino-cli not found; skipped nmsdk_motor_hub build" >&2
  exit 0
fi

MOTOR_INO="${SRC_FW}/nmsdk_motor_hub/nmsdk_motor_hub.ino"
if [[ ! -f "${MOTOR_INO}" ]]; then
  echo "missing ${MOTOR_INO}" >&2
  exit 1
fi

mkdir -p "${BIN_FW}/nmsdk_motor_hub"
build_motor() {
  local fqbn="$1"
  local outdir="$2"
  local dest="$3"
  mkdir -p "${outdir}"
  "${CLI}" compile -b "${fqbn}" "${SRC_FW}/nmsdk_motor_hub" --output-dir "${outdir}"
  local hex
  hex="$(find "${outdir}" -name '*.hex' | head -1)"
  cp "${hex}" "${dest}"
}

build_motor "arduino:avr:uno" "${BIN_FW}/.build_motor_uno" "${BIN_FW}/nmsdk_motor_hub/uno.hex"
build_motor "arduino:avr:mega" "${BIN_FW}/.build_motor_mega" "${BIN_FW}/nmsdk_motor_hub/mega2560.hex"

mkdir -p "${SRC_FW}/nmsdk_motor_hub"
cp "${BIN_FW}/nmsdk_motor_hub/"*.hex "${SRC_FW}/nmsdk_motor_hub/" 2>/dev/null || true
cp "${SRC_FW}/manifest.json" "${BIN_FW}/manifest.json" 2>/dev/null || true

echo "Done motor hub HEX → ${BIN_FW}/nmsdk_motor_hub/"
ls -la "${BIN_FW}/nmsdk_motor_hub/"*.hex 2>/dev/null || true
