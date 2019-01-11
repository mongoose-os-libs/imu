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

// Conversion constants between m/s/s and G's
#define MSS2G           (.10196798f)
#define G2MSS           (9.807f)

// Conversion constants between Rad/sec and Deg/sec
#define DEG2RAD         (.017453292519943f) /* PI / 180 */
#define RAD2DEG         (57.2957795130823f) /* 180 / PI */

// Conversion constants between uTesla and Gauss (1 tesla = 10^4 gauss)
#define UTESLA2GAUSS    (0.01f)
#define GAUSS2UTESLA    (100.f)

struct mgos_imu_mag;
struct mgos_imu_acc;
struct mgos_imu_gyro;

struct mgos_imu {
  struct mgos_imu_mag * mag;
  struct mgos_imu_acc * acc;
  struct mgos_imu_gyro *gyro;
  void *                user_data;
};

// Magnetometer
typedef bool (*mgos_imu_mag_detect_fn)(struct mgos_imu_mag *dev, void *imu_user_data);
typedef bool (*mgos_imu_mag_create_fn)(struct mgos_imu_mag *dev, void *imu_user_data);
typedef bool (*mgos_imu_mag_destroy_fn)(struct mgos_imu_mag *dev, void *imu_user_data);
typedef bool (*mgos_imu_mag_read_fn)(struct mgos_imu_mag *dev, void *imu_user_data);
typedef bool (*mgos_imu_mag_get_odr_fn)(struct mgos_imu_mag *dev, void *imu_user_data, float *odr);
typedef bool (*mgos_imu_mag_set_odr_fn)(struct mgos_imu_mag *dev, void *imu_user_data, float odr);
typedef bool (*mgos_imu_mag_get_scale_fn)(struct mgos_imu_mag *dev, void *imu_user_data, float *scale);
typedef bool (*mgos_imu_mag_set_scale_fn)(struct mgos_imu_mag *dev, void *imu_user_data, float scale);

struct mgos_imu_mag {
  mgos_imu_mag_detect_fn    detect;
  mgos_imu_mag_create_fn    create;
  mgos_imu_mag_destroy_fn   destroy;
  mgos_imu_mag_read_fn      read;
  mgos_imu_mag_get_odr_fn   get_odr;
  mgos_imu_mag_set_odr_fn   set_odr;
  mgos_imu_mag_get_scale_fn get_scale;
  mgos_imu_mag_set_scale_fn set_scale;

  struct mgos_i2c *         i2c;
  uint8_t                   i2caddr;
  enum mgos_imu_mag_type    type;

  void *                    user_data;

  float                     scale;
  float                     bias[3];
  float                     orientation[9];
  int16_t                   mx, my, mz;
};

// Accelerometer
typedef bool (*mgos_imu_acc_detect_fn)(struct mgos_imu_acc *dev, void *imu_user_data);
typedef bool (*mgos_imu_acc_create_fn)(struct mgos_imu_acc *dev, void *imu_user_data);
typedef bool (*mgos_imu_acc_destroy_fn)(struct mgos_imu_acc *dev, void *imu_user_data);
typedef bool (*mgos_imu_acc_read_fn)(struct mgos_imu_acc *dev, void *imu_user_data);
typedef bool (*mgos_imu_acc_get_odr_fn)(struct mgos_imu_acc *dev, void *imu_user_data, float *odr);
typedef bool (*mgos_imu_acc_set_odr_fn)(struct mgos_imu_acc *dev, void *imu_user_data, float odr);
typedef bool (*mgos_imu_acc_get_scale_fn)(struct mgos_imu_acc *dev, void *imu_user_data, float *scale);
typedef bool (*mgos_imu_acc_set_scale_fn)(struct mgos_imu_acc *dev, void *imu_user_data, float scale);

struct mgos_imu_acc {
  mgos_imu_acc_detect_fn    detect;
  mgos_imu_acc_create_fn    create;
  mgos_imu_acc_destroy_fn   destroy;
  mgos_imu_acc_read_fn      read;
  mgos_imu_acc_get_odr_fn   get_odr;
  mgos_imu_acc_set_odr_fn   set_odr;
  mgos_imu_acc_get_scale_fn get_scale;
  mgos_imu_acc_set_scale_fn set_scale;

  struct mgos_i2c *         i2c;
  uint8_t                   i2caddr;
  struct mgos_imu_acc_opts  opts;

  void *                    user_data;

  float                     scale;
  float                     offset_ax, offset_ay, offset_az;
  int16_t                   ax, ay, az;
};

// Gyroscope
typedef bool (*mgos_imu_gyro_detect_fn)(struct mgos_imu_gyro *dev, void *imu_user_data);
typedef bool (*mgos_imu_gyro_create_fn)(struct mgos_imu_gyro *dev, void *imu_user_data);
typedef bool (*mgos_imu_gyro_destroy_fn)(struct mgos_imu_gyro *dev, void *imu_user_data);
typedef bool (*mgos_imu_gyro_read_fn)(struct mgos_imu_gyro *dev, void *imu_user_data);
typedef bool (*mgos_imu_gyro_get_odr_fn)(struct mgos_imu_gyro *dev, void *imu_user_data, float *odr);
typedef bool (*mgos_imu_gyro_set_odr_fn)(struct mgos_imu_gyro *dev, void *imu_user_data, float odr);
typedef bool (*mgos_imu_gyro_get_scale_fn)(struct mgos_imu_gyro *dev, void *imu_user_data, float *scale);
typedef bool (*mgos_imu_gyro_set_scale_fn)(struct mgos_imu_gyro *dev, void *imu_user_data, float scale);

struct mgos_imu_gyro {
  mgos_imu_gyro_detect_fn    detect;
  mgos_imu_gyro_create_fn    create;
  mgos_imu_gyro_destroy_fn   destroy;
  mgos_imu_gyro_read_fn      read;
  mgos_imu_gyro_get_odr_fn   get_odr;
  mgos_imu_gyro_set_odr_fn   set_odr;
  mgos_imu_gyro_get_scale_fn get_scale;
  mgos_imu_gyro_set_scale_fn set_scale;

  struct mgos_i2c *          i2c;
  uint8_t                    i2caddr;
  enum mgos_imu_gyro_type    type;

  void *                     user_data;

  float                      scale;
  float                      offset_gx, offset_gy, offset_gz;
  float                      orientation[9];
  int16_t                    gx, gy, gz;
};

#ifdef __cplusplus
}
#endif
