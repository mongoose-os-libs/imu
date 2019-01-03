//=============================================================================================
// madgwick.c
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
#include "madgwick.h"

//-------------------------------------------------------------------------------------------
// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root

static float invSqrt(float x) {
  union {
    float    f;
    uint32_t i;
  } conv;

  float       x2;
  const float threehalfs = 1.5F;

  x2     = x * 0.5F;
  conv.f = x;
  conv.i = 0x5f3759df - (conv.i >> 1);
  conv.f = conv.f * (threehalfs - (x2 * conv.f * conv.f));
  return conv.f;
}

struct mgos_imu_madgwick *mgos_imu_madgwick_create(void) {
  struct mgos_imu_madgwick *filter;

  filter = calloc(1, sizeof(struct mgos_imu_madgwick));
  if (!filter) {
    return NULL;
  }
  mgos_imu_madgwick_set_params(filter, 100.0f, 0.1f);
  mgos_imu_madgwick_reset(filter);
  return filter;
}

bool mgos_imu_madgwick_destroy(struct mgos_imu_madgwick **filter) {
  if (!*filter) {
    return false;
  }
  free(*filter);
  *filter = NULL;
  return true;
}

bool mgos_imu_madgwick_set_params(struct mgos_imu_madgwick *filter, float freq, float beta) {
  if (!filter) {
    return false;
  }
  filter->beta     = beta;
  filter->freq     = freq;
  filter->inv_freq = 1.0f / freq;
  return true;
}

bool mgos_imu_madgwick_reset(struct mgos_imu_madgwick *filter) {
  if (!filter) {
    return false;
  }
  filter->q0      = 1.0f;
  filter->q1      = 0.0f;
  filter->q2      = 0.0f;
  filter->q3      = 0.0f;
  filter->counter = 0;
  return true;
}

static bool mgos_imu_madgwick_updateIMU(struct mgos_imu_madgwick *filter, float gx, float gy, float gz, float ax, float ay, float az) {
  float recipNorm;
  float s0, s1, s2, s3;
  float qDot1, qDot2, qDot3, qDot4;
  float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

  // No need to check filter pointer -- it's checked by _update()

  // Rate of change of quaternion from gyroscope
  qDot1 = 0.5f * (-filter->q1 * gx - filter->q2 * gy - filter->q3 * gz);
  qDot2 = 0.5f * (filter->q0 * gx + filter->q2 * gz - filter->q3 * gy);
  qDot3 = 0.5f * (filter->q0 * gy - filter->q1 * gz + filter->q3 * gx);
  qDot4 = 0.5f * (filter->q0 * gz + filter->q1 * gy - filter->q2 * gx);

  // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
  if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
    // Normalise accelerometer measurement
    recipNorm = invSqrt(ax * ax + ay * ay + az * az);
    ax       *= recipNorm;
    ay       *= recipNorm;
    az       *= recipNorm;

    // Auxiliary variables to avoid repeated arithmetic
    _2q0 = 2.0f * filter->q0;
    _2q1 = 2.0f * filter->q1;
    _2q2 = 2.0f * filter->q2;
    _2q3 = 2.0f * filter->q3;
    _4q0 = 4.0f * filter->q0;
    _4q1 = 4.0f * filter->q1;
    _4q2 = 4.0f * filter->q2;
    _8q1 = 8.0f * filter->q1;
    _8q2 = 8.0f * filter->q2;
    q0q0 = filter->q0 * filter->q0;
    q1q1 = filter->q1 * filter->q1;
    q2q2 = filter->q2 * filter->q2;
    q3q3 = filter->q3 * filter->q3;

    // Gradient decent algorithm corrective step
    s0        = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
    s1        = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * filter->q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
    s2        = 4.0f * q0q0 * filter->q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
    s3        = 4.0f * q1q1 * filter->q3 - _2q1 * ax + 4.0f * q2q2 * filter->q3 - _2q2 * ay;
    recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);             // normalise step magnitude
    s0       *= recipNorm;
    s1       *= recipNorm;
    s2       *= recipNorm;
    s3       *= recipNorm;

    // Apply feedback step
    qDot1 -= filter->beta * s0;
    qDot2 -= filter->beta * s1;
    qDot3 -= filter->beta * s2;
    qDot4 -= filter->beta * s3;
  }

  // Integrate rate of change of quaternion to yield quaternion
  filter->q0 += qDot1 * filter->inv_freq;
  filter->q1 += qDot2 * filter->inv_freq;
  filter->q2 += qDot3 * filter->inv_freq;
  filter->q3 += qDot4 * filter->inv_freq;

  // Normalise quaternion
  recipNorm   = invSqrt(filter->q0 * filter->q0 + filter->q1 * filter->q1 + filter->q2 * filter->q2 + filter->q3 * filter->q3);
  filter->q0 *= recipNorm;
  filter->q1 *= recipNorm;
  filter->q2 *= recipNorm;
  filter->q3 *= recipNorm;

  filter->counter++;
  return true;
}

bool mgos_imu_madgwick_update(struct mgos_imu_madgwick *filter, float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) {
  if (!filter) {
    return false;
  }
  float recipNorm;
  float s0, s1, s2, s3;
  float qDot1, qDot2, qDot3, qDot4;
  float hx, hy;
  float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

  if (!filter) {
    return false;
  }

  // Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
  if ((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f)) {
    mgos_imu_madgwick_updateIMU(filter, gx, gy, gz, ax, ay, az);
    return false;
  }

  // Rate of change of quaternion from gyroscope
  qDot1 = 0.5f * (-filter->q1 * gx - filter->q2 * gy - filter->q3 * gz);
  qDot2 = 0.5f * (filter->q0 * gx + filter->q2 * gz - filter->q3 * gy);
  qDot3 = 0.5f * (filter->q0 * gy - filter->q1 * gz + filter->q3 * gx);
  qDot4 = 0.5f * (filter->q0 * gz + filter->q1 * gy - filter->q2 * gx);

  // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
  if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
    // Normalise accelerometer measurement
    recipNorm = invSqrt(ax * ax + ay * ay + az * az);
    ax       *= recipNorm;
    ay       *= recipNorm;
    az       *= recipNorm;

    // Normalise magnetometer measurement
    recipNorm = invSqrt(mx * mx + my * my + mz * mz);
    mx       *= recipNorm;
    my       *= recipNorm;
    mz       *= recipNorm;

    // Auxiliary variables to avoid repeated arithmetic
    _2q0mx = 2.0f * filter->q0 * mx;
    _2q0my = 2.0f * filter->q0 * my;
    _2q0mz = 2.0f * filter->q0 * mz;
    _2q1mx = 2.0f * filter->q1 * mx;
    _2q0   = 2.0f * filter->q0;
    _2q1   = 2.0f * filter->q1;
    _2q2   = 2.0f * filter->q2;
    _2q3   = 2.0f * filter->q3;
    _2q0q2 = 2.0f * filter->q0 * filter->q2;
    _2q2q3 = 2.0f * filter->q2 * filter->q3;
    q0q0   = filter->q0 * filter->q0;
    q0q1   = filter->q0 * filter->q1;
    q0q2   = filter->q0 * filter->q2;
    q0q3   = filter->q0 * filter->q3;
    q1q1   = filter->q1 * filter->q1;
    q1q2   = filter->q1 * filter->q2;
    q1q3   = filter->q1 * filter->q3;
    q2q2   = filter->q2 * filter->q2;
    q2q3   = filter->q2 * filter->q3;
    q3q3   = filter->q3 * filter->q3;

    // Reference direction of Earth's magnetic field
    hx   = mx * q0q0 - _2q0my * filter->q3 + _2q0mz * filter->q2 + mx * q1q1 + _2q1 * my * filter->q2 + _2q1 * mz * filter->q3 - mx * q2q2 - mx * q3q3;
    hy   = _2q0mx * filter->q3 + my * q0q0 - _2q0mz * filter->q1 + _2q1mx * filter->q2 - my * q1q1 + my * q2q2 + _2q2 * mz * filter->q3 - my * q3q3;
    _2bx = sqrtf(hx * hx + hy * hy);
    _2bz = -_2q0mx * filter->q2 + _2q0my * filter->q1 + mz * q0q0 + _2q1mx * filter->q3 - mz * q1q1 + _2q2 * my * filter->q3 - mz * q2q2 + mz * q3q3;
    _4bx = 2.0f * _2bx;
    _4bz = 2.0f * _2bz;

    // Gradient decent algorithm corrective step
    s0        = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * filter->q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * filter->q3 + _2bz * filter->q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * filter->q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
    s1        = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * filter->q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * filter->q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * filter->q2 + _2bz * filter->q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * filter->q3 - _4bz * filter->q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
    s2        = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * filter->q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * filter->q2 - _2bz * filter->q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * filter->q1 + _2bz * filter->q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * filter->q0 - _4bz * filter->q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
    s3        = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * filter->q3 + _2bz * filter->q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * filter->q0 + _2bz * filter->q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * filter->q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
    recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);             // normalise step magnitude
    s0       *= recipNorm;
    s1       *= recipNorm;
    s2       *= recipNorm;
    s3       *= recipNorm;

    // Apply feedback step
    qDot1 -= filter->beta * s0;
    qDot2 -= filter->beta * s1;
    qDot3 -= filter->beta * s2;
    qDot4 -= filter->beta * s3;
  }

  // Integrate rate of change of quaternion to yield quaternion
  filter->q0 += qDot1 * filter->inv_freq;
  filter->q1 += qDot2 * filter->inv_freq;
  filter->q2 += qDot3 * filter->inv_freq;
  filter->q3 += qDot4 * filter->inv_freq;

  // Normalise quaternion
  recipNorm   = invSqrt(filter->q0 * filter->q0 + filter->q1 * filter->q1 + filter->q2 * filter->q2 + filter->q3 * filter->q3);
  filter->q0 *= recipNorm;
  filter->q1 *= recipNorm;
  filter->q2 *= recipNorm;
  filter->q3 *= recipNorm;

  filter->counter++;
  return true;
}

bool mgos_imu_madgwick_get_quaternion(struct mgos_imu_madgwick *filter, float *q0, float *q1, float *q2, float *q3) {
  if (!filter) {
    return false;
  }
  if (q0) {
    *q0 = filter->q0;
  }
  if (q1) {
    *q1 = filter->q1;
  }
  if (q2) {
    *q2 = filter->q2;
  }
  if (q3) {
    *q3 = filter->q3;
  }
  return true;
}

bool mgos_imu_madgwick_get_angles(struct mgos_imu_madgwick *filter, float *roll, float *pitch, float *yaw) {
  if (!filter) {
    return false;
  }
  if (roll) {
    *roll = asinf(-2.0f * (filter->q1 * filter->q3 - filter->q0 * filter->q2));
  }
  if (pitch) {
    *pitch = atan2f(filter->q0 * filter->q1 + filter->q2 * filter->q3, 0.5f - filter->q1 * filter->q1 - filter->q2 * filter->q2);
  }
  if (yaw) {
    *yaw = atan2f(filter->q1 * filter->q2 + filter->q0 * filter->q3, 0.5f - filter->q2 * filter->q2 - filter->q3 * filter->q3);
  }
  return true;
}

bool mgos_imu_madgwick_get_counter(struct mgos_imu_madgwick *filter, uint32_t *counter) {
  if (!filter || !counter) {
    return false;
  }
  *counter = filter->counter;
  return true;
}
