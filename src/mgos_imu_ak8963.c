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
#include "mgos_imu_ak8963.h"

bool mgos_imu_ak8963_detect(struct mgos_imu_mag *dev, void *imu_user_data) {
  int device_id;

  if (!dev) {
    return false;
  }

  device_id = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_AK8963_REG_WHO_AM_I);
  if (device_id == MGOS_AK8963_DEVID) {
    return true;
  }
  return false;

  (void)imu_user_data;
}

bool mgos_imu_ak8963_create(struct mgos_imu_mag *dev, void *imu_user_data) {
  if (!dev) {
    return false;
  }

  // Reset
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_AK8963_REG_CNTL, 0x00);
  mgos_usleep(10000);

  // Fuse ROM access mode
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_AK8963_REG_CNTL, 0x0F);
  mgos_usleep(10000);

  uint8_t data[3];
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_AK8963_REG_ASAX, 3, data)) {
    LOG(LL_ERROR, ("Could not read magnetometer adjustment registers"));
    return false;
  }
  dev->bias[0] = (float)(data[0] - 128) / 256. + 1.;
  dev->bias[1] = (float)(data[1] - 128) / 256. + 1.;
  dev->bias[2] = (float)(data[2] - 128) / 256. + 1.;

  LOG(LL_DEBUG, ("Magnetometer adjustment bias %.2f %.2f %.2f", dev->bias[0], dev->bias[1], dev->bias[2]));

  // Reset
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_AK8963_REG_CNTL, 0x00);
  mgos_usleep(10000);

  // Set magnetometer config: 16-bit, continuous measurement mode 2 (100Hz)
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_AK8963_REG_CNTL, 0x16);
  mgos_usleep(10000);
  dev->scale = 4192.0 / 32768.0;

  return true;

  (void)imu_user_data;
}

bool mgos_imu_ak8963_read(struct mgos_imu_mag *dev, void *imu_user_data) {
  uint8_t data[7];

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_AK8963_REG_XOUT_L, 7, data)) {
    return false;
  }
  if (data[6] & 0x08) {
    return false;
  }

  dev->mx = (data[1] << 8) | (data[0]);
  dev->my = (data[3] << 8) | (data[2]);
  dev->mz = (data[5] << 8) | (data[4]);
  return true;

  (void)imu_user_data;
}
