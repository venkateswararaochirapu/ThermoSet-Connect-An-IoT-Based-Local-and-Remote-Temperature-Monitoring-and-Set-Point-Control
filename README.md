<div align="center">

# 🌡️ ThermoSet Connect

### IoT-Based Local & Remote Temperature Monitoring, Set-Point Control and Alert System

![MCU](https://img.shields.io/badge/MCU-LPC2148%20(ARM7)-0b6e99?style=for-the-badge)
![IDE](https://img.shields.io/badge/IDE-Keil%20uVision-orange?style=for-the-badge)
![Language](https://img.shields.io/badge/Language-Embedded%20C-1f6feb?style=for-the-badge)
![Cloud](https://img.shields.io/badge/Cloud-ThingSpeak-2ea44f?style=for-the-badge)
![WiFi](https://img.shields.io/badge/Wi--Fi-ESP--01-8250df?style=for-the-badge)

<img src="docs/images/block_diagram.svg" alt="ThermoSet Connect block diagram" width="900">

</div>

---

## 📑 Table of Contents
1. [Overview](#-overview)
2. [Features](#-features)
3. [Block Diagram](#-block-diagram)
4. [Hardware & Software](#-hardware--software)
5. [Pin Connections](#-pin-connections)
6. [How It Works](#-how-it-works)
7. [LCD Output](#-lcd-output)
8. [Keypad Menu](#-keypad-menu)
9. [ESP-01 AT Commands](#-esp-01-at-commands)
10. [ThingSpeak Setup](#-thingspeak-setup)
11. [Build, Configure & Flash](#-build-configure--flash)
12. [Configuration (Macros)](#-configuration-macros)
13. [Project Structure](#-project-structure)
14. [Hardware Photos](#-hardware-photos)
15. [Limitations & Future Work](#-limitations--future-work)
16. [Author](#-author)

---

## 📖 Overview

**ThermoSet Connect** continuously measures temperature with an **LM35** sensor, shows it on a **16x2 LCD**, and uploads it to the **ThingSpeak cloud** through an **ESP-01 Wi-Fi module**. A configurable **set point** decides when the temperature is too high. When it is exceeded, the system:

* sounds a **buzzer** and lights an **alert LED** locally, and
* posts an **alert** to the cloud immediately.

The set point can be changed **locally** (switch + 4x4 keypad) or **remotely** (ThingSpeak channel). It is stored in an **AT24C256 EEPROM**, so it survives power failures.

Useful for: laboratories, server rooms, cold-storage facilities and industrial equipment.

## ✨ Features

| | Feature |
|---|---|
| 🌡️ | Continuous LM35 temperature measurement shown on LCD with live RTC time |
| ☁️ | Temperature uploaded to ThingSpeak at a fixed interval (150 s) |
| 🚨 | Local alarm (buzzer + LED) and cloud alert when temperature > set point |
| 🔕 | Press a keypad key during an alarm to silence the buzzer |
| ⌨️ | Local set-point change using switch interrupt (EINT2) and keypad menu |
| 🌐 | Remote set-point change from a dedicated ThingSpeak channel (checked every 90 s) |
| 💾 | Set point and previous set point stored in EEPROM (power-fail safe) |
| 🕒 | Time and date can be set from the keypad menu (on-chip RTC) |
| 📴 | Works offline: if Wi-Fi fails, local monitoring and alarm keep working |

## 🧩 Block Diagram

<p align="center"><img src="docs/images/block_diagram.svg" alt="Block diagram" width="900"></p> 

**Reading the diagram**

* **Inputs (left):** LM35 → ADC, switch → external interrupt, keypad → GPIO, EEPROM ↔ I2C.
* **Outputs (right):** LCD (8-bit GPIO), buzzer + LED (GPIO), ESP-01 ↔ UART0.
* **Cloud:** ESP-01 talks to ThingSpeak over Wi-Fi; a laptop or mobile views the graphs and can write a new set point.

## 🛠️ Hardware & Software

**Hardware**

* LPC2148 development board (12 MHz crystal, PLL to 60 MHz)
* LM35 temperature sensor
* AT24C256 I2C EEPROM
* 4x4 matrix keypad
* Push switch (external interrupt)
* 16x2 character LCD
* Buzzer and alert LED
* ESP-01 (ESP8266) Wi-Fi module
* USB-UART converter / DB-9 cable (programming)

**Software**

* Keil uVision (ARM compiler), Embedded C
* Flash Magic (ISP programming)
* ThingSpeak account (free)

## 🔌 Pin Connections

| Peripheral | LPC2148 pins | Notes |
|---|---|---|
| LCD data D0-D7 | P0.8 - P0.15 | 8-bit mode |
| LCD RS / RW / EN | P0.16 / P0.17 / P0.18 | |
| Keypad rows | P1.16 - P1.19 | |
| Keypad columns | P1.20 - P1.23 | |
| Buzzer | P1.24 | |
| Alert LED | P1.25 | |
| Switch (menu request) | P0.7 (EINT2) | falling-edge interrupt |
| LM35 output | P0.28 (AD0.1) | 10 mV / degree C |
| EEPROM SCL / SDA | P0.2 / P0.3 | I2C0, address 0x50 |
| ESP-01 TX / RX | P0.1 (RXD0) / P0.0 (TXD0) | UART0, 9600 baud |

> Clocks: CCLK = 60 MHz, PCLK = 15 MHz, UART 9600 baud, I2C 100 kHz.

## ⚙️ How It Works

```
Power on -> init UART, I2C, LCD, keypad, EINT2, RTC
         -> load set point from EEPROM (default 30 C if blank/invalid)
         -> show time + temperature, then connect Wi-Fi
         -> loop forever:
              read LM35
              temperature > set point ?  -> buzzer + LED + immediate cloud alert
              every 150 s                -> upload temperature (Channel 1)
              every  90 s                -> read remote set point (Channel 2)
                                            if different -> save to EEPROM
              switch pressed             -> open keypad menu
```

* **Alert rule:** alert only when `temperature > set point`; equal is not an alert.
* **Set-point range:** 1 - 150 C (default 30 C).
* **Remote update:** the device compares the cloud value with its current set point and only writes EEPROM when they differ.
* **Local update:** a new set point entered on the keypad is also pushed to Channel 2 so the cloud stays in sync.

## 🖥️ LCD Output

<p align="center"><img src="docs/images/lcd_screens.svg" alt="LCD screens" width="900"></p>

| Line 1 | Line 2 | Meaning |
|---|---|---|
| `10:25:42 SP:30` | `T:28C OK` | Time, set point, temperature - normal |
| `10:26:10 SP:30` | `T:35C ALERT!` | Temperature above set point |
| `10:26:30 SP:30` | `T:35C ALRT(sil)` | Alert active, buzzer silenced |
| `Cloud` | `Checking SP...` | Polling remote set point |
| `Cloud` | `SP update OK` | New set point received and saved |

## ⌨️ Keypad Menu

Press the **switch** to open the menu. The menu is driven from the keypad.

<p align="center"><img src="docs/images/keypad_menu.svg" alt="Keypad menu" width="800"></p>

| Page | Keys | Action |
|---|---|---|
| 1 | `1` / `2` / `3` | Set **Time** / **Date** / **Set point** |
| 1 | `N` | Next page |
| 2 | `4` | Show **Previous set point** |
| 2 | `P` / `C` | Previous page / Exit |

## 📡 ESP-01 AT Commands

<p align="center"><img src="docs/images/at_commands.svg" alt="AT command sequence" width="900"></p>

| Step | Command | Expected reply |
|---|---|---|
| Reset link | `AT+CWQAP` | `WIFI DISCONNECT` / `OK` |
| Check module | `AT` | `OK` |
| Echo off | `ATE0` | `OK` |
| Single connection | `AT+CIPMUX=0` | `OK` |
| Station mode | `AT+CWMODE=1` | `OK` |
| Join Wi-Fi | `AT+CWJAP="SSID","PASSWORD"` | `WIFI CONNECTED`, `WIFI GOT IP` |
| Open TCP | `AT+CIPSTART="TCP","<host>",80` | `CONNECT` |
| Send length | `AT+CIPSEND=<length>` | `>` |
| Upload | `GET /update?api_key=<KEY>&field1=<temp>&field2=<0/1>` | `SEND OK` |
| Read set point | `GET /channels/<ID>/fields/1.json?api_key=<KEY>&results=1` | `+IPD,...` |
| Close | `AT+CIPCLOSE` | `CLOSED` |

## ☁️ ThingSpeak Setup

<p align="center"><img src="docs/images/thingspeak_channels.svg" alt="ThingSpeak channels" width="900"></p>

1. Create a free account at [thingspeak.com](https://thingspeak.com).
2. Create **Channel 1 - Monitoring**: `Field 1 = Temperature`, `Field 2 = Alert`.
3. Create **Channel 2 - Set Point**: `Field 1 = Set Point`.
4. Note both **Channel IDs**, **Write API keys** and **Read API keys**.
5. To change the set point remotely, write a new value to Channel 2, Field 1 (ThingSpeak website or app).

> The image above is an illustration. Add your own screenshots to `docs/images/`.

## 🚀 Build, Configure & Flash

**1. Clone**
```bash
git clone https://github.com/venkateswararaochirapu/ThermoSet-Connect-An-IoT-Based-Local-and-Remote-Temperature-Monitoring-and-Set-Point-Control.git
```

**2. Enter your Wi-Fi and ThingSpeak details (local only)**

Open `esp01_defines.h` and replace the placeholders:
```c
#define WIFI_SSID         "YOUR_WIFI_SSID"
#define WIFI_PASSWORD     "YOUR_WIFI_PASSWORD"
#define TS1_WRITE_API_KEY "YOUR_CHANNEL1_WRITE_KEY"
#define TS1_READ_API_KEY  "YOUR_CHANNEL1_READ_KEY"
#define TS1_CHANNEL_ID    "<your channel 1 id>"
#define TS2_WRITE_API_KEY "YOUR_CHANNEL2_WRITE_KEY"
#define TS2_READ_API_KEY  "YOUR_CHANNEL2_READ_KEY"
#define TS2_CHANNEL_ID    "<your channel 2 id>"
```

**3. Create the Keil project**

1. In Keil uVision: *Project -> New uVision Project*, device **NXP LPC2148**.
2. Click **Yes** to copy the standard startup code (`Startup.s`) into the project.
3. In the startup configuration wizard set: PLL on, **CCLK = 60 MHz** (`PLLCFG = 0x24`, i.e. M = 5, P = 2 with the 12 MHz crystal), **VPBDIV = 0** (PCLK = 15 MHz), MAM fully enabled (`MAMCR = 2`, `MAMTIM = 4`).
4. Add all `.c` files from this repository: `thermoset_main.c`, `thermoset_fun_def.c`, `esp01.c`, `uart.c`, `i2c.c`, `i2c_eeprom.c`, `lcd.c`, `kpm.c`, `adc.c`, `lm35.c`, `rtc.c`, `delay.c`.
5. *Options for Target -> Output* -> tick **Create HEX File**.
6. Build (F7). Expected result: `0 Error(s), 0 Warning(s)`.

**4. Flash with Flash Magic**

* Device: LPC2148, COM port of your USB-UART, baud as per your board (e.g. 9600), oscillator 12 MHz.
* Select the generated `.hex` -> **Erase blocks used by Hex File** -> Start.
* Reset the board.

> The `.hex` you build contains your Wi-Fi password. Do not publish it (it is already ignored by `.gitignore`).

## 🔧 Configuration (Macros)

You can change the behaviour of the project by editing `#define` macros - no logic changes needed. **Rebuild and re-flash after any change.**

**`esp01_defines.h` (your network and cloud settings)**

| Macro | Purpose |
|---|---|
| `WIFI_SSID`, `WIFI_PASSWORD` | Wi-Fi network the ESP-01 joins |
| `TS1_CHANNEL_ID`, `TS1_WRITE_API_KEY`, `TS1_READ_API_KEY` | Channel 1 (temperature + alert) |
| `TS2_CHANNEL_ID`, `TS2_WRITE_API_KEY`, `TS2_READ_API_KEY` | Channel 2 (remote set point) |
| `TS_HOST` | ThingSpeak server address (`184.106.153.149`) |
| `BUFFER_SIZE` | ESP-01 receive buffer size in bytes (`350`) |

**`thermoset_defines.h` (behaviour)**

| Macro | Default | What it changes |
|---|---|---|
| `DEFAULT_SETPOINT` | `30` | Set point (C) used when EEPROM is blank or invalid |
| `MAX_SETPOINT` | `150` | Highest set point accepted from keypad or cloud |
| `CLOUD_SEND_INTERVAL_SEC` | `150` | How often temperature is uploaded (seconds) |
| `REMOTE_CHECK_INTERVAL_SEC` | `90` | How often the cloud set point is checked (seconds) |
| `BUZZER_PIN` / `ALERT_LED_PIN` | `24` / `25` | Port 1 pin numbers for buzzer and LED |
| `BUZZER_ACTIVE_HIGH` | `1` | Set `0` if your buzzer turns on with a LOW level |
| `EEPROM_SETPOINT_ADDR` / `EEPROM_PREVIOUS_SETPOINT_ADDR` | `0x0000` / `0x0001` | EEPROM locations of current and previous set point |

**`uart_defines.h`**

| Macro | Default | What it changes |
|---|---|---|
| `BAUD` | `9600` | UART0 speed - must match the ESP-01 baud rate |

**Rules to follow when changing values**

* ThingSpeak's free plan accepts an update about every 15 s per channel, so keep `CLOUD_SEND_INTERVAL_SEC` well above that (30 s or more).
* `MAX_SETPOINT` must stay 255 or less (one EEPROM byte) and within the LM35 range.
* Do not change the `EINT2_*` and `READNUM_*` macros unless you also change the hardware wiring or `kpm.c`.
* The `SET_INITIAL_RTC` / `INIT_*` macros are not active (the code is commented out); set the time and date from the keypad menu instead.

## 📂 Project Structure

```
ThermoSet-Connect/
|-- thermoset_main.c          main loop: alert, cloud timing, remote set-point check
|-- thermoset_fun_def.c       keypad menu, Wi-Fi connect, set-point handling, LCD status
|-- thermoset.h, thermoset_defines.h
|-- esp01.c / .h / _defines.h     ESP-01 AT commands, ThingSpeak read/write, credentials
|-- uart.c / .h / _defines.h      UART0 driver + receive interrupt for ESP-01 replies
|-- i2c.c / .h / _defines.h       I2C0 driver
|-- i2c_eeprom.c / .h / _defines.h    AT24C256 read/write
|-- lcd.c / .h / _defines.h       16x2 LCD driver
|-- kpm.c / .h / _defines.h       4x4 keypad driver
|-- adc.c / .h / _defines.h       ADC driver
|-- lm35.c / .h               LM35 temperature conversion
|-- rtc.c / .h / _defines.h       on-chip RTC (time and date)
|-- delay.c / .h              delay routines
|-- types.h, defines.h        common types and macros
|-- docs/images/              diagrams used in this README
|-- .gitignore, LICENSE
`-- README.md
```

## 📸 Hardware Photos

Add photos of your board in `docs/images/` and remove the comment markers below.

<!--
<p align="center">
  <img src="docs/images/setup_photo.jpg" alt="Complete hardware setup" width="420">
  <img src="docs/images/lcd_photo.jpg" alt="LCD showing temperature" width="420">
</p>
<p align="center">
  <img src="docs/images/alert_photo.jpg" alt="Alert condition" width="420">
  <img src="docs/images/thingspeak_screenshot.png" alt="ThingSpeak dashboard" width="420">
</p>
-->

## 🔭 Limitations & Future Work

* The keypad menu pauses temperature monitoring while it is open - add a timeout.
* Cloud transactions are blocking and take several seconds; use response polling with early exit.
* Wi-Fi is attempted at start-up only; add periodic reconnect.
* Add alert hysteresis to avoid repeated alerts when the temperature hovers at the set point.
* Verify ThingSpeak responses (rate-limit replies) before reporting success.
* Optional: buzzer ON/OFF cycles, SMS / e-mail alerts, data logging, watchdog timer.

## 👤 Author

**Venkateswara Rao Chirapu** - [@venkateswararaochirapu](https://github.com/venkateswararaochirapu)
Major Project: *ThermoSet Connect - IoT-Based Local and Remote Temperature Monitoring and Set-Point Control*

