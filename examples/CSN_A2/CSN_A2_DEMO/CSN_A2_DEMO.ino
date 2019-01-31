
#include "MiniGrafx.h" // General graphic library
#include "CSN_A2_ThermalPrinter.h" // Hardware-specific library

#define CSN_A2_RX D3
#define CSN_A2_TX D4

#define BITS_PER_PIXEL 1

uint16_t palette[] = {0, 1};

CSN_A2 driver = CSN_A2(CSN_A2_RX, CSN_A2_TX);
MiniGrafx gfx = MiniGrafx(&driver, BITS_PER_PIXEL, palette);

void setup() {
  Serial.begin(115200);
  gfx.init();
  gfx.fillBuffer(0);
  gfx.setColor(1);
  gfx.setFont(ArialMT_Plain_10);
  gfx.drawLine(0, 0, gfx.getWidth(), gfx.getHeight());
  gfx.drawString(10, 10, "Hello World");
  gfx.setFont(ArialMT_Plain_16);
  gfx.drawString(10, 30, "Everything works!");
  gfx.setFont(ArialMT_Plain_24);
  gfx.drawString(10, 55, "Yes! Millis: " + String(millis()));
  gfx.commit();
}

void loop() {

}
