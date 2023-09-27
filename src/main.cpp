
#include <Adafruit_GFX.h>    // Core graphics library
//#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include "windows.h"

#include "Adafruit_NeoTrellis.h"

#define TFT_CS        10
#define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         8
  
#define INT_PIN 6

#define STATE_INITIALIZE 0
#define STATE_WAITING 1
#define STATE_READY 2

#define MAX_PAYLOAD_SIZE 0x10

#define HELLO_MESSAGE 0x01
#define READY_MESSAGE 0x02
#define BUTTON_MESSAGE 0x03
#define GOODBYE_MESSAGE 0x04

// RawHID packets are always 64 bytes
byte buffer[64];

struct HIDEvent {
  uint8_t messageType;
  uint8_t payload[MAX_PAYLOAD_SIZE];
};

void blink(keyEvent evt);
uint32_t Wheel(byte WheelPos);
TrellisCallback handleKeyPress(keyEvent evt);
void sendKey(uint8_t key, uint8_t column, uint8_t row, uint8_t state);
void initializeTrellis();
void showTrellisStartup();
void handleInterrupt();

Adafruit_NeoTrellis trellis;

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

volatile unsigned long lastPressedTime = 0;
volatile unsigned int buttonEventReady = 0;
volatile unsigned int keyEventReady = 0;

volatile unsigned int lastKeyPressed = -1;

volatile uint8_t keyRow = 0;
volatile uint8_t keyColumn = 0;
volatile uint8_t keyState = 0;


HWindow mainWindow;
//HIDEvent currentEvent;
int lastState =  STATE_INITIALIZE;
int currentState = STATE_INITIALIZE;

void showState() {
  tft.setTextSize(0);

  switch(lastState) {
    case STATE_INITIALIZE:
      ClearText(mainWindow, "Initializing", 5, 15, 1, TextAlignment::LEFT);
      break;
    case STATE_WAITING  :
      ClearText(mainWindow, "Waiting for Connection", 5, 15, 1, TextAlignment::LEFT);
      break;
    case STATE_READY:
      ClearText(mainWindow, "Ready", 5, 15, 1, TextAlignment::LEFT);
      break;
  }

  switch(currentState) {
    case STATE_INITIALIZE:
      PrintText(mainWindow, "Initializing", 5, 15, 1, ST77XX_YELLOW, TextAlignment::LEFT);
      break;
    case STATE_WAITING  :
      PrintText(mainWindow, "Waiting for Connection", 5, 15, 1, ST77XX_RED, TextAlignment::LEFT);
      break;
    case STATE_READY:
      PrintText(mainWindow, "Ready", 5, 15, 1, ST77XX_GREEN, TextAlignment::LEFT);
      break;
  }

}
// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {

  if(WheelPos < 85) {
   return trellis.pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return trellis.pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return trellis.pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

TrellisCallback handleKeyPress(keyEvent evt) {

  //sendKey(evt.bit.NUM, evt.bit.EDGE);
  int state = 0x00;

  cli();
  
  blink(evt);
  keyRow = evt.bit.NUM / 4;
  keyColumn = evt.bit.NUM % 4;
 
  if(evt.bit.EDGE ==  SEESAW_KEYPAD_EDGE_RISING) {
    state |= 0x01;  
  } else if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {
    state |= 0x04;
  }

  lastKeyPressed = evt.bit.NUM;
  
  keyState = state;
  keyEventReady = 1;
  
  sei();

  
  return 0;
}

void sendKey(uint8_t key, uint8_t column, uint8_t row, uint8_t state) {

  buffer[0] = BUTTON_MESSAGE;
  buffer[1] = state;
  buffer[2] = column;
  buffer[3] = row;

  // currentEvent.messageType = BUTTON_MESSAGE;
  // currentEvent.payload[2] = column;
  // currentEvent.payload[1] = row;
  // currentEvent.payload[0] = state;

  RawHID.send(buffer, sizeof(buffer));
}

volatile bool isKeyDown = false;
volatile int lastKeyDown = -1;

//define a callback for key presses
void blink(keyEvent evt) {
  
  if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    if(!isKeyDown) {
      trellis.pixels.setPixelColor(evt.bit.NUM, Wheel(map(evt.bit.NUM, 0, trellis.pixels.numPixels(), 0, 255))); //on rising
      isKeyDown = true;

      lastKeyDown = evt.bit.NUM;
    }
  }
  else if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {
    trellis.pixels.setPixelColor(evt.bit.NUM, 0); //off falling

    if(evt.bit.NUM == lastKeyDown) {
      isKeyDown = false;    
      lastKeyDown = -1;
    }
  }
  
  trellis.pixels.show();
}

void initializeTrellis() {

  if(!trellis.begin()){
    while(1) {
      delay(1);
    }
  }

  //activate all keys and set callbacks
  for(int i=0; i<NEO_TRELLIS_NUM_KEYS; i++){
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_RISING);
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING);
    trellis.registerCallback(i, handleKeyPress);
  }
  
}

void showTrellisStartup() {
  //do a little animation to show we're on
  for(uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
    trellis.pixels.setPixelColor(i, Wheel(map(i, 0, trellis.pixels.numPixels(), 0, 255)));
  }

  trellis.pixels.show();
  delay(250);

  for(uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
    trellis.pixels.setPixelColor(i, 0x000000);
  }

  trellis.pixels.show();
  delay(250);

  for(uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
    trellis.pixels.setPixelColor(i, Wheel(map(i, 0, trellis.pixels.numPixels(), 0, 255)));
  }

  trellis.pixels.show();
  delay(250);

  for(uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
    trellis.pixels.setPixelColor(i, 0x000000);
    trellis.pixels.show();
    delay(25);
  }
 
}


void handleInterrupt() {
  
  cli();
  if(!digitalRead(INT_PIN)){
    buttonEventReady = 1;
  }  
  sei();
}

void setup() {
  
  pinMode(INT_PIN, INPUT_PULLUP);

  attachInterrupt(INT_PIN, handleInterrupt, CHANGE);

  tft.init(240, 320);           // Init ST7789 320x240
  tft.fillScreen(ST77XX_BLACK);


  tft.setCursor(0, 0);
  tft.setRotation(1);
  //tft.setTextSize(3);
  //tft.setTextColor(ST77XX_YELLOW);
  //tft.setTextWrap(true);
  //tft.print("Hello World!");
  mainWindow = CreateWindow(NULL, "EpiDeck Hub v0.21a", 20, 20, 280, 210);
  DrawWindow(mainWindow);

  PrintText( mainWindow, "Initializing...", 140, 50, 2, ST77XX_WHITE, TextAlignment::CENTER);
  initializeTrellis();
  showTrellisStartup();
  ClearText( mainWindow, "Initializing...", 140, 50, 2, TextAlignment::CENTER);

  PrintText( mainWindow, "Welcome to", 140, 80, 2, ST77XX_WHITE, TextAlignment::CENTER);
  PrintText( mainWindow, "EpiDeck!", 140, 95, 2, ST77XX_WHITE, TextAlignment::CENTER);

  currentState = STATE_WAITING;
  showState();
}


void loop() {

  int n;

  if(currentState == STATE_WAITING) {
     n = RawHID.recv(buffer, 0); // 0 timeout = do not wait

     if(n > 0) {
      // only need to check the first byte
      if(buffer[0] == HELLO_MESSAGE) {

        buffer[0] = READY_MESSAGE;
        RawHID.send(buffer, sizeof(buffer));

        lastState = currentState;
        currentState = STATE_READY;

        showState();
      }
     }
    // we are waiting for a connection
  } else if(currentState == STATE_READY) {

      n = RawHID.recv(buffer, 0); // 0 timeout = do not wait

      if(n > 0) {
        // only need to check the first byte
        if(buffer[0] == GOODBYE_MESSAGE) {
          currentState = STATE_WAITING;

          showState();
          return;
        }
     }

    // do we ahve a button event ready?
    if(buttonEventReady) {

      // clear interrupts to maintain sanity
      cli();
      // clear the flag
      buttonEventReady = 0;
      // and let's read that (handlers take care of the rest)
      trellis.read(false);

      // don't forget to reset interrupts or we won't know if we have a button event ready
      sei();
    }

    // this is set if a button event was detected
    // and it has been decoded ready to be sent
    if(keyEventReady) {

      keyEventReady = 0;
      sendKey(lastKeyPressed, keyColumn, keyRow, keyState);
    }
  }
  
  //delay(10);
}

