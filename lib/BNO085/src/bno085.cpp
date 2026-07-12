/**
 * @file bno085.cpp
 * @author Ben Beredo
 * @date 2026
 */

#include "inc/bno085.h"

sh2_SensorId_t reportType = SH2_ARVR_STABILIZED_RV;
long reportIntervalUs = 5000;  // at 200Hz

void BNO085::setupBNO085(bool reset) {
  if (!reset) {
    if (!bno08x.begin_I2C()) {
      Serial.println("Failed to find BNO08x chip");
      while (1) {
        delay(10);
      }
    }
    Serial.println("BNO08x Found!");
  }
  
  setReports(reportType, reportIntervalUs);

  delay(100);
}

void BNO085::setReports(sh2_SensorId_t reportType, long report_interval) {
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(reportType, report_interval)) {
    Serial.println("Could not enable stabilized remote vector");
  }
}

void BNO085::checkSensor() {
  if (bno08x.getSensorEvent(&sensorValue)) {
    // we retrieve a rotation vector from the sensor
    quaternionToEulerRV(&sensorValue.un.arvrStabilizedRV, &ypr, true);
  }
}

void BNO085::quaternionToEuler(float qr, float qi, float qj, float qk, euler_t* ypr, bool degrees) {
  float sqr = sq(qr);
  float sqi = sq(qi);
  float sqj = sq(qj);
  float sqk = sq(qk);

  ypr->yaw = atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr));
  ypr->pitch = asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr));
  ypr->roll = atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr));

  if (degrees) {
    ypr->yaw *= RAD_TO_DEG;
    ypr->pitch *= RAD_TO_DEG;
    ypr->roll *= RAD_TO_DEG;
  }
}

void BNO085::quaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, euler_t* ypr, bool degrees) {
  quaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j, rotational_vector->k, ypr,
                    degrees);
}

bool BNO085::wasReset() { return bno08x.wasReset(); }
