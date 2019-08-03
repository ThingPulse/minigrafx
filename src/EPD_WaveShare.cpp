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

#include "EPD_WaveShare.h"

EPD_WaveShare::EPD_WaveShare(EPD_TYPE epdType, uint8_t csPin, uint8_t rstPin, uint8_t dcPin, uint8_t busyPin) : DisplayDriver(getWidth(epdType), getHeight(epdType))  {
  this->csPin = csPin;
  this->rstPin = rstPin;
  this->dcPin = dcPin;
  this->busyPin = busyPin;
  switch(epdType) {
    case EPD1_54:
      delaytime = 1500;
      xDot = 200;
      yDot = 200;
      break;
    case EPD02_13:
      delaytime = 4000;
      xDot = 128;
      yDot = 250;
      break;
    case EPD2_9:
      delaytime = 1500;
      xDot = 128;
      yDot = 296;
      break;
  }
  GDOControl[0] = 0x01;
  GDOControl[1] = (yDot-1)%256;
  GDOControl[2] = (yDot-1)/256;
  GDOControl[3] = 0x00;
}

int EPD_WaveShare::getWidth(EPD_TYPE epdType) {
  switch(epdType) {
    case EPD1_54:
      return 200;
    case EPD02_13:
      return 128;
    case EPD2_9:
      return 128;
  }
  return 0;
}

int EPD_WaveShare::getHeight(EPD_TYPE epdType) {
  switch(epdType) {
    case EPD1_54:
      return 200;
    case EPD02_13:
      return 250;
    case EPD2_9:
      return 296;
  }
  return 0;
}

void EPD_WaveShare::setRotation(uint8_t r) {
  this->rotation = r;
  switch(r) {
    case 0:
      bufferWidth = width();
      bufferHeight = height();
      break;
    case 1:
      bufferWidth = height();
      bufferHeight = width();
      break;
    case 2:
      bufferWidth = width();
      bufferHeight = height();
      break;
    case 3:
      bufferWidth = height();
      bufferHeight = width();
      break;
  }
}
void EPD_WaveShare::init() {
  pinMode(csPin,OUTPUT);
  pinMode(dcPin,OUTPUT);
  pinMode(rstPin,OUTPUT);
  pinMode(busyPin,INPUT);

  Serial.begin(115200);
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  SPI.begin();

  EPD_Init();

}

void EPD_WaveShare::driver_delay_xms(unsigned long xms)
{
	delay(xms);
}

void EPD_WaveShare::SPI_Write(unsigned char value)
{
 SPI.transfer(value);
}

void EPD_WaveShare::writeBuffer(BufferInfo *bufferInfo) {
  Serial.println("Writing buffer");


  unsigned char m;
	//init
	Serial.println("full init");
	EPD_init_Full();
	driver_delay_xms(delaytime);

	//Clear screen
	//Serial.println("full clear\t\n");
	m=0xff;
	EPD_Dis_Full((unsigned char *)&m,0);  //all white
	driver_delay_xms(delaytime);

  uint16_t XSize = xDot;
  uint16_t YSize = yDot;
  EPD_SetRamPointer(0x00,(yDot-1)%256,(yDot-1)/256);	// set ram
  //Serial.println(">>>>>>------start send display data!!---------<<<<<<<");
  int i = 0,j = 0;
	if(XSize % 8 != 0){
		XSize = XSize+(8-XSize%8);
	}
	XSize = XSize/8;

	ReadBusy();
	digitalWrite(this->csPin,LOW);
	digitalWrite (this->dcPin,LOW);		//command write
	SPI_Write(0x24);
	digitalWrite (this->dcPin,HIGH);		//data write
  uint8_t data = 0;
  uint16_t x = 0;
  uint16_t y = 0;
	for(i=0;i<YSize;i++){
		for(j=0;j<XSize;j++){
      // data = buffer[i * XSize + XSize - j];
      data = 0;
      for (int b = 0; b < 8; b++) {
        data = data << 1;
        switch (rotation) {
          case 0:
            x = xDot - (j * 8 + b);
            y = i;
            break;
          case 1:
            x = i;
            y = (j * 8 + b);
            break;
          case 2:
            x = (j * 8 + b);
            y = yDot - i;
            break;
          case 3:
            x = yDot - i;
            y = xDot - (j * 8 + b);
            break;
        }
        data = data | (getPixel(bufferInfo->buffer, x, y) & 1);

      }
			SPI_Write(data);
		}
    yield();
	}
	digitalWrite(this->csPin,HIGH);

  EPD_Update();

}

void EPD_WaveShare::setFastRefresh(boolean isFastRefreshEnabled) {
  // Not enabled at the moment
}

uint8_t EPD_WaveShare::getPixel(uint8_t *buffer, uint16_t x, uint16_t y) {
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

uint8_t EPD_WaveShare::Reverse_bits(uint8_t num){

    int i=7; //size of unsigned char -1, on most machine is 8bits
    uint8_t j=0;
    uint8_t temp=0;

    while(i>=0){
    temp |= ((num>>j)&1)<< i;
    i--;
    j++;
    }
    return(temp);

}

/*******************************************************************************
function：
			read busy
*******************************************************************************/
unsigned char EPD_WaveShare::ReadBusy(void)
{
  unsigned long i=0;
  for(i=0;i<400;i++){
	//	println("isEPD_BUSY = %d\r\n",digitalRead(this->csPin));
      if(digitalRead(this->busyPin)==EPD_BUSY_LEVEL) {
				//Serial.println("Busy is Low \r\n");
      	return 1;
      }
	  driver_delay_xms(10);
  }
  return 0;
}
/*******************************************************************************
function：
		write command
*******************************************************************************/
void EPD_WaveShare::EPD_WriteCMD(unsigned char command)
{
  digitalWrite(this->csPin,LOW);
	digitalWrite (this->dcPin,LOW);		// command write
	SPI_Write(command);
	digitalWrite(this->csPin,HIGH);
}
/*******************************************************************************
function：
		write command and data
*******************************************************************************/
void EPD_WaveShare::EPD_WriteCMD_p1(unsigned char command,unsigned char para)
{
	ReadBusy();
  digitalWrite(this->csPin,LOW);
	digitalWrite (this->dcPin,LOW);		// command write
	SPI_Write(command);
	digitalWrite (this->dcPin,HIGH);		// command write
	SPI_Write(para);
  digitalWrite(this->csPin,HIGH);
}
/*******************************************************************************
function：
		Configure the power supply
*******************************************************************************/
void EPD_WaveShare::EPD_POWERON(void)
{
	EPD_WriteCMD_p1(0x22,0xc0);
	EPD_WriteCMD(0x20);
	//EPD_WriteCMD(0xff);
}

/*******************************************************************************
function：
		The first byte is written with the command value
		Remove the command value,
		the address after a shift,
		the length of less than one byte
*******************************************************************************/
void EPD_WaveShare::EPD_Write(const unsigned char *value, unsigned char datalen)
{
	unsigned char i = 0;
	const unsigned char *ptemp;
	ptemp = value;

	digitalWrite(this->csPin,LOW);
	digitalWrite (this->dcPin,LOW);		// When DC is 0, write command
	SPI_Write(*ptemp);	//The first byte is written with the command value
	ptemp++;
	digitalWrite (this->dcPin,HIGH);		// When DC is 1, write data
	//Serial.println("send data  :");
	for(i= 0;i<datalen-1;i++){	// sub the data
		SPI_Write(*ptemp);
		ptemp++;
	}
	digitalWrite(this->csPin,HIGH);
}
/*******************************************************************************
Function: Write the display buffer
Parameters:
		XSize x the direction of the direction of 128 points, adjusted to an
		integer multiple of 8 times YSize y direction quantity Dispbuff displays
		the data storage location. The data must be arranged in a correct manner
********************************************************************************/
void EPD_WaveShare::EPD_WriteDispRam(unsigned char XSize,unsigned int YSize,
							unsigned char *Dispbuff)
{
	int i = 0,j = 0;
	if(XSize%8 != 0){
		XSize = XSize+(8-XSize%8);
	}
	XSize = XSize/8;

	ReadBusy();
	digitalWrite(this->csPin,LOW);
	digitalWrite (this->dcPin,LOW);		//command write
	SPI_Write(0x24);
	digitalWrite (this->dcPin,HIGH);		//data write
	for(i=0;i<YSize;i++){
		for(j=0;j<XSize;j++){
			SPI_Write(*Dispbuff);
			Dispbuff++;
		}
	}
	digitalWrite(this->csPin,HIGH);
}

/*******************************************************************************
Function: Write the display buffer to write a certain area to the same display.
Parameters: XSize x the direction of the direction of 128 points,adjusted to
			an integer multiple of 8 times YSize y direction quantity  Dispdata
			display data.
********************************************************************************/
void EPD_WaveShare::EPD_WriteDispRamMono(unsigned char XSize,unsigned int YSize,
							unsigned char dispdata)
{
	int i = 0,j = 0;
	if(XSize%8 != 0){
		XSize = XSize+(8-XSize%8);
	}
	XSize = XSize/8;

	ReadBusy();
	digitalWrite(this->csPin,LOW);
	digitalWrite (this->dcPin,LOW);		// command write
	SPI_Write(0x24);
	digitalWrite (this->dcPin,HIGH);		// data write
	for(i=0;i<YSize;i++){
		for(j=0;j<XSize;j++){
		 SPI_Write(dispdata);
		}
	}
	digitalWrite(this->csPin,HIGH);
}

/********************************************************************************
Set RAM X and Y -address Start / End position
********************************************************************************/
void EPD_WaveShare::EPD_SetRamArea(unsigned char Xstart,unsigned char Xend,
						unsigned char Ystart,unsigned char Ystart1,unsigned char Yend,unsigned char Yend1)
{
  unsigned char RamAreaX[3];	// X start and end
	unsigned char RamAreaY[5]; 	// Y start and end
	RamAreaX[0] = 0x44;	// command
	RamAreaX[1] = Xstart;
	RamAreaX[2] = Xend;
	RamAreaY[0] = 0x45;	// command
	RamAreaY[1] = Ystart;
	RamAreaY[2] = Ystart1;
	RamAreaY[3] = Yend;
  RamAreaY[4] = Yend1;
	EPD_Write(RamAreaX, sizeof(RamAreaX));
	EPD_Write(RamAreaY, sizeof(RamAreaY));
}

/********************************************************************************
Set RAM X and Y -address counter
********************************************************************************/
void EPD_WaveShare::EPD_SetRamPointer(unsigned char addrX,unsigned char addrY,unsigned char addrY1)
{
  unsigned char RamPointerX[2];	// default (0,0)
	unsigned char RamPointerY[3];
	//Set RAM X address counter
	RamPointerX[0] = 0x4e;
	RamPointerX[1] = addrX;
	//Set RAM Y address counter
	RamPointerY[0] = 0x4f;
	RamPointerY[1] = addrY;
	RamPointerY[2] = addrY1;

	EPD_Write(RamPointerX, sizeof(RamPointerX));
	EPD_Write(RamPointerY, sizeof(RamPointerY));
}

/********************************************************************************
1.Set RAM X and Y -address Start / End position
2.Set RAM X and Y -address counter
********************************************************************************/
void EPD_WaveShare::EPD_part_display(unsigned char RAM_XST,unsigned char RAM_XEND,unsigned char RAM_YST,unsigned char RAM_YST1,unsigned char RAM_YEND,unsigned char RAM_YEND1)
{
	EPD_SetRamArea(RAM_XST,RAM_XEND,RAM_YST,RAM_YST1,RAM_YEND,RAM_YEND1);  	/*set w h*/
    EPD_SetRamPointer (RAM_XST,RAM_YST,RAM_YST1);		    /*set orginal*/
}

//=========================functions============================
/*******************************************************************************
Initialize the register
********************************************************************************/
void EPD_WaveShare::EPD_Init(void)
{
	//1.reset driver
	digitalWrite(this->rstPin,LOW);		// Module reset
	driver_delay_xms(100);
	digitalWrite(this->rstPin,HIGH);
	driver_delay_xms(100);

	//2. set register
	Serial.println("***********set register Start**********");
	EPD_Write(GDOControl, sizeof(GDOControl));	// Pannel configuration, Gate selection
	EPD_Write(softstart, sizeof(softstart));	// X decrease, Y decrease
	EPD_Write(VCOMVol, sizeof(VCOMVol));		// VCOM setting
	EPD_Write(DummyLine, sizeof(DummyLine));	// dummy line per gate
	EPD_Write(Gatetime, sizeof(Gatetime));		// Gage time setting
	EPD_Write(RamDataEntryMode, sizeof(RamDataEntryMode));	// X increase, Y decrease
	EPD_SetRamArea(0x00,(xDot-1)/8,(yDot-1)%256,(yDot-1)/256,0x00,0x00);	// X-source area,Y-gage area
	EPD_SetRamPointer(0x00,(yDot-1)%256,(yDot-1)/256);	// set ram
	Serial.println("***********set register  end**********");
}

/********************************************************************************
Display data updates
********************************************************************************/
void EPD_WaveShare::EPD_Update(void)
{
	EPD_WriteCMD_p1(0x22,0xc7);
	EPD_WriteCMD(0x20);
	EPD_WriteCMD(0xff);
}
void EPD_WaveShare::EPD_Update_Part(void)
{
	EPD_WriteCMD_p1(0x22,0x04);
	EPD_WriteCMD(0x20);
	EPD_WriteCMD(0xff);
}
/*******************************************************************************
write the waveform to the dirver's ram
********************************************************************************/
void EPD_WaveShare::EPD_WirteLUT(unsigned char *LUTvalue,unsigned char Size)
{
	EPD_Write(LUTvalue, Size);
}

/*******************************************************************************
Full screen initialization
********************************************************************************/
void EPD_WaveShare::EPD_init_Full(void)
{
	EPD_Init();			// Reset and set register
  EPD_WirteLUT((unsigned char *)LUTDefault_full,sizeof(LUTDefault_full));

	EPD_POWERON();
    //driver_delay_xms(100000);
}

/*******************************************************************************
Part screen initialization
********************************************************************************/
void EPD_WaveShare::EPD_init_Part(void)
{
	EPD_Init();			// display
	EPD_WirteLUT((unsigned char *)LUTDefault_part,sizeof(LUTDefault_part));
	EPD_POWERON();
}
/********************************************************************************
parameter:
	Label  :
       		=1 Displays the contents of the DisBuffer
	   		=0 Displays the contents of the first byte in DisBuffer,
********************************************************************************/
void EPD_WaveShare::EPD_Dis_Full(unsigned char *DisBuffer,unsigned char Label)
{
    EPD_SetRamPointer(0x00,(yDot-1)%256,(yDot-1)/256);	// set ram
	//Serial.println(">>>>>>------start send display data!!---------<<<<<<<");
	if(Label == 0){
		EPD_WriteDispRamMono(xDot, yDot, 0xff);	// white
	}else{
		EPD_WriteDispRam(xDot, yDot, (unsigned char *)DisBuffer);	// white
	}
	EPD_Update();

}

/********************************************************************************
parameter:
		xStart :   X direction Start coordinates
		xEnd   :   X direction end coordinates
		yStart :   Y direction Start coordinates
		yEnd   :   Y direction end coordinates
		DisBuffer : Display content
		Label  :
       		=1 Displays the contents of the DisBuffer
	   		=0 Displays the contents of the first byte in DisBuffer,
********************************************************************************/
void EPD_WaveShare::EPD_Dis_Part(unsigned char xStart,unsigned char xEnd,unsigned long yStart,unsigned long yEnd,unsigned char *DisBuffer,unsigned char Label)
{
	//Serial.println(">>>>>>------start send display data!!---------<<<<<<<");
	if(Label==0){// black
		EPD_part_display(xStart/8,xEnd/8,yEnd%256,yEnd/256,yStart%256,yStart/256);
		EPD_WriteDispRamMono(xEnd-xStart, yEnd-yStart+1, DisBuffer[0]);
 		EPD_Update_Part();
		driver_delay_xms(500);
		EPD_part_display(xStart/8,xEnd/8,yEnd%256,yEnd/256,yStart%256,yStart/256);
		EPD_WriteDispRamMono(xEnd-xStart, yEnd-yStart+1, DisBuffer[0]);
	}else{// show
		EPD_part_display(xStart/8,xEnd/8,yEnd%256,yEnd/256,yStart%256,yStart/256);
		EPD_WriteDispRam(xEnd-xStart, yEnd-yStart+1,DisBuffer);
		EPD_Update_Part();
		driver_delay_xms(500);
		EPD_part_display(xStart/8,xEnd/8,yEnd%256,yEnd/256,yStart%256,yStart/256);
		EPD_WriteDispRam(xEnd-xStart, yEnd-yStart+1,DisBuffer);
	}
}

/***********************************************************************************************************************
			------------------------------------------------------------------------
			|\\\																///|
			|\\\						App layer								///|
			------------------------------------------------------------------------
***********************************************************************************************************************/
/********************************************************************************
		clear full screen
********************************************************************************/
void EPD_WaveShare::Dis_Clear_full(void)
{
	unsigned char m;
	//init
	//Serial.println("full init");
	EPD_init_Full();
	driver_delay_xms(delaytime);

	//Clear screen
	//Serial.println("full clear\t\n");
	m=0xff;
	EPD_Dis_Full((unsigned char *)&m,0);  //all white
	driver_delay_xms(delaytime);
}
/********************************************************************************
		clear part screen
********************************************************************************/
/*void EPD_WaveShare::Dis_Clear_part(void)
{
	unsigned char m;
	//init
	EPD_init_Part();
	driver_delay_xms(DELAYTIME);

	//Clear screen
	m=0xff;
	EPD_Dis_Part(0,xDot-1,0,yDot-1,(unsigned char *)&m,0);	 //all white
	driver_delay_xms(DELAYTIME);
}*/





/***********************************************************
						end file
***********************************************************/
