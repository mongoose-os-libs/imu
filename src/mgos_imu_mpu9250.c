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
#include "mgos_imu_mpu9250.h"

static bool mgos_imu_mpu9250_detect(struct mgos_i2c *i2c, uint8_t i2caddr) {
  int device_id;

  if (!i2c) {
    return false;
  }

  device_id = mgos_i2c_read_reg_b(i2c, i2caddr, MGOS_MPU9250_REG_WHO_AM_I);
  switch (device_id) {
  case MGOS_MPU9250_DEVID_9250:
    LOG(LL_INFO, ("Detected MPU9250 at I2C 0x%02x", i2caddr));
    return true;

  case MGOS_MPU9250_DEVID_9255:
    LOG(LL_INFO, ("Detected MPU9255 at I2C 0x%02x", i2caddr));
    return true;
  }
  LOG(LL_ERROR, ("Failed to detect MPU925x at I2C 0x%02x (device_id=0x%02x)", i2caddr, device_id));
  return false;
}

static bool mgos_imu_mpu9250_create(struct mgos_i2c *i2c, uint8_t i2caddr) {
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

bool mgos_imu_mpu9250_acc_detect(struct mgos_imu_acc *dev) {
  return mgos_imu_mpu9250_detect(dev->i2c, dev->i2caddr);
}

bool mgos_imu_mpu9250_acc_create(struct mgos_imu_acc *dev) {
  if (!dev) {
    return false;
  }

  dev->scale = G * 16.0f / 32768.0f; // 16G default - scale to m/s/s

  if (!mgos_imu_mpu9250_create(dev->i2c, dev->i2caddr)) {
    return false;
  }
  return mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_MPU9250_REG_ACCEL_CONFIG, MGOS_MPU9250_ACCEL_FS_SEL_16G);
}

bool mgos_imu_mpu9250_acc_read(struct mgos_imu_acc *dev) {
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
  LOG(LL_DEBUG, ("ax=%d ay=%d az=%d", dev->ax, dev->ay, dev->az));
  return true;
}

bool mgos_imu_mpu9250_gyro_detect(struct mgos_imu_gyro *dev) {
  return mgos_imu_mpu9250_detect(dev->i2c, dev->i2caddr);
}

bool mgos_imu_mpu9250_gyro_create(struct mgos_imu_gyro *dev) {
  if (!dev) {
    return false;
  }
  dev->scale = 2000.0f / 32767.5f * DEG2RAD;

  if (!mgos_imu_mpu9250_create(dev->i2c, dev->i2caddr)) {
    return false;
  }
  return mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_MPU9250_REG_GYRO_CONFIG, MGOS_MPU9250_GYRO_FS_SEL_2000DPS);
}

bool mgos_imu_mpu9250_gyro_read(struct mgos_imu_gyro *dev) {
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
  LOG(LL_DEBUG, ("gx=%d gy=%d gz=%d", dev->gx, dev->gy, dev->gz));

  return true;
}
