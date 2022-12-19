## Flipper Xtreme, the most advanced Flipper firmware
![3a369eda-33b5-492e-a3d8-19425ca83a8a](https://user-images.githubusercontent.com/55334727/208327285-07abf8e9-7f11-491e-8c17-6fbbae4a5752.png)

[RogueMaster Firmware](https://github.com/RogueMaster/flipperzero-firmware-wPlugins) | [Official Version](https://github.com/flipperdevices/flipperzero-firmware)

This Firmware is a modified version of RM with some of my own Ideas added to it and some ignored stuff from the official one brought into it as well. Probably more bleeding edge than RM, but will always remove things that crash the flipper

-----

Note:

I'm currently working on getting my own custom animation style done. This is going  to be nsfw, as that suits me quite well. If you wanna help, lmk!

-----

## List of changes:

```txt
- Removed broken apps (bad apple, chess, etc.)

- Updated some NFC stuff

- Added multiple Animation profiles to fit your style
- Added new API Routes for Locale settings
- Added scrolling view for long file names in browser
```

----

## Known Bugs:

```txt
- Clock not yet synced with locale
- Wii EC can crash due to Null Pointer
- subghz brutemap not working
```

----

## Build it yourself:

```bash
$ git clone --recursive https://github.com/ClaraCrazy/Flipper-Xtreme.git
$ cd flipperzero-firmware-wPlugins/
$ ./fbt updater_package

# If building FAPS:
$ ./fbt fap_dist

# If building image assets:
$ ./fbt resources icons dolphin_ext
```
