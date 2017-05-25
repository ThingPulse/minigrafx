
#include <Arduino.h>
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
int BITS_PER_PIXEL = 1 ; // 2^4 = 16 colors

// Initialize the driver
ILI9341_SPI tft = ILI9341_SPI(TFT_CS, TFT_DC);
MiniGrafx gfx = MiniGrafx(&tft, SCREEN_WIDTH, SCREEN_HEIGHT, BITS_PER_PIXEL, palette);


void ASSERT_TRUE(String testName, bool condition) {
  if (condition) {
    Serial.println(testName + " PASS");
  } else {
    Serial.println(testName + " FAIL");
  }
}
void testSetPixel() {
  Serial.println("== Test Set Pixel==");
  gfx.fillBuffer(0);
  ASSERT_TRUE("Getting Black Pixel", gfx.getPixel(0, 0) == 0);
  gfx.setColor(1);
  gfx.setPixel(0, 0);
  ASSERT_TRUE("Getting White Pixel", gfx.getPixel(0, 0) == 1);
}

void testDrawLine() {
    Serial.println();
  Serial.println("== Test drawLine ==");
  gfx.fillBuffer(0);
  gfx.setColor(1);
  gfx.drawLine(1, 0, 10, 0);
  ASSERT_TRUE("DrawLine Pixel not set", gfx.getPixel(0, 0) == 0);
  for (int i = 1; i < 10; i++) {
    ASSERT_TRUE("DrawLine Pixel set by line", gfx.getPixel(i, 0) == 1);
  }
  ASSERT_TRUE("DrawLine Pixel not set", gfx.getPixel(11, 0) == 0);
}

void testDrawHorizontalLine() {
  Serial.println();
  Serial.println("== Test drawHorizontalLine ==");
  gfx.fillBuffer(0);
  gfx.setColor(1);
  //0000000000111111
  //0123456789012345
  //----XXXXXXXXX---
  gfx.drawHorizontalLine(4, 5, 9);
  ASSERT_TRUE("DrawLine Pixel not set", gfx.getPixel(3, 4) == 0);
  for (int i = 0; i < 9; i++) {
    ASSERT_TRUE("DrawLine Pixel set by line", gfx.getPixel(4 + i, 5) == 1);
  }
  ASSERT_TRUE("DrawLine Pixel not set", gfx.getPixel(13, 5) == 0);
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    Serial.begin(115200);
    delay(5000);
    Serial.println();
    Serial.println("Test Begin");

    gfx.init();
    testSetPixel();
    testDrawLine();
    testDrawHorizontalLine();
}



void loop() {

}
