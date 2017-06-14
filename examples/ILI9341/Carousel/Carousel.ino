
#include <SPI.h>
#include "MiniGrafx.h" // General graphic library
#include "ILI9341_SPI.h" // Hardware-specific library
#include "Carousel.h"

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
Carousel carousel(&gfx, 0, 0, 240, 100);

void drawFrame1(MiniGrafx *display, CarouselState* state, int16_t x, int16_t y) {
  display->setColor(1);
  display->setFont(ArialMT_Plain_16);
  display->drawString(x + 10, y + 10, "The Carousel is Back!");
}

void drawFrame2(MiniGrafx *display, CarouselState* state, int16_t x, int16_t y) {
  // Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
  // Besides the default fonts there will be a program to convert TrueType fonts into this format
  display->setColor(13);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(10 + x, 10 + y, "Arial 10");

  display->setColor(11);
  display->setFont(ArialMT_Plain_16);
  display->drawString(10 + x, 20 + y, "Arial 16");

  display->setColor(12);
  display->setFont(ArialMT_Plain_24);
  display->drawString(10 + x, 34 + y, "Arial 24");
}

FrameCallback frames[] = { drawFrame1, drawFrame2 };

// how many frames are there?
int frameCount = 2;


void setup() {

  Serial.begin(115200);

  // Turn on the background LED
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  carousel.setFrames(frames, frameCount);

  gfx.init();
  gfx.fillBuffer(0);
  gfx.commit();

}

void loop() {
  int remainingTimeBudget = carousel.update();

  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    delay(remainingTimeBudget);
  }
}
