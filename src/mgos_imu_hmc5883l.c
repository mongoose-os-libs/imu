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
#include "mgos_imu_hmc5883l.h"

bool mgos_imu_hmc5883l_detect(struct mgos_imu_mag *dev, void *imu_user_data) {
  int idA, idB, idC;

  if (!dev) {
    return false;
  }

  idA = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_HMC5883L_REG_ID_A);
  idB = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_HMC5883L_REG_ID_B);
  idC = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_HMC5883L_REG_ID_C);
  if (idA == 'H' && idB == '4' && idC == '3') {
    return true;
  }

  return false;

  (void)imu_user_data;
}

bool mgos_imu_hmc5883l_create(struct mgos_imu_mag *dev, void *imu_user_data) {
  if (!dev) {
    return false;
  }

  // CRA: 0; MA=00 (no averaging); DO=100 (15Hz ODR); MS=00 (normal)
  // CRB: GN=001 (1.3Ga); 00000
  // MODE: HS=0; 00000; MD=00 (continuous)
  if (!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_HMC5883L_REG_CONF_A, 0x10)) {
    return false;
  }
  if (!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_HMC5883L_REG_CONF_B, 0x20)) {
    return false;
  }
  if (!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_HMC5883L_REG_MODE, 0x00)) {
    return false;
  }
  mgos_usleep(1000);

  dev->scale   = 0.92;
  dev->bias[0] = 1.0;
  dev->bias[1] = 1.0;
  dev->bias[2] = 1.0;

  return true;

  (void)imu_user_data;
}

bool mgos_imu_hmc5883l_read(struct mgos_imu_mag *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_HMC5883L_REG_OUT_X_MSB, 6, data)) {
    return false;
  }

  dev->mx = (data[0] << 8) | (data[1]);
  dev->my = (data[2] << 8) | (data[3]);
  dev->mz = (data[4] << 8) | (data[5]);
  return true;

  (void)imu_user_data;
}
