# Contributing

When contributing to this repository, please first discuss the change you wish to make via issue,
email, or any other method with the owners of this repository before making a change. 

Please note we have a code of conduct, please follow it in all your interactions with the project.

## Product principles

ESPresense is a passive presence and positioning node. Check a feature against these before
writing or merging it, not just whether the code is correct.

* **Radio time goes to passive scanning.** Don't add anything that transmits on its own schedule
  or to every device in range. Active scanning sends a scan request that every advertiser in
  range must answer, which uses airtime and the batteries of tags we don't own. A GATT
  connection is fine only to a device the user asked about (Query), and only when needed.
* **Sensor readings come for free or not at all.** Decode battery, temperature and similar data
  from the adverts a passive scan already receives. Don't spend radio time to get more, except a
  query to a device the user named. If a device only sends its data on request, tell the user to
  switch it to a mode that advertises the data. Example: #2592 reads Teltonika EYE data from
  passive adverts, and the hourly active scan from #2490 was dropped.
