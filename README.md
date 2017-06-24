![](https://api.travis-ci.org/squix78/minigrafx.svg?branch=master)

# The MiniGrafx Library

This is a graphics library for embedded devices containing a framebuffer to avoid flickering. Instead of writing directly to the display all drawing operations are made on the buffer stored in memory. Once you are finished with one drawing cycle the whole content of the framebuffer is written to the display.

## Inspiration

This library is heavily based on another monochrome library https://github.com/squix78/esp8266-oled-ssd1306
developed by Fabrice Weinberg and myself mostly for tiny OLED displays. This new library targets also color displays and e-paper displays with higher resolution

## Basic flow

First you have to include the basic graphic routines and the header file matching your display:

## Fork by Vincent Dupont
Quick & Dirty Code adaptation - works on Arduino Mega 2560 with Waveshare 2.9 ePaper
Not tested with other board or display
EPD_WaveShare_2_9 example works

Lot of Linter warning (mostly types conversions), not fixed.

Adapt the library to Arduino Mega board (megaatmega2560)
Adding SD Library support instead of FS.h
Fixing Methods calls when not fully aligned with SD Methods
