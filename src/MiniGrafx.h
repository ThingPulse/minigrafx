#include "ILI9341_SPI.h"
#include "MiniGrafxFonts.h"
#include <FS.h>


#ifndef _MINI_GRAFXH_
#define _MINI_GRAFXH_

//#define DEBUG_MINI_GRAFX(...) Serial.printf( __VA_ARGS__ )

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
  MiniGrafx(DisplayDriver *driver, uint16_t width, uint16_t height, uint8_t bitsPerPixel, uint16_t *palette);
  void init();
  void setRotation(uint8_t r);
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
  void drawString(int16_t xMove, int16_t yMove, String strUser);
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

  uint16_t read16(File &f);
  uint32_t read32(File &f);
  void setFont(const char *fontData);
  void setTextAlignment(TEXT_ALIGNMENT textAlignment);
  void inline drawInternal(int16_t xMove, int16_t yMove, int16_t width, int16_t height, const char *data, uint16_t offset, uint16_t bytesInData);
  void commit();
  void clear();
  void fillBuffer(uint8_t pal);
  static char* utf8ascii(String s);
  static byte utf8ascii(byte ascii);

 private:
  DisplayDriver *driver;
  uint16_t width, height;
  uint16_t color;
  uint8_t rotation;
  int16_t transparentColor = -1;
  uint8_t bitsPerPixel = 4;
  uint8_t bitShift = 1;
  uint16_t bufferSize = 0;
  uint16_t* palette = 0;
  uint8_t *buffer = 0;
  uint8_t bitMask;
  uint8_t pixelsPerByte;
  const char *fontData = ArialMT_Plain_16;
  TEXT_ALIGNMENT textAlignment;

};

#endif
