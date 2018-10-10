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
#include "EPD_WaveShare_29.h"
#include "MiniGrafx.h"
#include "DisplayDriver.h"

#define MINI_BLACK 0
#define MINI_WHITE 1

/*
 Connect the following pins:
 Display  NodeMCU
 BUSY     D1
 RST      D2
 DC       D8
 CS       D3
 CLK      D5
 DIN      D7
 GND      GND
 3.3V     3V3
*/
/*#define CS D3
#define RST D2
#define DC D8
#define BUSY D1*/

#define CS 15  // D8
#define RST 2  // D4
#define DC 5   // D1
#define BUSY 4 // D2

#define BITS_PER_PIXEL 1


uint16_t palette[] = {0, 1};
boolean isFastRefreshEnabled = false;

EPD_WaveShare29 epd(CS, RST, DC, BUSY);
MiniGrafx screenGfx = MiniGrafx(&epd, BITS_PER_PIXEL, palette, EPD_WIDTH, EPD_HEIGHT);
MiniGrafx dialogGfx = MiniGrafx(&epd, BITS_PER_PIXEL, palette, 96, 96);

uint8_t counter = 0;
uint8_t rotation = 0;

void setup() {
  Serial.begin(115200);
  dialogGfx.init();
  screenGfx.init();
  screenGfx.setRotation(rotation);
  screenGfx.fillBuffer(MINI_WHITE);
  screenGfx.setColor(MINI_BLACK);
  screenGfx.setFont(ArialMT_Plain_10);
  screenGfx.drawLine(0, 0, screenGfx.getWidth(), screenGfx.getHeight());
  screenGfx.drawRect(0, 0, screenGfx.getWidth() - 1, screenGfx.getHeight() - 1);
  screenGfx.drawString(0, 0, "Hello World.\nMillis: " + String(millis()) + "\nRotation: " + String(rotation));
  screenGfx.commit();


}



void loop() {


  dialogGfx.setFastRefresh(true);
  dialogGfx.setRotation(rotation);
  dialogGfx.fillBuffer(MINI_BLACK);
  dialogGfx.setColor(MINI_WHITE);
  dialogGfx.setFont(ArialMT_Plain_10);
  dialogGfx.drawLine(0, 0, dialogGfx.getWidth(), dialogGfx.getHeight());
  dialogGfx.drawString(0, 0, "Hello World. Rotation:\n" + String(rotation));
  dialogGfx.commit(16, 16);
  /*gfx.setFont(ArialMT_Plain_16);
  gfx.drawString(10, 30, "Everything works!");
  gfx.setFont(ArialMT_Plain_24);
  gfx.drawString(10, 55, "Yes! Millis: \n" + String(counter));
  long startTime = millis();
  dialogGfx.commit(0, 0);
  Serial.printf("Time to update screen: %d, Width %d, Height: %d\n",  millis() - startTime,  dialogGfx.getWidth(), dialogGfx.getHeight());
  delay(1000);*/
  rotation = (rotation + 1) % 4;
  /*if (rotation == 3) {
    isFastRefreshEnabled = !isFastRefreshEnabled;
    gfx.setFastRefresh(isFastRefreshEnabled);
  }*/
  counter++;
  //delay(4000);

}
