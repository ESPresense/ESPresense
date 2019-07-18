# Generic BLE Hardware
Here is a list of devices known to be "trackable":
* MiFlora plant sensor
* MiBand 2 Fitness tracker
* [Puck-JS](https://www.espruino.com/Puck.js), if programmed to [broadcast beacon packets](https://gist.github.com/jptrsn/d6cb9b9cdbcd41f3500708f8b694cad2 "An example project to broadcast iBeacon packets")

#### Add to the list
If you have a different BLE-enabled piece of hardware that you have gotten to work with this project, please let me know about it! You can [open an issue](https://github.com/jptrsn/ESP32-mqtt-room/issues/new) or just submit a pull request to update the documentation right in the repository!

## Device ID
To track generic BLE advertisements, you will need to know the hardware MAC address of the device. You can use an app on your smart phone to scan for and identify the correct device - I recommend [NRF Connect](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp) from [Nordic Semiconductor](https://play.google.com/store/apps/dev?id=7265678888812659353) for Android (and I believe there is an iOS version as well).

Alternatively, you can subscribe to the `room_presence` topic and view what devices are being reported there. Note that modern smart phones will randomize their hardware MAC address when broadcasting advertisements, so you must use a beacon app to track your phone (see the [section on tracking phones](./android) for more information).

To track the device, specify the `device_id` entry as the hardware MAC address, without the colons.

## configuration.yaml
Here is an example of how an entry into your `configuration.yaml` file should look:
```yaml
sensor:

# One entry for each beacon you want to track
  - platform: mqtt_room
    device_id: C80F104DC7EB # Match the Bluetooth MAC address of the device
    name: 'Mi Band'
    state_topic: 'room_presence'
    timeout: 60
    away_timeout: 120
```
