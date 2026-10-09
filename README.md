# 000 OS — Cardputer ADV starter firmware

This is a real Arduino/PlatformIO firmware source project for the **M5Stack Cardputer ADV**. It is not a precompiled `.bin`, and it has not been compiled or tested on physical hardware in this environment.

## Implemented in this source
- Keyboard-driven main menu (`W`/`S` to move, `E` to select, `Q` to go back)
- Wi-Fi scan showing SSID and RSSI for nearby networks
- BLE discovery scan showing advertised device names and signal strength
- O-STYLE colour themes, saved in ESP32 NVS preferences
- Diagnostics: CPU frequency, free heap, Wi-Fi state, and accelerometer readings
- About / safe-use notice

## Hardware notes
- Cardputer ADV has Wi-Fi, BLE, IR emitter, BMI270 IMU, microSD, and a 240x135 display.
- This starter project does **not** yet implement IR transmission or SD browsing. NFC is not built into the Cardputer ADV; it requires compatible external hardware.
- Wi-Fi/BLE scans are passive discovery functions; use them responsibly and only test networks/devices you are authorised to inspect.

## Build
1. Install PlatformIO and the ESP32 platform in a development environment.
2. Open this folder as a PlatformIO project.
3. Build the environment `m5stack-cardputer-adv`.
4. Connect the Cardputer ADV with a USB data cable. For download mode, turn the side switch OFF, hold G0, connect/power it, then release G0.
5. Select the correct serial port and upload.

The official M5Stack Arduino setup page has the board/library setup and download-mode steps: https://docs.m5stack.com/en/arduino/m5cardputer/program

## Important
This project is a starter build. Because it has not been compiled against your exact installed library versions or tested on your device, build errors may require adjustments before flashing. Keep a factory-firmware recovery option available.
