#ifndef __ASTRO_POINT_H
#define __ASTRO_POINT_H

#include "inc/astro_demo_common.h"  // needed for types

#define USING_ENU 1

void rotate_vector(double roll, double pitch, double yaw, const double in[3], double out[3]);
void enu_to_azalt(const double enu[3], double mag_decl_deg, double* az_deg, double* alt_deg);
astro_equatorial_t azalt_to_eqj(double az_deg, double alt_deg, astro_time_t time, astro_observer_t observer,
                                astro_refraction_t refraction);

astro_equatorial_t imu_to_eqj(double roll_deg, double pitch_deg, double yaw_deg, double magnetic_declination_deg,
                              astro_time_t time, astro_observer_t observer, astro_refraction_t refraction);

#endif