/**
The MIT License (MIT)
Copyright (c) 2017 by Daniel Eichhorn
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

Please note: I am spending a lot of my free time in developing Software and Hardware
for these projects. Please consider supporting me by
a) Buying my hardware kits from https://blog.squix.org/shop
b) Send a donation: https://www.paypal.me/squix/5USD
c) Or using this affiliate link while shopping: https://www.banggood.com/?p=6R31122484684201508S

See more at https://blog.squix.org

Demo for the buffered graphics library. Renders a 3D cube
*/

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


// Used for fps measuring
uint16_t counter = 0;
long startMillis = millis();
uint16_t interval = 20;

// size / 2 of cube edge
float d = 15;
float px[] = {
  -d,  d,  d, -d, -d,  d,  d, -d };
float py[] = {
  -d, -d,  d,  d, -d, -d,  d,  d };
float pz[] = {
  -d, -d, -d, -d,  d,  d,  d,  d };

// define the triangles
// The order of the vertices MUST be CCW or the
// shoelace method won't work to detect visible edges
int  faces[12][3] = {
    {0,1,4},
    {1,5,4},
    {1,2,5},
    {2,6,5},
    {5,7,4},
    {6,7,5},
    {3,4,7},
    {4,3,0},
    {0,3,1},
    {1,3,2},
    {2,3,6},
    {6,3,7}
  };

// mapped coordinates on screen
float p2x[] = {
  0,0,0,0,0,0,0,0};
float p2y[] = {
  0,0,0,0,0,0,0,0};

// rotation angle in radians
float r[] = {
  0,0,0};

#define SHAPE_SIZE 600
// Define how fast the cube rotates. Smaller numbers are faster.
// This is the number of ms between draws.
#define ROTATION_SPEED 0
String fps = "0fps";

void setup() {
  Serial.begin(115200);

  // Turn on the background LED
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  gfx.init();
  gfx.fillBuffer(0);
  gfx.commit();


  startMillis = millis();
}



/**
* Detected visible triangles. If calculated area > 0 the triangle
* is rendered facing towards the viewer, since the vertices are CCW.
* If the area is negative the triangle is CW and thus facing away from us.
*/
int shoelace(int x1, int y1, int x2, int y2, int x3, int y3) {
  // (x1y2 - y1x2) + (x2y3 - y2x3)
  return x1 * y2 - y1 * x2 + x2*y3 - y2*x3 + x3*y1 - y3*x1;
}

/**
* Rotates and renders the cube.
**/
void drawCube()
{
  double speed = 90;
  r[0]=r[0]+PI/speed; // Add a degree
  r[1]=r[1]+PI/speed; // Add a degree
  r[2]=r[2]+PI/speed; // Add a degree
  if (r[0] >= 360.0*PI/90.0) r[0] = 0;
  if (r[1] >= 360.0*PI/90.0) r[1] = 0;
  if (r[2] >= 360.0*PI/90.0) r[2] = 0;

  float ax[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  float ay[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  float az[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  // Calculate all vertices of the cube
  for (int i=0;i<8;i++)
  {
    float px2 = px[i];
    float py2 = cos(r[0])*py[i] - sin(r[0])*pz[i];
    float pz2 = sin(r[0])*py[i] + cos(r[0])*pz[i];

    float px3 = cos(r[1])*px2 + sin(r[1])*pz2;
    float py3 = py2;
    float pz3 = -sin(r[1])*px2 + cos(r[1])*pz2;

    ax[i] = cos(r[2])*px3 - sin(r[2])*py3;
    ay[i] = sin(r[2])*px3 + cos(r[2])*py3;
    az[i] = pz3-150;

    p2x[i] = SCREEN_WIDTH/2+ax[i]*SHAPE_SIZE/az[i];
    p2y[i] = SCREEN_HEIGHT/2+ay[i]*SHAPE_SIZE/az[i];
  }

  // Fill the buffer with color 0 (Black)
  gfx.fillBuffer(0);

  for (int i = 0; i < 12; i++) {

    if (shoelace(p2x[faces[i][0]],p2y[faces[i][0]],p2x[faces[i][1]],p2y[faces[i][1]],p2x[faces[i][2]],p2y[faces[i][2]]) > 0) {
      gfx.setColor((i / 2) + 1);
      gfx.fillTriangle(p2x[faces[i][0]],p2y[faces[i][0]],p2x[faces[i][1]],p2y[faces[i][1]],p2x[faces[i][2]],p2y[faces[i][2]]);
      if (i % 2) {
        int avX = 0;
        int avY = 0;
        for (int v = 0; v < 3; v++) {
          avX += p2x[faces[i][v]];
          avY += p2y[faces[i][v]];
        }
        avX = avX / 3;
        avY = avY / 3;
      }
    }
  }
  gfx.setColor(1);
  gfx.drawString(2, 2, fps);
  gfx.commit();
}

void loop() {

  drawCube();

  counter++;
  // only calculate the fps every <interval> iterations.
  if (counter % interval == 0) {
    long millisSinceUpdate = millis() - startMillis;
    fps = String(interval * 1000.0 / (millisSinceUpdate)) + "fps";
    startMillis = millis();
  }
}
