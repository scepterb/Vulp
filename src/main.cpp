#include <Arduino.h>

#include "inc/astro_point.h"
#include "inc/astronomy.h"
#include "inc/bno085.h"
#include "inc/io.h"
#include "inc/wmm.h"

#define BNO08X_RESET -1
BNO085 imu;

static astro_observer_t observer;
static astro_time_t obsTime;

struct DateTime {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
};
static DateTime now = {2026, 8, 2, 12, 0, 0};
static float wmm_date;
static float mag_dec_deg;

void setup(void) {
  setupSerial();
  imu.setupBNO085();
  wmm_init();

  observer.height = 0.0;
  observer.latitude = 41.458074;
  observer.longitude = -81.927939;

  obsTime = Astronomy_MakeTime(now.year, now.month, now.day, now.hour, now.minute, now.second);
  wmm_date = wmm_get_date(now.year, now.month, now.day);
  E0000(observer.latitude, observer.longitude, wmm_date, &mag_dec_deg);
}

void loop() {
  if (imu.wasReset()) {
    /* Ask the hub why it restarted before re-enabling reports. */
    uint8_t cause = imu.readResetCause();
    Serial.printf("[reset] cause=%u (%s)\r\n", cause, BNO085::resetCauseName(cause));

    imu.setupBNO085(true);
  }

  bool fresh = imu.checkSensor();

  // To track if reading is stale
  static uint32_t reportsSincePrint = 0;
  if (fresh) reportsSincePrint++;

  astro_equatorial_t eq =
      imu_to_eqj(imu.ypr.roll, imu.ypr.pitch, imu.ypr.yaw, mag_dec_deg, obsTime, observer, REFRACTION_NORMAL);

  static uint32_t last = 0;
  uint32_t now = millis();

  if (now - last >= 250) {
    Serial.print(imu.sensorValue.status);  // This is accuracy in the range of 0 to 3
    Serial.print("\t");
    Serial.printf("RA = %.4f h   Dec = %+.4f deg  rpts=%lu%s\r\n", eq.ra, eq.dec, reportsSincePrint,
                  reportsSincePrint == 0 ? "  [stale]" : "");
    reportsSincePrint = 0;
    last = now;
  }
}
