#include <SPI.h>
#include "EPD_WaveShare_1_54.h"
#include "MiniGrafx.h"
#include "DisplayDriver.h"

#define CS D3
#define RST D2
#define DC D8
#define BUSY D1

#define SCREEN_WIDTH 200
#define SCREEN_HEIGHT 200
#define BITS_PER_PIXEL 1


uint16_t palette[] = {0, 1};

EPD_WS_154 epd(CS, RST, DC, BUSY, 200, 200);
MiniGrafx gfx = MiniGrafx(&epd, SCREEN_WIDTH, SCREEN_HEIGHT, BITS_PER_PIXEL, palette);

void setup() {
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
  //gfx.drawLine(0, 0, 200, 200);
  gfx.drawString(10, 10, "Hello World");
  gfx.setFont(ArialMT_Plain_16);
  gfx.drawString(10, 30, "Everything works!");
  gfx.setFont(ArialMT_Plain_24);
  gfx.drawString(10, 55, "Yes! Millis: " + String(millis()));
  gfx.commit();
  delay(5000);
  rotation = (rotation + 1) % 4;
}
