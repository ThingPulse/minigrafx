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
uint8_t mode = 0;
boolean modeChanged = true;

#define MODES 3

void testFullScreenCommit() {

  screenGfx.setRotation(rotation);
  screenGfx.fillBuffer(MINI_WHITE);
  screenGfx.setColor(MINI_BLACK);
  screenGfx.setFont(ArialMT_Plain_10);
  screenGfx.drawLine(0, 0, screenGfx.getWidth(), screenGfx.getHeight());
  screenGfx.drawRect(0, 0, screenGfx.getWidth() - 1, screenGfx.getHeight() - 1);
  screenGfx.drawString(0, 0, "Hello World.\nMillis: " + String(millis()) + "\nRotation: " + String(rotation));
  screenGfx.commit();
  rotation = (rotation + 1) % 4;
}

void testPictureInPicture() {
  if (modeChanged) {
    screenGfx.commit();
    screenGfx.commit();
  }
  dialogGfx.setFastRefresh(true);
  dialogGfx.setRotation(rotation);
  dialogGfx.fillBuffer(MINI_BLACK);
  dialogGfx.setColor(MINI_WHITE);
  dialogGfx.setFont(ArialMT_Plain_10);
  dialogGfx.drawLine(0, 0, dialogGfx.getWidth(), dialogGfx.getHeight());
  dialogGfx.drawString(0, 0, "Hello World. Rotation:\n" + String(rotation));
  dialogGfx.commit(16, 16);
  rotation = (rotation + 1) % 4;
}

void testWindowedCommit() {
  if (modeChanged) {
    screenGfx.setFastRefresh(false);
    screenGfx.commit();
    screenGfx.commit();
  }
  screenGfx.fillBuffer(MINI_BLACK);
  screenGfx.setColor(MINI_WHITE);
  screenGfx.setFastRefresh(true);
  screenGfx.setRotation(0);
  screenGfx.setFont(ArialMT_Plain_24);
  screenGfx.drawString(18, 18, String(millis()));
  screenGfx.commit(16, 16, 80, 32, 16, 16);
}

void setup() {
  Serial.begin(115200);

  screenGfx.init();
  screenGfx.fillBuffer(MINI_WHITE);
  screenGfx.setColor(MINI_BLACK);
  // the device thas two screen buffers. For partial update
  // make sure that both have the same content-> clear both.
  screenGfx.commit();
  screenGfx.commit();

  dialogGfx.init();


}

#define REPETITIONS 10
#define MODES 4

void loop() {
  uint64_t startTime = millis();
  switch(mode) {
    case 0:
      Serial.println("Testing Fullscreen commit. Fast Refresh");
      screenGfx.setFastRefresh(true);
      testFullScreenCommit();
      break;
    case 1:
      Serial.println("Testing Fullscreen commit. Full Refresh");
      screenGfx.setFastRefresh(false);
      testFullScreenCommit();
      break;
    case 2:
      Serial.println("Testing Picture in Picture/ Partial update");
      dialogGfx.setFastRefresh(true);
      testPictureInPicture();
      break;
    case 3:
      Serial.println("Testing windowed commit");
      testWindowedCommit();
  }
  Serial.printf("Cycle took: %dms\n", millis() - startTime );

  counter++;
  modeChanged = false;
  if (counter % REPETITIONS == 0) {
    modeChanged = true;
    mode = (mode + 1) % MODES;
  }
  //delay(4000);

}
