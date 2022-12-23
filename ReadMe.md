<h1 align="center">XFW - <code>Xtreme Firmware</code> for the Flipper Zero</h1>

<p align="center">
  <img src="https://user-images.githubusercontent.com/55334727/209031152-efbca66c-3fe0-41b5-a860-e0d7522a9279.png">
</p>

[Intro](https://github.com/ClaraCrazy/Flipper-Xtreme#What-makes-it-special) | [Animations](https://github.com/ClaraCrazy/Flipper-Xtreme#Animations) | [Changelog](https://github.com/ClaraCrazy/Flipper-Xtreme#list-of-changes) | [Known bugs](https://github.com/ClaraCrazy/Flipper-Xtreme#Known-bugs) | [Install](https://github.com/ClaraCrazy/Flipper-Xtreme#Install) | [Build](https://github.com/ClaraCrazy/Flipper-Xtreme#build-it-yourself)
-----

This firmware is a complete overhaul of "RM" (RogueMaster), with changes on all graphics, `disgusting` code and more. Xtreme brings you the most for your little flipper companion.

-----
<br>
<h2 align="center">What makes it special?</h2>

This firmware is based on RogueMaster (I know, should have forked unleashed, too late now). It combines the bleeding-edge PRs of Rogues firmware with actual understanding of whats going on, fixing bugs that are regularly talked about, removing unstable / broken applications (.FAP) and actually using the level system that just sits abandoned in RM.
<br><br>
- The focus of this firmware is functionality & stability: If an App doesnt work, its gone

- Giving the level system a purpose: Right now, each level unlocks a new wallpaper. More on that below

- Clean upgraded code: RM wrote some updates to certain files. These are... painful, to say the least. Here its all rewritten and cleaned up. The last messy file I have to get around to clean would be `applications\system\dolphin\helpers\dolphin-state.c`, in case anyone wants to help

- Up2Date: This firmware receives updates from a few repositories, not just from its Upstream. If there are functional, but yet un-merged Pull requests on another flipper firmware that are good, they will be in here!


-----
<br>
<h2 align="center">Animations:</h2>

This firmware contains NSFW animations. No, actually it consisnts purely of those in stock configuration. If you dont like that, we removed the "Dummy mode" as it serves no purpose, and switched with an SFW toggle. From the main menu, hit `Arrow UP` and select SFW mode. (This is a temporary solution and a proper FAP for handling SFW mode with multiple animations is in the making)

The animations are tied to the level system. Each level you reach, unlocks a new animation. The higher your level, the more lewd it will become. Rumors have it, I'm  to be found in at least one of those too

| Level  | Animations |
| ------------- | ------------- |
| 1-10  | Try harder. Just sexy clothes |
| 11-20 | Some tits, maybe an ass |
| 21-30 | Fully NSFW, graphic scenes |

-----
<br>
<h2 align="center">List of changes:</h2>

Note: This repo is always updated with OFW, Unleashed & Rogue. No need to mention all those here. I will only mention **MY** changes

```txt
[Added]

- Jamming Files
- Custom subghz presets
- Subghz and IR signal replication via gpio (allows using external antenas and emitters)
- Honda Keys (CVE-2022-27254)
- Multiple Animation profiles to fit your style
- New API Routes for Locale settings
- Scrolling view for long file names in browser
- Tamagotchi rom
```
```txt
[Updated]

- All graphics
- Applications now use the new Locale setting
- Compiler now handles all non-compiled faps during build
- Compiler now accepts WIP SDK 
- Compiler just stfu about non-fatal problems
- Some further NFC stuff
- Weather App
- Applications now use below mentioned API Routes
```
```txt
[Fixed]

- Leveling system
- Mood system
```
```txt
[REMOVED]

- Broken apps (bad apple, chess, etc.)
- Unused code from FAPs and system calls
```

----
<br>
<h2 align="center">Known Bugs:</h2>

```txt
- Wii EC can crash due to Null Pointer
- subghz brutemap not working
```

----
<br>
<h2 align="center">Install:</h2>

- Download the latest release from [The releases tab](https://github.com/ClaraCrazy/Flipper-Xtreme/releases)
- Extract the archive. Inside you will find a readme, alternate animations in a folder labelled "Animations" and a folder for your Firmware
- Open [qFlipper](https://flipperzero.one/update), head to `SD/Update` and simply move the firmware folder there
- On the Flipper, hit the `Arrow Down` button, this will get you to the file menu. Hit `Arrow Left` once, and then simply search for your updates folder
- Inside that folder, select the Firmware you just moved onto it, and run the file thats simply called `Update`

- Enjoy!

----
<br>
<h2 align="center">Build it yourself:</h2>

```bash
To download the needed tools:
$ git clone --recursive https://github.com/ClaraCrazy/Flipper-Xtreme.git
$ cd Flipper-Xtreme/

To flash directly to the Flipper (Needs to be connected via USB, qFlipper closed)
$ ./fbt flash_usb

To just compile firmware
$ ./fbt updater_package

If building FAPS:
$ ./fbt fap_dist

If building image assets:
$ ./fbt resources icons dolphin_ext
```
