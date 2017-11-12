/*
 * Project bed-lighting
 * Description:
 * Author:
 * Date:
 *
 * PIR output on D3
 * PIR mode repeat (H)
 */

#define PIR_INIT_TIME 60000

static const int PIR_PIN = D3;

SerialLogHandler logHandler;
uint32_t startUpTimestamp;
int pirState = LOW;
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

  // TODO wait startup time of PIR sensor until trying to use it (1 minute)

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
      pirState = HIGH;
    }
  } else {
    if (pirState == HIGH){
      Log.info("Motion ended!");
      digitalWrite(D7, pinValue);
      pirState = LOW;
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
