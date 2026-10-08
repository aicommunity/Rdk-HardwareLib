#!/usr/bin/env bash
# HardwareLib entry point for the repository-wide bundled AVR firmware builder.
set -euo pipefail

HW_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REPO_ROOT="$(cd "${HW_ROOT}/../.." && pwd)"
exec "${REPO_ROOT}/Scripts/build_arduino_firmware.sh" "$@"
