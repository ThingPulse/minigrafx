[![Build Status](https://travis-ci.org/ThingPulse/minigrafx.svg?branch=master)](https://travis-ci.org/ThingPulse/minigrafx)

# ThingPulse MiniGrafx Library

This is a graphics library for embedded devices containing a framebuffer to avoid flickering. Instead of writing directly to the display all drawing operations are made on the buffer stored in memory. Once you are finished with one drawing cycle the whole content of the framebuffer is written to the display.

## Service level promise

<table><tr><td><img src="https://thingpulse.com/assets/ThingPulse-open-source-prime.png" width="150">
</td><td>This is a ThingPulse <em>prime</em> project. See our <a href="https://thingpulse.com/about/open-source-commitment/">open-source commitment declaration</a> for what this means.</td></tr></table>

## Inspiration

This library is heavily based on [https://github.com/ThingPulse/esp8266-oled-ssd1306](https://github.com/ThingPulse/esp8266-oled-ssd1306)
developed by Fabrice Weinberg and myself mostly for tiny OLED displays. This new library targets also color displays and e-paper displays with higher resolution

## Philosophy

Often in technology you have to choose between several options and make a trade-off. While some graphic libraries
are optimized for minimal memory consumption they have to accept ugly visual side effects such as flickering or tearing.

## Getting started

[Getting Started](Introduction.md) by reading the intro.

## API

[API Documentation](API.md) of the library.
