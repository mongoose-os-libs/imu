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

#define MGOS_MMA8451_DEFAULT_I2CADDR         (0x68)
#define MGOS_MMA8451_DEVID                   (0x1A)

// MMA8451 -- Registers
#define MGOS_MMA8451_REG_STATUS              (0x00)
#define MGOS_MMA8451_REG_OUT_X_MSB           (0x01)
#define MGOS_MMA8451_REG_OUT_X_LSB           (0x02)
#define MGOS_MMA8451_REG_OUT_Y_MSB           (0x03)
#define MGOS_MMA8451_REG_OUT_Y_LSB           (0x04)
#define MGOS_MMA8451_REG_OUT_Z_MSB           (0x05)
#define MGOS_MMA8451_REG_OUT_Z_LSB           (0x06)
#define MGOS_MMA8451_REG_F_SETUP             (0x09)
#define MGOS_MMA8451_REG_TRIG_CFG            (0x0A)
#define MGOS_MMA8451_REG_SYSMOD              (0x0B)
#define MGOS_MMA8451_REG_INT_SOURCE          (0x0C)
#define MGOS_MMA8451_REG_WHO_AM_I            (0x0D)
#define MGOS_MMA8451_REG_XYZ_DATA_CFG        (0x0E)
#define MGOS_MMA8451_REG_HP_FILTER_CUTOFF    (0x0F)
#define MGOS_MMA8451_REG_PL_STATUS           (0x10)
#define MGOS_MMA8451_REG_PL_CFG              (0x11)
#define MGOS_MMA8451_REG_PL_COUNT            (0x12)
#define MGOS_MMA8451_REG_PL_BF_ZCOMP         (0x13)
#define MGOS_MMA8451_REG_P_L_THS_REG         (0x14)
#define MGOS_MMA8451_REG_FF_MT_CFG           (0x15)
#define MGOS_MMA8451_REG_FF_MT_SRC           (0x16)
#define MGOS_MMA8451_REG_FF_MT_THS           (0x17)
#define MGOS_MMA8451_REG_FF_MT_COUNT         (0x18)
#define MGOS_MMA8451_REG_TRANSIENT_CFG       (0x1D)
#define MGOS_MMA8451_REG_TRANSIENT_SCR       (0x1E)
#define MGOS_MMA8451_REG_TRANSIENT_THS       (0x1F)
#define MGOS_MMA8451_REG_TRANSIENT_COUNT     (0x20)
#define MGOS_MMA8451_REG_PULSE_CFG           (0x21)
#define MGOS_MMA8451_REG_PULSE_SRC           (0x22)
#define MGOS_MMA8451_REG_PULSE_THSX          (0x23)
#define MGOS_MMA8451_REG_PULSE_THSY          (0x24)
#define MGOS_MMA8451_REG_PULSE_THSZ          (0x25)
#define MGOS_MMA8451_REG_PULSE_TMLT          (0x26)
#define MGOS_MMA8451_REG_PULSE_LTCY          (0x27)
#define MGOS_MMA8451_REG_PULSE_WIND          (0x28)
#define MGOS_MMA8451_REG_ASLP_COUNT          (0x29)
#define MGOS_MMA8451_REG_CTRL_REG1           (0x2A)
#define MGOS_MMA8451_REG_CTRL_REG2           (0x2B)
#define MGOS_MMA8451_REG_CTRL_REG3           (0x2C)
#define MGOS_MMA8451_REG_CTRL_REG4           (0x2D)
#define MGOS_MMA8451_REG_CTRL_REG5           (0x2E)
#define MGOS_MMA8451_REG_OFF_X               (0x2F)
#define MGOS_MMA8451_REG_OFF_Y               (0x30)
#define MGOS_MMA8451_REG_OFF_Z               (0x31)

bool mgos_imu_mma8451_detect(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_mma8451_create(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_mma8451_read(struct mgos_imu_acc *dev, void *imu_user_data);
