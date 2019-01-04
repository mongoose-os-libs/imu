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

#include "mgos.h"
#include "mgos_i2c.h"
#include "mgos_imu_mpu925x.h"

static bool mgos_imu_mpu925x_detect(struct mgos_i2c *i2c, uint8_t i2caddr, uint8_t *devid) {
  int device_id;

  if (!i2c || !devid) {
    return false;
  }

  device_id = mgos_i2c_read_reg_b(i2c, i2caddr, MGOS_MPU9250_REG_WHO_AM_I);
  switch (device_id) {
  case MGOS_MPU9250_DEVID_9250:
    LOG(LL_INFO, ("Detected MPU9250 at I2C 0x%02x", i2caddr));
    *devid = MGOS_MPU9250_DEVID_9250;
    return true;

  case MGOS_MPU9250_DEVID_9255:
    LOG(LL_INFO, ("Detected MPU9255 at I2C 0x%02x", i2caddr));
    *devid = MGOS_MPU9250_DEVID_9255;
    return true;
  }
  LOG(LL_ERROR, ("Failed to detect MPU925x at I2C 0x%02x (device_id=0x%02x)", i2caddr, device_id));
  return false;
}

static bool mgos_imu_mpu925x_create(struct mgos_i2c *i2c, uint8_t i2caddr) {
  if (!i2c) {
    return false;
  }

  // Reset
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_MPU9250_REG_PWR_MGMT_1, 0x80);
  mgos_usleep(80000);

  // Enable IMU sensors
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_MPU9250_REG_PWR_MGMT_2, 0x00);

  // I2C Passthrough enable (exposes magnetometer to I2C bus)
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_MPU9250_REG_INT_PIN_CFG, 0x02);

  return true;
}

bool mgos_imu_mpu925x_acc_detect(struct mgos_imu_acc *dev, void *imu_user_data) {
  uint8_t devid;

  if (mgos_imu_mpu925x_detect(dev->i2c, dev->i2caddr, &devid)) {
    if (devid == MGOS_MPU9250_DEVID_9255) {
      dev->type = ACC_MPU9255;
    } else{
      dev->type = ACC_MPU9250;
    }
    return true;
  }
  return false;

  (void)imu_user_data;
}

bool mgos_imu_mpu925x_acc_create(struct mgos_imu_acc *dev, void *imu_user_data) {
  struct mgos_imu_mpu925x_userdata *iud = (struct mgos_imu_mpu925x_userdata *)imu_user_data;

  if (!dev) {
    return false;
  }

  // Only initialize the MPU9250 if gyro hasn't done so yet
  if (!iud->initialized) {
    if (!mgos_imu_mpu925x_create(dev->i2c, dev->i2caddr)) {
      return false;
    }
    iud->initialized = true;
  }
  if (!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_MPU9250_REG_ACCEL_CONFIG, MGOS_MPU9250_ACCEL_FS_SEL_16G)) {
    return false;
  }
  dev->scale = (16.0f * G2MSS) / 32768.0f;
  if (!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_MPU9250_REG_ACCEL_CONFIG2, MGOS_MPU9250_DLPF_41)) {
    return false;
  }
  return true;
}

bool mgos_imu_mpu925x_acc_read(struct mgos_imu_acc *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_MPU9250_REG_ACCEL_XOUT_H, 6, data)) {
    return false;
  }
  dev->ax = (data[0] << 8) | (data[1]);
  dev->ay = (data[2] << 8) | (data[3]);
  dev->az = (data[4] << 8) | (data[5]);
  // LOG(LL_DEBUG, ("ax=%d ay=%d az=%d", dev->ax, dev->ay, dev->az));
  return true;

  (void)imu_user_data;
}

bool mgos_imu_mpu925x_gyro_detect(struct mgos_imu_gyro *dev, void *imu_user_data) {
  uint8_t devid;

  if (mgos_imu_mpu925x_detect(dev->i2c, dev->i2caddr, &devid)) {
    if (devid == MGOS_MPU9250_DEVID_9255) {
      dev->type = GYRO_MPU9255;
    } else{
      dev->type = GYRO_MPU9250;
    }
    return true;
  }
  return false;

  (void)imu_user_data;
}

bool mgos_imu_mpu925x_gyro_create(struct mgos_imu_gyro *dev, void *imu_user_data) {
  struct mgos_imu_mpu925x_userdata *iud = (struct mgos_imu_mpu925x_userdata *)imu_user_data;

  if (!dev) {
    return false;
  }

  // Only initialize the MPU9250 if acc hasn't done so yet
  if (!iud->initialized) {
    if (!mgos_imu_mpu925x_create(dev->i2c, dev->i2caddr)) {
      return false;
    }
    iud->initialized = true;
  }
  if (!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_MPU9250_REG_GYRO_CONFIG, MGOS_MPU9250_GYRO_FS_SEL_2000DPS)) {
    return false;
  }
  dev->scale = 2000.0f / 32767.5f * DEG2RAD;
  if (!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_MPU9250_REG_CONFIG, MGOS_MPU9250_DLPF_41)) {
    return false;
  }
  return true;
}

bool mgos_imu_mpu925x_gyro_read(struct mgos_imu_gyro *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_MPU9250_REG_GYRO_XOUT_H, 6, data)) {
    return false;
  }
  dev->gx = (data[0] << 8) | (data[1]);
  dev->gy = (data[2] << 8) | (data[3]);
  dev->gz = (data[4] << 8) | (data[5]);
  // LOG(LL_DEBUG, ("gx=%d gy=%d gz=%d", dev->gx, dev->gy, dev->gz));

  return true;

  (void)imu_user_data;
}

struct mgos_imu_mpu925x_userdata *mgos_imu_mpu925x_userdata_create(void) {
  struct mgos_imu_mpu925x_userdata *iud;

  iud = calloc(1, sizeof(struct mgos_imu_mpu925x_userdata));
  if (!iud) {
    return NULL;
  }
  iud->initialized = false;
  return iud;
}
