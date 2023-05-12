#!/bin/bash

source version.cfg

# move files into release folder
zip -r release/CoolOS-$version.zip dist/f7-C/f7-update-CoolOS-v*/
mv dist/f7-C/flipper-z-f7-update-CoolOS-v*.tgz release/CoolOS-$version.tgz
