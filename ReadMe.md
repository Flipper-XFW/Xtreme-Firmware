# CoolOS for [Flipper Zero](https://flipperzero.one/)

Forked from [Flipper-Xtreme](https://github.com/ClaraCrazy/Flipper-Xtreme) with minimal changes to included apps and files

This is my personal firmware and you probably don't want to use it :)

Resources I use are availiable [here](https://github.com/o1001100/cool-resources)

## Installation

### Install using qFlipper or mobile app

Download the qFlipper package (.tgz) from the latest release page

Open qFlipper or the mobile app and connect your Flipper

Click Install from file

Select the .tgz you downloaded and wait for the update to complete

### Install from updater package

Download the zipped archive (.zip) from the latest release page

Extract the archive. This is now your new Firmware folder

Move the firmware folder to the update folder on the Flipper's SD with qFlipper or by mounting the SD to your computer/phone

On the Flipper, hit the Arrow Down button, this will get you to the file menu. In there simply search for your updates folder

Inside that folder, select the Firmware you just moved onto it, and run the file thats simply called Update

## Building

Build like any other firmware or use my build script `compile`

run `./compile` to build a full updater package with apps included

run `./compile -h` for help with more options

## Credits

#### Firmware
Based on [Flipper-Xtreme](https://github.com/ClaraCrazy/Flipper-Xtreme) by ClaraCrazy

Thanks to DarkFlippers for [Unleashed](https://github.com/DarkFlippers/unleashed-firmware) and the Flipper Zero team and community for the [Official Firmware](https://github.com/flipperdevices/flipperzero-firmware)

#### Apps

Included apps are the same as in Xtreme [(list of apps and authors)](https://github.com/ClaraCrazy/Flipper-Xtreme/wiki#included-apps) with the addition of:

[Flipagotchi](https://github.com/Matt-London/pwnagotchi-flipper/tree/main/flipagotchi) from the [Pwnagotchi-Flipper interface](https://github.com/Matt-London/pwnagotchi-flipper) project by Matt-London

[Flipper Zero DND Dice](https://github.com/Ka3u6y6a/flipper-zero-dice) by Ka3u6y6a

[flipper-zero-hex-editor](https://github.com/dunaevai135/flipper-zero-hex_editor) by dunaevai135

[flipper-zero-video-player](https://github.com/LTVA1/flipper-zero-video-player) by LTVA1

[fz-schip](https://github.com/Milk-Cool/fz-schip) by Milk-Cool

[ir_scope](https://github.com/kallanreed/unleashed-firmware/tree/dev/applications/external/ir_scope) by kallanreed

[flipperscope](https://github.com/anfractuosity/flipperscope) by anfractuosity

[flipperzero-text2sam](https://github.com/Round-Pi/flipperzero-text2sam) by Round-Pi

[dice](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/tree/420/applications/external/dice) by RogueMaster
