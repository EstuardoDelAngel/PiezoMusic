#include <Arduino.h> 

#define MAX(a,b) (a>b ? a : b)

const uint8_t tempo = 120;
const float dt = 1250 / tempo; //length of one division

struct osc {
public:
  int count;
  int matchLOW;
  int matchHIGH;
  uint8_t mode = 3;
};

volatile uint8_t *pinPort; //register of pin
volatile uint8_t pinMask; //bit mask for pin
uint8_t voices;
osc *oscs;

void speakerSetup(uint8_t pin, uint8_t _voices)
{
  pinMode(pin, OUTPUT);
     oscs = new osc[_voices];
   voices = _voices;
  pinPort = portOutputRegister(digitalPinToPort(pin)); //get register of pin
  pinMask = digitalPinToBitMask(pin); //get bit mask for pin
   TCCR2A = 1 << WGM21; //clear timer on compare match (CTC) mode
   TIMSK2|= 1 << OCIE2A; //enable CTC
   TCCR2B = 0b010; //prescaler = 8
    OCR2A = 49; //match register = 49. interrupt freq = 20kHz
}

void play(uint8_t note, uint8_t voice)
{
  oscs[voice].count = 0;
  oscs[voice].matchHIGH = 2e4 / (27.5 * pow(2, note/12.0));
}

void pulse(uint8_t note, float pwidth, uint8_t voice, uint16_t time = 0)
{
  if (pwidth > 50) pwidth = 100 - pwidth;
  play(note, voice);
  oscs[voice].matchLOW = oscs[voice].matchHIGH * pwidth/100.0;
  oscs[voice].mode = 0;
  delay(time * dt);
}

void saw(uint8_t note, uint8_t voice, uint16_t time = 0)
{
  play(note, voice);
  oscs[voice].matchLOW = 0;
  oscs[voice].mode = 1;
  delay(time * dt);
}

void noise(uint8_t note, uint8_t voice, uint16_t time = 0)
{
  play(note + 12, voice);
  oscs[voice].mode = 2;
  delay(time * dt);
}

void mute(uint8_t voice, uint16_t time = 0)
{
  oscs[voice].mode = 3;
  delay(time * dt);
}

void wait(uint16_t time)
{
  delay(time * dt);
}

uint16_t b;
uint16_t lfsr = 0xACE1u;

inline bool fastRand()
{
  b = (lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5);
  lfsr = (lfsr >> 1) | (b << 15);
  return lfsr & 1;
}

int k=0;
bool x[1000] = {};

ISR(TIMER2_COMPA_vect)
{
  for (uint8_t i = 0; i < voices; i++) {
    osc *x = &oscs[i];
    if (x->mode < 3) {
      x->count++;
      if (x->count == x->matchLOW) {
        if (x->mode != 2) *pinPort &= ~pinMask;
      }
      else if (x->count >= x->matchHIGH) {
        if (x->mode == 2) {
          if (fastRand()) *pinPort |= pinMask;
          else *pinPort &= ~pinMask;
        }
        else *pinPort |= pinMask;
        x->count = 0;
        
        if (x->mode == 1) {
          x->matchLOW++;
          if (x->matchLOW >= x->matchHIGH) x->matchLOW = 0;
        }
      }
    }
  }
}
