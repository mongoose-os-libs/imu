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
#include "mgos_imu_internal.h"

#define MGOS_ADXL345_DEFAULT_I2CADDR    (0x53)
#define MGOS_ADXL345_DEVID              (0xE5)

// ADXL345 -- Registers
#define MGOS_ADXL345_REG_WHO_AM_I       (0x00)
#define MGOS_ADXL345_REG_BW_RATE        (0x2C)
#define MGOS_ADXL345_REG_POWER_CTL      (0x2D)
#define MGOS_ADXL345_REG_INT_ENABLE     (0x2E)
#define MGOS_ADXL345_REG_DATA_FORMAT    (0x31)
#define MGOS_ADXL345_REG_DATA_OUT       (0x32)
#define MGOS_ADXL345_REG_FIFO_CTL       (0x38)

bool mgos_imu_adxl345_detect(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_adxl345_create(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_adxl345_read(struct mgos_imu_acc *dev, void *imu_user_data);
