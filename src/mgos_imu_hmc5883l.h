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

#define MGOS_HMC5883L_DEFAULT_I2CADDR    (0x0E)
#define MGOS_HMC5883L_DEVID              (0xC4)

// HMC5883L Magnetometer Registers
#define MGOS_HMC5883L_REG_CONF_A         (0x00)
#define MGOS_HMC5883L_REG_CONF_B         (0x01)
#define MGOS_HMC5883L_REG_MODE           (0x02)
#define MGOS_HMC5883L_REG_OUT_X_MSB      (0x03)
#define MGOS_HMC5883L_REG_OUT_X_LSB      (0x04)
#define MGOS_HMC5883L_REG_OUT_Y_MSB      (0x05)
#define MGOS_HMC5883L_REG_OUT_Y_LSB      (0x06)
#define MGOS_HMC5883L_REG_OUT_Z_MSB      (0x07)
#define MGOS_HMC5883L_REG_OUT_Z_LSB      (0x08)
#define MGOS_HMC5883L_REG_STATUS         (0x09)
#define MGOS_HMC5883L_REG_ID_A           (0x0a)
#define MGOS_HMC5883L_REG_ID_B           (0x0b)
#define MGOS_HMC5883L_REG_ID_C           (0x0c)

bool mgos_imu_hmc5883l_detect(struct mgos_imu_mag *dev, void *imu_user_data);
bool mgos_imu_hmc5883l_create(struct mgos_imu_mag *dev, void *imu_user_data);
bool mgos_imu_hmc5883l_read(struct mgos_imu_mag *dev, void *imu_user_data);
