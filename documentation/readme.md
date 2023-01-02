<h1 align="center"><code>Xtreme Firmware Documentation</code></h1>

<p align="center">
  <img src="https://user-images.githubusercontent.com/55334727/209031152-efbca66c-3fe0-41b5-a860-e0d7522a9279.png">
</p>

<h2 align="center">Install:</h2>

**NOTE: If you are coming from a different FW, it is recommended to delete / clear your "apps" folder on the SD card prior to updating. This folder houses all the .fap files, which do not update to the correct API versions by default if old ones are present (Thanks flipper devs). This does `NOT` remove any of your saved files!**
<br><br>

<h3> Web Updater / Mobile App / Automatic qFlipper Update</h3>

- Go to the latest release from [The releases tab](https://github.com/ClaraCrazy/Flipper-Xtreme/releases/latest)
- Download the .tgz file
- Run your updater of choice with it

- Enjoy!
<br>

-----
<br>

<h3> Manual Update (Because qFlipper loves to break every now and then for no reason at all)</h3>

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

<br>
<h2 align="center">Safe for work mode:</h2>

**Enable SFW mode with the steps listed below**
<br>

- Navigate to the main screen
- Press the UP button on the Flipper
- Press the 'SFW Mode' toggle

----

<br>
<h2 align="center">Support the project</h2>

**If you like Xtreme Firmware and wish to help out, we would love it if you contributed to the project ❤️**
<br>

- [Bunq - My bank](https://bunq.me/ClaraK)
- [Paypal](https://paypal.me/RdX2020)
- [Patreon](https://patreon.com/CynthiaLabs)
- `Monero`: 41kyWeeoVdK4quzQ4M9ikVGs6tCQCLfdx8jLExTNsAu2SF1QAyDqRdjfGM6EL8L9NpXwt89HJeAoGf1aoArk7nDr4AMMV4T



----


<br>
<h2 align="center">Applications we use thanks to the awesome contributers and community ❤️</h2>

## GPIO
- [Lightmeter](https://github.com/oleksiikutuzov/flipperzero-lightmeter)
- [Air Mouse](https://github.com/ginkage/FlippAirMouse/)
- [Wifi Marauder](https://github.com/0xchocolate/flipperzero-firmware-with-wifi-marauder-companion)
- [Wifi Deauther](https://github.com/Timmotools/flipperzero_esp8266_deautherv2)
- [IFTTT Virtual Button for ESP8266 (By Ferrazzi)](https://github.com/Ferrazzi/FlipperZero_IFTTT_Virtual_Button)
- [Flashlight (By xMasterX)](https://github.com/xMasterX/flipper-flashlight)
- [GPIO Reader 1](https://github.com/biotinker/flipperzero-gpioreader)
- [GPIO Reader 2](https://github.com/aureli1c/flipperzero_GPIO_read)
- [Sentry Safe](https://github.com/H4ckd4ddy/flipperzero-sentry-safe-plugin)
- [Distance Sensor](https://github.com/Sanqui/flipperzero-firmware/tree/59656ca5fb644e0d4484259986b86a0b963f323d/applications/hc_sr04)
- [Temperature Sensor](https://github.com/Mywk/FlipperTemperatureSensor)
- [i2c Tools](https://github.com/NaejEL/flipperzero-i2ctools)
- [GPS](https://github.com/ezod/flipperzero-gps)
- [Mouse Jacker](https://github.com/mothball187/flipperzero-nrf24/tree/main/mousejacker)
- [Sniffer](https://github.com/mothball187/flipperzero-nrf24/tree/main/nrfsniff)
- [Scanner](https://github.com/vad7/nrf24scan)
- [ColecoVision Controller](https://github.com/ezod/flipperzero-rc2014-coleco)


## Games
- [2048](https://github.com/eugene-kirzhanov/flipper-zero-2048-game)
- [Arkanoid](https://github.com/DarkFlippers/unleashed-firmware/tree/dev/applications/plugins/arkanoid)
- [BlackJack](https://github.com/teeebor/flipper_games)
- [Doom](https://github.com/p4nic4ttack/doom-flipper-zero/)
- Multi Dice - In-house
- [Flappy Bird](https://github.com/DroomOne/flipperzero-firmware/tree/dev/applications/flappy_bird)
- [Game 15](https://github.com/x27/flipperzero-game15)
- [Game of Life (Updated to work by tgxn) (By itsyourbedtime)](https://github.com/tgxn/flipperzero-firmware/blob/dev/applications/game_of_life/game_of_life.c)
- Heap defense - Author Unknown
- [Mandelbrot Set (By Possibly-Matt)](https://github.com/Possibly-Matt/flipperzero-firmware-wPlugins)
- [Minesweeper](https://github.com/panki27/minesweeper)
- [Snake](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications/plugins/snake_game)
- [Solitaire](https://github.com/teeebor/flipper_games)
- [Tetris](https://github.com/jeffplang/flipperzero-firmware/tree/tetris_game/applications/tetris_game)
- [Tic Tac Toe](https://github.com/gotnull/flipperzero-firmware-wPlugins/tree/420/applications/plugins/tictactoe_game)
- [Tamagotchi(By GMMan)](https://github.com/GMMan/flipperzero-tamagotch-p1)
- [Video Poker (By PixlEmly)](https://github.com/PixlEmly/flipperzero-firmware-testing/blob/420/applications/VideoPoker/poker.c)
- [Yatzee (By emfleak)](https://github.com/emfleak/flipperzero-yatzee)


## Misc
- [Authenticator (TOTP / HOTP)](https://github.com/akopachov/flipper-zero_authenticator)
- [Barcode Generator](https://github.com/McAzzaMan/flipperzero-firmware/tree/UPC-A_Barcode_Generator/applications/barcode_generator)
- [Caesar Cipher](https://github.com/panki27/caesar-cipher)
- Calculator - In house / notIntense
- [Counter](https://github.com/Krulknul/dolphin-counter)
- [Hex Viewer](https://github.com/QtRoS/flipper-zero-hex-viewer)
- [Mouse Jiggler](https://github.com/Jacob-Tate/flipperzero-firmware/blob/dev/applications/mouse_jiggler/mouse_jiggler.c)
- [Converter](https://github.com/theisolinearchip/flipperzero_stuff/tree/main/applications/multi_converter)
- [Vibrator](https://github.com/qqmajikpp/flipperzero-firmware-wPlugins/tree/420/applications/plugins/orgasmotron)
- [Paint](https://github.com/n-o-T-I-n-s-a-n-e)
- [UART Echo](https://github.com/flipperdevices/flipperzero-firmware/pull/831)
- [USB HID Autofire](https://github.com/pbek/usb_hid_autofire)
- [USB Keyboard & Mouse](https://github.com/huuck/FlipperZeroUSBKeyboard) / (https://github.com/DarkFlippers/unleashed-firmware)
- [Wii EC Analyzer](https://github.com/csBlueChip/FlipperZero_WiiEC)


## Music
- [BPM Tapper (By panki27)](https://github.com/panki27/bpm-tapper)
- [Metronome](https://github.com/panki27/Metronome)
- [Morse Code](https://github.com/DarkFlippers/unleashed-firmware/pull/144)
- Music Beeper - Author Unknown
- [Music Player](https://github.com/flipperdevices/flipperzero-firmware/pull/1189)
- [Ocarina](https://github.com/invalidna-me/flipperzero-ocarina)
- [Software Automatic Mouth aka SAM](https://github.com/ctoth/SAM)
- [Tuning Fork](https://github.com/besya/flipperzero-tuning-fork)
- [Music Tracker](https://github.com/DrZlo13/flipper-zero-music-tracker)


## Tools
- [Bluetooth Remote](https://github.com/flipperdevices/flipperzero-firmware/pull/1330)
- [Clock](https://github.com/kowalski7cc/flipperzero-firmware/tree/clock-v1) / In house
- [Countdown Timer](https://github.com/0w0mewo/fpz_cntdown_timer)
- [DAP Link](https://github.com/flipperdevices/flipperzero-firmware/pull/1897)
- [DTMF](https://github.com/litui/dtmf_dolphin)
- [Dolphin Backup & Restore](https://github.com/flipperdevices/flipperzero-firmware/pull/1384)
- [iButton Fuzzer](https://github.com/DarkFlippers/unleashed-firmware)
- [NFC Magic](https://github.com/flipperdevices/flipperzero-firmware/pull/1966)
- [POCSAG Pager](https://github.com/xMasterX/flipper-pager)
- [Password Generator](https://github.com/anakod/flipper_passgen)
- [PicoPass](https://github.com/flipperdevices/flipperzero-firmware/pull/1366)
- [RFID Fuzzer](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/245)
- [Spectrum Analyzer](https://github.com/jolcese/flipperzero-firmware/tree/spectrum/applications/spectrum_analyzer)
- [Sub-Ghz Bruteforce](https://github.com/derskythe/flipperzero-subbrute/tree/master)
- [Sub-Ghz Playlist](https://github.com/darmiel/flipper-playlist)
- [Protocol Visualizer](https://github.com/antirez/protoview)


---
