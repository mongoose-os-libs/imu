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
#include "mgos_imu_mma8451.h"

bool mgos_imu_mma8451_detect(struct mgos_imu_acc *dev, void *imu_user_data) {
  int device_id;

  if (!dev) {
    return false;
  }

  device_id = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_MMA8451_REG_WHO_AM_I);
  if (device_id == MGOS_MMA8451_DEVID) {
    return true;
  }
  return false;

  (void)imu_user_data;
}

bool mgos_imu_mma8451_create(struct mgos_imu_acc *dev, void *imu_user_data) {
  if (!dev) {
    return false;
  }

  // XYZ_DATA_CFG: 000; HPF=0; 00; FS=10 (8G)
  // CTRL_REG1: ASLP_RATE=00; DR=011 (100Hz ODR); LNOISE=0; F_READ=0; ACTIVE=1
  // CTRL_REG2: ST=0; RST=0; 0; SMODS=10 (hires); SLPE=0; MODS=10 (hires)
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_MMA8451_REG_XYZ_DATA_CFG, 0x02);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_MMA8451_REG_CTRL_REG1, 0x19);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_MMA8451_REG_CTRL_REG2, 0x12);

  dev->scale = (8.f / 32767.5);
  return true;

  (void)imu_user_data;
}

bool mgos_imu_mma8451_read(struct mgos_imu_acc *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_MMA8451_REG_OUT_X_MSB, 6, data)) {
    return false;
  }
  dev->ax = (data[1]) | (data[0] << 8);
  dev->ay = (data[3]) | (data[2] << 8);
  dev->az = (data[5]) | (data[4] << 8);

  return true;

  (void)imu_user_data;
}
