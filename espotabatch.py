#!/usr/bin/env python
import subprocess
import time

#this list contains array of esp32 clients,
# and each client contains mDNS name and the path to .bin file
#I only have 1 ESP so I duplicate mDNS entry for testing
esps = [
  #mDNS name of ESP   #path to ".bin" file
    ['192.168.128.114', 'macchina-a0'],
]
esp_respond_sender_port = '3232'
sender_to_esp_port = '3232'

cmd = 'pio run -e m5atom-matrix -e macchina-a0'
p = subprocess.call(cmd, shell=True)

for esp in esps:
    cmd = 'python3 ~/.platformio/packages/framework-arduinoespressif32/tools/espota.py -i '+esp[0]+' -p '+sender_to_esp_port+' -P '+esp_respond_sender_port+' -f .pio/build/'+esp[1]+'/firmware.bin'
    print (cmd)
    p = subprocess.call(cmd, shell=True)
