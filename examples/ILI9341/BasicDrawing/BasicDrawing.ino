/**
The MIT License (MIT)
Copyright (c) 2017 by Daniel Eichhorn
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please note: I am spending a lot of my free time in developing Software and Hardware
for these projects. Please consider supporting me by
a) Buying my hardware kits from https://blog.squix.org/shop
b) Send a donation: https://www.paypal.me/squix/5USD
c) Or using this affiliate link while shopping: https://www.banggood.com/?p=6R31122484684201508S

See more at https://blog.squix.org

Demo for the buffered graphics library. Renders a 3D cube
*/

#include <SPI.h>
#include "MiniGrafx.h" // General graphic library
#include "ILI9341_SPI.h" // Hardware-specific library
#include "image.h"

#define TFT_DC D2
#define TFT_CS D1
#define TFT_LED D8

// defines the colors usable in the paletted 16 color frame buffer
uint16_t palette[256];



int SCREEN_WIDTH = 240;
int SCREEN_HEIGHT = 320;
int BITS_PER_PIXEL = 16;

// Initialize the driver
ILI9341_SPI tft = ILI9341_SPI(TFT_CS, TFT_DC);
MiniGrafx gfx = MiniGrafx(&tft, BITS_PER_PIXEL, palette, 240, 40);

uint64_t lastRefresh = 0;
uint16_t counter = 0;
void setup() {
  Serial.begin(115200);

  // Turn on the background LED
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);
  for (uint16_t i = 0; i < 255; i++) {
    palette[i] = ((i / 8) << 11) | ((i / 4) << 5) | (i / 8);
  }
  // Initialize the driver only once
  gfx.init();
  // fill the buffer with black
  gfx.fillBuffer(0);
  // write the buffer to the display
  //gfx.commit();

  gfx.drawBmpFromPgm(image, 0, 0);
  gfx.setColor(ILI9341_WHITE);
  gfx.drawString(0, 0, String(1000.0 / (millis() - lastRefresh), 1) + "fps");
  lastRefresh = millis();
  gfx.commit(0, 0);
  gfx.fillBuffer(0);
  gfx.drawBmpFromPgm(image, 0, -40);
  gfx.commit(0, 40);
}

//uint8_t counter = 0;
void loop() {
  gfx.fillBuffer(0);
  gfx.setColor(1);
  gfx.setFont(ArialMT_Plain_16);
  gfx.setColor(ILI9341_WHITE);
  gfx.drawString(0, 0, String(1000.0 / (millis() - lastRefresh), 1) + "fps");
  lastRefresh = millis();
  gfx.commit(0, 80);




  counter++;

}
