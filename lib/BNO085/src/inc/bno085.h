#ifndef __BNO085_H
#define __BNO085_H

#include <Adafruit_BNO08x.h>

#define RESET_CAUSE_QUERY_FAILED 0xFF

struct euler_t {
  float yaw;
  float pitch;
  float roll;
};

class BNO085 {
 private:
  Adafruit_BNO08x bno08x;
  void setReports(sh2_SensorId_t, long);

  void quaternionToEuler(float, float, float, float, euler_t*, bool = false);
  void quaternionToEulerRV(sh2_RotationVectorWAcc_t*, euler_t*, bool = false);

 public:
  sh2_SensorValue_t sensorValue;
  euler_t ypr;

  BNO085() = default;

  void setupBNO085(bool = false);

  /** @brief Services the sensor, returning true if a fresh report was detected. */
  bool checkSensor();
  bool wasReset();
  /** @brief When called after wasReset() returns true, it returns the SH-2 reset cause byte. */
  uint8_t readResetCause();

  static const char* resetCauseName(uint8_t);
};

#endif
