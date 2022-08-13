#!/usr/bin/env python
import subprocess
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

cmd = 'pio run -e m5atom -e macchina-a0'
p = subprocess.call(cmd, shell=True)

for esp in esps:
    cmd = 'python3 ~/.platformio/packages/framework-arduinoespressif32/tools/espota.py -i '+esp[0]+' -p '+sender_to_esp_port+' -P '+esp_respond_sender_port+' -f .pio/build/'+esp[1]+'/firmware.bin'
    print (cmd)
    p = subprocess.call(cmd, shell=True)
