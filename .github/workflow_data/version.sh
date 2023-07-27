#!/bin/bash

export VERSION_TAG="$(python -c '''
import datetime as dt
import json
import os
with open(os.environ["GITHUB_EVENT_PATH"], "r") as f:
    event = json.load(f)
version = int(event["pull_request"]["title"].removeprefix("V").removesuffix(" Release").removesuffix(" Hotfix"))
date = dt.datetime.now().strftime("%d%m%Y")
print(f"XFW-{version:04}_{date}", end="")
''')"
echo "VERSION_TAG=${VERSION_TAG}" >> $GITHUB_ENV
