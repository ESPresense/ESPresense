# Contributing

When contributing to this repository, please first discuss the change you wish to make via issue,
email, or any other method with the owners of this repository before making a change. 

Please note we have a code of conduct, please follow it in all your interactions with the project.

## Product principles

ESPresense is a passive presence and positioning node. Check a feature against these before
writing or merging it, not just whether the code is correct.

* **Passive scanning comes first.** Time spent transmitting is time not spent listening, and it
  uses up airtime and the batteries of other people's devices. Anything that transmits should be
  limited to devices the user picked, bounded in how often it runs, and worth what it costs.
  Untargeted active scanning, where every advertiser in range must answer a scan request, fails
  that test.
* **Prefer sensor readings that come for free.** Decode battery, temperature and similar data from
  the adverts a passive scan already receives. If a reading needs a request, send it only to the
  devices the user configured, not to everything in range. Example: #2592 reads
  Teltonika EYE data from passive adverts, and the hourly active scan for every device from #2490
  was dropped.
