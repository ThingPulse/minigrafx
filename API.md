# MiniGrafx API

This chapter explains the available methods of the graphics library.

## Constructor

The constructor creates the graphic library and allocates the frame buffer memory.
Parameters:
* DisplayDriver: the hardware driver for your library. See [How to implement a new driver](Driver.md)
* bitsPerPixel: bit depth, currently supported are 1, 2, 4 and 8
* palette: array with length 2^bitsPerPixel containing the mapping from palette color to display color

```C++
MiniGrafx(DisplayDriver *driver, uint8_t bitsPerPixel, uint16_t *palette);
```

Example:
```C++
#define BITS_PER_PIXEL 1
uint16_t palette[] = {ILI9341_BLACK, // 0
                      ILI9341_WHITE};

ILI9341_SPI tft = ILI9341_SPI(TFT_CS, TFT_DC);
MiniGrafx gfx = MiniGrafx(&tft, BITS_PER_PIXEL, palette);
```

## init

With the init function you initialize the driver as well as the graphics library. Usually you only do this
once in the setup method:
```C++
gfx.init();
```

## commit

The commit() method calls the driver to write the content of the frame buffer to the display. Without calling
commit() the content of the frame buffer will not become visible. You usually do this at the end of all the drawing in one loop iteration:

```C++
gfx.commit();
```

## fillBuffer and clear()

With clear() and fillBuffer() you fill the frame buffer with one value; you erase it. The only difference between
the two is that clear() fills the frame buffer with the palette index 0, whereas with fillBuffer(uint8_t) you
can define which color should be used to fill the buffer.

```C++
void clear();
void fillBuffer(uint8_t pal);
```
