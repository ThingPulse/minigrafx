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


#include "EPD_WaveShare_43.h"
#include "MiniGrafx.h"
#include "DisplayDriver.h"



#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BITS_PER_PIXEL 1

const int wake_up = 18;
const int reset = 19;

uint16_t palette[] = {0, 1};

HardwareSerial mySerial(2);
EPD_WaveShare_43 epd(&mySerial);
MiniGrafx gfx = MiniGrafx(&epd, BITS_PER_PIXEL, palette);

void setup() {
  pinMode(wake_up, HIGH);
  pinMode(reset, HIGH);
  gfx.init();
  
}

uint8_t rotation = 0;

void loop() {
  //epd.Dis_Clear_full();
  //delay(1500);
  //epd.EPD_init_Part();
  //delay(1000);

  //gfx.init();
  gfx.setRotation(rotation);
  gfx.fillBuffer(1);
  gfx.setColor(0);
  gfx.setFont(ArialMT_Plain_10);
  gfx.drawLine(0, 0, gfx.getWidth(), gfx.getHeight());
  gfx.drawString(10, 10, "Hello World");
  gfx.setFont(ArialMT_Plain_16);
  gfx.drawString(10, 30, "Everything works!");
  gfx.setFont(ArialMT_Plain_24);
  gfx.drawString(10, 55, "Yes! Millis: " + String(millis()));
  gfx.commit();
  delay(5000);
  rotation = (rotation + 1) % 4;
}
