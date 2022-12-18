## Custom Firmware, based on the RogueMaster fork.
Mainly changing icons, animations & some other minor things. :)

## Build
```bash
$ git clone --recursive https://github.com/ClaraCrazy/flipper-firmware.git
$ cd flipperzero-firmware-wPlugins/
$ ./fbt updater_package

# If building FAPS:
$ ./fbt fap_dist

# If building image assets:
$ ./fbt resources icons dolphin_ext
```
