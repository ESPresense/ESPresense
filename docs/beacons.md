# Tracking Hardware

## Beacons
Generic beacon hardware should be compatible, provided it meets a beacon standard (such as iBeacon or Eddystone). There are plenty of generic beacons available from sellers such as:
* [Amazon](https://www.amazon.ca/gp/product/B07S3DQM6N/ref=as_li_tl?ie=UTF8&camp=15121&creative=330641&creativeASIN=B07S3DQM6N&linkCode=as2&tag=ptrsnja-20&linkId=2f55c80199c88db853bd583182384f50) - generic iBeacon
* [eBay](https://www.ebay.com/sch/i.html?_nkw=nrf51822+ibeacon)
* [Ali Express](https://www.aliexpress.com/wholesale?SearchText=nrf51822+ibeacon).

## Non-beacon Hardware
Some other devices that advertise as BLE (such as the Mi Flora plant sensors or the Mi Band fitness tracker) can also be tracked, as long as you can scan the device and see its hardware ID. Here is a list of devices known to be "trackable":
* MiFlora plant sensor
* MiBand 2 Fitness tracker
* [Puck-JS](https://www.espruino.com/Puck.js), if programmed to [broadcast beacon packets](https://gist.github.com/jptrsn/d6cb9b9cdbcd41f3500708f8b694cad2 "An example project to broadcast iBeacon packets")
