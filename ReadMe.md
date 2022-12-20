## XFW - `Xtrme Firmware` for the Flipper Zero
![3a369eda-33b5-492e-a3d8-19425ca83a8a](https://user-images.githubusercontent.com/55334727/208327285-07abf8e9-7f11-491e-8c17-6fbbae4a5752.png)

[Official Version](https://github.com/flipperdevices/flipperzero-firmware)


This firmware is a complete overhaul of "RM" (RogueMaster), with changes on all graphics, `disgusting` code and more. Xtreme brings you the most for your little flipper companion.

-----

## What makes it special?

This firmware is based on RogueMaster (I know, should have forked unleashed, too late now). It combines the bleeding-edge PRs of Rogues firmware with actual understanding of whats going on, fixing bugs that are regularly talked about, removing unstable / broken applications (.FAP) and actually using the level system that just sits abandoned in RM.

- The focus of this firmware is functionality & stability: If an App doesnt work, its gone
- Giving the level system a purpose: Right now, each level unlocks a new wallpaper. More on that below
- Clean upgraded code: RM wrote some updates to certain files. These are... painful, to say the least. Here its all rewritten and cleaned up. I invite you all to compare the `applications\system\dolphin\helpers\dolphin-state.c` file as an example
- Up2Date: This firmware receives updates from a few repositories, not just from its Upstream. If there are functional, but yet un-merged Pull requests on another flipper firmware that are good, they will be in here!


-----

## Animations:

This firmware contains NSFW animations. No, actually it consisnts purely of those in stock configuration. If you dont like that, There will also be an SFW version soon.

The animations are tied to the level system. Each level you reach, unlocks a new animation. The higher your level, the more lewd it will become. Rumors have it, the author is to be found in at least one of those too

| Level  | Animations |
| ------------- | ------------- |
| 1-10  | Try harder. Just sexy clothes |
| 11-20 | Some tits, maybe an ass |
| 21-30 | Fully NSFW, graphic scenes |
-----

## List of changes:

Note: This repo is always updated with OFW, Unleashed & Rogue. No need to mention all those here. I will only mention **MY** changes

```txt

- Removed broken apps (bad apple, chess, etc.)
- Removed unused code from FAPs and system calls

- Updated all graphics
- Updated Compiler to now handle all non-compiled faps during build
- Updated Compiler to accept WIP SDK 
- Updated Compiler to just stfu about non-fatal problems
- Updated some further NFC stuff
- Updated Weather App
- Updated Applications to use below mentioned API Routes

- Added multiple Animation profiles to fit your style
- Added new API Routes for Locale settings
- Added Applications to new Locale setting
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
