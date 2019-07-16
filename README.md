# Midimon

## Uploading Sketches

Make sure that 'Midimon' is selected as the board (see below section), then restart Midimon into the bootloader
mode by holding down the OK button while powering it up. Then do Sketch->Upload. During firmware
flashing, the Blokas logo should play scrolling animation. Once the firmware is flashed, you'll
have to restart the device manually by switching it off and on again.


### Setting up the Board Definition

Go to your Arduino Sketchbook location (you can find the full path to it in the Arduino IDE's Preferences dialog),
create a 'hardware' folder if there isn't one yet, and clone Blokas-Boards repo inside it:

```sh
cd .../arduino/
mkdir -p hardware
cd hardware
git clone https://github.com/BlokasLabs/Blokas-Boards
```
Then restart the Arduino IDE.

After this, 'Midimon' should appear in the Arduino IDE's Tools->Board menu.

## See Also
* https://github.com/BlokasLabs/Midiboy - Midiboy Core library.
* https://github.com/BlokasLabs/Blokas-Boards - The board definition repository.
* https://github.com/BlokasLabs/Midimon-Bootloader - The bootloader.
* https://github.com/BlokasLabs/USBMIDI - USBMIDI library.
