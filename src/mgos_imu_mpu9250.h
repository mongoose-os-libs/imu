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
#define MGOS_AK8963_DEFAULT_I2CADDR         (0x0C)

#define MGOS_MPU9250_DEVID_9250             (0x71)
#define MGOS_MPU9250_DEVID_9255             (0x73)
#define MGOS_MPU9250_DEVID_AK8963           (0x48)

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

// AK8963 Companion -- Magnetometer Registers
#define MGOS_MPU9250_REG_AK8963_WHO_AM_I    (0x00)
#define MGOS_MPU9250_REG_AK8963_ST1         (0x02)
#define MGOS_MPU9250_REG_AK8963_XOUT_L      (0x03)
#define MGOS_MPU9250_REG_AK8963_CNTL        (0x0A)
#define MGOS_MPU9250_REG_AK8963_ASAX        (0x10)
#define MGOS_MPU9250_REG_AK8963_ASAY        (0x11)
#define MGOS_MPU9250_REG_AK8963_ASAZ        (0x12)

bool mgos_imu_mpu9250_acc_detect(struct mgos_imu_acc *dev);
bool mgos_imu_mpu9250_acc_create(struct mgos_imu_acc *dev);
bool mgos_imu_mpu9250_acc_destroy(struct mgos_imu_acc *dev);
bool mgos_imu_mpu9250_acc_read(struct mgos_imu_acc *dev);

bool mgos_imu_mpu9250_gyro_detect(struct mgos_imu_gyro *dev);
bool mgos_imu_mpu9250_gyro_create(struct mgos_imu_gyro *dev);
bool mgos_imu_mpu9250_gyro_destroy(struct mgos_imu_gyro *dev);
bool mgos_imu_mpu9250_gyro_read(struct mgos_imu_gyro *dev);

bool mgos_imu_mpu9250_mag_detect(struct mgos_imu_mag *dev);
bool mgos_imu_mpu9250_mag_create(struct mgos_imu_mag *dev);
bool mgos_imu_mpu9250_mag_destroy(struct mgos_imu_mag *dev);
bool mgos_imu_mpu9250_mag_read(struct mgos_imu_mag *dev);
