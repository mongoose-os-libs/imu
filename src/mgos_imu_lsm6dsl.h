/*
 * Copyright 2019 Google Inc.
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

#include <stdint.h>
#include "mgos_imu_internal.h"

#define MGOS_LSM6DSL_DEFAULT_I2CADDR    (0x6b)
#define MGOS_LSM6DSL_DEVID              (0x6A)

enum mgos_imu_lsm6dsl_fifo_mode {
  MGOS_LSM6DSL_FIFO_MODE_BYPASS      = 0,
  MGOS_LSM6DSL_FIFO_MODE_FIFO        = 1,
  MGOS_LSM6DSL_FIFO_MODE_CONT_FIFO   = 3,
  MGOS_LSM6DSL_FIFO_MODE_BYPASS_CONT = 4,
  MGOS_LSM6DSL_FIFO_MODE_CONT        = 6,
};

// LSM6DSL -- Registers (Acc/Gyro)
#define MGOS_LSM6DSL_REG_FUNC_CFG_ACCESS             (0x01)
#define MGOS_LSM6DSL_REG_SENSOR_SYNC_TIME_FRAME      (0x04)
#define MGOS_LSM6DSL_REG_SENSOR_SYNC_RES_RATIO       (0x05)
#define MGOS_LSM6DSL_REG_FIFO_CTRL1                  (0x06)
#define MGOS_LSM6DSL_REG_FIFO_CTRL2                  (0x07)
#define MGOS_LSM6DSL_REG_FIFO_CTRL3                  (0x08)
#define MGOS_LSM6DSL_REG_FIFO_CTRL4                  (0x09)
#define MGOS_LSM6DSL_REG_FIFO_CTRL5                  (0x0A)
#define MGOS_LSM6DSL_REG_DRDY_PULSE_CFG_G            (0x0B)
#define MGOS_LSM6DSL_REG_INT1_CTRL                   (0x0D)
#define MGOS_LSM6DSL_REG_INT2_CTRL                   (0x0E)
#define MGOS_LSM6DSL_REG_WHO_AM_I                    (0x0F)
#define MGOS_LSM6DSL_REG_CTRL1_XL                    (0x10)
#define MGOS_LSM6DSL_REG_CTRL2_G                     (0x11)
#define MGOS_LSM6DSL_REG_CTRL3_C                     (0x12)
#define MGOS_LSM6DSL_REG_CTRL4_C                     (0x13)
#define MGOS_LSM6DSL_REG_CTRL5_C                     (0x14)
#define MGOS_LSM6DSL_REG_CTRL6_C                     (0x15)
#define MGOS_LSM6DSL_REG_CTRL7_G                     (0x16)
#define MGOS_LSM6DSL_REG_CTRL8_XL                    (0x17)
#define MGOS_LSM6DSL_REG_CTRL9_XL                    (0x18)
#define MGOS_LSM6DSL_REG_CTRL10_C                    (0x19)
#define MGOS_LSM6DSL_REG_WAKE_UP_SRC                 (0x1B)
#define MGOS_LSM6DSL_REG_TAP_SRC                     (0x1C)
#define MGOS_LSM6DSL_REG_D6D_SRC                     (0x1D)
#define MGOS_LSM6DSL_REG_STATUS_REG                  (0x1E)
#define MGOS_LSM6DSL_REG_OUT_TEMP_L                  (0x20)
#define MGOS_LSM6DSL_REG_OUT_TEMP_H                  (0x21)
#define MGOS_LSM6DSL_REG_OUTX_L_G                    (0x22)
#define MGOS_LSM6DSL_REG_OUTX_H_G                    (0x23)
#define MGOS_LSM6DSL_REG_OUTY_L_G                    (0x24)
#define MGOS_LSM6DSL_REG_OUTY_H_G                    (0x25)
#define MGOS_LSM6DSL_REG_OUTZ_L_G                    (0x26)
#define MGOS_LSM6DSL_REG_OUTZ_H_G                    (0x27)
#define MGOS_LSM6DSL_REG_OUTX_L_XL                   (0x28)
#define MGOS_LSM6DSL_REG_OUTX_H_XL                   (0x29)
#define MGOS_LSM6DSL_REG_OUTY_L_XL                   (0x2A)
#define MGOS_LSM6DSL_REG_OUTY_H_XL                   (0x2B)
#define MGOS_LSM6DSL_REG_OUTZ_L_XL                   (0x2C)
#define MGOS_LSM6DSL_REG_OUTZ_H_XL                   (0x2D)
#define MGOS_LSM6DSL_REG_SENSORHUB1_REG              (0x2E)
#define MGOS_LSM6DSL_REG_SENSORHUB2_REG              (0x2F)
#define MGOS_LSM6DSL_REG_SENSORHUB3_REG              (0x30)
#define MGOS_LSM6DSL_REG_SENSORHUB4_REG              (0x31)
#define MGOS_LSM6DSL_REG_SENSORHUB5_REG              (0x32)
#define MGOS_LSM6DSL_REG_SENSORHUB6_REG              (0x33)
#define MGOS_LSM6DSL_REG_SENSORHUB7_REG              (0x34)
#define MGOS_LSM6DSL_REG_SENSORHUB8_REG              (0x35)
#define MGOS_LSM6DSL_REG_SENSORHUB9_REG              (0x36)
#define MGOS_LSM6DSL_REG_SENSORHUB10_REG             (0x37)
#define MGOS_LSM6DSL_REG_SENSORHUB11_REG             (0x38)
#define MGOS_LSM6DSL_REG_SENSORHUB12_REG             (0x39)
#define MGOS_LSM6DSL_REG_FIFO_STATUS1                (0x3A)
#define MGOS_LSM6DSL_REG_FIFO_STATUS2                (0x3B)
#define MGOS_LSM6DSL_REG_FIFO_STATUS3                (0x3C)
#define MGOS_LSM6DSL_REG_FIFO_STATUS4                (0x3D)
#define MGOS_LSM6DSL_REG_FIFO_DATA_OUT_L             (0x3E)
#define MGOS_LSM6DSL_REG_FIFO_DATA_OUT_H             (0x3F)
#define MGOS_LSM6DSL_REG_TIMESTAMP0_REG              (0x40)
#define MGOS_LSM6DSL_REG_TIMESTAMP2_REG              (0x42)
#define MGOS_LSM6DSL_REG_STEP_TIMESTAMP_L            (0x49)
#define MGOS_LSM6DSL_REG_STEP_TIMESTAMP_H            (0x4A)
#define MGOS_LSM6DSL_REG_STEP_COUNTER_L              (0x4B)
#define MGOS_LSM6DSL_REG_STEP_COUNTER_H              (0x4C)
#define MGOS_LSM6DSL_REG_SENSORHUB13_REG             (0x4D)
#define MGOS_LSM6DSL_REG_SENSORHUB14_REG             (0x4E)
#define MGOS_LSM6DSL_REG_SENSORHUB15_REG             (0x4F)
#define MGOS_LSM6DSL_REG_SENSORHUB16_REG             (0x50)
#define MGOS_LSM6DSL_REG_SENSORHUB17_REG             (0x51)
#define MGOS_LSM6DSL_REG_SENSORHUB18_REG             (0x52)
#define MGOS_LSM6DSL_REG_FUNC_SRC1                   (0x53)
#define MGOS_LSM6DSL_REG_FUNC_SRC2                   (0x54)
#define MGOS_LSM6DSL_REG_WRIST_TILT_IA               (0x55)
#define MGOS_LSM6DSL_REG_TAP_CFG                     (0x58)
#define MGOS_LSM6DSL_REG_TAP_THS_6D                  (0x59)
#define MGOS_LSM6DSL_REG_INT_DUR2                    (0x5A)
#define MGOS_LSM6DSL_REG_WAKE_UP_THS                 (0x5B)
#define MGOS_LSM6DSL_REG_WAKE_UP_DUR                 (0x5C)
#define MGOS_LSM6DSL_REG_FREE_FALL                   (0x5D)
#define MGOS_LSM6DSL_REG_MD1_CFG                     (0x5E)
#define MGOS_LSM6DSL_REG_MD2_CFG                     (0x5F)
#define MGOS_LSM6DSL_REG_MASTER_CMD_CODE             (0x60)
#define MGOS_LSM6DSL_REG_SENS_SYNC_SPI_ERROR_CODE    (0x61)
#define MGOS_LSM6DSL_REG_OUT_MAG_RAW_X_L             (0x66)
#define MGOS_LSM6DSL_REG_OUT_MAG_RAW_X_H             (0x67)
#define MGOS_LSM6DSL_REG_OUT_MAG_RAW_Y_L             (0x68)
#define MGOS_LSM6DSL_REG_OUT_MAG_RAW_Y_H             (0x69)
#define MGOS_LSM6DSL_REG_OUT_MAG_RAW_Z_L             (0x6A)
#define MGOS_LSM6DSL_REG_OUT_MAG_RAW_Z_H             (0x6B)
#define MGOS_LSM6DSL_REG_X_OFS_USR                   (0x73)
#define MGOS_LSM6DSL_REG_Y_OFS_USR                   (0x74)
#define MGOS_LSM6DSL_REG_Z_OFS_USR                   (0x75)

// Interrupt handler will receive.
// int_mask will be a combination of MGOS_LSM6DSL_INT* bits defined below.
typedef void (*mgos_imu_lsm6dsl_int_cb)(struct mgos_imu *imu, uint32_t int_mask, void *user_data);

struct mgos_imu_lsm6dsl_userdata {
  bool                    initialized;
  mgos_imu_lsm6dsl_int_cb int_cb;
  void *                  int_cb_user_data;
  int                     int_gpio;
};

struct mgos_imu_lsm6dsl_userdata *mgos_imu_lsm6dsl_userdata_create(void);

bool mgos_imu_lsm6dsl_acc_detect(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_lsm6dsl_acc_create(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_lsm6dsl_acc_read(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_lsm6dsl_acc_get_scale(struct mgos_imu_acc *dev, void *imu_user_data, float *scale);
bool mgos_imu_lsm6dsl_acc_set_scale(struct mgos_imu_acc *dev, void *imu_user_data, float scale);
bool mgos_imu_lsm6dsl_acc_get_odr(struct mgos_imu_acc *dev, void *imu_user_data, float *odr);
bool mgos_imu_lsm6dsl_acc_set_odr(struct mgos_imu_acc *dev, void *imu_user_data, float odr);

bool mgos_imu_lsm6dsl_gyro_detect(struct mgos_imu_gyro *dev, void *imu_user_data);
bool mgos_imu_lsm6dsl_gyro_create(struct mgos_imu_gyro *dev, void *imu_user_data);
bool mgos_imu_lsm6dsl_gyro_read(struct mgos_imu_gyro *dev, void *imu_user_data);

// Interrupts
#define MGOS_LSM6DSL_INT_DRDY_XL        (1 << 0)
#define MGOS_LSM6DSL_INT_DRDY_G         (1 << 1)
#define MGOS_LSM6DSL_INT_FIFO_THR       (1 << 3)
#define MGOS_LSM6DSL_INT_FIFO_OVR       (1 << 4)
#define MGOS_LSM6DSL_INT_FIFO_FULL      (1 << 5)
#define MGOS_LSM6DSL_INT_TIMER          (1 << 8)
#define MGOS_LSM6DSL_INT_TILT           (1 << 9)
#define MGOS_LSM6DSL_INT_D6D            (1 << 10)
#define MGOS_LSM6DSL_INT_DBL_TAP        (1 << 11)
#define MGOS_LSM6DSL_INT_FF             (1 << 12)
#define MGOS_LSM6DSL_INT_WU             (1 << 13)
#define MGOS_LSM6DSL_INT_TAP            (1 << 14)
#define MGOS_LSM6DSL_INT_INACT          (1 << 15)
// These interrupt sources are only available on INT1 or INT2 pin.
#define MGOS_LSM6DSL_INT1_BOOT          (1 << 2)
#define MGOS_LSM6DSL_INT1_SIGN_MOT      (1 << 6)
#define MGOS_LSM6DSL_INT1_STEP_DET      (1 << 7)
#define MGOS_LSM6DSL_INT2_DRDY_TEMP     (1 << 18)
#define MGOS_LSM6DSL_INT2_STEP_OVR      (1 << 22)
#define MGOS_LSM6DSL_INT2_STEP_DELTA    (1 << 23)

// Set INT1 and/or INT2 interrupt handlers.
// One or both can be specified, unused should be set to -1.
// If both pins are set and are the same, will configure the device to send INT2 interrupts to INT1.
bool mgos_imu_lsm6dsl_set_int_handler(
  struct mgos_imu *imu, int int1_gpio, int int2_gpio,
  mgos_imu_lsm6dsl_int_cb cb, void *user_data);

// Enable/disable specific interrupt sources.
bool mgos_imu_lsm6dsl_int1_enable(struct mgos_imu *imu, uint32_t int1_mask);
bool mgos_imu_lsm6dsl_int1_disable(struct mgos_imu *imu, uint32_t int1_mask);
bool mgos_imu_lsm6dsl_int2_enable(struct mgos_imu *imu, uint32_t int2_mask);
bool mgos_imu_lsm6dsl_int2_disable(struct mgos_imu *imu, uint32_t int2_mask);

// Read and clear all the interrupt sources.
// *res contains mask of MGOS_LSM6DSL_INT_* values.
bool mgos_imu_lsm6dsl_get_and_clear_ints(struct mgos_imu *imu, uint32_t *res);
