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

#include "mgos.h"
#include "mgos_i2c.h"
#include "mgos_imu_l3gd20.h"

bool mgos_imu_l3gd20_detect(struct mgos_imu_gyro *dev, void *imu_user_data) {
  int device_id;

  if (!dev) {
    return false;
  }

  device_id = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_L3GD20_REG_WHO_AM_I);
  switch (device_id) {
  case MGOS_L3GD20_DEVID: dev->opts.type = GYRO_L3GD20; return true;

  case MGOS_L3GD20H_DEVID: dev->opts.type = GYRO_L3GD20H; return true;

  default:
    break;
  }
  return false;

  (void)imu_user_data;
}

bool mgos_imu_l3gd20_create(struct mgos_imu_gyro *dev, void *imu_user_data) {
  if (!dev) {
    return false;
  }

  // Reset
  if (!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_L3GD20_REG_CTRL_REG1, 0x00)) {
    return false;
  }
  mgos_usleep(5000);

  // Enable sensors: DR=01 (190Hz) BW=10 (50Hz) PD=1 [XYZ]EN=1
  if (!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_L3GD20_REG_CTRL_REG1, 0x6F)) {
    return false;
  }

  // Set 2000DPS
  if (!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_L3GD20_REG_CTRL_REG4, 0x20)) {
    return false;
  }

  // Datasheet says 2000dps is 0.07 deg per LSB.
  dev->scale = (2000.f * 1.1468625f) / 32767.5f;

  return true;

  (void)imu_user_data;
}

bool mgos_imu_l3gd20_read(struct mgos_imu_gyro *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }
  // Device cannot read multiple registers at once.
  data[0] = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_L3GD20_REG_OUT_X_L);
  data[1] = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_L3GD20_REG_OUT_X_H);
  data[2] = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_L3GD20_REG_OUT_Y_L);
  data[3] = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_L3GD20_REG_OUT_Y_H);
  data[4] = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_L3GD20_REG_OUT_Z_L);
  data[5] = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_L3GD20_REG_OUT_Z_H);
  dev->gx = (int16_t)((data[1] << 8) | data[0]);
  dev->gy = (int16_t)((data[3] << 8) | data[2]);
  dev->gz = (int16_t)((data[5] << 8) | data[4]);

  return true;

  (void)imu_user_data;
}
