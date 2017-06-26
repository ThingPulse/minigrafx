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
#include <XPT2046_Touchscreen.h>
#include "MiniGrafx.h" // General graphic library
#include "ILI9341_SPI.h" // Hardware-specific library
#include <FS.h>

#define TFT_DC D2
#define TFT_CS D1
#define TFT_LED D8
#define TOUCH_CS D3
#define TOUCH_IRQ  D4

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

XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

bool loadTouchCalibration();
bool saveTouchCalibration();

TS_Point p1, p2;
int state = 0;
int lastStateChange = 0;

float dx = 0;
float dy = 0;
int ax = 0;
int ay = 0;
int color = 1;

void setup() {
    // Turn on the background LED
    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, HIGH);

    ts.begin();

    gfx.init();
    gfx.fillBuffer(0);
    gfx.commit();
    bool isCalibrationAvailable = loadTouchCalibration();
    if (isCalibrationAvailable) {
      state = 2;
    }
}

void loop() {

  TS_Point p = ts.getPoint();

    if (state == 0) {
      gfx.fillBuffer(0);
      gfx.setColor(1);
      gfx.setTextAlignment(TEXT_ALIGN_CENTER);
      gfx.drawString(120, 160, "Please Touch Circle");
      gfx.fillCircle(5, 5, 5);
      if (ts.touched()) {
        p1 = p;
        state++;
        lastStateChange = millis();
        gfx.fillBuffer(0);
      }

    } else if (state == 1) {
      gfx.fillBuffer(0);
      gfx.setColor(1);
      gfx.setTextAlignment(TEXT_ALIGN_CENTER);
      gfx.drawString(120, 160, "Please Touch Circle");
      gfx.drawCircle(235, 315, 5);
      if (ts.touched() && (millis() - lastStateChange > 1000)) {

        p2 = p;
        state++;
        lastStateChange = millis();
        dx = 240.0 / abs(p1.y - p2.y);
        dy = 320.0 / abs(p1.x - p2.x);
        ax = p1.y < p2.y ? p1.y : p2.y;
        ay = p1.x < p2.x ? p1.x : p2.x;
        gfx.fillBuffer(0);
        saveTouchCalibration();
      }

    } else {
      for (int i = 0; i < 16; i++) {
        gfx.setColor(i);
        gfx.fillRect(i* 15, 0, 20, 20);
      }
      if (ts.touched() && (millis() - lastStateChange > 1000)) {

        int x = (p.y - ax) * dx;
        int y = 320 - (p.x - ay) * dy;
        if (y < 15) {
          color = x / 15;
          if (color == 0) {
            gfx.fillBuffer(0);
            color = 1;
          }
        }
        gfx.setColor(color);
        gfx.fillCircle(x, y, 5);

      }
    }

  gfx.commit();
}

bool loadTouchCalibration() {

  // always use this to "mount" the filesystem
  bool result = SPIFFS.begin();
  Serial.println("SPIFFS opened: " + result);

  // this opens the file "f.txt" in read-mode
  File f = SPIFFS.open("/calibration.txt", "r");

  if (!f) {
    return false;
  } else {

      //Lets read line by line from the file
      String dxStr = f.readStringUntil('\n');
      String dyStr = f.readStringUntil('\n');
      String axStr = f.readStringUntil('\n');
      String ayStr = f.readStringUntil('\n');

      dx = dxStr.toFloat();
      dy = dyStr.toFloat();
      ax = axStr.toInt();
      ay = ayStr.toInt();

  }
  f.close();
}

bool saveTouchCalibration() {


  // always use this to "mount" the filesystem
  bool result = SPIFFS.begin();

  // open the file in write mode
  File f = SPIFFS.open("/calibration.txt", "w");
  if (!f) {
    Serial.println("file creation failed");
  }
  // now write two lines in key/value style with  end-of-line characters
  f.println(dx);
  f.println(dy);
  f.println(ax);
  f.println(ay);

  f.close();
}
