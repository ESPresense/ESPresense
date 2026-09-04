#!/usr/bin/env python
import asyncio
import os
import time

#this list contains array of esp32 clients,
# and each client contains mDNS name and the path to .bin file
esps = [
  #mDNS name of ESP   #path to ".bin" file
    ['192.168.128.113', 'macchina-a0'],
    ['192.168.128.64', 'm5atom'],
    ['192.168.128.84', 'm5atom']
]
esp_respond_sender_port = '3232'
sender_to_esp_port = '3232'

async def main():
    proc = await asyncio.create_subprocess_exec('pio', 'run', '-e', 'm5atom', '-e', 'macchina-a0')
    await proc.wait()

    for esp in esps:
        espota = os.path.expanduser('~/.platformio/packages/framework-arduinoespressif32/tools/espota.py')
        args = ['python3', espota, '-i', esp[0], '-p', sender_to_esp_port, '-P', esp_respond_sender_port,
                '-f', '.pio/build/' + esp[1] + '/firmware.bin']
        print(' '.join(args))
        proc = await asyncio.create_subprocess_exec(*args)
        await proc.wait()

asyncio.run(main())
