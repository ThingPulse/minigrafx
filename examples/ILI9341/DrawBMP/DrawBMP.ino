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
#include <FS.h>

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
                      ILI9341_DARKGREY, // 9
                      ILI9341_BLUE, // 10
                      ILI9341_GREEN, // 11
                      ILI9341_CYAN, // 12
                      ILI9341_RED, // 13
                      ILI9341_MAGENTA, // 14
                      ILI9341_YELLOW}; // 15



int SCREEN_WIDTH = 240;
int SCREEN_HEIGHT = 320;
int BITS_PER_PIXEL = 4; // 2^4 = 16 colors

// Initialize the driver
ILI9341_SPI tft = ILI9341_SPI(TFT_CS, TFT_DC);
MiniGrafx gfx = MiniGrafx(&tft, BITS_PER_PIXEL, palette);


// Used for fps measuring
uint16_t counter = 0;
long startMillis = millis();
uint16_t interval = 20;
int color = 1;
String fps = "0fps";

void setup() {
  Serial.begin(115200);

  // Turn on the background LED
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  // create grey scale palette for greay image
  for (int i = 0; i < 16; i++) {
    palette[i] = ((i * 31 / 16) << 11) | ((i * 63 / 16) << 5) | ((i * 31 / 16));
  }

  gfx.init();
  gfx.fillBuffer(0);
  gfx.commit();
  //SPIFFS.format();
  SPIFFS.begin();
  /*if (!SPIFFS.remove("file.bmp")) {
    Serial.println("Could not remove file");
  }
  File f = SPIFFS.open("file.bmp", "w+");
  for (int i = 0; i < image_len; i++) {
    uint8_t data = pgm_read_byte(image + i);
    f.write(data);
  }
  f.close();*/

  //SPIFFS.end();

  startMillis = millis();
}


void loop() {

  gfx.fillBuffer(0);
  gfx.setColor(color);
  gfx.drawBmpFromFile("file.bmp", 10, 40 + counter);
  //gfx.drawBmpFromPgm(image, 10, 40);
  gfx.setColor(15);
  gfx.drawString(2, 2, fps);
  for (int i = 0; i < 16; i++) {
    gfx.setColor(i);
    gfx.drawLine(0, i + 100, 240, i + 100);
  }
  gfx.commit();

  counter++;
  // only calculate the fps every <interval> iterations.
  if (counter % interval == 0) {
    color = (color + 1) % 15 + 1;
    long millisSinceUpdate = millis() - startMillis;
    fps = String(interval * 1000.0 / (millisSinceUpdate)) + "fps";
    startMillis = millis();
  }
}
