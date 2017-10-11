# dmn-edc32
DMN-EDC diesel engine control (Bosch VE and inline pumps) for STM32. Current status: porting from avr (Arduino) in progress. For more information, contact syncro16@outlook.com.

## Tools needed
* stm32f334 board (nucleo or discovery version)
* openocd (debugger interface over usb)
* gcc-arm toolchain
* gdb recommended for uploading binary. Other ways may work, but they might erase entire flash, including the last 3 pages, which is reserved for saved user settings.
* terminal emulator (screen is fine)
* STM32CubeMX if you want to reconfigure some lowlevel HAL things (automatically generates main.c and some other files)

## Compiling
Manually check Makefile in sources directory, edit BINPATH for your gcc-arm toolchain location. Type `make` to compile sources.

### Communicating with openocd

Start openocd with `openocd -f util/stm32f334discovery.cfg`

### Uploading binary with GDB

Go to `sources/build` directory aAnd start gdb with command `arm-none-eabi-gdb dmn-edc32.elf --eval-command="target remote localhost:3333"`

* Type `load` to load binary to stm32 board
* Type `c` to start the program
* You may stop the program execution in with `ctrl-c`

### Serial communucation

While program is running on board, you can connect it with serial port. Port name will vary depending your operating system (/dev/ttyUSBx, COMx, etc..). In my system (macOS), it appears at `/dev/tty.usbmodem1413`.

* Type `screen /dev/tty.usbmodem1413 115200` to open connection.
* Type `ctrl-k` and `y` to exit from screen.

## Daughter card for communicating with engine electronics

Development in progress. Available **soon**. See docs directory for updates.

