#include <FastLED.h>

// The code has only been tested for 8 instruments 4 rows (32 elements)
#define INSTRUMENTS 8
#define PATROWS 4
#define PERROW 8
#define PATTERNS 8

struct beat {
  byte power;
};

struct pattern {
  beat beats[PERROW*PATROWS];
};

struct instrument {
  pattern patterns[PATTERNS];
};

#define LED_PIN  6

#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#define BRIGHTNESS 25

// Params for width and height
const uint8_t kMatrixWidth = 8;
const uint8_t kMatrixHeight = 5;

const bool    kMatrixSerpentineLayout = false;
uint16_t XY( uint8_t x, uint8_t y)
{
  return (y * kMatrixWidth) + x;
}

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);

void loop()
{
    uint32_t ms = millis();
    static byte y=0, x=0;

    leds[XY(x,y)] = CHSV(255,255,0);
    x++;
    if (x==kMatrixWidth) {x=0;y++;}
    if (y==kMatrixHeight) y=0;

    leds[XY(x,y)] = CHSV(255,255,255);
    if( ms < 5000 ) {
      FastLED.setBrightness( scale8( BRIGHTNESS, (ms * 256) / 5000));
    } else {
      FastLED.setBrightness(BRIGHTNESS);
    }
    FastLED.show();
    delay(100);
}

void drawScreen()
{
  // Draw the sounds (first four rows) of current pattern
  // Draw the instrument bar
  // Draw the current instrument cursor
  // Use the blue cursor position (sound position)
  // Use the white cursor position (user position)
  
}



void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );
}

