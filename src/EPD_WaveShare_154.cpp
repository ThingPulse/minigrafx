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

#include <stdlib.h>
#include "EPD_WaveShare_154.h"

EPD_WaveShare154::~EPD_WaveShare154() {
};

EPD_WaveShare154::EPD_WaveShare154(uint8_t csPin, uint8_t rstPin, uint8_t dcPin, uint8_t busyPin) : DisplayDriver(EPD_WIDTH, EPD_HEIGHT) {
    this->reset_pin = rstPin;
    this->dc_pin = dcPin;
    this->cs_pin = csPin;
    this->busy_pin = busyPin;
    width = EPD_WIDTH;
    height = EPD_HEIGHT;
};

void EPD_WaveShare154::setRotation(uint8_t r) {
  this->rotation = r;

}
void EPD_WaveShare154::init() {
  Init(lut_full_update);
}

void EPD_WaveShare154::setFastRefresh(boolean isFastRefreshEnabled) {
  if (isFastRefreshEnabled != this->isFastRefreshEnabled) {
    if (isFastRefreshEnabled) {
      Init(lut_partial_update);
    } else {
      Init(lut_full_update);
    }
  }
  this->isFastRefreshEnabled = isFastRefreshEnabled;

}

// uint8_t *buffer, uint8_t bitsPerPixel, uint16_t *palette, uint16_t xPosOrig, uint16_t yPosOrig, uint16_t bufferWidth, uint16_t bufferHeight
void EPD_WaveShare154::writeBuffer(BufferInfo *bufferInfo) {
  //SetFrameMemory(buffer, 0, 0, EPD_WIDTH, EPD_HEIGHT);
  uint16_t xPos = (bufferInfo->targetX / 8) * 8;
  uint16_t yPos = bufferInfo->targetY;
  uint16_t x = 0;
  uint16_t y = 0;
  uint16_t targetWidth = this->width;
  uint16_t targetHeight = this->height;
  uint16_t sourceWidth = bufferInfo->bufferWidth;
  uint16_t sourceHeight = bufferInfo->bufferHeight;
  uint16_t windowWidth = bufferInfo->windowWidth;
  uint16_t windowHeight = bufferInfo->windowHeight;
  switch(rotation) {
    case 0:
    case 2:
      targetWidth = windowWidth;
      targetHeight = windowHeight;
      break;
    case 1:
    case 3:
      targetWidth = windowHeight;
      targetHeight = windowWidth;
      break;
  }

  uint8_t data;

  SetMemoryArea(xPos, yPos, xPos + targetWidth - 1, yPos + targetHeight - 1);
  SetMemoryPointer(xPos, yPos);
  SendCommand(WRITE_RAM);
  /* send the image data */
  for (int i = 0; i < targetHeight; i++) {
      for (int j = 0; j < (targetWidth) / 8; j++) {

          data = 0;
          // fill the whole byte
          for (int b = 0; b < 8; b++) {
            data = data << 1;
            switch (rotation) {
              case 0:
                x = (j * 8 + b);
                y = i;
                break;
              case 1:
                x = sourceWidth - i - 1;
                y = (j * 8 + b);
                break;
              case 2:
                x = sourceWidth - (j * 8 + b) - 1;
                y = sourceHeight - i - 1;
                break;
              case 3:
                x = i;
                y = sourceHeight - (j * 8 + b) - 1;
                break;
            }

            //
            data = data | (getPixel(bufferInfo->buffer, bufferInfo->windowX + x, bufferInfo->windowY + y, sourceWidth, sourceHeight) & 1);

          }

          SendData(data);
          yield();
      }

    }

    DisplayFrame();

}

uint8_t EPD_WaveShare154::getPixel(uint8_t *buffer, uint16_t x, uint16_t y, uint16_t bufferWidth, uint16_t bufferHeight) {
  uint8_t bitsPerPixel = 1;
  uint8_t bitMask = (1 << bitsPerPixel) - 1;
  uint8_t pixelsPerByte = 8 / bitsPerPixel;
  uint8_t bitShift = 3;

  if (x >= bufferWidth || y >= bufferHeight) return 0;
  // bitsPerPixel: 8, pixPerByte: 1, 0  1 = 2^0
  // bitsPerPixel: 4, pixPerByte: 2, 1  2 = 2^1
  // bitsPerPixel  2, pixPerByte: 4, 2  4 = 2^2
  // bitsPerPixel  1, pixPerByte: 8, 3  8 = 2^3
  uint16_t pos = (y * bufferWidth + x) >> bitShift;

  uint8_t shift = (x & (pixelsPerByte - 1)) * bitsPerPixel;

  return (buffer[pos] >> shift) & bitMask;
}

int EPD_WaveShare154::IfInit(void) {
    pinMode(this->cs_pin, OUTPUT);
    pinMode(this->reset_pin, OUTPUT);
    pinMode(this->dc_pin, OUTPUT);
    pinMode(this->busy_pin, INPUT);
    SPI.setBitOrder(MSBFIRST);  
    SPI.setDataMode(SPI_MODE0);
    SPI.setFrequency(4000000);
    SPI.begin();
    return 0;
}


void EPD_WaveShare154::DigitalWrite(int pin, int value) {
    digitalWrite(pin, value);
}

int EPD_WaveShare154::DigitalRead(int pin) {
    return digitalRead(pin);
}

void EPD_WaveShare154::DelayMs(unsigned int delaytime) {
    delay(delaytime);
}

void EPD_WaveShare154::SpiTransfer(unsigned char data) {
    digitalWrite(this->cs_pin, LOW);
    SPI.transfer(data);
    digitalWrite(this->cs_pin, HIGH);
}

int EPD_WaveShare154::Init(const unsigned char* lut) {
    /* this calls the peripheral hardware interface, see epdif */
    if (IfInit() != 0) {
        return -1;
    }

    /* EPD hardware init start */
    this->lut = lut;
    Reset();
    SendCommand(DRIVER_OUTPUT_CONTROL);
    SendData((EPD_HEIGHT - 1) & 0xFF);
    SendData(((EPD_HEIGHT - 1) >> 8) & 0xFF);
    SendData(0x00);                     // GD = 0; SM = 0; TB = 0;
    SendCommand(BOOSTER_SOFT_START_CONTROL);
    SendData(0xD7);
    SendData(0xD6);
    SendData(0x9D);
    SendCommand(WRITE_VCOM_REGISTER);
    SendData(0x9A);                     // VCOM 7C
    SendCommand(SET_DUMMY_LINE_PERIOD);
    SendData(0x1A);                     // 4 dummy lines per gate
    SendCommand(SET_GATE_TIME);
    SendData(0x08);                     // 2us per line
    SendCommand(DATA_ENTRY_MODE_SETTING);
    SendData(0x03);                     // X increment; Y increment
    SetLut(this->lut);
    /* EPD hardware init end */
    return 0;
}

/**
 *  @brief: basic function for sending commands
 */
void EPD_WaveShare154::SendCommand(unsigned char command) {
    DigitalWrite(dc_pin, LOW);
    SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void EPD_WaveShare154::SendData(unsigned char data) {
    DigitalWrite(dc_pin, HIGH);
    SpiTransfer(data);
}

/**
 *  @brief: Wait until the busy_pin goes LOW
 */
void EPD_WaveShare154::WaitUntilIdle(void) {
    Serial.println(F("Waiting for display idle"));
    unsigned long start = micros();
    while (1) {
      if (digitalRead(this->busy_pin) == LOW) {
        break;
      }
      delay(1);
      if (micros() - start > 10000000) {
        Serial.println(F("Busy Timeout!"));
        break;
      }
    }
    Serial.println(F("Display ready"));
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
void EPD_WaveShare154::Reset(void) {
    DigitalWrite(reset_pin, LOW);                //module reset
    DelayMs(200);
    DigitalWrite(reset_pin, HIGH);
    DelayMs(200);
}

/**
 *  @brief: set the look-up table register
 */
void EPD_WaveShare154::SetLut(const unsigned char* lut) {
    this->lut = lut;
    SendCommand(WRITE_LUT_REGISTER);
    /* the length of look-up table is 30 bytes */
    for (int i = 0; i < 30; i++) {
        SendData(this->lut[i]);
    }
}



/**
 *  @brief: update the display
 *          there are 2 memory areas embedded in the e-paper display
 *          but once this function is called,
 *          the the next action of SetFrameMemory or ClearFrame will
 *          set the other memory area.
 */
void EPD_WaveShare154::DisplayFrame(void) {
    SendCommand(DISPLAY_UPDATE_CONTROL_2);
    SendData(0xc4);
    SendCommand(MASTER_ACTIVATION);
    SendCommand(TERMINATE_FRAME_READ_WRITE);
    WaitUntilIdle();
}

/**
 *  @brief: private function to specify the memory area for data R/W
 */
void EPD_WaveShare154::SetMemoryArea(int x_start, int y_start, int x_end, int y_end) {
    SendCommand(SET_RAM_X_ADDRESS_START_END_POSITION);
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    SendData((x_start >> 3) & 0xFF);
    SendData((x_end >> 3) & 0xFF);
    SendCommand(SET_RAM_Y_ADDRESS_START_END_POSITION);
    SendData(y_start & 0xFF);
    SendData((y_start >> 8) & 0xFF);
    SendData(y_end & 0xFF);
    SendData((y_end >> 8) & 0xFF);
}

/**
 *  @brief: private function to specify the start point for data R/W
 */
void EPD_WaveShare154::SetMemoryPointer(int x, int y) {
    SendCommand(SET_RAM_X_ADDRESS_COUNTER);
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    SendData((x >> 3) & 0xFF);
    SendCommand(SET_RAM_Y_ADDRESS_COUNTER);
    SendData(y & 0xFF);
    SendData((y >> 8) & 0xFF);
    WaitUntilIdle();
}

uint8_t EPD_WaveShare154::reverse(uint8_t in)
{
  uint8_t out;
  out = 0;
  if (in & 0x01) out |= 0x80;
  if (in & 0x02) out |= 0x40;
  if (in & 0x04) out |= 0x20;
  if (in & 0x08) out |= 0x10;
  if (in & 0x10) out |= 0x08;
  if (in & 0x20) out |= 0x04;
  if (in & 0x40) out |= 0x02;
  if (in & 0x80) out |= 0x01;

  return(out);
}

/**
 *  @brief: After this command is transmitted, the chip would enter the
 *          deep-sleep mode to save power.
 *          The deep sleep mode would return to standby by hardware reset.
 *          You can use Epd::Init() to awaken
 */
void EPD_WaveShare154::Sleep() {
    digitalWrite(this->cs_pin, LOW);
    digitalWrite(this->reset_pin, LOW);
    digitalWrite(this->dc_pin, LOW);
    SendCommand(DEEP_SLEEP_MODE);
    //WaitUntilIdle();
}

void EPD_WaveShare154::setTemperature(float temperature) {
  int16_t digitalTemperature = temperature * 16;
  
  uint8_t msb = (digitalTemperature >> 8) & 0xFF;
  uint8_t lsb = digitalTemperature & 0xFF;
  SendCommand(TEMPERATURE_SENSOR_CONTROL);
  SendData(msb);
  SendData(lsb);
  /*Serial.println();
  Serial.print(msb, BIN);
  Serial.print(lsb, BIN);
  Serial.println();*/
  WaitUntilIdle();
}

/* END OF FILE */
