# Introduction

First you have to include the basic graphic routines and the header file matching your display:

```C++
#include <SPI.h>
#include "MiniGrafx.h" // General graphic library
#include "ILI9341_SPI.h" // Hardware-specific library
```
The MiniGrafx.h is your API containing all important drawing routines. The ILI9341.h contains the driver
for ILI9341 based displays. This display is available in the Squix Color display starter kit:
https://blog.squix.org/product/esp8266-wifi-color-display-kit-2-4

Now we will define the pins of the display which can be freely chosen:

```C++
#define TFT_DC D2
#define TFT_CS D1
#define TFT_LED D8
```
D2 and D1 are used for the SPI protocol. D8 controls the background light of the display.

Now it get to the interesting part, the definition of the palette array. In this example we will use a 16 color palette which means that our frame buffer will only know colors represented by the values from 0 to 15. Before writing the values to the display the driver will turn these values back into the colors of the display and a pixel represented by only 4 bit suddenly will be represented by 16 (in case of the ILI9341 display). Why would we intentionally reduce the amount of available colors from 65k (2^16) to only 16? Because our embedded device might not have enough memory to keep the hole screen in the buffer.

Or let me explain this in other words. Imagine you are a painter in front of a canvas. The palette you hold in
your hands only has space for 16 different colors. While the canvas technically can take much more colors you
decided to reduce complexity and use these 16 colors without mixing them. Got it?

The values in the arrays are the aliases for the colors you can use. Internally they represent a 565 coded color: 5bit for red, 6bit for green and 5bit for blue. These are the color codes which the driver for the ILI9341 understand. The numbers on each line are the aliases for these 565 colors. By calling gfx.setColor(5)
you will in fact draw ILI9341_MAROON color to the display which is the same as RGB(128, 0, 0).


```C++
uint16_t palette[] = {ILI9341_BLACK,     //  0
                      ILI9341_WHITE,     //  1
                      ILI9341_NAVY,      //  2
                      ILI9341_DARKCYAN,  //  3
                      ILI9341_DARKGREEN, //  4
                      ILI9341_MAROON,    //  5
                      ILI9341_PURPLE,    //  6
                      ILI9341_OLIVE,     //  7
                      ILI9341_LIGHTGREY, //  8
                      ILI9341_DARKGREY,  //  9
                      ILI9341_BLUE,      // 10
                      ILI9341_GREEN,     // 11
                      ILI9341_CYAN,      // 12
                      ILI9341_RED,       // 13
                      ILI9341_MAGENTA,   // 14
                      0xFD80};           // 15
```

Now it's time to initialize the graphics library and the driver:

```C++
int BITS_PER_PIXEL = 4 ; // 2^4 = 16 colors

// Initialize the driver
ILI9341_SPI tft = ILI9341_SPI(TFT_CS, TFT_DC);
MiniGrafx gfx = MiniGrafx(&tft, BITS_PER_PIXEL, palette);
```

The MiniGrafx is very flexible when it comes to hardware support. When initializing the graphics library
you define how many bits should be internally be used to represent each pixel.
 * 1bit:  2 colors (2^1 =   2)
 * 2bit:  4 colors (2^2 =   4)
 * 4bit: 16 colors (2^4 =  16)
 * 8bit 256 colors (2^8 = 256)

 Please bear in mind that with more bits per pixel also the memory (RAM/heap) consumption grows rapidly. Assuming your display has 240x320 pixels:
 *  1bit:  9600 bytes
 *  2bit: 19200 bytes
 *  4bit: 32400 bytes
 *  8bit: 64800 bytes

 So check first how much memory your embedded device offers!

 In the setup() method we make sure that the background light will be turned on:

```C++
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);
```

and we initialize the graphic library with gfx.init(). This will also call the driver's init method
where the driver can execute methods which should only be called once. With gfx.fillBuffer(0) we clear
the frame buffer and with gfx.commit() we write the content of the frame buffer to the display. This means
that you usually redraw everything on the screen with every iteration. This is a best practice but there
are good reasons why you don't want to do this.

```C++
  gfx.init();
  gfx.fillBuffer(0);
  gfx.commit();
```

Now let's do some drawing in the loop method. Clear the buffer by calling gfx.fillBuffer(0). What would
happen if you'd call gfx.fillBuffer(1)? This would draw the second color from the palette to the buffer
which is ILI9341_WHITE.

After filling the buffer with black pixels we set the color to white and draw a line from (0,0) to
(20,20). Then we go on and draw a filled circle at the position (20, 20) with radius 5.

```C++
void loop() {
  gfx.fillBuffer(0);
  gfx.setColor(1);
  gfx.drawLine(0, 0, 20, 20);
  gfx.setColor(13);
  gfx.fillCircle(20, 20, 5);
  gfx.commit();
}
```


And here everything together:
```C++
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
int BITS_PER_PIXEL = 4 ; // 2^4 = 16 colors

// Initialize the driver
ILI9341_SPI tft = ILI9341_SPI(TFT_CS, TFT_DC);
MiniGrafx gfx = MiniGrafx(&tft, BITS_PER_PIXEL, palette);

void setup() {
  Serial.begin(115200);

  // Turn on the background LED
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  // Initialize the driver only once
  gfx.init();
  // fill the buffer with black
  gfx.fillBuffer(0);
  // write the buffer to the display
  gfx.commit();
}


void loop() {
  gfx.fillBuffer(0);
  gfx.setColor(1);
  gfx.drawLine(0, 0, 20, 20);
  gfx.setColor(13);
  gfx.fillCircle(20, 20, 5);
  gfx.commit();
}
```
