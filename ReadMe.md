## Custom Firmware, based on the RogueMaster fork.
![3a369eda-33b5-492e-a3d8-19425ca83a8a](https://user-images.githubusercontent.com/55334727/208327285-07abf8e9-7f11-491e-8c17-6fbbae4a5752.png)

[RogueMaster Firmware](https://github.com/RogueMaster/flipperzero-firmware-wPlugins) | [Official Version](https://github.com/flipperdevices/flipperzero-firmware)

This Firmware is a modified version of RM with some of my own Ideas added to it and some ignored stuff from the official one brought into it as well. Probably more bleeding edge than RM, but will always remove things that crash the flipper

-----

## List of changes:

```txt
- Removed lots of animations due to personal choice
- Removed broken apps (bad apple, chess)
- Removed gpio apps I dont own modules for

- Added new API Routes for Locale settings
- Added scrolling view for long file names in browser
```

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
