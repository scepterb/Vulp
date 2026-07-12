#include <Adafruit_BNO08x.h>
#include <Arduino.h>

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
  void checkSensor();
  bool wasReset();
};
