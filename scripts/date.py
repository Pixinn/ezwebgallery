#!/usr/bin/env python

import datetime
now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
print("#define BUILD_DATE \"" + now + '\"')
