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

  if (!i2c) return false;

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

bool mgos_imu_mpu9250_acc_detect(struct mgos_imu_acc *dev) {
  return mgos_imu_mpu9250_detect(dev->i2c, dev->i2caddr);
}

bool mgos_imu_mpu9250_gyro_detect(struct mgos_imu_gyro *dev) {
  return mgos_imu_mpu9250_detect(dev->i2c, dev->i2caddr);
}

bool mgos_imu_mpu9250_acc_create(struct mgos_imu_acc *dev) {
  if (!dev) return false;
  return false;
}

bool mgos_imu_mpu9250_acc_destroy(struct mgos_imu_acc *dev) {
  if (!dev) return false;
  return false;
}

bool mgos_imu_mpu9250_acc_read(struct mgos_imu_acc *dev) {
  if (!dev) return false;
  return false;
}
