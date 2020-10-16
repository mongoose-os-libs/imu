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

#define MGOS_BMM150_DEFAULT_I2CADDR    (0x10)
#define MGOS_BMM150_DEVID              (0x32)

// BMM150 Magnetometer Registers
#define MGOS_BMM150_REG_CHIPID         (0x40)
#define MGOS_BMM150_REG_OUT_X_LSB      (0x42)
#define MGOS_BMM150_REG_OUT_X_MSB      (0x43)
#define MGOS_BMM150_REG_OUT_Y_LSB      (0x44)
#define MGOS_BMM150_REG_OUT_Y_MSB      (0x45)
#define MGOS_BMM150_REG_OUT_Z_LSB      (0x46)
#define MGOS_BMM150_REG_OUT_Z_MSB      (0x47)
#define MGOS_BMM150_REG_RHALL_LSB      (0x48)
#define MGOS_BMM150_REG_RHALL_MSB      (0x49)
#define MGOS_BMM150_REG_INTSTATUS      (0x4A)
#define MGOS_BMM150_REG_POWMODE        (0x4B)
#define MGOS_BMM150_REG_OPMODE         (0x4C)
#define MGOS_BMM150_REG_REPXY          (0x51)
#define MGOS_BMM150_REG_REPZ           (0x52)

// BMM150 Magnetometer Trim Extended Registers
#define MGOS_BMM150_DIG_X1             (0x5D)
#define MGOS_BMM150_DIG_Y1             (0x5E)
#define MGOS_BMM150_DIG_Z4_LSB         (0x62)
#define MGOS_BMM150_DIG_Z4_MSB         (0x63)
#define MGOS_BMM150_DIG_X2             (0x64)
#define MGOS_BMM150_DIG_Y2             (0x65)
#define MGOS_BMM150_DIG_Z2_LSB         (0x68)
#define MGOS_BMM150_DIG_Z2_MSB         (0x69)
#define MGOS_BMM150_DIG_Z1_LSB         (0x6A)
#define MGOS_BMM150_DIG_Z1_MSB         (0x6B)
#define MGOS_BMM150_DIG_XYZ1_LSB       (0x6C)
#define MGOS_BMM150_DIG_XYZ1_MSB       (0x6D)
#define MGOS_BMM150_DIG_Z3_LSB         (0x6E)
#define MGOS_BMM150_DIG_Z3_MSB         (0x6F)
#define MGOS_BMM150_DIG_XY2            (0x70)
#define MGOS_BMM150_DIG_XY1            (0x71)

// OVERFLOW DEFINITIONS
#define MGOS_BMM150_OVERFLOW_ADCVAL_XYAXES_FLIP        (-4096)
#define MGOS_BMM150_OVERFLOW_ADCVAL_ZAXIS_HALL         (-16384)
#define MGOS_BMM150_OVERFLOW_OUTPUT                    (-32768)
#define MGOS_BMM150_NEGATIVE_SATURATION_Z              (-32767)
#define MGOS_BMM150_POSITIVE_SATURATION_Z              (32767)


bool mgos_imu_bmm150_detect(struct mgos_imu_mag *dev, void *imu_user_data);
bool mgos_imu_bmm150_create(struct mgos_imu_mag *dev, void *imu_user_data);
bool mgos_imu_bmm150_read(struct mgos_imu_mag *dev, void *imu_user_data);
