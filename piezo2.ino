
const uint8_t pin = 13; //use pin 13 for builtin LED
const uint8_t nvoices = 8;

void slide(uint8_t end, uint16_t time, uint8_t voice)
{
  int target = 2e4 / (27.5 * pow(2, end/12.0)) - oscs[voice].matchHIGH;
  for (int i = 0; i < abs(target); i++) {
    if (i%2) oscs[voice].matchLOW += abs(target)/target;
    oscs[voice].matchHIGH += abs(target)/target;
    delay(dt * time/abs(target));
  }
  return;
}


void delayArp(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
  for (int i = 0; i < 4; i++) {
    pulse(a, 25, 0);
    if (i) pulse(b, 1, 2);
    mute(1, 12);
    
    mute(0);
    pulse(d, 2, 1);
    mute(2, 12);
    
    pulse(b, 25, 0);
    mute(1);
    pulse(c, 1, 2, 12);
    
    mute(0);
    pulse(a, 2, 1);
    mute(2, 12);
  
    pulse(c, 25, 0);
    mute(1);
    pulse(d, 1, 2, 12);
    
    mute(0);
    pulse(b, 2, 1);
    mute(2, 12);
  
    pulse(d, 25, 0);
    mute(1);
    pulse(a, 1, 2, 12);
    
    mute(0);
    pulse(c, 2, 1);
    mute(2, 12);
  }
}

void strum(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t time)
{
  pulse(a, 5, 0, 4);
  pulse(b, 5, 1, 4);
  pulse(c, 5, 2, 4);
  pulse(d, 5, 3, MAX(time-12, 0));
}

void kick(uint16_t time)
{
  noise(27, 4, 5);
  mute(4, MAX(time-5,0));
}

void bumpin(uint8_t a, uint8_t b=255, uint8_t c=255, uint8_t d=255, uint8_t e=255)
{
  if (d!=255) pulse(d, 1, 1);
  else mute(1);
  if (e!=255) pulse(e, 1, 2);
  else mute(2);
  pulse(a, 12, 0);
  if (b == 255) mute(5);
  else pulse(b, 5, 5);
  kick(48);
  kick(24);
  pulse(a+12, 12, 0, 24);
  pulse(a+24, 12, 0);
  if (c == 255) mute(5);
  else pulse(c, 5, 5);
  kick(24);
  pulse(a+12, 12, 0, 24);
  noise(60, 4, 12);
  mute(4, 12);
  kick(24);
}

void setup()
{
  speakerSetup(pin, nvoices);
  int i, j;

  for (j=0;j<2;j++) {
    for (i=0;i<2;i++) {
      delayArp(15, 22, 30, 37);
      delayArp(12, 19, 27, 34);
    }
    for (i=0;i<3;i+=2) {
      delayArp( 8, 22, 25, 30);
      delayArp( 5, 24, 27, 32+i);
    }
  }

  for (i=0;i<13;i+=12) {
    strum(15-i, 22-i, 30-i, 37-i, 192);
    for (j=0;j<3;j++) mute(j);
    mute(3, 192);
    strum(12-i, 19-i, 27-i, 34-i, 192);
    for (j=0;j<4;j++) mute(j);
    for (j=0;j<4;j++) {
      noise(30+i, 0, 6);
      mute(0, 42);
    }
  }

  for (j=0;j<2;j++) {
    for (i=0;i<3;i+=2) bumpin(3, 30+i, 30+i, 22, 25);
    bumpin(0, 31, 34, 22, 27);
    bumpin(0, 38, 39, 22, 27);
    for (i=0;i<2;i++) bumpin(3, 41, 41, 22, 25);
    for (i=0;i<2;i++) bumpin(0, 43, 43, 22, 27);
    for (i=0;i<3;i+=2) bumpin(11, 44-i, 44-i, 22, 27);
    for (i=0;i<3;i+=2) bumpin(12, 41-i, 41-i, 22, 27);
    for (i=0;i<3;i+=2) bumpin(11, 34-i, 34-i, 22, 27);
    for (i=0;i<5;i+=4) bumpin(12, 31-i, 31-i, 22, 27);
  }
  
  pulse(3, 25, 0);
  mute(1);
  mute(2);
  noise(80, 4);
  slide(40, 384, 4);
  for (i=3;i<=5;i++) mute(i);
  saw(15, 0);
  pulse(15, 1, 1);
  slide(3, 768, 0);
  wait(768);
  
  
  
  for (i=0;i<nvoices;i++) mute(i);
}


void loop(){}
