<h1 align="center">XFW - <code>Xtreme Firmware</code> for the Flipper Zero</h1>

<p align="center">
  <img src="https://user-images.githubusercontent.com/55334727/215170306-051eeb8f-8f72-437f-8c2d-0e4be009bdad.png">
</p>

[Intro](https://github.com/ClaraCrazy/Flipper-Xtreme#What-makes-it-special) | [Animations](https://github.com/ClaraCrazy/Flipper-Xtreme#Animations--Asset-Packs) | [Docs](https://github.com/ClaraCrazy/Flipper-Xtreme/wiki) | [Changelog](https://github.com/ClaraCrazy/Flipper-Xtreme#list-of-changes) | [Known bugs](https://github.com/ClaraCrazy/Flipper-Xtreme#Known-bugs) | [Install](https://github.com/ClaraCrazy/Flipper-Xtreme#Install) | [Build](https://github.com/ClaraCrazy/Flipper-Xtreme#build-it-yourself) | [Discord](https://discord.gg/flipper-xtreme)
-----

This firmware is a complete overhaul of the [Official Firmware](https://github.com/flipperdevices/flipperzero-firmware), it also features lots of awesome code-bits from [Unleashed](https://github.com/DarkFlippers/unleashed-firmware).

-----
<br>
<h2 align="center">What makes it special?</h2>

We have spent many hours perfecting this code even further, and getting the most out of it.

The goal of this Firmware is to regularly bring out amazing updates based on what the community wants, with an actual understanding of whats going on. Fixing bugs that are regularly talked about, removing unstable / broken applications (.FAP) and actually using the level system that just sits abandoned everywhere else.
<br><br>
- The focus of this firmware is functionality & stability: If an App doesnt work, its gone

- Asset Packs: Are you tired of having to remake your custom animations after every update, switching manifests and having a hard time sharing them, especially once you modify scanning assets too? Those times are over. Scroll down to learn more

- Giving the level system a purpose: Right now, each level unlocks a new wallpaper. More on that below

- Clean upgraded code: Some people wrote updates to certain files. These are... painful, to say the least. Here its all built with perfection in mind and integrated in a mostly clean way. I invite you all to compare the code with theirs.

- Up2Date: This firmware receives updates from a few repositories, not just from its Upstream. If there are functional, but yet un-merged Pull requests on another flipper firmware that are good, they will be in here!


-----
<br>
<h2 align="center">Xtreme Settings:</h2>


We wrote a powerful yet easy-to-use application specifically for our Firmware, that gives you easy-access to all the fancy things we implemented:

<img src="https://user-images.githubusercontent.com/55334727/215137728-529274e8-ab95-4164-a2a0-9ff712c9d3c7.gif" align="left" width="400px"/>

<code>Base Graphics:</code> Change the fallback assets used. Its either SFW (default) or NSFW
<br><code>Asset Pack:</code> Allows you to easily customize your firmware, more on that below 
<br><code>Anim Speed:</code> Speed in which the animations play
<br><code>Cycle Anims:</code> Duration of how long animations are played before switching to next
<br><code>Unlock Anims:</code> Custom setting just for NSFW fallback animations. Figure it out ;)
<br><code>Battery style:</code> Classic Firmware battery style toggle, just at a more convenient place
<br><code>XP Level:</code> Changes your Flippers level
<br><code>SubGhz Extend:</code> Allows you to extend the subghz range beyond what FZ devs planned
<br><code>SubGhz Bypass:</code> Allows you to bypass the subghz region locks of the Flipper

<br clear="left"/>

-----
<br>
<h2 align="center">Animations / Asset Packs:</h2>

We created our own, new & improved Animation / Asset system, that we can finally reveal. It lets you to create and cycle through your own `Asset Packs` with only a few button presses, allowing you to easily load custom Animations and Icons like never before.

<img src="https://user-images.githubusercontent.com/55334727/214010675-9eddb8f5-1dd6-4cf4-a0ee-e37af8b6c933.PNG" align="left" width="200px"/>
You can easily create your own pack, or find some user made ones in the discord channel. Check <a href="https://github.com/ClaraCrazy/Flipper-Xtreme/wiki/1.-File-Formats">here</a> for a tutorial on creating your own. Essentially, we got our own <code>Anims</code> & <code>Icons</code> folders, inside each <code>Asset Pack</code>.

<br clear="left"/>

<br>

<img src="https://user-images.githubusercontent.com/55334727/214016338-95a619c7-88d2-4db5-bb7a-75282d9082b8.png" align="left" width="200px"/>
Once you have some packs, upload them to your Flipper in <code>SD/dolphin_custom</code> (if you did this right you should see <code>SD/dolphin_custom/PackName/Anims</code> and/or <code>SD/dolphin_custom/PackName/Icons</code>).


<br clear="left"/>

<br>

<img src="https://user-images.githubusercontent.com/55334727/214013624-25dad48e-72ea-4a90-9060-66e137e0d61a.png" align="left" width="200px"/>
After installing the packs to Flipper, hit the <code>Arrow UP</code> button on the main menu and go to <code>Xtreme Settings</code>. Here choose which pack you want and tweak the other settings how you prefer, then press back to reboot and enjoy your new assets & animations!

<br clear="left"/>

-----
<br>
<h2 align="center">Levels:</h2>

This Firmware has 30 levels, not just the basic 3 the official one has.

With this new system in place, it allows for some cool stuff like locking animations behind a certain level. This can be done fairly easy: The idle_animations are tied to the level system. Specifically, the `Min level` variable of your manifest file is used here. Each level you reach, unlocks a new animation. The higher your level, the more animations people can see.

<details>
<summary>Our example</summary>

In our case, this is used with the NSFW animations. Dont worry, these are disabled by default because I know not everyone likes to see my / anime tits and thats fine. Anyways.. each level gives a brand new background animation, they also become more and more lewd over time. (Funfact for those reading.. thats why the repository has this warning. Github doesnt like my tits :c)

| Level  | Animations |
| ------------- | ------------- |
| 1-10  | Try harder. Just sexy clothes |
| 11-20 | Some tits, maybe an ass |
| 21-30 | Fully NSFW, graphic scenes |

</details>

-----
<br>
<h2 align="center">List of changes:</h2>

Note: This repo is always updated with OFW & Unleashed. No need to mention all those here. We will only mention **our** changes that we can actually be credited for.

```txt
[Added]

- Xtreme App
- Asset Packs
- More UI options
- A new battery display-type
- Scrolling view for long file names in browser
- NSFW Animations tied to the level system. Read more above
- Folder handling for empty ones (Now indicate they are empty)

- Jamming Files
- Custom subghz presets
- Multiple NFC protocols
- Subghz and IR signal replication via gpio | Credits to @ankris812
- Honda Keys (CVE-2022-27254) & Ford blockers

- New API Routes for Locale settings
```
```txt
[Updated]

- All Assets

- Tons of apps
- Massive compiler re-do
- About 1k files to speed things up a lot
- Applications to now use the new Locale setting
```
```txt
[Fixed]

- Passport crash on high level
- SFW / Dummy_mode getting you XP
- Leveling system
- Mood system
```
```txt
[REMOVED]

- Unused Dummy Mode
- Broken apps (bad apple, chess, etc.)
- Tons of unused code from FAPs and system calls
```

----
<br>
<h2 align="center">Known Bugs:</h2>

```txt
- Nothing rn. Hopefully that wont change
```

----
<br>
<h2 align="center">Install:</h2>

**NOTE: If you are coming from a different FW, it is recommended to delete / clear your "apps" folder on the SD card prior to updating. This folder houses all the .fap files, which do not update to the correct API versions by default if old ones are present (Thanks flipper devs). This does `NOT` remove any of your saved files!**
<br><br>

- Download the latest release (.zip) from [The releases tab](https://github.com/ClaraCrazy/Flipper-Xtreme/releases/latest)
- Extract the archive. This is now your new Firmware folder
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

----
<h2 align="center">Stargazers over time</h2>

[![Stargazers over time](https://starchart.cc/ClaraCrazy/Flipper-Xtreme.svg)](https://starchart.cc/ClaraCrazy/Flipper-Xtreme)

----
<h2 align="center">Contributors</h2>
<p align="center">
  <img src="https://user-images.githubusercontent.com/55334727/212134625-21383102-02f3-453f-b1d7-8a9c65b27612.svg">
</p>

----
## SAST Tools

This helps us a lot, thanks for the free license for this project!

[PVS-Studio](https://pvs-studio.com/en/pvs-studio/?utm_source=github&utm_medium=organic&utm_campaign=open_source) - static analyzer for C, C++, C#, and Java code.

----
<p align="center"> "What we do for ourselves dies with us. What we do for others and the world remains and is immortal.” ― Albert Pine </p>
