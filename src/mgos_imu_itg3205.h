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

#define MGOS_ITG3205_DEFAULT_I2CADDR    (0x68)
#define MGOS_ITG3205_DEVID              (0x68)

// ITG3205 Registers
#define MGOS_ITG3205_REG_WHO_AM_I       (0x00)
#define MGOS_ITG3205_REG_SMPLRT_DIV     (0x15)
#define MGOS_ITG3205_REG_DLPF_FS        (0x16)
#define MGOS_ITG3205_REG_INT_CFG        (0x17)
#define MGOS_ITG3205_REG_INT_STATUS     (0x1A)
#define MGOS_ITG3205_REG_TEMP_OUT_H     (0x1B)
#define MGOS_ITG3205_REG_TEMP_OUT_L     (0x1C)
#define MGOS_ITG3205_REG_GYRO_XOUT_H    (0x1D)
#define MGOS_ITG3205_REG_GYRO_XOUT_L    (0x1E)
#define MGOS_ITG3205_REG_GYRO_YOUT_H    (0x1F)
#define MGOS_ITG3205_REG_GYRO_YOUT_L    (0x20)
#define MGOS_ITG3205_REG_GYRO_ZOUT_H    (0x21)
#define MGOS_ITG3205_REG_GYRO_ZOUT_L    (0x22)
#define MGOS_ITG3205_REG_PWR_MGM        (0x3E)

bool mgos_imu_itg3205_detect(struct mgos_imu_gyro *dev, void *imu_user_data);
bool mgos_imu_itg3205_create(struct mgos_imu_gyro *dev, void *imu_user_data);
bool mgos_imu_itg3205_read(struct mgos_imu_gyro *dev, void *imu_user_data);
