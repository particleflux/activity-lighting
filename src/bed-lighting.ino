/*
 * Project bed-lighting
 * Description:
 * Author:
 * Date:
 *
 * PIR output on D3
 * PIR mode repeat (H)
 */

static const int PIR_PIN = D3;

SerialLogHandler logHandler;
uint32_t startUpTimestamp;
int pirState = LOW;             // we start, assuming no motion detected
int pinValue = 0;

void setup() {
  Log.info("Starting setup sequence...");
  Log.info("System version: %s", (const char*) System.version());
  Log.info("device id: %s", (const char*) System.deviceID());

  pinMode(PIR_PIN, INPUT);
  pinMode(D7, OUTPUT);
  startUpTimestamp = millis();
}

void loop() {
  checkMotion();
}

void checkMotion() {
  pinValue = digitalRead(PIR_PIN);  // read input pinValue
  if (pinValue == HIGH) {            // check if the input is HIGH
    if (pirState == LOW) {
      // we have just turned on
      Log.info("Motion detected!");
      digitalWrite(D7, pinValue);
      // We only want to print on the output change, not state
      pirState = HIGH;
    }
  } else {
    if (pirState == HIGH){
      // we have just turned of
      Log.info("Motion ended!");
      digitalWrite(D7, pinValue);
      // We only want to print on the output change, not state
      pirState = LOW;
    }
  }
}
