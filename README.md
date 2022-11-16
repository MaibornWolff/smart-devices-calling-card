# Smart Devices Day Calling Card

<section style="text-align:center;">
    <img style="background-color:white; height:160px; padding:20px;" src="docs/maibornwolff.png"/>
    <img style="background-color:white; height:160px; padding:20px;" src="docs/intuity.jpg"/>
</section>

This repository documents the collaborative work of [MaibornWolff](https://maibornwolff.de) and [intuity](https://www.intuity.de/) for an Arduino-like calling card for one of our events.

## Features

The are an implementation of the classic __"Simon Says"__-Memory Game, where you have to remember an increasing sequence of button presses.
The design implements multiple features:
- Allowing for sequences of up to 15, which are suprisingly difficult
- Different Effects on the 4 installe LEDs, using the PWM pins of the microcontroller
- Highscore tracking with EEPROM
- Energy Saving with smart sleep modes

## Hardware

<img src="docs/cards.jpg"/>

The hardware design is based on the Arduino Pro Mini.
We use a small CR1220 battery cell - though we found out CR1225 also fit in the socket and have a little more mAh - to deliver 3V of power to the device, which results in surprisingly long battery lives (greater than an hour of play time).

<!-- TODO schematics -->

## Software

The Software is simple C code created with arduino studio.
We use a custom bootloader to use the internal 8Mhz clock, this saves us one part on the BOM and we do not need the accuracy anyway.
The current code is ~350 lines of code, but a lot is to make the usability better and cover edge cases.
After compilation, the binary uses around ~20% of the available flash memory, so if you have any cool ideas feel free to contribute or create your own firmware!