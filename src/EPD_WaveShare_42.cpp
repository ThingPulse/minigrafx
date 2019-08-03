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

See more at https://thingpulse.com

Many thanks go to various contributors such as Adafruit, Waveshare.
*/

#include "EPD_WaveShare_42.h"

EPD_WaveShare42::EPD_WaveShare42(uint8_t csPin, uint8_t rstPin, uint8_t dcPin, uint8_t busyPin) : DisplayDriver(400, 300)  {
  this->csPin = csPin;
  this->rstPin = rstPin;
  this->dcPin = dcPin;
  this->busyPin = busyPin;
  width = 400;
  height = 300;

}

int EPD_WaveShare42::getWidth() {
  return 400;
}

int EPD_WaveShare42::getHeight() {
  return 300;
}

void EPD_WaveShare42::setRotation(uint8_t r) {
  this->rotation = r;
  switch(r) {
    case 0:
      bufferWidth = width;
      bufferHeight = height;
      break;
    case 1:
      bufferWidth = height;
      bufferHeight = width;
      break;
    case 2:
      bufferWidth = width;
      bufferHeight = height;
      break;
    case 3:
      bufferWidth = height;
      bufferHeight = width;
      break;
  }
}

void EPD_WaveShare42::init() {
  /* this calls the peripheral hardware interface, see epdif */
  if (IfInit() != 0) {
      return;
  }
  /* EPD hardware init start */
  Reset();
  SendCommand(BOOSTER_SOFT_START);         //boost soft start
  SendData (0x37);   //A, default 0x17, 3F has max contrast
  SendData (0x37);   //B,
  SendData (0x37);   //C

  SendCommand(POWER_ON);

  SendCommand(PANEL_SETTING);     //panel setting
  SendData(0x1f);    //LUT from OTP
  SendData(0x0d);    //VCOM to 0V

  return;

}

void EPD_WaveShare42::setFastRefresh(boolean isFastRefreshEnabled) {
  this->isFastRefreshEnabled = isFastRefreshEnabled;
}

void EPD_WaveShare42::DigitalWrite(int pin, int value) {
    digitalWrite(pin, value);
}

int EPD_WaveShare42::DigitalRead(int pin) {
    return digitalRead(pin);
}

void EPD_WaveShare42::DelayMs(unsigned int delaytime) {
    delay(delaytime);
}

void EPD_WaveShare42::SpiTransfer(unsigned char data) {
    digitalWrite(this->csPin, LOW);
    SPI.transfer(data);
    digitalWrite(this->csPin, HIGH);
}

int EPD_WaveShare42::IfInit(void) {
    Serial.println("Setting pin modes");
    pinMode(this->csPin, OUTPUT);
    digitalWrite(this->csPin, HIGH);
    pinMode(this->rstPin, OUTPUT);
    //digitalWrite(this->rstPin, HIGH);
    digitalWrite(this->dcPin, HIGH);
    pinMode(this->dcPin, OUTPUT);
    pinMode(this->busyPin, INPUT);
    Serial.println("Starting SPI transaction");
    SPI.begin();
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    Serial.println("Beginning SPI");

    return 0;
}



void EPD_WaveShare42::writeBuffer(BufferInfo *bufferInfo) {
  DisplayFrame(bufferInfo->buffer);
}

/**
 *  @brief: basic function for sending commands
 */
void EPD_WaveShare42::SendCommand(unsigned char command) {
    DigitalWrite(this->dcPin, LOW);
    SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void EPD_WaveShare42::SendData(unsigned char data) {
    DigitalWrite(this->dcPin, HIGH);
    SpiTransfer(data);
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void EPD_WaveShare42::WaitUntilIdle(void) {
    Serial.println("Waiting until idle: ");
    uint32_t startMillis = millis();
    unsigned char busy;
    do {
      SendCommand(GET_STATUS);
      busy = DigitalRead(this->busyPin);
      busy =!(busy & 0x01);
      DelayMs(20);
      if (millis() - startMillis > DISPLAY_TIMEOUT) {
        Serial.println("Display timeout reached");
        return;
      }
    } while (busy);
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
void EPD_WaveShare42::Reset(void) {
    DigitalWrite(this->rstPin, LOW);
    DelayMs(200);
    DigitalWrite(this->rstPin, HIGH);
    DelayMs(200);
}

/**
 *  @brief: transmit partial data to the SRAM
 */
void EPD_WaveShare42::SetPartialWindow(const unsigned char* buffer_black, int x, int y, int w, int l) {
    SendCommand(PARTIAL_IN);
    SendCommand(PARTIAL_WINDOW);
    SendData(x >> 8);
    SendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
    SendData(((x & 0xf8) + w  - 1) >> 8);
    SendData(((x & 0xf8) + w  - 1) | 0x07);
    SendData(y >> 8);
    SendData(y & 0xff);
    SendData((y + l - 1) >> 8);
    SendData((y + l - 1) & 0xff);
    SendData(0x01);         // Gates scan both inside and outside of the partial window. (default)
    DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_2);
    if (buffer_black != NULL) {
        for(int i = 0; i < w  / 8 * l; i++) {
            SendData(reverse(buffer_black[i]));
        }
    } else {
        for(int i = 0; i < w  / 8 * l; i++) {
            SendData(0x00);
        }
    }
    DelayMs(2);
    SendCommand(PARTIAL_OUT);
}

/**
 *  @brief: set the look-up table
 */
void EPD_WaveShare42::SetLut(void) {
    unsigned int count;
    const unsigned char *lut;
    if (isFastRefreshEnabled) {
      lut = lut_vcom0_quick;
    } else {
      lut = lut_vcom0;
    }
    SendCommand(PANEL_SETTING);
    SendData(0x3F); //300x400 B/W mode, LUT set by register
    SendCommand(LUT_FOR_VCOM);                            //vcom
    for(count = 0; count < 44; count++) {
      SendData(lut[count]);

    }
    if (isFastRefreshEnabled) {
      lut = lut_ww_quick;
    } else {
      lut = lut_ww;
    }
    SendCommand(LUT_WHITE_TO_WHITE);                      //ww --
    for(count = 0; count < 42; count++) {
        SendData(lut[count]);
    }
    if (isFastRefreshEnabled) {
      lut = lut_bw_quick;
    } else {
      lut = lut_bw;
    }
    SendCommand(LUT_BLACK_TO_WHITE);                      //bw r
    for(count = 0; count < 42; count++) {
      SendData(lut[count]);
    }
    if (isFastRefreshEnabled) {
      lut = lut_bb_quick;
    } else {
      lut = lut_bb;
    }
    SendCommand(LUT_WHITE_TO_BLACK);                      //wb w
    for(count = 0; count < 42; count++) {
      SendData(lut[count]);
    }
    if (isFastRefreshEnabled) {
      lut = lut_wb_quick;
    } else {
      lut = lut_wb;
    }
    SendCommand(LUT_BLACK_TO_BLACK);                      //bb b
    for(count = 0; count < 42; count++) {
        SendData(lut_wb[count]);
    }
}

/**
 * @brief: refresh and displays the frame
 */
void EPD_WaveShare42::DisplayFrame(const unsigned char* frame_buffer) {
    uint16_t x = 0;
    uint16_t y = 0;
    int x_end;
    int y_end;
    uint16_t image_width = width;
    uint16_t image_height = height;
    uint16_t bufferSize = width * height / 8;
    uint16_t xDot = bufferWidth;
    uint16_t yDot = bufferHeight;
    uint8_t data;

    SendCommand(RESOLUTION_SETTING);
    SendData(width >> 8);
    SendData(width & 0xff);
    SendData(height >> 8);
    SendData(height & 0xff);

    SendCommand(VCM_DC_SETTING);
    SendData(0x12);

    SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendData(0x97);    //VBDF 17|D7 VBDW 97  VBDB 57  VBDF F7  VBDW 77  VBDB 37  VBDR B7

    if (frame_buffer != NULL) {
        SendCommand(DATA_START_TRANSMISSION_1);
        for(int i = 0; i < width / 8 * height; i++) {
            SendData(0xFF);      // bit set: white, bit reset: black
        }
        DelayMs(2);
        /*SendCommand(DATA_START_TRANSMISSION_2);
        for(int i = 0; i < width / 8 * height; i++) {
            SendData(reverse(frame_buffer[i]));
        }
        DelayMs(2);*/
        SendCommand(DATA_START_TRANSMISSION_2);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < (width) / 8; j++) {

                data = 0;
                for (int b = 0; b < 8; b++) {
                  data = data << 1;
                  switch (rotation) {
                    case 0:
                      x = (j * 8 + b);
                      y = i;
                      break;
                    case 1:
                      x = bufferWidth - i - 1;
                      y = (j * 8 + b);
                      break;
                    case 2:
                      x = xDot - (j * 8 + b) - 1;
                      y = yDot - i - 1;
                      break;
                    case 3:
                      x = i;
                      y = bufferHeight - (j * 8 + b) - 1;
                      break;
                  }
                  data = data | (getPixel(frame_buffer, x, y) & 1);

                }
                SendData(data);
                //SendData(reverse(buffer[(i + j * (image_width / 8))]));
                //SendData(i);
                yield();
            }
        }
    }

    SetLut();

    SendCommand(DISPLAY_REFRESH);
    DelayMs(100);
    WaitUntilIdle();
}

uint8_t EPD_WaveShare42::getPixel(const unsigned char *buffer, uint16_t x, uint16_t y) {
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

uint8_t EPD_WaveShare42::reverse(uint8_t in)
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
 * @brief: clear the frame data from the SRAM, this won't refresh the display
 */
void EPD_WaveShare42::ClearFrame(void) {
    SendCommand(RESOLUTION_SETTING);
    SendData(width >> 8);
    SendData(width & 0xff);
    SendData(height >> 8);
    SendData(height & 0xff);

    SendCommand(DATA_START_TRANSMISSION_1);
    DelayMs(2);
    for(int i = 0; i < width / 8 * height; i++) {
        SendData(0xFF);
    }
    DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_2);
    DelayMs(2);
    for(int i = 0; i < width / 8 * height; i++) {
        SendData(0xFF);
    }
    DelayMs(2);
}

/**
 * @brief: This displays the frame data from SRAM
 */
void EPD_WaveShare42::DisplayFrame(void) {
    SetLut();
    SendCommand(DISPLAY_REFRESH);
    DelayMs(100);
    WaitUntilIdle();
}

/**
 * @brief: After this command is transmitted, the chip would enter the deep-sleep mode to save power.
 *         The deep sleep mode would return to standby by hardware reset. The only one parameter is a
 *         check code, the command would be executed if check code = 0xA5.
 *         You can use Epd::Reset() to awaken and use Epd::Init() to initialize.
 */
void EPD_WaveShare42::Sleep() {
    /*SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendData(0x17);                       //border floating
    SendCommand(VCM_DC_SETTING);          //VCOM to 0V
    SendCommand(PANEL_SETTING);
    DelayMs(100);

    SendCommand(POWER_SETTING);           //VG&VS to 0V fast
    SendData(0x00);
    SendData(0x00);
    SendData(0x00);
    SendData(0x00);
    SendData(0x00);
    DelayMs(100);*/

    SendCommand(POWER_OFF);          //power off
    WaitUntilIdle();
    SendCommand(DEEP_SLEEP);         //deep sleep
    SendData(0xA5);
}
