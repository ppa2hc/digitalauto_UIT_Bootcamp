#include <mbed.h>
#include <Arduino_CAN.h>

using namespace rtos;

Thread thread1;
Thread thread2;

#define SERIAL_BAUDRATE (115200)
#define DELAY_PERIOD (5000)
#define MAX_MESSAGE_SIZE (8)

#define HAZARDLIGHT LED_BUILTIN
#define LIGHT_HAZARDLIGHT_MODE0 (0)  // HAZARDLIGHT OFF
#define LIGHT_HAZARDLIGHT_MODE1 (4)  // HAZARDLIGHT ON

bool hazardLightState = false;
int hazardLightMode = LIGHT_HAZARDLIGHT_MODE0;

void CanHandling() {
  while (true) {
    if (CAN.available()) {
      /* Receiving CAN message */
      CanMsg const msg = CAN.read();
      // Serial.println(msg);

      // copy read msg to a variable
      CanMsg const inputMsg(msg);

      // CAN ID for lightControl
      if ((inputMsg.id == 0x3E9)) {
        printCanMsg(inputMsg);
        Serial.println(inputMsg.data[0] & 0b1100);
        switch (inputMsg.data[0] & 0b1100) {
          case (LIGHT_HAZARDLIGHT_MODE0):
            hazardLightMode = LIGHT_HAZARDLIGHT_MODE0;
            Serial.println("CAN LIGHT_HAZARDLIGHT_MODE0");
            break;
          case (LIGHT_HAZARDLIGHT_MODE1):
            hazardLightMode = LIGHT_HAZARDLIGHT_MODE1;
            Serial.println("CAN LIGHT_HAZARDLIGHT_MODE1");
            break;
          default:
            break;
        }
      }
    }
    delay(10);
  }
}

void LedBlinking() {
  while (true) {
    switch (hazardLightMode) {
      case (LIGHT_HAZARDLIGHT_MODE0):
        // Serial.println("LIGHT_HAZARDLIGHT_MODE0");
        turnHazardOff();
        delay(1000);
        break;
      case (LIGHT_HAZARDLIGHT_MODE1):
        // Serial.println("LIGHT_HAZARDLIGHT_MODE1");
        toggleHazard();
        delay(1000);
        break;
      default:
        break;
    }
  }
}

void setup() {
  Serial.begin(SERIAL_BAUDRATE);

  if (!CAN.begin(CanBitRate::BR_500k)) {
    Serial.println("CAN.begin(...) failed.");
    for (;;) {}
  }

  pinMode(HAZARDLIGHT, OUTPUT);
  turnHazardOff();

  thread1.start(CanHandling);
  thread2.start(LedBlinking);
}

void loop() {
  // Main loop can do other tasks
}

void turnHazardOff() {
  digitalWrite(HAZARDLIGHT, HIGH);
}

void turnHazardOn() {
  digitalWrite(HAZARDLIGHT, LOW);
}

void toggleHazard() {
  if (hazardLightState) {
    Serial.println("turnHazardOn");
    turnHazardOn();
  } else {
    Serial.println("turnHazardOff");
    turnHazardOff();
  }
  hazardLightState = !hazardLightState;
}

void printCanMsg(CanMsg const feed) {
  for (int i = 0; i < MAX_MESSAGE_SIZE; i++) {
    Serial.print(feed.data[i]);
    Serial.print("\t");
  }
  Serial.println();
}
