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

// MPU6000 allows for both I2C and SPI, while MPU6050 has only I2C.
#define MGOS_MPU60X0_DEFAULT_I2CADDR         (0x68)
#define MGOS_MPU60X0_DEVID                   (0x68)

#define MGOS_MPU60X0_REG_SELF_TEST_X         (0x0D)
#define MGOS_MPU60X0_REG_SELF_TEST_Y         (0x0E)
#define MGOS_MPU60X0_REG_SELF_TEST_Z         (0x0F)
#define MGOS_MPU60X0_REG_SELF_TEST_A         (0x10)
#define MGOS_MPU60X0_REG_SMPLRT_DIV          (0x19)
#define MGOS_MPU60X0_REG_CONFIG              (0x1A)
#define MGOS_MPU60X0_REG_GYRO_CONFIG         (0x1B)
#define MGOS_MPU60X0_REG_ACCEL_CONFIG        (0x1C)
#define MGOS_MPU60X0_REG_FIFO_EN             (0x23)
#define MGOS_MPU60X0_REG_I2C_MST_CTRL        (0x24)
#define MGOS_MPU60X0_REG_I2C_SLV0_ADDR       (0x25)
#define MGOS_MPU60X0_REG_I2C_SLV0_REG        (0x26)
#define MGOS_MPU60X0_REG_I2C_SLV0_CTRL       (0x27)
#define MGOS_MPU60X0_REG_I2C_SLV1_ADDR       (0x28)
#define MGOS_MPU60X0_REG_I2C_SLV1_REG        (0x29)
#define MGOS_MPU60X0_REG_I2C_SLV1_CTRL       (0x2A)
#define MGOS_MPU60X0_REG_I2C_SLV2_ADDR       (0x2B)
#define MGOS_MPU60X0_REG_I2C_SLV2_REG        (0x2C)
#define MGOS_MPU60X0_REG_I2C_SLV2_CTRL       (0x2D)
#define MGOS_MPU60X0_REG_I2C_SLV3_ADDR       (0x2E)
#define MGOS_MPU60X0_REG_I2C_SLV3_REG        (0x2F)
#define MGOS_MPU60X0_REG_I2C_SLV3_CTRL       (0x30)
#define MGOS_MPU60X0_REG_I2C_SLV4_ADDR       (0x31)
#define MGOS_MPU60X0_REG_I2C_SLV4_REG        (0x32)
#define MGOS_MPU60X0_REG_I2C_SLV4_DO         (0x33)
#define MGOS_MPU60X0_REG_I2C_SLV4_CTRL       (0x34)
#define MGOS_MPU60X0_REG_I2C_SLV4_DI         (0x35)
#define MGOS_MPU60X0_REG_I2C_MST_STATUS      (0x36)
#define MGOS_MPU60X0_REG_INT_PIN_CFG         (0x37)
#define MGOS_MPU60X0_REG_INT_ENABLE          (0x38)
#define MGOS_MPU60X0_REG_INT_STATUS          (0x3A)
#define MGOS_MPU60X0_REG_ACCEL_XOUT_H        (0x3B)
#define MGOS_MPU60X0_REG_ACCEL_XOUT_L        (0x3C)
#define MGOS_MPU60X0_REG_ACCEL_YOUT_H        (0x3D)
#define MGOS_MPU60X0_REG_ACCEL_YOUT_L        (0x3E)
#define MGOS_MPU60X0_REG_ACCEL_ZOUT_H        (0x3F)
#define MGOS_MPU60X0_REG_ACCEL_ZOUT_L        (0x40)
#define MGOS_MPU60X0_REG_TEMP_OUT_H          (0x41)
#define MGOS_MPU60X0_REG_TEMP_OUT_L          (0x42)
#define MGOS_MPU60X0_REG_GYRO_XOUT_H         (0x43)
#define MGOS_MPU60X0_REG_GYRO_XOUT_L         (0x44)
#define MGOS_MPU60X0_REG_GYRO_YOUT_H         (0x45)
#define MGOS_MPU60X0_REG_GYRO_YOUT_L         (0x46)
#define MGOS_MPU60X0_REG_GYRO_ZOUT_H         (0x47)
#define MGOS_MPU60X0_REG_GYRO_ZOUT_L         (0x48)
#define MGOS_MPU60X0_REG_I2C_MST_DELAY_CT    (0x67)
#define MGOS_MPU60X0_REG_SIGNAL_PATH_RES     (0x68)
#define MGOS_MPU60X0_REG_USER_CTRL           (0x6A)
#define MGOS_MPU60X0_REG_PWR_MGMT_1          (0x6B)
#define MGOS_MPU60X0_REG_PWR_MGMT_2          (0x6C)
#define MGOS_MPU60X0_REG_FIFO_COUNTH         (0x72)
#define MGOS_MPU60X0_REG_FIFO_COUNTL         (0x73)
#define MGOS_MPU60X0_REG_FIFO_R_W            (0x74)
#define MGOS_MPU60X0_REG_WHO_AM_I            (0x75)

struct mgos_imu_mpu60x0_userdata {
  bool initialized;
};

struct mgos_imu_mpu60x0_userdata *mgos_imu_mpu60x0_userdata_create(void);

bool mgos_imu_mpu60x0_acc_detect(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_mpu60x0_acc_create(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_mpu60x0_acc_read(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_mpu60x0_acc_get_scale(struct mgos_imu_acc *dev,
                                    void *imu_user_data, float *scale);
bool mgos_imu_mpu60x0_acc_set_scale(struct mgos_imu_acc *dev,
                                    void *imu_user_data, float scale);

bool mgos_imu_mpu60x0_gyro_detect(struct mgos_imu_gyro *dev,
                                  void *imu_user_data);
bool mgos_imu_mpu60x0_gyro_create(struct mgos_imu_gyro *dev,
                                  void *imu_user_data);
bool mgos_imu_mpu60x0_gyro_read(struct mgos_imu_gyro *dev, void *imu_user_data);
bool mgos_imu_mpu60x0_gyro_get_scale(struct mgos_imu_gyro *dev,
                                     void *imu_user_data, float *scale);
bool mgos_imu_mpu60x0_gyro_set_scale(struct mgos_imu_gyro *dev,
                                     void *imu_user_data, float scale);
