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
#include "mgos_imu_adxl345.h"

bool mgos_imu_adxl345_detect(struct mgos_imu_acc *dev) {
  int device_id;

  if (!dev) {
    return false;
  }

  device_id = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_ADXL345_REG_WHO_AM_I);
  if (device_id == MGOS_ADXL345_DEVID) {
    return true;
  }
  LOG(LL_ERROR, ("Failed to detect MPU925x at I2C 0x%02x (device_id=0x%02x)", dev->i2caddr, device_id));
  return false;
}

bool mgos_imu_adxl345_create(struct mgos_imu_acc *dev) {
  if (!dev) {
    return false;
  }

  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ADXL345_REG_POWER_CTL, MGOS_ADXL345_POWER_MEAS);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ADXL345_REG_DATA_FORMAT, MGOS_ADXL345_FULL_RANGE | MGOS_ADXL345_RANGE_2G);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ADXL345_REG_BW_RATE, MGOS_ADXL345_RATE_100);

  dev->scale = 1. / 1024;
  return true;
}

bool mgos_imu_adxl345_read(struct mgos_imu_acc *dev) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_ADXL345_REG_DATA_OUT, 6, data)) {
    return false;
  }
  dev->ax = (data[0]) | (data[1] << 8);
  dev->ay = (data[2]) | (data[3] << 8);
  dev->az = (data[4]) | (data[5] << 8);
  LOG(LL_DEBUG, ("ax=%d ay=%d az=%d", dev->ax, dev->ay, dev->az));
  return true;
}
