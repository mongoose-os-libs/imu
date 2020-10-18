/*
 * Copyright (c) 2020 Sergio R. Caprile and Cika Electronica S.R.L.
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "mgos.h"
#include "mgos_imu_internal.h"

#define MGOS_MPU6886_DEFAULT_I2CADDR         (0x68)
#define MGOS_MPU6886_DEVID                   (0x19)

#define MGOS_MPU6886_GYRO_OFFSET             (0x13)
#define MGOS_MPU6886_ACCEL_CONFIG2           (0x1D)
#define MGOS_MPU6886_ACCEL_INTEL_CTRL        (0x69)


bool mgos_imu_mpu6886_acc_detect(struct mgos_imu_acc *dev, void *imu_user_data);

bool mgos_imu_mpu6886_gyro_detect(struct mgos_imu_gyro *dev,
                                  void *imu_user_data);
