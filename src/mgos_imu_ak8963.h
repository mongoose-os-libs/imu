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

#define MGOS_AK8963_DEFAULT_I2CADDR    (0x0C)
#define MGOS_AK8963_DEVID              (0x48)

// AK8963 Magnetometer Registers
#define MGOS_AK8963_REG_WHO_AM_I       (0x00)
#define MGOS_AK8963_REG_ST1            (0x02)
#define MGOS_AK8963_REG_XOUT_L         (0x03)
#define MGOS_AK8963_REG_CNTL           (0x0A)
#define MGOS_AK8963_REG_ASAX           (0x10)
#define MGOS_AK8963_REG_ASAY           (0x11)
#define MGOS_AK8963_REG_ASAZ           (0x12)

bool mgos_imu_ak8963_detect(struct mgos_imu_mag *dev, void *imu_user_data);
bool mgos_imu_ak8963_create(struct mgos_imu_mag *dev, void *imu_user_data);
bool mgos_imu_ak8963_read(struct mgos_imu_mag *dev, void *imu_user_data);
