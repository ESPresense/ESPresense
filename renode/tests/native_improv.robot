*** Variables ***
${UART}                 sysbus.uart0
${BOOTLOADER}          ${ENV:ESPRESENSE_BOOTLOADER}
${PARTITIONS}          ${ENV:ESPRESENSE_PARTITIONS}
${FIRMWARE}            ${ENV:ESPRESENSE_FIRMWARE}

*** Test Cases ***
Serial Improv Unity tests succeed on ESP32
    Execute Command           mach create "esp32-improv"
    Execute Command           machine LoadPlatformDescription @platforms/cpus/esp32.repl
    Execute Command           sysbus LoadBinary @${BOOTLOADER} 0x1000
    Execute Command           sysbus LoadBinary @${PARTITIONS} 0x8000
    Execute Command           sysbus LoadBinary @${FIRMWARE} 0x10000
    Create Terminal Tester    ${UART}
    Start Emulation
    Wait For Line On Uart     RENODE_TEST_RESULT=0  timeout=30
