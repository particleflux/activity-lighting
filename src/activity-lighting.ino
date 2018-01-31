/*
 * Project bed-lighting
 * Description:
 * Author:
 * Date:
 *
 * PIR output on D3
 * PIR mode repeat (H)
 */

#include "ir-control.h"

#define PIR_INIT_TIME 60000
#define LIGHT_TIME 30000

// number of bits
#define SIGNAL_LENGTH 32


/* IR Codes*/
#define IR_BRIGHTNESS_PLUS  0xF700FF
#define IR_BRIGHTNESS_MINUS 0xF7807F
#define IR_OFF    0xF740BF
#define IR_ON     0xF7C03F
#define IR_R      0xF720DF
#define IR_G      0xF7A05F
#define IR_B      0xF7609F
#define IR_W      0xF7E01F
#define IR_B1     0xF710EF
#define IR_B2     0xF7906F
#define IR_B3     0xF750AF
#define IR_FLASH  0xF7D02F
#define IR_B4     0xF730CF
#define IR_B5     0xF7B04F
#define IR_B6     0xF7708F
#define IR_STROBE 0xF7F00F
#define IR_B7     0xF708F7
#define IR_B8     0xF78877
#define IR_B9     0xF748B7
#define IR_FADE   0xF7C837
#define IR_B10    0xF728D7
#define IR_B11    0xF7A857
#define IR_B12    0xF76897
#define IR_SMOOTH 0xF7E817

static const int PIR_PIN = D3;
static const int IR_PIN = D2;

SerialLogHandler logHandler;
uint32_t startUpTimestamp, lastMotionTime;
int pirState = LOW, isLightOn = false;
int pinValue = 0;
IRsend irsend(IR_PIN);


void setup() {
  Log.info("Starting setup sequence...");
  Log.info("System version: %s", (const char*) System.version());
  Log.info("device id: %s", (const char*) System.deviceID());

  pinMode(PIR_PIN, INPUT);
  pinMode(D7, OUTPUT);
  startUpTimestamp = lastMotionTime = millis();
}

void loop() {

  checkMotion();
}

void checkMotion() {
  if (!sensorIsInitialized()) {
    return;
  }

  pinValue = digitalRead(PIR_PIN);
  if (pinValue == HIGH) {
    if (pirState == LOW) {
      Log.info("Motion detected!");
      digitalWrite(D7, pinValue);
      irsend.sendNEC(IR_ON, SIGNAL_LENGTH);
      pirState = HIGH;
      lastMotionTime = millis();
      isLightOn = true;
    }
  } else {
    if (pirState == HIGH){
      Log.info("Motion ended!");
      pirState = LOW;
    }

    // turn lights off after time x
    if (isLightOn && (millis() - lastMotionTime) > LIGHT_TIME) {
      Log.info("Reached light time, turning lights off");
      digitalWrite(D7, LOW);
      irsend.sendNEC(IR_OFF, SIGNAL_LENGTH);
      isLightOn = false;
    }
  }

}

bool sensorIsInitialized() {
  static bool isInitialized = false;

  if (!isInitialized) {
    // Log.info("Waiting for PIR to initialize");
    isInitialized = (millis() - startUpTimestamp) > PIR_INIT_TIME;
  }

  return isInitialized;
}
