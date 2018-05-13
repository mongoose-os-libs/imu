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
#include "mgos_imu.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mgos_imu_mag;
struct mgos_imu_acc;
struct mgos_imu_gyro;

struct mgos_imu {
  struct mgos_imu_mag * mag;
  struct mgos_imu_acc * acc;
  struct mgos_imu_gyro *gyro;
  uint8_t               capabilities;
};

// Magnetometer
typedef bool (*mgos_imu_mag_detect_fn)(struct mgos_imu_mag *dev);
typedef bool (*mgos_imu_mag_create_fn)(struct mgos_imu_mag *dev);
typedef bool (*mgos_imu_mag_destroy_fn)(struct mgos_imu_mag *dev);
typedef bool (*mgos_imu_mag_read_fn)(struct mgos_imu_mag *dev);

struct mgos_imu_mag {
  mgos_imu_mag_detect_fn detect;
  mgos_imu_mag_create_fn  create;
  mgos_imu_mag_destroy_fn   destroy;
  mgos_imu_mag_read_fn   read;

  struct mgos_i2c *      i2c;
  uint8_t                i2caddr;
  enum mgos_imu_mag_type type;

  void *user_data;

  float                  gain[3];
  int16_t                data[3];
};

struct mgos_imu_mag *mgos_imu_mag_create(void);
bool mgos_imu_mag_destroy(struct mgos_imu_mag **mag);

// Accelerometer
typedef bool (*mgos_imu_acc_detect_fn)(struct mgos_imu_acc *dev);
typedef bool (*mgos_imu_acc_create_fn)(struct mgos_imu_acc *dev);
typedef bool (*mgos_imu_acc_destroy_fn)(struct mgos_imu_acc *dev);
typedef bool (*mgos_imu_acc_read_fn)(struct mgos_imu_acc *dev);

struct mgos_imu_acc {
  mgos_imu_acc_detect_fn detect;
  mgos_imu_acc_create_fn  create;
  mgos_imu_acc_destroy_fn   destroy;
  mgos_imu_acc_read_fn   read;

  struct mgos_i2c *      i2c;
  uint8_t                i2caddr;
  enum mgos_imu_acc_type type;

  void *user_data;

  float                  scale;
  int16_t                data[3];
};

struct mgos_imu_acc *mgos_imu_acc_create(void);
bool mgos_imu_acc_destroy(struct mgos_imu_acc **acc);

// Gyroscope
typedef bool (*mgos_imu_gyro_detect_fn)(struct mgos_imu_gyro *dev);
typedef bool (*mgos_imu_gyro_create_fn)(struct mgos_imu_gyro *dev);
typedef bool (*mgos_imu_gyro_destroy_fn)(struct mgos_imu_gyro *dev);
typedef bool (*mgos_imu_gyro_read_fn)(struct mgos_imu_gyro *dev);

struct mgos_imu_gyro {
  mgos_imu_gyro_detect_fn detect;
  mgos_imu_gyro_create_fn  create;
  mgos_imu_gyro_destroy_fn   destroy;
  mgos_imu_gyro_read_fn   read;

  struct mgos_i2c *       i2c;
  uint8_t                 i2caddr;
  enum mgos_imu_gyro_type type;

  void *user_data;

  float                   scale;
  float                   bias[3];
  int16_t                 data[3];
  int16_t                 temperature;
};

struct mgos_imu_gyro *mgos_imu_gyro_create(void);
bool mgos_imu_gyro_destroy(struct mgos_imu_gyro **gyro);

#ifdef __cplusplus
}
#endif
