/**
 * @file bno085.cpp
 * @author Ben Beredo
 * @date 2026
 */

#include "inc/bno085.h"

#include <Adafruit_BNO08x.h>
#include <Arduino.h>

sh2_SensorId_t reportType = SH2_ARVR_STABILIZED_RV;
long reportIntervalUs = 20000;  // at 50Hz

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

bool BNO085::checkSensor() {
  if (bno08x.getSensorEvent(&sensorValue)) {
    // we retrieve a rotation vector from the sensor
    quaternionToEulerRV(&sensorValue.un.arvrStabilizedRV, &ypr, true);
    return true;
  }
  return false;
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

uint8_t BNO085::readResetCause() {
  /* Service the sensor once first: sh2_getProdIds() spins SHTP service loop,
   * and the library's sensor callback decodes into a static pointer that only
   * getSensorEvent() ever sets. Without this it could still be null on the very first loop. */
  checkSensor();

  sh2_ProductIds_t prodIds;
  memset(&prodIds, 0, sizeof(prodIds));

  if (sh2_getProdIds(&prodIds) != SH2_OK || prodIds.numEntries == 0) {
    return RESET_CAUSE_QUERY_FAILED;
  }

  return prodIds.entry[0].resetCause;
}

const char* BNO085::resetCauseName(uint8_t cause) {
  /* Values per the BNO08x datasheet. */
  switch (cause) {
    case 0:
      return "n/a";
    case 1:
      return "power-on reset";
    case 2:
      return "internal system reset";
    case 3:
      return "watchdog timeout";
    case 4:
      return "external reset";
    case 5:
      return "other";
    case RESET_CAUSE_QUERY_FAILED:
      return "query failed";
    default:
      return "unknown";
  }
}
