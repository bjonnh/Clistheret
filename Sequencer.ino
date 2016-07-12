#include <FastLED.h>
#include <Wire.h>

unsigned int joy_x = 0;
unsigned int joy_y = 0;
unsigned int acc_x = 0;
unsigned int acc_y = 0;
unsigned int acc_z = 0;
unsigned int btn_c = 0;
unsigned int btn_z = 0;

byte rebound_x = 0;
byte rebound_c = 0;
byte rebound_z = 0;

void read_nunchuk_data()
{
   unsigned int buffer[6];
   byte buffer_index = 0;
 
   Wire.beginTransmission(0x52);
   Wire.write(0x00);
   Wire.endTransmission();
   

   delay(1); /* This delay is required for a wired nunchuk otherwise the data will appear maxed out */
 
   Wire.requestFrom(0x52, 6);
   while(Wire.available())    
   {
       buffer[buffer_index] = Wire.read();
       buffer_index++;
   }
   
   joy_x = buffer[0];
   joy_y = buffer[1];
   acc_x = ((buffer[2] << 2) | ((buffer[5] & 0x0C) >> 2) & 0x03FF);
   acc_y = ((buffer[3] << 2) | ((buffer[5] & 0x30) >> 4) & 0x03FF);
   acc_z = ((buffer[4] << 2) | ((buffer[5] & 0xC0) >> 6) & 0x03FF);
   btn_c = !((buffer[5] & 0x02) >> 1);
   btn_z = !(buffer[5] & 0x01);
   
}

void initialize_nunchuk()
{
   Wire.beginTransmission(0x52);
   Wire.write (0x40);      
   Wire.write (0x00);      
   Wire.endTransmission();
   delay(30);
}

// The code has only been tested for 8 instruments 4 rows (32 elements)
#define INSTRUMENTS 8
#define PATTERNS 8
#define PATROWS 4
#define PERROW 8
#define INSTRUMENTROW 4

struct beat {
  byte power=0;
};

struct pattern {
  beat beats[PERROW*PATROWS];
};

struct instrument {
  pattern patterns[PATTERNS];
};

instrument current[INSTRUMENTS];
byte current_pattern=0;
byte current_instrument=0;
byte current_position_beat=0;
byte current_cursor_x=0;
byte current_cursor_y=0;

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

void drawScreen()
{
/*  byte current_pattern=0;
byte current_instrument=0;
byte current_position_beat=0;
byte current_cursor_x=0;
byte current_cursor_y=0;
*/
byte bright;
  // Draw the sounds (first four rows) of current pattern
  for (int i=0;i<PERROW*PATROWS;i++) {
    byte power = current[current_instrument].patterns[current_pattern].beats[i].power;
    leds[XY(i%PERROW, i/PERROW)] = CHSV(0,0,power);
  }
  // Draw the instrument bar
  // Draw the current instrument cursor
  for (int i=0;i<INSTRUMENTS;i++) {
    if (i==current_instrument) { bright=255; } else {bright=0;}
    leds[XY(i, INSTRUMENTROW)] = CHSV(100,255,bright);
  }
  // Use the blue cursor position (sound position)
  leds[XY(current_position_beat%PERROW, current_position_beat/PERROW)] = CHSV(155,255,255);

  // Use the white cursor position (user position)
//leds[XY(x%,y)
  leds[XY(current_cursor_x,current_cursor_y)] = CHSV(55,155,255);
  
}

void loop()
{
    uint32_t ms = millis();
    read_nunchuk_data();
    
    // Instrument change
  if (joy_x>100 && joy_x < 140) { rebound_x=0; } else {
      if (rebound_x==0) {
                  rebound_x=1;
        if (joy_x<100) {
          if (current_instrument>0) { current_instrument--; }
        }
           if (joy_x>140) {
          if (current_instrument<INSTRUMENTS-1) { current_instrument++; }
        }

      }
  
  }
  if (btn_c==0) { rebound_c =0; }
    if (btn_z==0) { rebound_z =0; }
  if (btn_c==1 && rebound_c == 0) {
    rebound_c = 1;
    current[current_instrument].patterns[current_pattern].beats[current_position_beat].power = 255;
  }
    if (btn_z==1) {
    current[current_instrument].patterns[current_pattern].beats[current_position_beat].power = 0;
  }

    current_position_beat++;
    if (current_position_beat > PATROWS*PERROW-1) current_position_beat = 0;
    
    if( ms < 5000 ) {
      FastLED.setBrightness( scale8( BRIGHTNESS, (ms * 256) / 5000));
    } else {
      FastLED.setBrightness(BRIGHTNESS);
    }
    drawScreen();
    FastLED.show();
    delay(100);
}

void init_instruments() {
// lets randomize shit
for (int i=0; i<PATROWS*PERROW;i++) {
  current[current_instrument].patterns[current_pattern].beats[i].power = i*4*(i%4);
}
};

void setup() {
   Wire.begin();
   
   initialize_nunchuk();

  init_instruments();
  
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );
}


