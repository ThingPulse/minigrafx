![](https://api.travis-ci.org/squix78/minigrafx.svg?branch=master)

# The MiniGrafx Library

This is a graphics library for embedded devices containing a framebuffer to avoid flickering. Instead of writing directly to the display all drawing operations are made on the buffer stored in memory. Once you are finished with one drawing cycle the whole content of the framebuffer is written to the display.

## Inspiration

This library is heavily based on another library https://github.com/squix78/esp8266-oled-ssd1306
developed by Fabrice Weinberg and myself mostly for tiny OLED displays. This new library targets also color displays and e-paper displays with higher resolution

## Basic flow

First you have to include the basic graphic routines and the header file matching your display:

```C++
#include <SPI.h>
#include "MiniGrafx.h" // General graphic library
#include "ILI9341_SPI.h" // Hardware-specific library

#define TFT_DC D2
#define TFT_CS D1
#define TFT_LED D8

// defines the colors usable in the paletted 16 color frame buffer
uint16_t palette[] = {ILI9341_BLACK, // 0
                      ILI9341_WHITE, // 1
                      ILI9341_NAVY, // 2
                      ILI9341_DARKCYAN, // 3
                      ILI9341_DARKGREEN, // 4
                      ILI9341_MAROON, // 5
                      ILI9341_PURPLE, // 6
                      ILI9341_OLIVE, // 7
                      ILI9341_LIGHTGREY, // 8
                      0x39E7, //ILI9341_DARKGREY, // 9
                      ILI9341_BLUE, // 10
                      ILI9341_GREEN, // 11
                      ILI9341_CYAN, // 12
                      ILI9341_RED, // 13
                      ILI9341_MAGENTA, // 14
                      0xFD80}; // 15



int SCREEN_WIDTH = 240;
int SCREEN_HEIGHT = 320;
int BITS_PER_PIXEL = 4 ; // 2^4 = 16 colors

// Initialize the driver
ILI9341_SPI tft = ILI9341_SPI(TFT_CS, TFT_DC);
MiniGrafx gfx = MiniGrafx(&tft, BITS_PER_PIXEL, palette);

void setup() {
  Serial.begin(115200);

  // Turn on the background LED
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  // Initialize the driver only once
  gfx.init();
  // fill the buffer with black
  gfx.fillBuffer(0);
  // write the buffer to the display
  gfx.commit();
}


void loop() {
  gfx.fillBuffer(0);
  gfx.setColor(1);
  gfx.drawLine(0, 0, 20, 20);
  gfx.setColor(13);
  gfx.fillCircle(20, 20, 5);
  gfx.commit();
}
```

So what are we doing here? On the first lines we include MiniGrafx header and the driver. The MiniGrafx object
is responsible for drawing operations, whereas the driver is only responsible for writing the framebuffer to the display.

Then the next important step is the definition of the palette array. In this example we will use a 16 color palette which means that our framebuffer will only know colors represented by the values from 0 to 15. Before writing the values to the display the driver will turn these values back into the colors of the display and a pixel represented by only 4 bit suddenly will be represented by 16 (in case of the ILI9341 display).
