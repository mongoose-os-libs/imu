/*
 * Copyright 2019 Vasily Kiniv
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

#define MGOS_ICM20948_DEFAULT_I2CADDR         (0x68)
#define MGOS_ICM20948_DEFAULT_M_I2CADDR       (0x0C)
#define MGOS_ICM20948_DEVID                   (0xEA)
#define MGOS_ICM20948_DEVID_M                 (0x09)

// ICM20948 -- Registers (Acc/Gyro)
#define MGOS_ICM20948_REG0_WHO_AM_I             (0x00)
#define MGOS_ICM20948_REG0_USER_CTRL            (0x03)
#define MGOS_ICM20948_REG0_LP_CONFIG            (0x05)
#define MGOS_ICM20948_REG0_PWR_MGMT_1           (0x06)
#define MGOS_ICM20948_REG0_PWR_MGMT_2           (0x07)
#define MGOS_ICM20948_REG0_INT_PIN_CFG          (0x0f)
#define MGOS_ICM20948_REG0_ACCEL_XOUT_H         (0x2d)
#define MGOS_ICM20948_REG0_GYRO_XOUT_H          (0x33)
#define MGOS_ICM20948_REG0_EXT_SLV_SENS_DATA_00 (0x3b)
#define MGOS_ICM20948_REG0_BANK_SEL             (0x7f)
#define MGOS_ICM20948_REG2_GYRO_SMPLRT_DIV      (0x00)
#define MGOS_ICM20948_REG2_GYRO_CONFIG_1        (0x01)
#define MGOS_ICM20948_REG2_ACCEL_SMPLRT_DIV_1   (0x10)
#define MGOS_ICM20948_REG2_ACCEL_SMPLRT_DIV_2   (0x11)
#define MGOS_ICM20948_REG2_ACCEL_CONFIG         (0x14)
#define MGOS_ICM20948_REG3_I2C_MST_CTRL         (0x01)
#define MGOS_ICM20948_REG3_I2C_SLV0_ADDR        (0x03)
#define MGOS_ICM20948_REG3_I2C_SLV0_REG         (0x04)
#define MGOS_ICM20948_REG3_I2C_SLV0_CTRL        (0x05)
#define MGOS_ICM20948_REG3_I2C_SLV0_DO          (0x06)

// ICM20948 -- Registers (Mag)
#define MGOS_ICM20948_WHO_AM_I_M                (0x01)
#define MGOS_ICM20948_HXL_M                     (0x11)
#define MGOS_ICM20948_ST2_M                     (0x18)
#define MGOS_ICM20948_CNTL2_M                   (0x31)
#define MGOS_ICM20948_CNTL3_M                   (0x32)

struct mgos_imu_icm20948_userdata {
  bool    accgyro_initialized;
  int8_t  current_bank_no;
};

struct mgos_imu_icm20948_userdata *mgos_imu_icm20948_userdata_create(void);

bool mgos_imu_icm20948_acc_detect(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_icm20948_acc_create(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_icm20948_acc_read(struct mgos_imu_acc *dev, void *imu_user_data);
bool mgos_imu_icm20948_acc_get_scale(struct mgos_imu_acc *dev, void *imu_user_data, float *scale);
bool mgos_imu_icm20948_acc_set_scale(struct mgos_imu_acc *dev, void *imu_user_data, float scale);
bool mgos_imu_icm20948_acc_get_odr(struct mgos_imu_acc *dev, void *imu_user_data, float *odr);
bool mgos_imu_icm20948_acc_set_odr(struct mgos_imu_acc *dev, void *imu_user_data, float odr);

bool mgos_imu_icm20948_gyro_detect(struct mgos_imu_gyro *dev, void *imu_user_data);
bool mgos_imu_icm20948_gyro_create(struct mgos_imu_gyro *dev, void *imu_user_data);
bool mgos_imu_icm20948_gyro_read(struct mgos_imu_gyro *dev, void *imu_user_data);
bool mgos_imu_icm20948_gyro_get_scale(struct mgos_imu_gyro *dev, void *imu_user_data, float *scale);
bool mgos_imu_icm20948_gyro_set_scale(struct mgos_imu_gyro *dev, void *imu_user_data, float scale);
bool mgos_imu_icm20948_gyro_get_odr(struct mgos_imu_gyro *dev, void *imu_user_data, float *odr);
bool mgos_imu_icm20948_gyro_set_odr(struct mgos_imu_gyro *dev, void *imu_user_data, float odr);

bool mgos_imu_icm20948_mag_detect(struct mgos_imu_mag *dev, void *imu_user_data);
bool mgos_imu_icm20948_mag_create(struct mgos_imu_mag *dev, void *imu_user_data);
bool mgos_imu_icm20948_mag_read(struct mgos_imu_mag *dev, void *imu_user_data);
bool mgos_imu_icm20948_mag_get_odr(struct mgos_imu_mag *dev, void *imu_user_data, float *odr);
bool mgos_imu_icm20948_mag_set_odr(struct mgos_imu_mag *dev, void *imu_user_data, float odr);
