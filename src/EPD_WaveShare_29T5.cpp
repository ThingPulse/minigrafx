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

#include <stdlib.h>
#include "EPD_WaveShare_29T5.h"

EPD_WaveShare29T5::~EPD_WaveShare29T5() {
};

EPD_WaveShare29T5::EPD_WaveShare29T5(uint8_t csPin, uint8_t rstPin, uint8_t dcPin, uint8_t busyPin) : DisplayDriver(EPD_WIDTH, EPD_HEIGHT) {
    this->reset_pin = rstPin;
    this->dc_pin = dcPin;
    this->cs_pin = csPin;
    this->busy_pin = busyPin;
    width = EPD_WIDTH;
    height = EPD_HEIGHT;
};

void EPD_WaveShare29T5::setRotation(uint8_t r) {
  this->rotation = r;
}

void EPD_WaveShare29T5::setFastRefresh(boolean isFastRefreshEnabled) {
  this->isFastRefreshEnabled = isFastRefreshEnabled;
}

// uint8_t *buffer, uint8_t bitsPerPixel, uint16_t *palette, uint16_t xPosOrig, uint16_t yPosOrig, uint16_t bufferWidth, uint16_t bufferHeight
void EPD_WaveShare29T5::writeBuffer(BufferInfo *bufferInfo) {
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
  SetLut();

  SetMemoryArea(xPos, yPos, xPos + targetWidth - 1, yPos + targetHeight - 1);



  if (isFastRefreshEnabled) {
    SendCommand(EPD_PARTIAL_IN);		//This command makes the display enter partial mode
  } else {
    SendCommand(EPD_DISPLAY_START_TRANSMISSION_1);
    for (int i = 0; i < targetHeight; i++) {
        for (int j = 0; j < (targetWidth) / 8; j++) {
          // Clear screen
          SendData(0xFF);
        }
    }
  }
  // This command is not documented
  SendCommand(EPD_DISPLAY_START_TRANSMISSION_2);
  
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
    if (isFastRefreshEnabled) {
      SendCommand(EPD_PARTIAL_OUT);
    }


}

uint8_t EPD_WaveShare29T5::getPixel(uint8_t *buffer, uint16_t x, uint16_t y, uint16_t bufferWidth, uint16_t bufferHeight) {
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

int EPD_WaveShare29T5::IfInit(void) {
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


void EPD_WaveShare29T5::DigitalWrite(int pin, int value) {
    digitalWrite(pin, value);
}

int EPD_WaveShare29T5::DigitalRead(int pin) {
    return digitalRead(pin);
}

void EPD_WaveShare29T5::DelayMs(unsigned int delaytime) {
    delay(delaytime);
}

void EPD_WaveShare29T5::SpiTransfer(unsigned char data) {
    digitalWrite(this->cs_pin, LOW);
    SPI.transfer(data);
    digitalWrite(this->cs_pin, HIGH);
}

void EPD_WaveShare29T5::init() {
    /* this calls the peripheral hardware interface, see epdif */
    if (IfInit() != 0) {
        return;
    }

    /* EPD hardware init start */
    boolean isGrey = false;
    if (isGrey) {
      Reset();
      unsigned char  HRES=0x80;						//128
      unsigned char  VRES_byte1=0x01;				//296
      unsigned char VRES_byte2=0x28;

      SendCommand(EPD_POWER_SETTING);			//POWER SETTING
      SendData (0x03);
      SendData (0x00);      
      SendData (0x2b);																	 
      SendData (0x2b);		
      SendData (0x13);

      SendCommand(EPD_BOOSTER_SOFT_START);         //booster soft start
      SendData (0x17);		//A
      SendData (0x17);		//B
      SendData (0x17);		//C 
      
      SendCommand(EPD_POWER_ON);
      WaitUntilIdle();
      
      SendCommand(EPD_PANEL_SETTING);			//panel setting
      SendData(0x3f);		//KW-3f   KWR-2F	BWROTP 0f	BWOTP 1f
      
      SendCommand(EPD_PLL_SETTING);			//PLL setting
      SendData (0x3c);      	//100hz 
      
      SendCommand(EPD_RESOLUTION_SETTING);			//resolution setting
      SendData (HRES);        	 
      SendData (VRES_byte1);		
      SendData (VRES_byte2);

      SendCommand(EPD_VCOM_DC_SETTING);			//vcom_DC setting
      SendData (0x12);

      SendCommand(EPD_VCOM_DATA_INTERNAL_SETTING);			//VCOM AND DATA INTERVAL SETTING			
      SendData(0x97);
      SetLut();
    } else {

      unsigned char  HRES=0x80;						//128
      unsigned char  VRES_byte1=0x01;				//296
      unsigned char VRES_byte2=0x28;
    
      Reset();
    
      SendCommand(EPD_BOOSTER_SOFT_START);         //boost soft start
      SendData (0x17);		//A
      SendData (0x17);		//B
      SendData (0x17);		//C       

      SendCommand(EPD_POWER_ON);  
      WaitUntilIdle();

      SendCommand(EPD_PANEL_SETTING);			//panel setting
      SendData(0xbf);		

      SendCommand(EPD_RESOLUTION_SETTING);			//resolution setting
      SendData (HRES);        	 
      SendData (VRES_byte1);		
      SendData (VRES_byte2);

      SendCommand(EPD_VCOM_DATA_INTERNAL_SETTING);			//VCOM AND DATA INTERVAL SETTING			
      SendData(0x97);		//WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7

      SetLut();
    }
    /* EPD hardware init end */
    return;
}

/**
 *  @brief: basic function for sending commands
 */
void EPD_WaveShare29T5::SendCommand(unsigned char command) {
    DigitalWrite(dc_pin, LOW);
    SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void EPD_WaveShare29T5::SendData(unsigned char data) {
    DigitalWrite(dc_pin, HIGH);
    SpiTransfer(data);
}

/**
 *  @brief: Wait until the busy_pin goes LOW
 */
void EPD_WaveShare29T5::WaitUntilIdle(void) {
    Serial.println(F("Waiting for display idle"));
    unsigned long start = micros();
    while (1) {
      //SendCommand(0x71);
      delay(1);
      if (digitalRead(this->busy_pin) == HIGH) {
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
void EPD_WaveShare29T5::Reset(void) {
    DigitalWrite(reset_pin, LOW);                //module reset
    DelayMs(10);
    DigitalWrite(reset_pin, HIGH);
    DelayMs(10);
}

/**
 *  @brief: set the look-up table register
 */
void EPD_WaveShare29T5::SetLut() {
    unsigned int count;	 

    const unsigned char** lut;
    if (isFastRefreshEnabled) {
      Serial.println("Using fast refresh lut");
      lut = lut_partial;
      SendCommand(EPD_VCOM_DC_SETTING);     //vcom_DC setting
      SendData (0x08);
      SendCommand(EPD_VCOM_DATA_INTERNAL_SETTING);
      SendData(0x17);
    } else {
      Serial.println("Using full refresh lut");
      lut = lut_full;
      SendCommand(EPD_VCOM_DC_SETTING); //vcom_DC setting
      SendData (0x08);
      SendCommand(EPD_VCOM_DATA_INTERNAL_SETTING); //VCOM AND DATA INTERVAL SETTING
      SendData(0x97);    //WBmode:VBDF 17|D7 VBDW 97 VBDB 57   WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7

    }
	
		SendCommand(EPD_SET_LUT_VCOM);							//vcom
		for(count=0;count<44;count++) {
      SendData(lut[0][count]);
    }
		
	  SendCommand(EPD_SET_LUT_WW);							//red not use
	  for(count=0;count<42;count++) {
      SendData(lut[1][count]);
    }

		SendCommand(EPD_SET_LUT_BW);							//bw r
		for(count=0;count<42;count++) {
      SendData(lut[2][count]);
    }

		SendCommand(EPD_SET_LUT_WB);							//wb w
		for(count=0;count<42;count++) {
      SendData(lut[3][count]);
    }

		SendCommand(EPD_SET_LUT_BB);							//bb b
		for(count=0;count<42;count++) {
      SendData(lut[4][count]);
    }

		/*SendCommand(0x25);							//vcom
		for(count=0;count<42;count++) {
      //SendData(lut_ww[count]);
      SendData(lut[0][count]);
    }*/
	
}



/**
 *  @brief: update the display
 *          there are 2 memory areas embedded in the e-paper display
 *          but once this function is called,
 *          the the next action of SetFrameMemory or ClearFrame will
 *          set the other memory area.
 */
void EPD_WaveShare29T5::DisplayFrame(void) {
    SendCommand(EPD_REFRESH_FRAME);
    delay(100);
    WaitUntilIdle();
}

/**
 *  @brief: private function to specify the memory area for data R/W
 */
void EPD_WaveShare29T5::SetMemoryArea(int x, int y, int xe, int ye) {
  SendCommand(EPD_PARTIAL_RESOLUTION_SETTING);		//resolution setting
  SendData (x);   //x-start     
  SendData (xe - 1);	 //x-end	

  SendData (y/256);
  SendData (y%256);   //y-start    
    
  SendData ((ye)/256);		
  SendData ((ye)%256-1);  //y-end
  SendData (0x28);	
}

uint8_t EPD_WaveShare29T5::reverse(uint8_t in)
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
void EPD_WaveShare29T5::Sleep() {
    SendCommand(EPD_POWER_OFF);
    WaitUntilIdle();
    SendCommand(EPD_DEEPL_SLEEP);
    SendData(0xA5);
}

//0~3 gray
const unsigned char EPD_WaveShare29T5::lut_20_vcomDC_2bit[] =
{
0x00	,0x0A	,0x00	,0x00	,0x00	,0x01,
0x60	,0x14	,0x14	,0x00	,0x00	,0x01,
0x00	,0x14	,0x00	,0x00	,0x00	,0x01,
0x00	,0x13	,0x0A	,0x01	,0x00	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00
				
};
//R21
const unsigned char EPD_WaveShare29T5::lut_21_ww_2bit[] ={
0x40	,0x0A	,0x00	,0x00	,0x00	,0x01,
0x90	,0x14	,0x14	,0x00	,0x00	,0x01,
0x10	,0x14	,0x0A	,0x00	,0x00	,0x01,
0xA0	,0x13	,0x01	,0x00	,0x00	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};
//R22H	r
const unsigned char EPD_WaveShare29T5::lut_22_bw_2bit[] ={
0x40	,0x0A	,0x00	,0x00	,0x00	,0x01,
0x90	,0x14	,0x14	,0x00	,0x00	,0x01,
0x00	,0x14	,0x0A	,0x00	,0x00	,0x01,
0x99	,0x0C	,0x01	,0x03	,0x04	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};
//R23H	w
const unsigned char EPD_WaveShare29T5::lut_23_wb_2bit[] ={
0x40	,0x0A	,0x00	,0x00	,0x00	,0x01,
0x90	,0x14	,0x14	,0x00	,0x00	,0x01,
0x00	,0x14	,0x0A	,0x00	,0x00	,0x01,
0x99	,0x0B	,0x04	,0x04	,0x01	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};
//R24H	b
const unsigned char EPD_WaveShare29T5::lut_24_bb_2bit[] ={
0x80	,0x0A	,0x00	,0x00	,0x00	,0x01,
0x90	,0x14	,0x14	,0x00	,0x00	,0x01,
0x20	,0x14	,0x0A	,0x00	,0x00	,0x01,
0x50	,0x13	,0x01	,0x00	,0x00	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};

const unsigned char* EPD_WaveShare29T5::lut_2bit[] = {
  EPD_WaveShare29T5::lut_20_vcomDC_2bit,
  EPD_WaveShare29T5::lut_21_ww_2bit,
  EPD_WaveShare29T5::lut_22_bw_2bit,
  EPD_WaveShare29T5::lut_23_wb_2bit,
  EPD_WaveShare29T5::lut_24_bb_2bit
};

//full screen update LUT
const unsigned char EPD_WaveShare29T5::lut_20_vcomDC[] =
{
  0x00, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x60, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00,
};

const unsigned char EPD_WaveShare29T5::lut_21_ww[] =
{
  0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
  0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char EPD_WaveShare29T5::lut_22_bw[] =
{
  0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
  0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char EPD_WaveShare29T5::lut_23_wb[] =
{
  0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char EPD_WaveShare29T5::lut_24_bb[] =
{
  0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char* EPD_WaveShare29T5::lut_full[] = {
  EPD_WaveShare29T5::lut_20_vcomDC,
  EPD_WaveShare29T5::lut_21_ww,
  EPD_WaveShare29T5::lut_22_bw,
  EPD_WaveShare29T5::lut_23_wb,
  EPD_WaveShare29T5::lut_24_bb
};

#define Tx19 0x20   // new value for test is 32 (phase length)
const unsigned char EPD_WaveShare29T5::lut_20_vcomDC_partial[] =
{
0x00	,0x19	,0x01	,0x00	,0x00	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00
	,0x00	,0x00,
};

const unsigned char EPD_WaveShare29T5::lut_21_ww_partial[] =
{
0x00	,0x19	,0x01	,0x00	,0x00	,0x01,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};

const unsigned char EPD_WaveShare29T5::lut_22_bw_partial[] =
{
0x80	,0x19	,0x01	,0x00	,0x00	,0x01,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};

const unsigned char EPD_WaveShare29T5::lut_23_wb_partial[] =
{
0x40	,0x19	,0x01	,0x00	,0x00	,0x01,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};

const unsigned char EPD_WaveShare29T5::lut_24_bb_partial[] =
{
0x00	,0x19	,0x01	,0x00	,0x00	,0x01,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,	
0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
};

const unsigned char* EPD_WaveShare29T5::lut_partial[] = {
  EPD_WaveShare29T5::lut_20_vcomDC_partial,
  EPD_WaveShare29T5::lut_21_ww_partial,
  EPD_WaveShare29T5::lut_22_bw_partial,
  EPD_WaveShare29T5::lut_23_wb_partial,
  EPD_WaveShare29T5::lut_24_bb_partial
};


/* END OF FILE */
