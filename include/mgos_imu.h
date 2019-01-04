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

enum mgos_imu_acc_type {
  ACC_NONE = 0,
  ACC_MPU9250,
  ACC_MPU9255,
  ACC_ADXL345,
  ACC_LSM303D,
  ACC_LSM303DLM
};

enum mgos_imu_gyro_type {
  GYRO_NONE = 0,
  GYRO_MPU9250,
  GYRO_MPU9255,
  GYRO_L3GD20,
  GYRO_L3GD20H
};

enum mgos_imu_mag_type {
  MAG_NONE = 0,
  MAG_AK8963,
  MAG_AK8975,
  MAG_MAG3110,
  MAG_LSM303D,
  MAG_LSM303DLM
};

struct mgos_imu;

struct mgos_imu *mgos_imu_create(void);
void mgos_imu_destroy(struct mgos_imu **imu);

// Gyroscope functions
bool mgos_imu_gyroscope_create_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, enum mgos_imu_gyro_type type);
bool mgos_imu_gyroscope_destroy(struct mgos_imu *imu);
bool mgos_imu_gyroscope_present(struct mgos_imu *imu);

// String representation of the sensor types, guaranteed to be less than or equal to 10 chars
const char *mgos_imu_gyroscope_get_name(struct mgos_imu *imu);

// Return gyroscope data in units of Rads/sec
bool mgos_imu_gyroscope_get(struct mgos_imu *imu, float *x, float *y, float *z);

// Get/set gyroscope offset in units of m/s/s
bool mgos_imu_gyroscope_get_offset(struct mgos_imu *imu, float *x, float *y, float *z);
bool mgos_imu_gyroscope_set_offset(struct mgos_imu *imu, float x, float y, float z);

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
bool mgos_imu_accelerometer_create_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, enum mgos_imu_acc_type type);
bool mgos_imu_accelerometer_destroy(struct mgos_imu *imu);
bool mgos_imu_accelerometer_present(struct mgos_imu *imu);

// String representation of the sensor types, guaranteed to be less than or equal to 10 chars
const char *mgos_imu_accelerometer_get_name(struct mgos_imu *imu);

// Return accelerometer data in units of m/s/s
bool mgos_imu_accelerometer_get(struct mgos_imu *imu, float *x, float *y, float *z);

// Get/set accelerometer offset in units of m/s/s
bool mgos_imu_accelerometer_get_offset(struct mgos_imu *imu, float *x, float *y, float *z);
bool mgos_imu_accelerometer_set_offset(struct mgos_imu *imu, float x, float y, float z);


// Magnetometer functions
bool mgos_imu_magnetometer_create_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, enum mgos_imu_mag_type type);
bool mgos_imu_magnetometer_destroy(struct mgos_imu *imu);
bool mgos_imu_magnetometer_present(struct mgos_imu *imu);

// String representation of the sensor types, guaranteed to be less than or equal to 10 chars
const char *mgos_imu_magnetometer_get_name(struct mgos_imu *imu);

// Return magnetometer data in units of microtesla (1000 microTesla = 10 Gauss)
bool mgos_imu_magnetometer_get(struct mgos_imu *imu, float *x, float *y, float *z);

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
