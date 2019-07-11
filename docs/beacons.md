# Tracking Hardware

## Beacons
Generic beacon hardware should be compatible, provided it meets a beacon standard (such as iBeacon or Eddystone). There are plenty of generic beacons available from sellers such as:
* [Amazon](https://www.amazon.ca/gp/product/B07S3DQM6N/ref=as_li_tl?ie=UTF8&camp=15121&creative=330641&creativeASIN=B07S3DQM6N&linkCode=as2&tag=ptrsnja-20&linkId=2f55c80199c88db853bd583182384f50) - generic iBeacon
* [eBay](https://www.ebay.com/sch/i.html?_nkw=nrf51822+ibeacon)
* [Ali Express](https://www.aliexpress.com/wholesale?SearchText=nrf51822+ibeacon).

## configuration.yaml
Here is an example of how an entry into your `configuration.yaml` file should look:
```yaml
sensor:

# One entry for each beacon you want to track
  - platform: mqtt_room
    device_id: "fda50693a4e24fb1afcfc6eb07647825-5-0" # Note that major version must match, but any minor version will be ignored
    name: 'iBeacon Room Presence'
    state_topic: 'room_presence'
    timeout: 60
    away_timeout: 120
```
