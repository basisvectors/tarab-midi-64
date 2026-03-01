# **`Tarab Instruments MIDI-64: 16-Knob USB MIDI CC Controller`**

<img width="1280" height="1280" alt="midi_twister_2025-Dec-18_10-33-41AM-000_CustomizedView34942085826" src="https://github.com/user-attachments/assets/f539a174-5c0c-4dfc-8211-bce65ab771ca" />

## **`Overview`**

`This controller sends MIDI Control Change (CC) messages over USB. It features 16 knobs, 4 pages of CC assignments (64 CCs total), 4 status LEDs, and 2 buttons. It is recognized as a standard USB MIDI device — no driver installation required.`

**`Controls`**  
**`16 Knobs:`** `Each knob sends a CC value (0–127). The CC number depends on the active page (see table below).`  
**`Left Button (Page Down):`** `Cycles the active page backward (4 → 3 → 2 → 1 → 4...).`  
**`Right Button (Page Up):`** `Cycles the active page forward (1 → 2 → 3 → 4 → 1...).`  
**`Both Buttons (Channel Mode):`** `Press both buttons simultaneously to enter channel mode. The LEDs flash the current MIDI channel in binary. Use Left/Right buttons to increment/decrement the channel (1–16). Channel mode exits automatically after 2 seconds of inactivity.`

**`Quick Start`**

**`1.`** `Connect via USB. The controller appears as a standard MIDI device.`  
**`2.`** `Select it as a MIDI input in your DAW or synth software.`  
**`3.`** `Turn knobs to send CC messages. Use buttons to switch pages or change the MIDI channel.`  
*`Defaults: Page 1 (LED1), MIDI Channel 1. The controller sends on CCs 16–79 across 4 pages.`*

## **`Page Assignments`**

| `Page` | `LED Index` | `CC Range` | `Knob 1–16 Maps To` |
| :---- | :---- | :---- | :---- |
| `1` | `1` | `CC 16–31` | `Knob 1 = CC 16, Knob 2 = CC 17, ... Knob 16 = CC 31` |
| `2` | `2` | `CC 32–47` | `Knob 1 = CC 32, Knob 2 = CC 33, ... Knob 16 = CC 47` |
| `3` | `3` | `CC 48–63` | `Knob 1 = CC 48, Knob 2 = CC 49, ... Knob 16 = CC 63` |
| `4` | `4` | `CC 64–79` | `Knob 1 = CC 64, Knob 2 = CC 65, ... Knob 16 = CC 79` |

## **`LED Indicators & MIDI Channel`** `Four red LEDs sit between the two buttons. During normal use, one LED lights up to indicate the active page (LED 1 = Page 1, LED 2 = Page 2, etc.). In channel mode, the LEDs flash the MIDI channel number in binary: LED 1 = bit 0 (1), LED 2 = bit 1 (2), LED 3 = bit 2 (4), LED 4 = bit 3 (8). Examples: Channel 1 = LED 1 only. Channel 3 = LEDs 1+2. Channel 5 = LEDs 1+3. Channel 10 = LEDs 2+4. Channel 16 = all four LEDs.`

## **`Knob Pickup (Anti-Jump)`** `When you switch pages, knobs are locked to prevent value jumps. A knob will not send CC data on the new page until you turn it past the previously stored value for that page. This lets you switch pages safely without accidentally changing parameters.`

## 

## 

## 

## **`Ableton Live MIDI Remote Script Setup`**

[`MIDITwister.zip`](https://drive.google.com/file/d/18PWd5HxEwH-kTtX2BjnFFBZiqqTWufoS/view?usp=drive_link) `- Google Drive`

[`Creating your own Control Surface script – Ableton`](https://help.ableton.com/hc/en-us/articles/206240184-Creating-your-own-Control-Surface-script)

`Since not all MIDI controllers have a native Control Surface in Live, you can create your own using the UserConfiguration.txt for basic functions like Device Control, Track Control and Transport Control.`

`The UserConfiguration.txt file and directions on how to use it are found in the User Remote Scripts folder, which is stored in Live's Preferences folder:`

`Mac: HD:/Users/[Username]/Library/Preferences/Ableton/Live x.x.x/User Remote Scripts`

`Windows: C:\Users\[Username]\AppData\Roaming\Ableton\Live x.x.x\Preferences\User Remote Scripts`

`Note: These folders are hidden by default.`  
`The User Remote Scripts Folder`

`In the User Remote Scripts folder, you will find two files, the InstantMappings-HowTo.txt file and the UserConfiguration.txt file.`

`The InstantMappings-HowTo.txt file describes how and where to save the file.`  
`The UserConfiguration.txt file contains instructions on how to edit the file itself so that it works with your MIDI controller.`   
   
`You will need to know on what MIDI Channel your controller sends data, and you will also need to know the specific MIDI CC assignments for the various pads, encoders, and sliders.`

`One quick way to find this out is to make a MIDI remote control assignment in Live and note down the corresponding CC value. Alternatively, you can use Snoize MIDI Monitor for Mac, or OBDS MIDI Monitor for PC.`

## `.`

