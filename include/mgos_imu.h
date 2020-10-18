/*
 * Copyright 2018 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "mgos.h"
#include "mgos_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

// Conversion constants between m/s/s and G's
#define MSS2G           (.10196798f)
#define G2MSS           (9.807f)

// Conversion constants between Rad/sec and Deg/sec
#define DEG2RAD         (.017453292519943f) /* PI / 180 */
#define RAD2DEG         (57.2957795130823f) /* 180 / PI */

// Conversion constants between uTesla and Gauss (1 tesla = 10^4 gauss)
#define UTESLA2GAUSS    (0.01f)
#define GAUSS2UTESLA    (100.f)


enum mgos_imu_acc_type {
  ACC_NONE = 0,
  ACC_MPU9250,
  ACC_MPU9255,
  ACC_ADXL345,
  ACC_LSM303D,
  ACC_LSM303DLM,
  ACC_MMA8451,
  ACC_LSM9DS1,
  ACC_LSM6DSL,
  ACC_MPU6000,
  ACC_MPU6050,
  ACC_MPU6886,
  ACC_ICM20948
};

enum mgos_imu_gyro_type {
  GYRO_NONE = 0,
  GYRO_MPU9250,
  GYRO_MPU9255,
  GYRO_L3GD20,
  GYRO_L3GD20H,
  GYRO_ITG3205,
  GYRO_LSM9DS1,
  GYRO_LSM6DSL,
  GYRO_MPU6000,
  GYRO_MPU6050,
  GYRO_MPU6886,
  GYRO_ICM20948
};

enum mgos_imu_mag_type {
  MAG_NONE = 0,
  MAG_AK8963,
  MAG_AK8975,
  MAG_BMM150,
  MAG_MAG3110,
  MAG_LSM303D,
  MAG_LSM303DLM,
  MAG_HMC5883L,
  MAG_LSM9DS1,
  MAG_ICM20948
};

struct mgos_imu;

struct mgos_imu *mgos_imu_create(void);
void mgos_imu_destroy(struct mgos_imu **imu);

// Gyroscope functions
struct mgos_imu_gyro_opts {
  enum mgos_imu_gyro_type type;   // Gyroscope type.
  float                   odr;    // Data rate, in Hz. See doc for set_odr().
  float                   scale;  // Scale. See doc for set_scale().
  bool                    no_rst; // Do not perform reset of the device when configuring.
};

bool mgos_imu_gyroscope_create_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, const struct mgos_imu_gyro_opts *opts);
bool mgos_imu_gyroscope_destroy(struct mgos_imu *imu);
bool mgos_imu_gyroscope_present(struct mgos_imu *imu);

// String representation of the sensor types, guaranteed to be less than or equal to 10 chars
const char *mgos_imu_gyroscope_get_name(struct mgos_imu *imu);

// Return gyroscope data in units of degrees/sec
bool mgos_imu_gyroscope_get(struct mgos_imu *imu, float *x, float *y, float *z);

// Get/set gyroscope offset in units of degrees/sec
bool mgos_imu_gyroscope_get_offset(struct mgos_imu *imu, float *x, float *y, float *z);
bool mgos_imu_gyroscope_set_offset(struct mgos_imu *imu, float x, float y, float z);

// Get/set gyroscope scale in units of degrees/sec
// The driver will set the scale to at least the given `scale` parameter, eg 1000
// Will return true upon success, false if setting the scale is not feasible.
bool mgos_imu_gyroscope_get_scale(struct mgos_imu *imu, float *scale);
bool mgos_imu_gyroscope_set_scale(struct mgos_imu *imu, float scale);

// Get/set gyroscope output data rate in units Hertz
// The driver will set the data rate to at least the given `hertz` parameter, eg 100
// Will return true upon success, false if setting the data rate is not feasible.
bool mgos_imu_gyroscope_get_odr(struct mgos_imu *imu, float *hertz);
bool mgos_imu_gyroscope_set_odr(struct mgos_imu *imu, float hertz);

// Get/set gyroscope axes orientation relatve to accelerometer
// *vector is a list of 9 floats which determine how much of a certain sensor axis
// should be blended into calls to mgos_imu_gyroscope_get().
// Examples: swap x/y axes, and flip direction of z-axis
// float v[9]={0, 1, 0,     // x gets 0% of x-sensor, 100% of y-sensor, 0% of z-sensor
//             1, 0, 0,     // y gets 100% of x-sensor, 0% of y-sensor, 0% of z-sensor
//             0, 0, -1};   // z gets 0% of x-sensor, 0% of y-sensor, -100% of z-sensor
bool mgos_imu_gyroscope_get_orientation(struct mgos_imu *imu, float v[9]);
bool mgos_imu_gyroscope_set_orientation(struct mgos_imu *imu, float v[9]);

// Accelerometer functions
struct mgos_imu_acc_opts {
  enum mgos_imu_acc_type type;   // Accelerometer type.
  float                  odr;    // Data rate, in Hz. See doc for set_odr().
  float                  scale;  // Scale. See doc for set_scale().
  bool                   no_rst; // Do not perform reset of the device when configuring.
};

bool mgos_imu_accelerometer_create_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, const struct mgos_imu_acc_opts *opts);
bool mgos_imu_accelerometer_destroy(struct mgos_imu *imu);
bool mgos_imu_accelerometer_present(struct mgos_imu *imu);

// String representation of the sensor types, guaranteed to be less than or equal to 10 chars
const char *mgos_imu_accelerometer_get_name(struct mgos_imu *imu);

// Return accelerometer data in units of G
bool mgos_imu_accelerometer_get(struct mgos_imu *imu, float *x, float *y, float *z);

// Get/set accelerometer offset in units of G
bool mgos_imu_accelerometer_get_offset(struct mgos_imu *imu, float *x, float *y, float *z);
bool mgos_imu_accelerometer_set_offset(struct mgos_imu *imu, float x, float y, float z);

// Get/set accelerometer scale in units of G
// The driver will set the scale to at least the given `scale` parameter, eg 20 m/s/s
// Will return true upon success, false if setting the scale is not feasible.
bool mgos_imu_accelerometer_get_scale(struct mgos_imu *imu, float *scale);
bool mgos_imu_accelerometer_set_scale(struct mgos_imu *imu, float scale);

// Get/set accelerometer output data rate in units Hertz
// The driver will set the data rate to at least the given `hertz` parameter, eg 100
// Will return true upon success, false if setting the data rate is not feasible.
bool mgos_imu_accelerometer_get_odr(struct mgos_imu *imu, float *hertz);
bool mgos_imu_accelerometer_set_odr(struct mgos_imu *imu, float hertz);


// Magnetometer functions
struct mgos_imu_mag_opts {
  enum mgos_imu_mag_type type;   // Magnetometer type.
  float                  odr;    // Data rate, in Hz. See doc for set_odr().
  float                  scale;  // Scale. See doc for set_scale().
  bool                   no_rst; // Do not perform reset of the device when configuring.
};

bool mgos_imu_magnetometer_create_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, const struct mgos_imu_mag_opts *opts);
bool mgos_imu_magnetometer_destroy(struct mgos_imu *imu);
bool mgos_imu_magnetometer_present(struct mgos_imu *imu);

// String representation of the sensor types, guaranteed to be less than or equal to 10 chars
const char *mgos_imu_magnetometer_get_name(struct mgos_imu *imu);

// Return magnetometer data in units of Gauss
bool mgos_imu_magnetometer_get(struct mgos_imu *imu, float *x, float *y, float *z);

// Get/set magnetometer scale in units of Gauss
// The driver will set the scale to at least the given `scale` parameter, eg 400
// Will return true upon success, false if setting the scale is not feasible.
bool mgos_imu_magnetometer_get_scale(struct mgos_imu *imu, float *scale);
bool mgos_imu_magnetometer_set_scale(struct mgos_imu *imu, float scale);

// Get/set magnetometer output data rate in units Hertz
// The driver will set the data rate to at least the given `hertz` parameter, eg 100
// Will return true upon success, false if setting the data rate is not feasible.
bool mgos_imu_magnetometer_get_odr(struct mgos_imu *imu, float *hertz);
bool mgos_imu_magnetometer_set_odr(struct mgos_imu *imu, float hertz);

// Get/set magnetometer axes orientation relative to accelerometer
// *vector is a list of 9 floats which determine how much of a certain sensor axis
// should be blended into calls to mgos_imu_magnetometer_get().
// Examples: swap x/y axes, and flip direction of z-axis
// float v[9]={0, 1, 0,     // x gets 0% of x-sensor, 100% of y-sensor, 0% of z-sensor
//             1, 0, 0,     // y gets 100% of x-sensor, 0% of y-sensor, 0% of z-sensor
//             0, 0, -1};   // z gets 0% of x-sensor, 0% of y-sensor, -100% of z-sensor
bool mgos_imu_magnetometer_get_orientation(struct mgos_imu *imu, float v[9]);
bool mgos_imu_magnetometer_set_orientation(struct mgos_imu *imu, float v[9]);


// Initialization function for MGOS -- currently a noop.
bool mgos_imu_init(void);

#ifdef __cplusplus
}
#endif
