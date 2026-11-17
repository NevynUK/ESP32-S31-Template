#!/usr/bin/env bash
# flash.sh -- build and flash the ESP32-S31 Function-Coreboard-1.
#
#   ./flash.sh                        autodetect the esptool port
#   ./flash.sh /dev/cu.usbmodem101    use a specific one
#
# The board has two USB paths and they are NOT interchangeable:
#
#   /dev/cu.usbmodem*    the S31's built-in USB-Serial/JTAG -- esptool, OpenOCD
#                        (and the secondary console)
#   /dev/cu.usbserial-*  the external bridge on UART0 (GPIO 58/59) -- the
#                        primary console
#
# OpenOCD and esptool cannot share the USB-Serial/JTAG port: stop any debug
# session before flashing.  CLion's "S31 Debug" configuration runs this script
# before it starts OpenOCD, so that ordering is handled there.

set -euo pipefail
cd "$(dirname "$0")"
# shellcheck source=scripts/idf-env.sh
. scripts/idf-env.sh

PORT="${1:-${ESPPORT:-}}"
if [ -z "$PORT" ]; then
    PORT="$(pick_port '/dev/cu.usbmodem*' || true)"
fi

if [ -z "$PORT" ]; then
    echo "error: no /dev/cu.usbmodem* found; pass the port explicitly" >&2
    ls /dev/cu.* 2>/dev/null >&2 || true
    exit 1
fi

if pgrep -qf 'openocd.*debug\.cfg'; then
    echo "error: OpenOCD is running and holds $PORT; stop the debug session first" >&2
    exit 1
fi

echo "==> building and flashing via $PORT"
idf_py -p "$PORT" build flash
date '+%Y-%m-%d %H:%M:%S' > "$IDF_BUILD_DIR/.last_flash"
