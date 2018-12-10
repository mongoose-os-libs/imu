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

#define MGOS_MPU9250_DEFAULT_I2CADDR        (0x68)

#define MGOS_MPU9250_DEVID_9250             (0x71)
#define MGOS_MPU9250_DEVID_9255             (0x73)

// MPU9250 -- Accelerometer and Gyro registers
#define MGOS_MPU9250_REG_SMPLRT_DIV         (0x19)
#define MGOS_MPU9250_REG_CONFIG             (0x1A)
#define MGOS_MPU9250_REG_GYRO_CONFIG        (0x1B)
#define MGOS_MPU9250_REG_ACCEL_CONFIG       (0x1C)
#define MGOS_MPU9250_REG_ACCEL_CONFIG2      (0x1D)
#define MGOS_MPU9250_REG_INT_PIN_CFG        (0x37)
#define MGOS_MPU9250_REG_INT_ENABLE         (0x38)
#define MGOS_MPU9250_REG_INT_STATUS         (0x3A)
#define MGOS_MPU9250_REG_ACCEL_XOUT_H       (0x3B)
#define MGOS_MPU9250_REG_TEMP_OUT_H         (0x41)
#define MGOS_MPU9250_REG_GYRO_XOUT_H        (0x43)
#define MGOS_MPU9250_REG_PWR_MGMT_1         (0x6B)
#define MGOS_MPU9250_REG_PWR_MGMT_2         (0x6C)
#define MGOS_MPU9250_REG_WHO_AM_I           (0x75)

#define MGOS_MPU9250_ACCEL_FS_SEL_2G        (0x00)
#define MGOS_MPU9250_ACCEL_FS_SEL_4G        (0x08)
#define MGOS_MPU9250_ACCEL_FS_SEL_8G        (0x10)
#define MGOS_MPU9250_ACCEL_FS_SEL_16G       (0x18)
#define MGOS_MPU9250_GYRO_FS_SEL_250DPS     (0x00)
#define MGOS_MPU9250_GYRO_FS_SEL_500DPS     (0x08)
#define MGOS_MPU9250_GYRO_FS_SEL_1000DPS    (0x10)
#define MGOS_MPU9250_GYRO_FS_SEL_2000DPS    (0x18)

struct mgos_imu_mpu9250_userdata {
  bool initialized;
};

struct mgos_imu_mpu9250_userdata *mgos_imu_mpu9250_userdata_create(void);

bool mgos_imu_mpu9250_acc_detect(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_mpu9250_acc_create(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_mpu9250_acc_read(struct mgos_imu_acc *dev, void *imu_user_data);

bool mgos_imu_mpu9250_gyro_detect(struct mgos_imu_gyro *dev, void *imu_user_data);
bool mgos_imu_mpu9250_gyro_create(struct mgos_imu_gyro *dev, void *imu_user_data);
bool mgos_imu_mpu9250_gyro_read(struct mgos_imu_gyro *dev, void *imu_user_data);
