# Debugging FreeRTOS Stacks

PlatformIOs built in tools for debugging the FreeRTOS stack are limited.
NXP have a free FreeRTOS debug plugin for Eclipse which will give more detailed debug information regarding stack usage. 

### Requirements 
- [**MCUXpresso IDE**](https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE) - This requires a free NXP account to access the download link. Any Eclipse install will work but if using another install, the FreeRTOS plugin will need to be installed separately. 
- **JTAG Debugger** - Any debugger compatible with OpenOCD should work. This below steps were tested with a Segger J-Link
- **PlatformIO**
- **OpenOCD** - Changing the PlatformIO project environment to an environment set to use ```upload_protocol = jlink``` will automatically download the ```tool-openocd-esp32``` package.

### Setup the MCUXpresso debugger
1. Open MCUXpresso - any workspace can be used
2. Select Run > Debug Configurations
3. Double click on ``` GDB Hardware Debugging``` to create a new configuration
4. On the *Main* tab -
    a. **Project** - Can be left blank, or point to project code.
    b. **C/C++ Application** - Point to the ```firmware.elf``` file. If built from PlatformIO will be in the ```ESPresense/.pio/build/<env name>``` directory. 
    c. **Build** - Disable auto build
5. On the *Debugger* tab -
    a. **GDB Command** - Path to ```xtensa-esp32-elf-gdb.exe```, if using defaults in PlatformIO will be ```~/.platformio/packages/toolchain-xtensa32/bin/xtensa-esp32-elf-gdb.exe```
    b. **Remote Target** - Check Use remote target, JTAG Device - ```OpenOCD (via socket)```, GDB Connection String ```localhost:3333```
6. On the *Startup* tab - 
    a. **Initialisation commands**:
        - ```set mem inaccessible-by-default off```
        - ```mon reset halt```
        - ```flushregs```
        - ```set remote hardware-watchpoint-limit 2```
    b. **Load Symbols** - Use project binary
    c. Uncheck all boxes except ```Load Symbols``` and ```Resume```
7. Apply Changes

### Running the debug tool
1. Program the ESP32 with the target build and connect the ESP32 to the debug probe.
2. Start the OpenOCD Server:
```bash
cd ~\.platformio\packages\tool-openocd-esp32
.\bin\openocd.exe -f .\share\openocd\scripts\interface\J-Link.cfg -f .\share\openocd\scripts\board\esp-wroom-32.cfg -c "adapter_khz 4000"
```
3. Start the debug tool in MCDExpresso with the debug symbol on the taskbar. **Note**: I have found the first connection to the debug session often fails with a warning ```failed to execute MI Command
--exec-continue```. Canceling this and selecting the debug symbol again correctly starts the debugger.
4. FreeRTOS information can be viewed from the RTOS > Task List from the top menu. This will refresh after pausing the debugger or reaching a breakpoint. 


### References
https://mcuoneclipse.com/2019/09/22/eclipse-jtag-debugging-the-esp32-with-a-segger-j-link/