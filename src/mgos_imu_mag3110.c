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
#include "mgos_imu_mag3110.h"

bool mgos_imu_mag3110_detect(struct mgos_imu_mag *dev, void *imu_user_data) {
  int device_id;

  if (!dev) {
    return false;
  }

  device_id = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_MAG3110_REG_WHO_AM_I);
  if (device_id == MGOS_MAG3110_DEVID) {
    return true;
  }
  return false;

  (void)imu_user_data;
}

bool mgos_imu_mag3110_create(struct mgos_imu_mag *dev, void *imu_user_data) {
  if (!dev) {
    return false;
  }

  // Standby
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_MAG3110_REG_CTRL_REG1, 0x00);
  mgos_usleep(10000);

  // Enable automatic magnetic sensor resets by setting bit AUTO_MRST_EN
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_MAG3110_REG_CTRL_REG2, 0x80);
  mgos_usleep(20000);

  // Put MAG3110 in active mode 10 Hz ODR with 128x oversampling, noise 0.25 uT RMS
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_MAG3110_REG_CTRL_REG1, 0x19);
  mgos_usleep(20000);

  dev->scale   = 0.001;
  dev->bias[0] = 1.0;
  dev->bias[1] = 1.0;
  dev->bias[2] = 1.0;

  return true;

  (void)imu_user_data;
}

bool mgos_imu_mag3110_read(struct mgos_imu_mag *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_MAG3110_REG_OUT_X_MSB, 6, data)) {
    return false;
  }

  dev->mx = (data[1] << 8) | (data[0]);
  dev->my = (data[3] << 8) | (data[2]);
  dev->mz = (data[5] << 8) | (data[4]);
  return true;

  (void)imu_user_data;
}
