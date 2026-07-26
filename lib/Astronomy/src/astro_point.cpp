/**
 * @file astro_point.cpp
 * @author Ben Beredo
 * @date 2026
 */

#include "inc/astro_point.h"

#include <math.h>

/*
 * Rotate a body-frame vector using Euler angles.
 * https://en.wikipedia.org/wiki/Rotation_formulations_in_three_dimensions#Rotation_matrix_%E2%86%92_Euler_angles_(z-x-z_extrinsic)
 */
void rotate_vector(double roll, double pitch, double yaw, const double in[3], double out[3]) {
  if (USING_ENU) yaw = 90.0 - yaw;

  /* memoize trig values of roll, pitch, and yaw */
  const double cr = cos(roll * DEG2RAD), sr = sin(roll * DEG2RAD);
  const double cp = cos(pitch * DEG2RAD), sp = sin(pitch * DEG2RAD);
  const double cy = cos(yaw * DEG2RAD), sy = sin(yaw * DEG2RAD);

  /* calculate rotation matrix */
  const double R[3][3] = {{cp * cy, sr * sp * cy - cr * sy, cr * sp * cy + sr * sy},
                          {cp * sy, sr * sp * sy + cr * cy, cr * sp * sy - sr * cy},
                          {-sp, sr * cp, cr * cp}};
  for (int i = 0; i < 3; ++i) out[i] = R[i][0] * in[0] + R[i][1] * in[1] + R[i][2] * in[2];
}

/**
 *  Derives altitude / azimuth from an input ENU vector
 */
void enu_to_azalt(const double enu[3], double mag_decl_deg, double* az_deg, double* alt_deg) {
  double e = enu[0];
  double n = enu[1];
  double u = enu[2];

  double azi = atan2(enu[0], enu[1]) * RAD2DEG;
  azi = fmod(azi + mag_decl_deg, 360.0);
  if (azi < 0.0) azi += 360.0;
  *az_deg = azi;

  double r_horizontal = hypot(e, n);
  *alt_deg = atan2(u, r_horizontal) * RAD2DEG;
}

astro_equatorial_t azalt_to_eqj(double az_deg, double alt_deg, astro_time_t time, astro_observer_t observer,
                                astro_refraction_t refraction) {
  astro_spherical_t hor;
  hor.status = ASTRO_SUCCESS;
  hor.lat = alt_deg;
  hor.lon = az_deg;
  hor.dist = 1.0;  // arbitrary: we only want direction

  astro_vector_t v_hor = Astronomy_VectorFromHorizon(hor, time, refraction);

  astro_rotation_t rot = Astronomy_Rotation_HOR_EQJ(&time, observer);
  astro_vector_t v_eqj = Astronomy_RotateVector(rot, v_hor);

  return Astronomy_EquatorFromVector(v_eqj);
}

/**
 * @brief Converts the IMU roll/pitch/yaw output into RA/declination coordinates
 *
 * @param magnetic_declination_deg the local offset between true north and magnetic north, east-positive
 * @param time the local time in UTC
 */
astro_equatorial_t imu_to_eqj(double roll_deg, double pitch_deg, double yaw_deg, double magnetic_declination_deg,
                              astro_time_t time, astro_observer_t observer, astro_refraction_t refraction) {
  const double boresight[3] = {-1.0, 0.0, 0.0}; /* body -X forward */
  double ned[3], az, alt;

  rotate_vector(roll_deg, pitch_deg, yaw_deg, boresight, ned);
  enu_to_azalt(ned, magnetic_declination_deg, &az, &alt);

  return azalt_to_eqj(az, alt, time, observer, refraction);
}