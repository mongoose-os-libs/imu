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

#define MGOS_L3GD20_DEFAULT_I2CADDR      (0x68)
#define MGOS_L3GD20_DEVID                (0xD4)
#define MGOS_L3GD20H_DEVID               (0xD7)

// L3GD20 Registers
#define MGOS_L3GD20_REG_WHO_AM_I         (0x0F)
#define MGOS_L3GD20_REG_CTRL_REG1        (0x20)
#define MGOS_L3GD20_REG_CTRL_REG2        (0x21)
#define MGOS_L3GD20_REG_CTRL_REG3        (0x22)
#define MGOS_L3GD20_REG_CTRL_REG4        (0x23)
#define MGOS_L3GD20_REG_CTRL_REG5        (0x24)
#define MGOS_L3GD20_REG_DATACAPTURE      (0x25)
#define MGOS_L3GD20_REG_OUT_TEMP         (0x26)
#define MGOS_L3GD20_REG_STATUS_REG       (0x27)
#define MGOS_L3GD20_REG_OUT_X_L          (0x28)
#define MGOS_L3GD20_REG_OUT_X_H          (0x29)
#define MGOS_L3GD20_REG_OUT_Y_L          (0x2A)
#define MGOS_L3GD20_REG_OUT_Y_H          (0x2B)
#define MGOS_L3GD20_REG_OUT_Z_L          (0x2C)
#define MGOS_L3GD20_REG_OUT_Z_H          (0x2D)
#define MGOS_L3GD20_REG_FIFO_CTRL_REG    (0x2E)
#define MGOS_L3GD20_REG_FIFO_SRC_REG     (0x2F)
#define MGOS_L3GD20_REG_INT1_CFG         (0x30)
#define MGOS_L3GD20_REG_INT1_SRC         (0x31)
#define MGOS_L3GD20_REG_INT1_THS_XH      (0x32)
#define MGOS_L3GD20_REG_INT1_THS_XL      (0x33)
#define MGOS_L3GD20_REG_INT1_THS_YH      (0x34)
#define MGOS_L3GD20_REG_INT1_THS_YL      (0x35)
#define MGOS_L3GD20_REG_INT1_THS_ZH      (0x36)
#define MGOS_L3GD20_REG_INT1_THS_ZL      (0x37)
#define MGOS_L3GD20_REG_INT1_DURATION    (0x38)

bool mgos_imu_l3gd20_detect(struct mgos_imu_gyro *dev, void *imu_user_data);
bool mgos_imu_l3gd20_create(struct mgos_imu_gyro *dev, void *imu_user_data);
bool mgos_imu_l3gd20_read(struct mgos_imu_gyro *dev, void *imu_user_data);
