# ESP32-S31-Template

Minimal ESP-IDF 6.1.1 project for the ESP32-S31 (tested on the ESP32-S31
Function-Coreboard-1), set up for building, flashing and debugging from CLion
or the command line.

## Command line

```bash
idf.py --preview set-target esp32s31   # once; esp32s31 is a preview target in IDF 6.1.1
./flash.sh                             # build + flash over /dev/cu.usbmodem*
./monitor.sh                           # console on UART0 (/dev/cu.usbserial-*)
```

`idf.py build flash monitor` also works. Only `set-target` needs `--preview`.

## CLion

Open the folder, enable the `default` CMake profile, and use the **Flash** and
**S31 Debug** run configurations. **S31 Debug** flashes the board, resets and
halts it, then attaches GDB through OpenOCD. For the one-time toolchain setup,
see [scripts/clion/README.md](scripts/clion/README.md).
