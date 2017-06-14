// -*- mode: c++ -*-
// Copyright 2013 Pervasive Displays, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.  See the License for the specific language
// governing permissions and limitations under the License.

#include <inttypes.h>
#include <ctype.h>

// required libraries
#include <SPI.h>
#include "EPaperPervasive.h"
#include "MiniGrafx.h"
#include "DisplayDriver.h" 

// Arduino IO layout
//const int Pin_TEMPERATURE = A0; // Temperature is handled by LM75 over I2C and not an analog pin
const int Pin_PANEL_ON = D2;//5;
const int Pin_BORDER = D4;//10;
const int Pin_DISCHARGE = D1;//4;
//const int Pin_PWM = 5;    // Not used by COG v2
const int Pin_RESET = D0;//6;
const int Pin_BUSY = D1;//7;
const int Pin_EPD_CS = D3;
const int Pin_FLASH_CS = D8;//9;
const int Pin_SW2 = D8;//12;
const int Pin_RED_LED = D8;//13;


// defines the colors usable in the paletted 16 color frame buffer
uint16_t palette[] = {0, 1};


// LED anode through resistor to I/O pin
// LED cathode to Ground
#define LED_ON  HIGH
#define LED_OFF LOW

#define EPD_SIZE EPD_2_7
#define SCREEN_WIDTH 264
#define SCREEN_HEIGHT 176
#define BITS_PER_PIXEL 1

// define the E-Ink display
EPD_Class epd(EPD_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT, Pin_PANEL_ON, Pin_BORDER, Pin_DISCHARGE, Pin_RESET, Pin_BUSY, Pin_EPD_CS);
MiniGrafx gfx = MiniGrafx(&epd, BITS_PER_PIXEL, palette);

// I/O setup
void setup() {
  pinMode(Pin_RED_LED, OUTPUT);
  //pinMode(Pin_SW2, INPUT);
  //pinMode(Pin_TEMPERATURE, INPUT);
  //pinMode(Pin_PWM, OUTPUT);
  //pinMode(Pin_BUSY, INPUT);
  pinMode(Pin_RESET, OUTPUT);
  pinMode(Pin_PANEL_ON, OUTPUT);
  pinMode(Pin_DISCHARGE, OUTPUT);
  pinMode(Pin_BORDER, OUTPUT);
  pinMode(Pin_EPD_CS, OUTPUT);
  //pinMode(Pin_FLASH_CS, OUTPUT);

  digitalWrite(Pin_RED_LED, LOW);
  //digitalWrite(Pin_PWM, LOW);  // not actually used - set low so can use current eval board unmodified
  digitalWrite(Pin_RESET, LOW);
  digitalWrite(Pin_PANEL_ON, LOW);
  digitalWrite(Pin_DISCHARGE, LOW);
  digitalWrite(Pin_BORDER, LOW);
  digitalWrite(Pin_EPD_CS, LOW);
  //digitalWrite(Pin_FLASH_CS, HIGH);

  Serial.begin(115200);

  Serial.println();
  Serial.println();
  //Serial.println("Demo G2 version: " DEMO_VERSION);
  //Serial.println("Display: " MAKE_STRING(EPD_SIZE));
  Serial.println();

  gfx.init();
  epd.init(); // power up the EPD panel
  if (!epd) {
    Serial.print("EPD error = ");
    Serial.print(epd.error());
    Serial.println("");
    return;
  } else {
    Serial.println("Good to go");
  }
  epd.setFactor(25);
  epd.clear();
}


static int state = 2;


// main loop
void loop() {
  gfx.fillBuffer(0);
  gfx.setFont(ArialMT_Plain_24);
  gfx.setColor(1);
  gfx.setTextAlignment(TEXT_ALIGN_LEFT);
  gfx.drawString(0, 0, "EPaper Display Driver");
  gfx.drawLine(0, 26, 264, 26);
  gfx.fillCircle(264/2, 176 / 2, 10);
  gfx.drawRect(264/2 - 12, 176 / 2 - 12, 24, 24);
  gfx.commit();


  epd.end();   // power down the EPD panel
  delay(50000);
}
