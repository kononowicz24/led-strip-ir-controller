/*
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

int RECV_PIN = 7;
int RED = 6;
int GRN = 10;
int BLU = 5;
int timeout = 250;
int currentBrightness=0;
bool countingUp;
int smoothstep=0;
float finR=255, finG=255, finB=255;
float red=120, green=120, blue=120;
//float momR=0.0f, momG=0.0f, momB=0.0f;
float deltaR=0.0f, deltaG=0.0f, deltaB=0.0f;
int nextcolor;
enum State {
  FLASH, STROBE, FADE, SMOOTH, SOLID, FADE2
} state=SOLID;
enum Color {
  R, G, B, W, RED2, ORANGE1, ORANGE2, YELLOW, GREEN2, CYAN1, CYAN2, CYAN3, BLUE2, VIOL1, VIOL2, PINK
} color=R;
int brightness = 255;
bool on = true;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  Serial.begin(9600);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");
  pinMode(RED, OUTPUT);
  pinMode(BLU, OUTPUT);
  pinMode(GRN, OUTPUT);
  //setPwmFrequency(9, 8);
  //setPwmFrequency(10, 8);
  //setPwmFrequency(11, 8);
  randomSeed(analogRead(A1));
}

float getBrightness (int bright) {
  return pow((exp((float)bright/255.0f)-1.0f)/(exp(1)-1.0f),1.5f);
}
float getBrightness () {
  return getBrightness(brightness);
}
void setcolor(int color, float& red, float& green, float& blue) {
  if (color==R) {red=255; green=0; blue=0;}
  if (color==G) {red=0; green=255; blue=0;}
  if (color==B) {red=0; green=0; blue=255;}
  if (color==W) {red=255; green=255; blue=255;}
  if (color==ORANGE1) {red=255; green=128; blue=0;}
  if (color==ORANGE2) {red=255; green=192; blue=0;}
  if (color==YELLOW) {red=255; green=255; blue=0;}
  if (color==GREEN2) {red=170; green=255; blue=0;}
  if (color==CYAN1) {red=0; green=255; blue=128;}
  if (color==CYAN2) {red=0; green=255; blue=255;}
  if (color==CYAN3) {red=0; green=170; blue=255;}
  if (color==VIOL1) {red=64; green=0; blue=255;}
  if (color==VIOL2) {red=128; green=0; blue=255;}
  if (color==PINK) {red=255; green=64; blue=128;}
  if (color==BLUE2) {red=0; green=86; blue=255;}
  if (color==RED2) {red=255; green=64; blue=0;}
}
void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    switch (results.value) {
      case 0xF7C03F: on=true; state=SOLID; break;
      case 0xF740BF: on=false; break;
      case 0xF7807F: brightness-=17; Serial.println(getBrightness()); break;
      case 0xF700FF: brightness+=17; Serial.println(getBrightness()); break;
      case 0xF720DF: color=R; setcolor(color, red,green,blue); break;
      case 0xF7A05F: color=G; setcolor(color, red,green,blue); break;
      case 0xF7609F: color=B; setcolor(color, red,green,blue); break;
      case 0xF7E01F: color=W; setcolor(color, red,green,blue); break;
      case 0xF7D02F: if (state==FLASH) {
                        switch (timeout) {
                          case 300: timeout=400; break;
                          case 150: timeout=300; break;
                          case 400: timeout=150; break;
                          default: timeout=150;
                        }
                      } else { 
                        state=FLASH; 
                        timeout=250;
                      } break;
      case 0xF7F00F: if (state==STROBE) {
                        switch (timeout) {
                          case 200: timeout=300; break;
                          case 300: timeout=100; break;
                          case 100: timeout=200; break;
                          default: timeout=200;
                        }
                      } else { 
                        state=STROBE; 
                        timeout=250;
                      } break;
      case 0xF7C837: if (state!=FADE) {
                       state=FADE;
                       Serial.println("FADE");
                     } else {
                       state=FADE2;
                       Serial.println("FADE2");
                     } break;
      case 0xF7E817: state=SMOOTH;  break;
      case 0xf730cf: color=ORANGE1; setcolor(color, red,green,blue); break;
      case 0xf708f7: color=ORANGE2; setcolor(color, red,green,blue); break;
      case 0xf728d7: color=YELLOW; setcolor(color, red,green,blue); break;
      case 0xf7906f: color=GREEN2; setcolor(color, red,green,blue); break;
      case 0xf7b04f: color=CYAN1; setcolor(color, red,green,blue); break;
      case 0xf78877: color=CYAN2; setcolor(color, red,green,blue); break;
      case 0xf7a857: color=CYAN3; setcolor(color, red,green,blue); break;
      case 0xf7708f: color=VIOL1; setcolor(color, red,green,blue); break;
      case 0xf748b7: color=VIOL2; setcolor(color, red,green,blue); break;
      case 0xf76897: color=PINK; setcolor(color, red,green,blue); break;
      case 0xf750af: color=BLUE2; setcolor(color, red,green,blue); break;
      case 0xf710ef: color=RED2; setcolor(color, red,green,blue); break;
    }
    
    irrecv.resume(); // Receive the next value
  }
  if (brightness>255) brightness=255;
  if (brightness<0) brightness=0;
  
  
  //analogWrite(9,128);
  //analogWrite(RED,64);
  if (on)
    switch (state) {
    case SOLID:
      //setcolor(color, red,green,blue);
      analogWrite(RED,red*getBrightness());
      analogWrite(GRN,green*getBrightness());
      analogWrite(BLU,blue*getBrightness());
      break;
    case STROBE:
      //setcolor(color, red,green,blue);
      if ((millis()%timeout)<timeout/2) {
        analogWrite(RED,red*getBrightness());
        analogWrite(GRN,green*getBrightness());
        analogWrite(BLU,blue*getBrightness());
        setcolor(color, red,green,blue);
      } else {
        analogWrite(RED,0);
        analogWrite(GRN,0);
        analogWrite(BLU,0);
      }
      break;
    case FLASH: 
      //setcolor(color, red,green,blue);
      if ((millis()%timeout)<timeout/2) {
        analogWrite(RED,red*getBrightness());
        analogWrite(GRN,green*getBrightness());
        analogWrite(BLU,blue*getBrightness());
      } else {
        analogWrite(RED,red*getBrightness()*getBrightness(192));
        analogWrite(GRN,green*getBrightness()*getBrightness(192));
        analogWrite(BLU,blue*getBrightness()*getBrightness(192));
      }
      break;
    case FADE2: if (currentBrightness<1) color=random(0,15);
    case FADE:
       //setcolor(color, red,green,blue);
       analogWrite(RED,red*getBrightness()*getBrightness(currentBrightness));
       analogWrite(GRN,green*getBrightness()*getBrightness(currentBrightness));
       analogWrite(BLU,blue*getBrightness()*getBrightness(currentBrightness));
       if (countingUp) currentBrightness++; else currentBrightness--; //todo zmienic
       if (currentBrightness>=255) countingUp=false;
       if (currentBrightness<=0) countingUp=true;
       break;
    case SMOOTH:
      if (smoothstep>=500) {
        smoothstep=0;
      }
      if (smoothstep==0) {
        nextcolor=random(0,15);
        setcolor(nextcolor, finR, finG, finB);
        deltaR=(finR-red)/500.0f;
        deltaG=(finG-green)/500.0f;
        deltaB=(finB-blue)/500.0f;
      }
      red+=deltaR; green+=deltaG; blue+=deltaB;
      analogWrite(RED,red*getBrightness());
      analogWrite(GRN,green*getBrightness());
      analogWrite(BLU,blue*getBrightness());
      smoothstep++;
      break;
    }
    
  else {
      analogWrite(RED,0);
      analogWrite(GRN,0);
      analogWrite(BLU,0);
    }
  delay(10); //not to stall receiver?
}
