# CLion set-up (ESP32-S31, ESP-IDF 6.1.1)

Committed project files name no machine-specific path; tools are found through
`scripts/idf-env.sh`.

| What | Committed reference | Resolved by |
|---|---|---|
| CMake profile | `CMakePresets.json` preset `default` → `build/` | CLion toolchain **ESP-IDF 6.1.1** |
| Flash | run config **Flash** → `flash.sh` | `scripts/idf-env.sh` (idf.py) |
| Debug | run config **S31 Debug** (Embedded GDB Server) | `scripts/clion/openocd` + `scripts/clion/gdb` |
| OpenOCD config | `debug.cfg` | `board/esp32s31-builtin.cfg` |

`idf.py` 6.1.1 also uses the `default` preset, so CLion and `idf.py` share one
`build/`. The ELF you debug is always the one that was flashed.

## One-time per machine

1. Install ESP-IDF 6.1.1 (EIM), so that `~/.espressif/tools/activate_idf_v6.1.1.sh` exists.
2. CLion → Settings → Build → Toolchains: add **ESP-IDF 6.1.1** (System), with the
   environment file `~/.espressif/tools/activate_idf_v6.1.1.sh`.
3. Open the project and enable the `default` CMake profile. Disable any other
   profile that also uses `build/`.

## Use

- **Build:** the hammer icon (CMake target `ESP32-S31-Template.elf`).
- **Flash:** run **Flash** (builds first).
- **Debug:** debug **S31 Debug**. It runs Flash, starts OpenOCD, then
  `reset halt`, then attaches GDB, so breakpoints from `app_main` onward hit.
  Stop leaves both cores running (`gdb-detach` hook in `debug.cfg`).
- **Monitor:** `./monitor.sh` from a terminal (UART0 bridge, `/dev/cu.usbserial-*`).

## Notes

- `esp32s31` is a *preview* target in IDF 6.1.1, and only `set-target` needs this:
  `idf.py --preview set-target esp32s31`. Build, flash and monitor work without
  `--preview`.
- OpenOCD and esptool both use `/dev/cu.usbmodem*`; `flash.sh` refuses to run
  while a debug session is open.
- All breakpoints are hardware ones (`gdb breakpoint_override hard`). Keep only a
  few enabled at a time; you'll see `Remote failure reply: 0E` if you run out.
- If CLion misses a breakpoint stop (UI says running, core halted), uncomment
  `set ESP_RTOS none` in `debug.cfg`.
- Session ends at once with SIGKILL → OpenOCD failed to start. Check with
  `scripts/clion/openocd -f debug.cfg -c "init; shutdown"`.
