#include <Arduino.h>
#include "inc/astro_demo_common.h"
#include "inc/io.h"
#include "inc/bno085.h"

#define BNO08X_RESET -1
BNO085 imu;

void setup(void) {
  setupSerial();
  imu.setupBNO085();

  delay(100);
}

void loop() {
  if (imu.wasReset()) {
    Serial.print("sensor was reset ");
    imu.setupBNO085(true);
  }

  imu.checkSensor();

  static long last = 0;
  long now = millis();

  if (now - last >= 250) {
    Serial.print(imu.sensorValue.status);  // This is accuracy in the range of 0 to 3
    Serial.print("\t");
    Serial.print(imu.ypr.roll);
    Serial.print("\t");
    Serial.print(imu.ypr.pitch);
    Serial.print("\t");
    Serial.println(imu.ypr.yaw);
    last = now;
  }
}