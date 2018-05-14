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
  ACC_ADXL345
};

enum mgos_imu_gyro_type {
  GYRO_NONE = 0,
  GYRO_MPU9250
};

enum mgos_imu_mag_type {
  MAG_NONE = 0,
  MAG_AK8963,
  MAG_AK8975,
  MAG_MAG3110
};

struct mgos_imu;

struct mgos_imu *mgos_imu_create(void);
bool mgos_imu_gyroscope_create_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, enum mgos_imu_gyro_type type);
bool mgos_imu_accelerometer_create_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, enum mgos_imu_acc_type type);
bool mgos_imu_magnetometer_create_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, enum mgos_imu_mag_type type);

/* TODO(pim): Add SPI adders
bool mgos_imu_gyroscope_create_spi(struct mgos_imu *imu, struct mgos_spi *spi, uint8_t cs_gpio, enum mgos_imu_gyro_type type);
bool mgos_imu_accelerometer_create_spi(struct mgos_imu *imu, struct mgos_spi *spi, uint8_t cs_gpio, enum mgos_imu_acc_type type);
bool mgos_imu_magnetometer_create_spi(struct mgos_imu *imu, struct mgos_spi *spi, uint8_t cs_gpio, enum mgos_imu_mag_type type);
*/

bool mgos_imu_gyroscope_destroy(struct mgos_imu *imu);
bool mgos_imu_accelerometer_destroy(struct mgos_imu *imu);
bool mgos_imu_magnetometer_destroy(struct mgos_imu *imu);

void mgos_imu_destroy(struct mgos_imu **imu);

bool mgos_imu_has_accelerometer(struct mgos_imu *imu);
bool mgos_imu_has_gyroscope(struct mgos_imu *imu);
bool mgos_imu_has_magnetometer(struct mgos_imu *imu);

/* Read all available imu data from the IMU */
bool mgos_imu_read(struct mgos_imu *imu);

/* Return accelerometer data in units of m/s/s */
bool mgos_imu_accelerometer_get(struct mgos_imu *imu, float *x, float *y, float *z);

/* Return accelerometer data in units of deg/sec rotation rate */
bool mgos_imu_gyroscope_get(struct mgos_imu *imu, float *x, float *y, float *z);

/* Return magnetometer data in units of microtesla (1 microtesla = 10 milligauss) */
bool mgos_imu_magnetometer_get(struct mgos_imu *imu, float *x, float *y, float *z);

/* Return compass heading based on magnetometer data, from [0..359] */
bool mgos_imu_get_compass_heading(struct mgos_imu *imu, uint16_t *heading);

/* String representation of the sensor types, guaranteed to be le 10 characters. */
const char *mgos_imu_gyroscope_get_name(struct mgos_imu *imu);
const char *mgos_imu_magnetometer_get_name(struct mgos_imu *imu);
const char *mgos_imu_accelerometer_get_name(struct mgos_imu *imu);


/*
 * Initialization function for MGOS -- currently a noop.
 */
bool mgos_imu_init(void);

#ifdef __cplusplus
}
#endif
