![](https://api.travis-ci.org/squix78/minigrafx.svg?branch=master)

# The MiniGrafx Library

This is a graphics library for embedded devices containing a framebuffer to avoid flickering. Instead of writing directly to the display all drawing operations are made on the buffer stored in memory. Once you are finished with one drawing cycle the whole content of the framebuffer is written to the display.

## Inspiration

This library is heavily based on another library https://github.com/squix78/esp8266-oled-ssd1306
developed by Fabrice Weinberg and myself mostly for tiny OLED displays. This new library targets also color displays and e-paper displays with higher resolution

## Getting started

[Getting Started](Introduction.md)

## API

[API Documentation](API.md)
