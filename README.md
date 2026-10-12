# CHIP-8 Handheld Console

A handheld game console built on an STM32F103C8T6 "Blue Pill". A CHIP-8 emulator written in C++ runs the games, on top of device drivers written in C that follow the Unix driver model: every device exposes the same five operations (open, close, read, write, ioctl).

## Status

In development. The toolchain, pin map, and architecture are settled, and the firmware is being written in the order below.

- [ ] Driver pattern proven with the LED driver
- [ ] Display initializes (all pixels on)
- [ ] Display accepts a full frame
- [ ] Pixel addressing
- [ ] Emulator runs on the board (IBM logo)
- [ ] Sound
- [ ] Input and a playable game

Planned after that: loading games from an SD card, DMA for display transfers, a game menu, and battery power.

## Hardware

| Part | Details |
| --- | --- |
| Microcontroller | STM32F103C8T6 (Cortex-M3, 64 KB flash, 20 KB RAM) on a Blue Pill board |
| Display | 0.96 inch 128x64 OLED, SSD1306 controller, 4-wire SPI |
| Input | 8 push buttons to ground, internal pull-ups |
| Sound | Active buzzer, switched through a transistor |
| Indicators | Red and green LEDs |
| Storage | microSD breakout on the display's SPI bus (firmware planned) |
| Programmer | ST-Link over SWD |
| Power | USB |

CHIP-8 expects a 16-key keypad. This console has 8 buttons, so each game carries its own map from buttons to CHIP-8 keys.

## Pin map

| Function | Pin | Notes |
| --- | --- | --- |
| Red LED | PB8 | |
| Green LED | PB9 | |
| SPI1 clock | PB3 | SPI1 on its alternate pins |
| SPI1 MISO | PB4 | Reserved for the SD card |
| SPI1 MOSI | PB5 | |
| OLED reset | PB6 | |
| OLED data/command | PB7 | |
| OLED chip select | PA1 | |
| SD card chip select | PA8 | |
| Buzzer | PA0 | |
| Buttons 0 to 3 | PB11, PB10, PB1, PB0 | Input, pull-up |
| Buttons 4 to 7 | PA7, PA6, PA5, PA4 | Input, pull-up |
| Debug (SWD) | PA13, PA14 | |

## Architecture

The firmware has four layers. Calls only go downward.

```
+---------------------------------------------------------------+
| app.cpp                                                       |
| Opens the devices, then runs one frame 60 times a second      |
+---------------------------------------------------------------+
        |  calls directly                  |  calls through driver_t
        v                                  |
+----------------------------------------+ |
| Logic (no hardware, runs on a desktop) | |
|   chip8.cpp   screen.c   roms.c        | |
+----------------------------------------+ |
                                           v
+---------------------------------------------------------------+
| Drivers in C (each one exports a driver_t)                    |
|   drv_led.c   drv_oled.c   drv_buzzer.c   drv_keypad.c        |
+---------------------------------------------------------------+
        |  HAL calls
        v
+---------------------------------------------------------------+
| STM32 HAL and CubeMX setup (generated)                        |
+---------------------------------------------------------------+
```

- Only the drivers call the HAL.
- The logic files and the drivers never call each other.
- `app.cpp` is the only file that sees every layer.

### The driver interface

Every driver is one `.c` file that fills in this struct and exports it as a constant. It is the same idea as the file-operations table in a Linux character driver, cut down to five entries.

```c
/* Core/Inc/driver.h */
typedef struct {
    int (*open)(void);
    int (*close)(void);
    int (*read)(void *buf, size_t len);
    int (*write)(const void *buf, size_t len);
    int (*ioctl)(int request, void *arg);
} driver_t;

enum {
    DRV_OK      =  0,
    DRV_ERR     = -1,   /* hardware or HAL failure */
    DRV_ENOTSUP = -2,   /* this device has no such operation */
    DRV_EINVAL  = -3,   /* bad length, request or argument */
    DRV_EBUSY   = -4    /* reserved for later (DMA) */
};
```

- `read` and `write` return the number of bytes transferred, or a negative code.
- `open`, `close` and `ioctl` return `DRV_OK` or a negative code.
- No pointer in the struct is ever null. An operation a device lacks points at a shared stub that returns `DRV_ENOTSUP`.
- Callers reach a device only through its struct, for example `dev_led.write(&bits, 1)`.

### Devices

| Device | File | open | read | write | ioctl |
| --- | --- | --- | --- | --- | --- |
| `dev_led` | `drv_led.c` | Both LEDs off | Not supported | 1 byte: bit 0 red, bit 1 green | Not supported |
| `dev_display` | `drv_oled.c` | Reset pulse, startup sequence, display on | Not supported | Exactly 1024 bytes: one full frame in the display's page format | Contrast, power, invert, all-pixels-on test |
| `dev_buzzer` | `drv_buzzer.c` | Buzzer off | Not supported | 1 byte: 0 off, nonzero on | Not supported |
| `dev_keypad` | `drv_keypad.c` | Clear the debounce state | 1 byte: bit n set means button n is held | Not supported | Not supported |

The keypad driver also exports `keypad_tick()`, which samples the buttons once and updates the debounce state. The five operations have no slot for periodic work, so this is the driver's interrupt half, and the struct is the half that programs call.

### One frame

`app_main` opens the four devices, loads a game, and then runs these steps 60 times a second.

1. Call `keypad_tick()`, then read one byte from `dev_keypad`.
2. Translate held buttons into CHIP-8 keys using the current game's button map.
3. Run ten emulator instructions (600 per second).
4. Decrement the emulator's delay and sound timers.
5. Switch the buzzer on if the sound timer is above zero, otherwise off.
6. If the emulator drew something, scale its 64x32 screen to 128x64 and write the frame to `dev_display`.

## Source layout

All project code lives flat in `Core/Inc` and `Core/Src`, next to the generated files.

| File | Language | Job |
| --- | --- | --- |
| `Core/Inc/driver.h` | C | `driver_t` and the return codes |
| `Core/Src/driver.c` | C | Shared stubs that return `DRV_ENOTSUP` |
| `Core/Inc/devices.h` | C | Device declarations, ioctl requests, `keypad_tick` |
| `Core/Src/drv_led.c` | C | LED driver |
| `Core/Src/drv_oled.c` | C | Display driver |
| `Core/Src/drv_buzzer.c` | C | Buzzer driver |
| `Core/Src/drv_keypad.c` | C | Button driver with debounce |
| `Core/Inc/screen.h`, `Core/Src/screen.c` | C | Framebuffer, pixel function, CHIP-8 scaling |
| `Core/Inc/chip8.h`, `Core/Src/chip8.cpp` | C++ | The emulator core |
| `Core/Inc/roms.h`, `Core/Src/roms.c` | C | Built-in games and their button maps |
| `Core/Inc/app.h`, `Core/Src/app.cpp` | C++ | Startup and the main loop |
| `Core/Src/main.c` | C, generated | Clock and peripheral setup, then one call to `app_main` |

`chip8.cpp` and `screen.c` include no hardware headers, so both also compile and run on a desktop.

## Building

Requirements:

- STM32CubeMX
- STM32CubeIDE 2.x
- An ST-Link programmer

Steps:

1. Open the `.ioc` file in STM32CubeMX and click Generate Code. The toolchain is set to STM32CubeIDE.
2. In STM32CubeIDE, choose File > Import > General > Existing Projects into Workspace and select this folder.
3. Build with the hammer icon.
4. Connect the ST-Link to the board's SWD header (3.3 V, SWDIO, SWCLK, ground) and start a debug session to flash.

The `.ioc` sets Debug to Serial Wire. Leave that in place: without it, the generated code disables the debug pins and the ST-Link cannot reconnect after the first flash.

## Acknowledgements

The emulator core began from Austin Morlan's guide, [Building a CHIP-8 Emulator](https://austinmorlan.com/posts/chip8_emulator/).
