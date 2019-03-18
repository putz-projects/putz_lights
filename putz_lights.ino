#include "FastLED.h"

FASTLED_USING_NAMESPACE

#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    53

CRGB ledsP[NUM_LEDS];
CRGB ledsT[NUM_LEDS];
CRGB ledsZ[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

int Z_FLASH_PIN = 4;
int P_FLASH_PIN = 5;
int T_FLASH_PIN = 6;

#define FLASH_LENGTH 10 // ms

#define PTZ_FLASH 0

#define P_FLASH 1
#define T_FLASH 2
#define Z_FLASH 3

#define P_LED 8
#define T_LED 10
#define Z_LED 9

#define PTZ_DELAY_FLASH 4

#define BAUD 9600

uint8_t gHue = 100;

char state = 'c';
char buff = state;
char last_valid_state = state;

void setup() {
  delay(3000);
                  
  pinMode(P_FLASH_PIN, OUTPUT);     
  pinMode(T_FLASH_PIN, OUTPUT);    
  pinMode(Z_FLASH_PIN, OUTPUT);   

  FastLED.addLeds<LED_TYPE,P_LED,COLOR_ORDER>(ledsP, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,T_LED,COLOR_ORDER>(ledsT, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,Z_LED,COLOR_ORDER>(ledsZ, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  FastLED.setBrightness(BRIGHTNESS);

  Serial.begin(BAUD);
}

void flash(int mode) {
  if (mode == PTZ_FLASH) {
    digitalWrite(P_FLASH_PIN, HIGH);
    digitalWrite(T_FLASH_PIN, HIGH);
    digitalWrite(Z_FLASH_PIN, HIGH);

    delay(FLASH_LENGTH);

    digitalWrite(P_FLASH_PIN, LOW);
    digitalWrite(T_FLASH_PIN, LOW);
    digitalWrite(Z_FLASH_PIN, LOW);
  } else if (mode == P_FLASH) {
    digitalWrite(P_FLASH_PIN, HIGH);
    delay(FLASH_LENGTH);
    digitalWrite(P_FLASH_PIN, LOW);
  } else if (mode == T_FLASH) {
    digitalWrite(T_FLASH_PIN, HIGH);
    delay(FLASH_LENGTH);
    digitalWrite(T_FLASH_PIN, LOW);
  } else if (mode == Z_FLASH) {
    digitalWrite(Z_FLASH_PIN, HIGH);
    delay(FLASH_LENGTH);
    digitalWrite(Z_FLASH_PIN, LOW);
  } else if (mode == PTZ_DELAY_FLASH) {
    flash(P_FLASH);
    delay(30);
    flash(T_FLASH);
    delay(30);
    flash(Z_FLASH);
  }
}



void rainbow(CRGB* leds) 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void addGlitter(CRGB* leds, fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void rainbowWithGlitter(CRGB* leds) 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow(leds);
  addGlitter(leds, 80);
}


void confetti(CRGB* leds) 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon(CRGB* leds)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm(CRGB* leds)
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 128;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle(CRGB* leds) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void clearLights(CRGB* leds) {
  for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
}

void loop() {
  if (Serial.available() > 0) {
    buff = Serial.read();

    // Letter specific
    if (buff == 'p') {
      flash(P_FLASH);
    }
    else if (buff == 't') {
      flash(T_FLASH);
    }
    else if (buff == 'z') {
      flash(Z_FLASH);
    }

    // Global
    else if (buff == 'd') {
      flash(PTZ_DELAY_FLASH);
    }
    else if (buff == 'a') {
      flash(PTZ_FLASH);
    } else {
      state = buff;
    }
  }

  switch (state) {
  case 'c':
    last_valid_state = state;
    confetti(ledsZ);  
    confetti(ledsT);
    confetti(ledsP);
    break;
  case 'r':
    last_valid_state = state;
    rainbow(ledsZ);  
    rainbow(ledsT);
    rainbow(ledsP);
    break;
  case 'l':
    last_valid_state = state;
    rainbowWithGlitter(ledsZ);  
    rainbowWithGlitter(ledsT);
    rainbowWithGlitter(ledsP);
    break;
  case 's':
    last_valid_state = state;
    sinelon(ledsZ);  
    sinelon(ledsT);
    sinelon(ledsP);
    break;
  case 'b':
    last_valid_state = state;
    bpm(ledsZ);  
    bpm(ledsT);
    bpm(ledsP);
    break;
  case 'j':
    last_valid_state = state;
    juggle(ledsZ);  
    juggle(ledsT);
    juggle(ledsP);
    break;
  case 'q':
    last_valid_state = state;
    clearLights(ledsZ);
    clearLights(ledsP);
    clearLights(ledsT);
    break;
  default:
    state = last_valid_state;
    break;
  }
  
  FastLED.show();  
  FastLED.delay(1000/FRAMES_PER_SECOND); 
}
