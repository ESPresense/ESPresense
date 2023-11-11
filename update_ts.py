#!/usr/bin/env python3
from datetime import datetime

with open("src/build_timestamp_value.h", "w") as fp:
    timestamp = datetime.now().strftime("%Y-%m-%dT%H:%M:%S")
    fp.write(f'#define BUILD_TIMESTAMP "{timestamp}"')
