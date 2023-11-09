# ESPresense

![GitHub release (latest by date)](https://img.shields.io/github/v/release/ESPresense/ESPresense)
![GitHub all releases](https://img.shields.io/github/downloads/ESPresense/ESPresense/total)
[![.github/workflows/main.yml](https://github.com/ESPresense/ESPresense/actions/workflows/build.yml/badge.svg)](https://github.com/ESPresense/ESPresense/actions/workflows/build.yml)


An ESP32 based presence detection node for use with the [Home Assistant](https://www.home-assistant.io/) [`mqtt_room` component](https://www.home-assistant.io/components/sensor.mqtt_room/) or [ESPresense-companion](https://github.com/ESPresense/ESPresense-companion) for indoor positioning

**Documentation:** https://espresense.com/

**Building:** [building](./BUILDING.md).

**Release Notes:** [changelog](./CHANGELOG.md).


# Manual Installation Firmware Update Guide

This guide will help you update the firmware on an ESP32 board using PlatformIO and VS Code. Make sure you have the following prerequisites in place:

- PlatformIO installed
- Downloaded and extracted the ESPresense project

## Steps:

1. **Erase the ESP32 Board**

    - Download [esptool.py](https://github.com/espressif/esptool).
    - Connect the board to your computer.
    - Open a terminal and navigate to the directory where esptool.py is located.
    - Run the following command to erase the ESP32 board:
      ```
      python esptool.py erase_flash
      ```
    - Note: The tool should detect the port automatically, but refer to the documentation to avoid erasing the wrong device.

2. **Open the ESPresense Project**

    - Open the ESPresense project in PlatformIO using Visual Studio Code (VS Code). The first-time setup may take a while as it downloads and installs dependencies.

3. **Select the Correct Port**

    - In PlatformIO, select the correct port for your ESP32 board.

4. **Upload the Firmware**

    - Click the right arrow icon at the bottom of the VS Code window to upload the firmware.
    - ![image](https://github.com/ESPresense/ESPresense/assets/19153649/53af9221-e027-480b-99d6-3bf35084b09f)

5. **Enter Bootloader Mode**

    - While the VS Code terminal indicates that it is connecting to the COM X port, follow these steps to enter bootloader mode:
      - Press both the `EN` and `BOOT` buttons on the board.
      - Release the `EN` button.
      - Wait 2-3 seconds.
      - Release the `BOOT` button.
      - The device should now start flashing the firmware.

7. **Replace Firmware File**

    - Visit the [ESPresense releases page](https://github.com/ESPresense/ESPresense/releases).
    - Download the latest firmware version (e.g., v3.3.1, named `esp32.bin`) and rename it to `firmware.bin`.

8. **Replace Firmware File in .pio Folder**

    - After the build process, PlatformIO should have created a .pio folder. Inside it, locate the file named `firmware.bin` (usually in `.pio/build/esp32`).
    - Replace this `firmware.bin` file with the latest firmware you downloaded and renamed in step 6.

9. **Upload the New Firmware**

    - Repeat step 4 to upload the new firmware and update it to the latest version.

10. **Configure Wi-Fi Credentials**

    - Once the firmware update is complete, go to [espresense.com](https://espresense.com/).
    - Connect to the ESP32 board using your web browser.
    - Insert the correct Wi-Fi credentials.
    - Note: You may encounter a Timeout error even with correct credentials. To check the connection, refresh the page to free up the COM port and open the Serial Monitor on PlatformIO or the tools you prefer (e.g. ArduinoIDE).
    - After pressing the `EN` button on the board, monitor the serial output to verify if it connects to your Wi-Fi network.

**Note**: Even if the firmware is updated to the latest version, the web interface is outdated and I don't find a way to update it.
Enjoy using your ESPresense board!

This text has been corrected and formatted using chatGPT
