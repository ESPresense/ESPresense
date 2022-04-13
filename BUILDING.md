# Building

This project uses [PlatformIO](https://platformio.org/) to simplify development.  The easiest way to get started is to install [VSCode](https://code.visualstudio.com/) and the [PlatformIO VSCode plugin](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide) (simple instructions [here](https://platformio.org/install/ide?install=vscode)).

Once you have opened this project, building and uploading the project to your ESP32 board is as easy as clicking on the Build and Upload buttons.

![PlatformIO Build/Upload Buttons](https://docs.platformio.org/en/latest/_images/platformio-ide-vscode-build-project.png)

## Changing PlatformIO project environment  
Project environments are used for different board configurations and are stored in ```platformio.ini```
Platforms can be changed by:

- Selecting the PlatformIO icon from the left bar in VSCode
- Selecting the desired build target under project tasks

![PlatformIO Env](https://community.platformio.org/uploads/default/original/2X/4/4d87f4672f1892ce54852fed3b8e3cf21b8aed4f.png)