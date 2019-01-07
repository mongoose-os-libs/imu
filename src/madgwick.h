//=============================================================================================
// madgwick.h
//=============================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/open-source-imu-and-ahrs-algorithms/
//
// From the x-io website "Open-source resources available on this website are
// provided under the GNU General Public Licence unless an alternative licence
// is provided in source."
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
// 12/11/2018	Pim van Pelt    Rewritten to C for Mongoose OS
//
//=============================================================================================
#pragma once
#include "mgos.h"
#include <math.h>

/* Madgwick filter structure. */
struct mgos_imu_madgwick {
  float    beta;
  float    q0;
  float    q1;
  float    q2;
  float    q3;
  float    freq;
  float    inv_freq;
  uint32_t counter;
};

/* Create a new filter and initialize it by resetting the Quaternion and setting
 * the update rate to 100Hz and the gain to 0.1
 * Returns a pointer to a `struct mgos_imu_madgwick`, or NULL otherwise.
 */
struct mgos_imu_madgwick *mgos_imu_madgwick_create(void);

/* Clean up and return memory for the filter
 */
bool mgos_imu_madgwick_destroy(struct mgos_imu_madgwick **filter);

/* Sets the filter update rate and gain (defaults to freq=100Hz and gain=0.1)
 * The `mgos_imu_madgwick_update()` function then expects to be called at `freq`
 * per second.
 */
bool mgos_imu_madgwick_set_params(struct mgos_imu_madgwick *filter, float frequency, float beta);

/* Resets the filter Quaternion to an initial state (of {1,0,0,0}).
 */
bool mgos_imu_madgwick_reset(struct mgos_imu_madgwick *filter);

/* Run an update cycle on the filter. Inputs gx/gy/gz are in any calibrated input (for example,
 * m/s/s or G), inputs of ax/ay/az are in Rads/sec, inputs of mx/my/mz are in any calibrated
 * input (for example, uTesla or Gauss). The inputs of mx/my/mz can be passed as 0.0, in which
 * case the magnetometer fusion will not occur.
 * Returns true on success, false on failure.
 */
bool mgos_imu_madgwick_update(struct mgos_imu_madgwick *filter, float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);

/*
 * Returns AHRS Quaternion, as values between -1.0 and +1.0.
 * Each of q0, q1, q2, q3 pointers may be NULL, in which case they will not be
 * filled in.
 * Returns true on success, false in case of error, in which case the values of
 * q0, q1, q2 and q3 are undetermined.
 */
bool mgos_imu_madgwick_get_quaternion(struct mgos_imu_madgwick *filter, float *q0, float *q1, float *q2, float *q3);

/*
 * Returns AHRS angles of roll, pitch and yaw, in Radians between -Pi and +Pi.
 * Each of the roll, pitch and yaw pointers may be NULL, in which case they will
 * not be filled in.
 * Returns true on success, false in case of error, in which case the values of
 * roll, pitch and yaw are undetermined.
 */
bool mgos_imu_madgwick_get_angles(struct mgos_imu_madgwick *filter, float *roll, float *pitch, float *yaw);

/*
 * Returns filter counter. Each call to `mgos_imu_madgwick_update()` increments the
 * counter by one.
 * Returns true on success, false in case of error, in which case the value of
 * counter is undetermined.
 */
bool mgos_imu_madgwick_get_counter(struct mgos_imu_madgwick *filter, uint32_t *counter);
