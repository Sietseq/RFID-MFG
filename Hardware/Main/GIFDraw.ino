// =========================== GIF & TFT LIbrariers ========================================
#define DISPLAY_WIDTH  tft.width()
#define DISPLAY_HEIGHT tft.height()
#define BUFFER_SIZE 256            // Optimum is >= GIF width or integral division of width

#ifdef USE_DMA
  uint16_t usTemp[2][BUFFER_SIZE]; // Global to support DMA use
#else
  uint16_t usTemp[1][BUFFER_SIZE];    // Global to support DMA use
#endif
bool     dmaBuf = 0;

#define NORMAL_SPEED  // Comment out for rame rate for render speed test

// Load GIF library
AnimatedGIF gif;

// 
#include "success.h"
#include "fail.h"
#include "progress.h"
#define GIF_IMAGE_SUCESS success   //  No DMA  90 fps, DMA:  78 fps
#define GIF_IMAGE_FAIL fail   //  No DMA  90 fps, DMA:  78 fps
#define GIF_IMAGE_LOADING progress   //  No DMA  90 fps, DMA:  78 fps

#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// ==================== END OF TFT DISPLAY LIBRARIES ===========================

// ==================== FAST LED LIBRARIES ====================================

#include "FastLED.h"                                          // FastLED library.
 
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif
 
// Fixed definitions cannot change on the fly.
#define LED_DT 33                                             // Serial data pin
#define COLOR_ORDER GRB                                       // It's GRB for WS2812B and GBR for APA102
#define LED_TYPE WS2812                                       // What kind of strip are you using (APA102, WS2801 or WS2812B)?
#define NUM_LEDS 8                                       // Number of LED's

// Initialize changeable global variables.
uint8_t max_bright = 125;                                     // Overall brightness definition. It can be changed on the fly.
 
struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array.

// ============== END OF FAST LED LIBRARIES =====================================
 
  
// ====================== DRAWS THE GIF ========================================

// Draw a line of image directly on the LCD
void GIFDraw(GIFDRAW *pDraw)
{
  uint8_t *s;
  uint16_t *d, *usPalette;
  int x, y, iWidth, iCount;

  // Display bounds check and cropping
  iWidth = pDraw->iWidth;
  if (iWidth + pDraw->iX > DISPLAY_WIDTH)
    iWidth = DISPLAY_WIDTH - pDraw->iX;
  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y; // current line
  if (y >= DISPLAY_HEIGHT || pDraw->iX >= DISPLAY_WIDTH || iWidth < 1)
    return;

  // Old image disposal
  s = pDraw->pPixels;
  if (pDraw->ucDisposalMethod == 2) // restore to background color
  {
    for (x = 0; x < iWidth; x++)
    {
      if (s[x] == pDraw->ucTransparent)
        s[x] = pDraw->ucBackground;
    }
    pDraw->ucHasTransparency = 0;
  }

  // Apply the new pixels to the main image
  if (pDraw->ucHasTransparency) // if transparency used
  {
    uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
    pEnd = s + iWidth;
    x = 0;
    iCount = 0; // count non-transparent pixels
    while (x < iWidth)
    {
      c = ucTransparent - 1;
      d = &usTemp[0][0];
      while (c != ucTransparent && s < pEnd && iCount < BUFFER_SIZE )
      {
        c = *s++;
        if (c == ucTransparent) // done, stop
        {
          s--; // back up to treat it like transparent
        }
        else // opaque
        {
          *d++ = usPalette[c];
          iCount++;
        }
      } // while looking for opaque pixels
      if (iCount) // any opaque pixels?
      {
        // DMA would degrtade performance here due to short line segments
        tft.setAddrWindow(pDraw->iX + x, y, iCount, 1);
        tft.pushPixels(usTemp, iCount);
        x += iCount;
        iCount = 0;
      }
      // no, look for a run of transparent pixels
      c = ucTransparent;
      while (c == ucTransparent && s < pEnd)
      {
        c = *s++;
        if (c == ucTransparent)
          x++;
        else
          s--;
      }
    }
  }
  else
  {
    s = pDraw->pPixels;

    // Unroll the first pass to boost DMA performance
    // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
    if (iWidth <= BUFFER_SIZE)
      for (iCount = 0; iCount < iWidth; iCount++) usTemp[dmaBuf][iCount] = usPalette[*s++];
    else
      for (iCount = 0; iCount < BUFFER_SIZE; iCount++) usTemp[dmaBuf][iCount] = usPalette[*s++];

#ifdef USE_DMA // 71.6 fps (ST7796 84.5 fps)
    tft.dmaWait();
    tft.setAddrWindow(pDraw->iX, y, iWidth, 1);
    tft.pushPixelsDMA(&usTemp[dmaBuf][0], iCount);
    dmaBuf = !dmaBuf;
#else // 57.0 fps
    tft.setAddrWindow(pDraw->iX, y, iWidth, 1);
    tft.pushPixels(&usTemp[0][0], iCount);
#endif

    iWidth -= iCount;
    // Loop if pixel buffer smaller than width
    while (iWidth > 0)
    {
      // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
      if (iWidth <= BUFFER_SIZE)
        for (iCount = 0; iCount < iWidth; iCount++) usTemp[dmaBuf][iCount] = usPalette[*s++];
      else
        for (iCount = 0; iCount < BUFFER_SIZE; iCount++) usTemp[dmaBuf][iCount] = usPalette[*s++];

#ifdef USE_DMA
      tft.dmaWait();
      tft.pushPixelsDMA(&usTemp[dmaBuf][0], iCount);
      dmaBuf = !dmaBuf;
#else
      tft.pushPixels(&usTemp[0][0], iCount);
#endif
      iWidth -= iCount;
    }
  }
} /* GIFDraw() */

// ========================== END OF DRAWING GIF ========================================

// ========================== INITIALIZE ALL LIBRARIES ==================================

void animation_start_up(){
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);

  gif.begin(BIG_ENDIAN_PIXELS);
  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);         // For WS2812B
 
  FastLED.setBrightness(max_bright);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);              // FastLED power management set at 5V, 500mA
}

// ================= END OF INITIALIZING LIBRARIES =======================================


// ============================= PLAYS ANIMATION =========================================
void playAnimation(volatile bool* state, volatile int* animation_state ){

  // Decide which animation
  const uint8_t *ptrArray;
  if(*animation_state == 0){
    ptrArray = GIF_IMAGE_LOADING;
  }
  else if (*animation_state == 1){
    ptrArray = GIF_IMAGE_SUCESS;
  }
  else if (*animation_state == 2){
    ptrArray = GIF_IMAGE_FAIL;
  }


  // Put your main code here, to run repeatedly:
  if (gif.open((uint8_t *)ptrArray, sizeof(GIF_IMAGE_FAIL), GIFDraw))
  {
    //Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    tft.startWrite(); // The TFT chip select is locked low
    while (gif.playFrame(true, NULL))
    {
      yield();
      rainbow_wave(10, 10);                                      // Speed, delta hue values.
      FastLED.show();
      if (*state != false){
        gif.close();
        tft.endWrite(); // Release TFT chip select for other SPI devices
        return;
      }
    }
    gif.close();
    tft.endWrite(); // Release TFT chip select for other SPI devices
    *animation_state = 0;
  }
}

void rainbow_wave(uint8_t thisSpeed, uint8_t deltaHue) {     // The fill_rainbow call doesn't support brightness levels.
 
// uint8_t thisHue = beatsin8(thisSpeed,0,255);                // A simple rainbow wave.
 uint8_t thisHue = beat8(thisSpeed,255);                     // A simple rainbow march.
  
 fill_rainbow(leds, NUM_LEDS, thisHue, deltaHue);            // Use FastLED's fill_rainbow routine.
 
} // rainbow_wave()
