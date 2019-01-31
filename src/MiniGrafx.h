/**
The MIT License (MIT)
Copyright (c) 2019 by Daniel Eichhorn, ThingPulse
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

Please note: We are spending a lot of time to write and maintain open source codes
Please support us by buying our products from https://thingpulse.com/shop/

See more at https://thigpulse.com

Many thanks go to various contributors such as Adafruit, Waveshare.
*/

#include <FS.h>
#if defined(ESP32)
  #include <SPIFFS.h>
#endif
#include "ILI9341_SPI.h"
#include "MiniGrafxFonts.h"



#ifndef _MINI_GRAFXH_
#define _MINI_GRAFXH_

#ifndef DEBUG_MINI_GRAFX
#define DEBUG_MINI_GRAFX(...)
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

// Header Values
#define JUMPTABLE_BYTES 4

#define JUMPTABLE_LSB   1
#define JUMPTABLE_SIZE  2
#define JUMPTABLE_WIDTH 3
#define JUMPTABLE_START 4

#define WIDTH_POS 0
#define HEIGHT_POS 1
#define FIRST_CHAR_POS 2
#define CHAR_NUM_POS 3

#define CUSTOM_BITMAP_DATA_START 6

enum TEXT_ALIGNMENT {
  TEXT_ALIGN_LEFT = 0,
  TEXT_ALIGN_RIGHT = 1,
  TEXT_ALIGN_CENTER = 2,
  TEXT_ALIGN_CENTER_BOTH = 3
};

enum BUFFER_COLOR_DEPTH {
  BIT_1 = 1,
  BIT_2 = 2,
  BIT_4 = 4,
  BIT_8 = 8,
  BIT_16 = 16
};

#undef max
#define max(a,b) ((a)>(b)?(a):(b))
#undef min
#define min(a,b) ((a)<(b)?(a):(b))

class MiniGrafx {

 public:
  MiniGrafx(DisplayDriver *driver, uint8_t bitsPerPixel, uint16_t *palette);
  MiniGrafx(DisplayDriver *driver, uint8_t bitsPerPixel, uint16_t *palette, uint16_t width, uint16_t height);
  void init();
  void changeBitDepth(uint8_t bitsPerPixel, uint16_t *palette);
  uint16_t getHeight();
  uint16_t getWidth();
  void setRotation(uint8_t r);
  void setMirroredHorizontally(boolean isMirroredHorizontally);
  void setMirroredVertically(boolean isMirroredVertically);
  void setPixel(uint16_t x, uint16_t y);
  uint16_t getPixel(uint16_t x, uint16_t y);
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
  void setColor(uint16_t color);
  void setTransparentColor(uint16_t transparentColor);
  void drawCircle(int16_t x0, int16_t y0, uint16_t radius);
  void drawRect(int16_t x, int16_t y, int16_t width, int16_t height);
  void fillRect(int16_t xMove, int16_t yMove, int16_t width, int16_t height);
  void fillCircle(int16_t x0, int16_t y0, int16_t radius);
  void drawHorizontalLine(int16_t x, int16_t y, int16_t length);
  void drawVerticalLine(int16_t x, int16_t y, int16_t length);
  uint16_t drawString(int16_t xMove, int16_t yMove, String strUser);

  // Draws a String with a maximum width at the given location.
  // If the given String is wider than the specified width
  // The text will be wrapped to the next line at a space or dash
  void drawStringMaxWidth(int16_t x, int16_t y, uint16_t maxLineWidth, String text);

  void drawStringInternal(int16_t xMove, int16_t yMove, char* text, uint16_t textLength, uint16_t textWidth);
  void fillBottomFlatTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);
  void fillTopFlatTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);
  void fillTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);
  void drawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);
  uint16_t getStringWidth(const char* text, uint16_t length);
  void drawXbm(int16_t x, int16_t y, int16_t width, int16_t height, const char *xbm);
  void drawBmpFromFile(String filename, uint8_t x, uint16_t y);
  void drawBmpFromPgm(const char *xbm, uint8_t x, uint16_t y);
  void drawPalettedBitmapFromPgm(uint16_t x, uint16_t y, const char *palBmp);
  void drawPalettedBitmapFromFile(uint16_t x, uint16_t y, String fileName);

  uint16_t read16(File &f);
  uint32_t read32(File &f);
  void setFont(const char *fontData);
  void setFontFile(String fontFile);
  void setTextAlignment(TEXT_ALIGNMENT textAlignment);
  void inline drawInternal(int16_t xMove, int16_t yMove, int16_t width, int16_t height, const char *data, uint16_t offset, uint16_t bytesInData);
  void commit();
  void commit(uint16_t xPos, uint16_t yPos);
  void commit(uint16_t srcXPos, uint16_t srcYPos, uint16_t srcWidth, uint16_t srcHeight, uint16_t targetXPos, uint16_t targetYPos);

  void clear();
  void freeBuffer();
  void setFastRefresh(boolean isFastRefreshEnabled);
  void fillBuffer(uint8_t pal);
  static char* utf8ascii(String s);
  static byte utf8ascii(byte ascii);
  void colorSwap(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color1, uint16_t color2);

 private:
  void initializeBuffer();
  DisplayDriver *driver;
  File fontFile;
  uint16_t width, height;
  uint16_t initialWidth, initialHeight;
  uint16_t color;
  uint8_t rotation;
  int16_t transparentColor = -1;
  uint8_t bitsPerPixel = 4;
  uint8_t bitShift = 1;
  uint16_t bufferSize = 0;
  uint16_t* palette = 0;
  uint8_t *buffer = 0;
  uint16_t bitMask;
  uint8_t pixelsPerByte;
  boolean isPgmFont = true;
  boolean isMirroredHorizontally = false;
  boolean isMirroredVertically = false;
  const char *fontData = ArialMT_Plain_16;
  TEXT_ALIGNMENT textAlignment;
  uint8_t readFontData(const char * start, uint32_t offset);

};

#endif
