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

#include "mgos.h"
#include "mgos_imu_internal.h"

#define MGOS_LSM9DS1_DEFAULT_I2CADDR         (0x6b)
#define MGOS_LSM9DS1_DEFAULT_M_I2CADDR       (0x1e)
#define MGOS_LSM9DS1_DEVID                   (0x68)
#define MGOS_LSM9DS1_DEVID_M                 (0x3d)

// LSM9DS1 -- Registers (Acc/Gyro)
#define MGOS_LSM9DS1_REG_ACT_THS             (0x04)
#define MGOS_LSM9DS1_REG_ACT_DUR             (0x05)
#define MGOS_LSM9DS1_REG_INT_GEN_CFG_XL      (0x06)
#define MGOS_LSM9DS1_REG_INT_GEN_THS_X_XL    (0x07)
#define MGOS_LSM9DS1_REG_INT_GEN_THS_Y_XL    (0x08)
#define MGOS_LSM9DS1_REG_INT_GEN_THS_Z_XL    (0x09)
#define MGOS_LSM9DS1_REG_INT_GEN_DUR_XL      (0x0A)
#define MGOS_LSM9DS1_REG_REFERENCE_G         (0x0B)
#define MGOS_LSM9DS1_REG_INT1_CTRL           (0x0C)
#define MGOS_LSM9DS1_REG_INT2_CTRL           (0x0D)
#define MGOS_LSM9DS1_REG_WHO_AM_I            (0x0F)
#define MGOS_LSM9DS1_REG_CTRL_REG1_G         (0x10)
#define MGOS_LSM9DS1_REG_CTRL_REG2_G         (0x11)
#define MGOS_LSM9DS1_REG_CTRL_REG3_G         (0x12)
#define MGOS_LSM9DS1_REG_ORIENT_CFG_G        (0x13)
#define MGOS_LSM9DS1_REG_INT_GEN_SRC_G       (0x14)
#define MGOS_LSM9DS1_REG_OUT_TEMP_L          (0x15)
#define MGOS_LSM9DS1_REG_OUT_TEMP_H          (0x16)
#define MGOS_LSM9DS1_REG_STATUS1_REG         (0x17)
#define MGOS_LSM9DS1_REG_OUT_X_L_G           (0x18)
#define MGOS_LSM9DS1_REG_OUT_X_H_G           (0x19)
#define MGOS_LSM9DS1_REG_OUT_Y_L_G           (0x1A)
#define MGOS_LSM9DS1_REG_OUT_Y_H_G           (0x1B)
#define MGOS_LSM9DS1_REG_OUT_Z_L_G           (0x1C)
#define MGOS_LSM9DS1_REG_OUT_Z_H_G           (0x1D)
#define MGOS_LSM9DS1_REG_CTRL_REG4           (0x1E)
#define MGOS_LSM9DS1_REG_CTRL_REG5_XL        (0x1F)
#define MGOS_LSM9DS1_REG_CTRL_REG6_XL        (0x20)
#define MGOS_LSM9DS1_REG_CTRL_REG7_XL        (0x21)
#define MGOS_LSM9DS1_REG_CTRL_REG8           (0x22)
#define MGOS_LSM9DS1_REG_CTRL_REG9           (0x23)
#define MGOS_LSM9DS1_REG_CTRL_REG10          (0x24)
#define MGOS_LSM9DS1_REG_INT_GEN_SRC_XL      (0x26)
#define MGOS_LSM9DS1_REG_STATUS2_REG         (0x27)
#define MGOS_LSM9DS1_REG_OUT_X_L_XL          (0x28)
#define MGOS_LSM9DS1_REG_OUT_X_H_XL          (0x29)
#define MGOS_LSM9DS1_REG_OUT_Y_L_XL          (0x2A)
#define MGOS_LSM9DS1_REG_OUT_Y_H_XL          (0x2B)
#define MGOS_LSM9DS1_REG_OUT_Z_L_XL          (0x2C)
#define MGOS_LSM9DS1_REG_OUT_Z_H_XL          (0x2D)
#define MGOS_LSM9DS1_REG_FIFO_CTRL           (0x2E)
#define MGOS_LSM9DS1_REG_FIFO_SRC            (0x2F)
#define MGOS_LSM9DS1_REG_INT_GEN_CFG_G       (0x30)
#define MGOS_LSM9DS1_REG_INT_GEN_THS_XH_G    (0x31)
#define MGOS_LSM9DS1_REG_INT_GEN_THS_XL_G    (0x32)
#define MGOS_LSM9DS1_REG_INT_GEN_THS_YH_G    (0x33)
#define MGOS_LSM9DS1_REG_INT_GEN_THS_YL_G    (0x34)
#define MGOS_LSM9DS1_REG_INT_GEN_THS_ZH_G    (0x35)
#define MGOS_LSM9DS1_REG_INT_GEN_THS_ZL_G    (0x36)
#define MGOS_LSM9DS1_REG_INT_GEN_DUR_G       (0x37)

// LSM9DS1 -- Registers (Mag)
#define MGOS_LSM9DS1_REG_OFFSET_X_REG_L_M    (0x05)
#define MGOS_LSM9DS1_REG_OFFSET_X_REG_H_M    (0x06)
#define MGOS_LSM9DS1_REG_OFFSET_Y_REG_L_M    (0x07)
#define MGOS_LSM9DS1_REG_OFFSET_Y_REG_H_M    (0x08)
#define MGOS_LSM9DS1_REG_OFFSET_Z_REG_L_M    (0x09)
#define MGOS_LSM9DS1_REG_OFFSET_Z_REG_H_M    (0x0A)
#define MGOS_LSM9DS1_REG_WHO_AM_I_M          (0x0F)
#define MGOS_LSM9DS1_REG_CTRL_REG1_M         (0x20)
#define MGOS_LSM9DS1_REG_CTRL_REG2_M         (0x21)
#define MGOS_LSM9DS1_REG_CTRL_REG3_M         (0x22)
#define MGOS_LSM9DS1_REG_CTRL_REG4_M         (0x23)
#define MGOS_LSM9DS1_REG_CTRL_REG5_M         (0x24)
#define MGOS_LSM9DS1_REG_STATUS_REG_M        (0x27)
#define MGOS_LSM9DS1_REG_OUT_X_L_M           (0x28)
#define MGOS_LSM9DS1_REG_OUT_X_H_M           (0x29)
#define MGOS_LSM9DS1_REG_OUT_Y_L_M           (0x2A)
#define MGOS_LSM9DS1_REG_OUT_Y_H_M           (0x2B)
#define MGOS_LSM9DS1_REG_OUT_Z_L_M           (0x2C)
#define MGOS_LSM9DS1_REG_OUT_Z_H_M           (0x2D)
#define MGOS_LSM9DS1_REG_INT_CFG_M           (0x30)
#define MGOS_LSM9DS1_REG_INT_SRC_M           (0x31)
#define MGOS_LSM9DS1_REG_INT_THS_L_M         (0x32)
#define MGOS_LSM9DS1_REG_INT_THS_H_M         (0x33)

struct mgos_imu_lsm9ds1_userdata {
  bool accgyro_initialized;
};

struct mgos_imu_lsm9ds1_userdata *mgos_imu_lsm9ds1_userdata_create(void);

bool mgos_imu_lsm9ds1_acc_detect(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_lsm9ds1_acc_create(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_lsm9ds1_acc_read(struct mgos_imu_acc *dev, void *imu_user_data);

bool mgos_imu_lsm9ds1_gyro_detect(struct mgos_imu_gyro *dev, void *imu_user_data);
bool mgos_imu_lsm9ds1_gyro_create(struct mgos_imu_gyro *dev, void *imu_user_data);
bool mgos_imu_lsm9ds1_gyro_read(struct mgos_imu_gyro *dev, void *imu_user_data);

bool mgos_imu_lsm9ds1_mag_detect(struct mgos_imu_mag *dev, void *imu_user_data);
bool mgos_imu_lsm9ds1_mag_create(struct mgos_imu_mag *dev, void *imu_user_data);
bool mgos_imu_lsm9ds1_mag_read(struct mgos_imu_mag *dev, void *imu_user_data);
