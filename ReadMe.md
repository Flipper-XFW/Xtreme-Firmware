## Custom Firmware, based on the RogueMaster fork.

List of changes:
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
