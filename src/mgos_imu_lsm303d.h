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

#define MGOS_LSM303D_DEFAULT_I2CADDR     (0x1d)
#define MGOS_LSM303D_DEVID               (0x49)
#define MGOS_LSM303DLM_DEVID             (0x3c)

// LSM303D -- Registers
#define MGOS_LSM303D_REG_TEMP_OUT_L      (0x05)
#define MGOS_LSM303D_REG_TEMP_OUT_H      (0x06)
#define MGOS_LSM303D_REG_STATUS_M        (0x07)
#define MGOS_LSM303D_REG_OUT_X_L_M       (0x08)
#define MGOS_LSM303D_REG_OUT_X_H_M       (0x09)
#define MGOS_LSM303D_REG_OUT_Y_L_M       (0x0A)
#define MGOS_LSM303D_REG_OUT_Y_H_M       (0x0B)
#define MGOS_LSM303D_REG_OUT_Z_L_M       (0x0C)
#define MGOS_LSM303D_REG_OUT_Z_H_M       (0x0D)
#define MGOS_LSM303D_REG_WHO_AM_I        (0x0F)
#define MGOS_LSM303D_REG_INT_CTRL_M      (0x12)
#define MGOS_LSM303D_REG_INT_SRC_M       (0x13)
#define MGOS_LSM303D_REG_INT_THS_L_M     (0x14)
#define MGOS_LSM303D_REG_INT_THS_H_M     (0x15)
#define MGOS_LSM303D_REG_OFFSET_X_L_M    (0x16)
#define MGOS_LSM303D_REG_OFFSET_X_H_M    (0x17)
#define MGOS_LSM303D_REG_OFFSET_Y_L_M    (0x18)
#define MGOS_LSM303D_REG_OFFSET_Y_H_M    (0x19)
#define MGOS_LSM303D_REG_OFFSET_Z_L_M    (0x1A)
#define MGOS_LSM303D_REG_OFFSET_Z_H_M    (0x1B)
#define MGOS_LSM303D_REG_REFERENCE_X     (0x1C)
#define MGOS_LSM303D_REG_REFERENCE_Y     (0x1D)
#define MGOS_LSM303D_REG_REFERENCE_Z     (0x1E)
#define MGOS_LSM303D_REG_CTRL0           (0x1F)
#define MGOS_LSM303D_REG_CTRL1           (0x20)
#define MGOS_LSM303D_REG_CTRL2           (0x21)
#define MGOS_LSM303D_REG_CTRL3           (0x22)
#define MGOS_LSM303D_REG_CTRL4           (0x23)
#define MGOS_LSM303D_REG_CTRL5           (0x24)
#define MGOS_LSM303D_REG_CTRL6           (0x25)
#define MGOS_LSM303D_REG_CTRL7           (0x26)
#define MGOS_LSM303D_REG_STATUS_A        (0x27)
#define MGOS_LSM303D_REG_OUT_X_L_A       (0x28)
#define MGOS_LSM303D_REG_OUT_X_H_A       (0x29)
#define MGOS_LSM303D_REG_OUT_Y_L_A       (0x2A)
#define MGOS_LSM303D_REG_OUT_Y_H_A       (0x2B)
#define MGOS_LSM303D_REG_OUT_Z_L_A       (0x2C)
#define MGOS_LSM303D_REG_OUT_Z_H_A       (0x2D)
#define MGOS_LSM303D_REG_FIFO_CTRL       (0x2E)
#define MGOS_LSM303D_REG_FIFO_SRC        (0x2F)
#define MGOS_LSM303D_REG_IG_CFG1         (0x30)
#define MGOS_LSM303D_REG_IG_SRC1         (0x31)
#define MGOS_LSM303D_REG_IG_THS1         (0x32)
#define MGOS_LSM303D_REG_IG_DUR1         (0x33)
#define MGOS_LSM303D_REG_IG_CFG2         (0x34)
#define MGOS_LSM303D_REG_IG_SRC2         (0x35)
#define MGOS_LSM303D_REG_IG_THS2         (0x36)
#define MGOS_LSM303D_REG_IG_DUR2         (0x37)
#define MGOS_LSM303D_REG_CLICK_CFG       (0x38)
#define MGOS_LSM303D_REG_CLICK_SRC       (0x39)
#define MGOS_LSM303D_REG_CLICK_THS       (0x3A)
#define MGOS_LSM303D_REG_TIME_LIMIT      (0x3B)
#define MGOS_LSM303D_REG_TIME_LATENCY    (0x3C)
#define MGOS_LSM303D_REG_TIME_WINDOW     (0x3D)
#define MGOS_LSM303D_REG_ACT_THS         (0x3E)
#define MGOS_LSM303D_REG_ACT_DUR         (0x3F)

struct mgos_imu_lsm303d_userdata {
  bool initialized;
};

struct mgos_imu_lsm303d_userdata *mgos_imu_lsm303d_userdata_create(void);

bool mgos_imu_lsm303d_acc_detect(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_lsm303d_acc_create(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_lsm303d_acc_read(struct mgos_imu_acc *dev, void *imu_user_data);

bool mgos_imu_lsm303d_mag_detect(struct mgos_imu_mag *dev, void *imu_user_data);
bool mgos_imu_lsm303d_mag_create(struct mgos_imu_mag *dev, void *imu_user_data);
bool mgos_imu_lsm303d_mag_read(struct mgos_imu_mag *dev, void *imu_user_data);
