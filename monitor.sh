#!/usr/bin/env bash
# monitor.sh -- open the ESP-IDF monitor on the console.
#
#   ./monitor.sh                         autodetect: UART0 bridge, else USB-JTAG
#   ./monitor.sh /dev/cu.usbserial-110   use a specific port
#
# The primary console is UART0 on the external bridge (/dev/cu.usbserial-*).
# Close any other terminal program (CoolTerm etc.) on that port first.
# Needs a real terminal: run it from a shell, not as a CLion run configuration.

set -euo pipefail
cd "$(dirname "$0")"
# shellcheck source=scripts/idf-env.sh
. scripts/idf-env.sh

PORT="${1:-}"
if [ -z "$PORT" ]; then
    PORT="$(pick_port '/dev/cu.usbserial-*' || pick_port '/dev/cu.usbmodem*' || true)"
fi
[ -n "$PORT" ] || { echo "error: no serial port found; pass one explicitly" >&2; exit 1; }

# --no-reset: monitor without resetting the chip, so it can also watch a board
# that is under a debug session.
exec "$IDF_PYTHON" "$IDF_PATH/tools/idf.py" -B "$IDF_BUILD_DIR" -p "$PORT" monitor --no-reset
