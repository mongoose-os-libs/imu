/*
 * Copyright (c) 2020 Sergio R. Caprile and Cika Electronica S.R.L.
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mgos_imu_bmm150.h"
#include "bmm150.h"
#include "mgos.h"
#include "mgos_i2c.h"

bool mgos_imu_bmm150_detect(struct mgos_imu_mag *dev, void *imu_user_data) {
  int device_id;

  if (!dev) {
    return false;
  }

  // Exit from Suspend mode
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_BMM150_REG_POWMODE, 0x01);
  mgos_usleep(5000);

  // Now read Chip ID
  device_id =
      mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_BMM150_REG_CHIPID);
  if (device_id == MGOS_BMM150_DEVID) {
    return true;
  }
  return false;

  (void)imu_user_data;
}

bool mgos_imu_bmm150_create(struct mgos_imu_mag *dev, void *imu_user_data) {
  struct mgos_imu_bmm150_trim_registers *imud;

  if (!dev) {
    return false;
  }

  imud = calloc(1, sizeof(struct mgos_imu_bmm150_trim_registers));
  if (!imud) {
    return false;
  }
  dev->user_data = imud;

  // Exit from Suspend mode
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_BMM150_REG_POWMODE, 0x01);
  mgos_usleep(5000);

  // Soft Reset
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_BMM150_REG_POWMODE, 0x83);
  mgos_usleep(5000);

  // Regular repetition rate, active mode 10 Hz ODR, noise 0.6 uT RMS
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_BMM150_REG_OPMODE, 0x00);
  // datasheet page 31, table 3 in page 13. Bosch API sets as if 1+2REPZ (?)
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_BMM150_REG_REPXY,
                       4); //  9; 1+2REPXY
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_BMM150_REG_REPZ,
                       14); // 15; 1+REPZ
  mgos_usleep(5000);

  dev->scale = 0.3;
  dev->bias[0] = 1.0;
  dev->bias[1] = 1.0;
  dev->bias[2] = 1.0;

  return read_trim_registers(dev);

  (void)imu_user_data;
}

bool mgos_imu_bmm150_read(struct mgos_imu_mag *dev, void *imu_user_data) {
  uint8_t reg_data[8];
  int16_t raw_datax, raw_datay, raw_dataz;
  uint16_t raw_data_r;
  struct mgos_imu_bmm150_trim_registers *trim_regs =
      (struct mgos_imu_bmm150_trim_registers *)dev->user_data;

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_BMM150_REG_OUT_X_LSB, 8,
                           reg_data)) {
    return false;
  }

  raw_datax = (((int8_t)reg_data[1]) << 5) |
              (((uint8_t)(reg_data[0] & 0xF8) >> 3)); // 13-bits LSB first
  raw_datay = (((int8_t)reg_data[3]) << 5) |
              (((uint8_t)(reg_data[2] & 0xF8) >> 3)); // 13-bits
  raw_dataz = (((int8_t)reg_data[5]) << 7) |
              (((uint8_t)(reg_data[4] & 0xFE) >> 1)); // 15-bits
  raw_data_r =
      (reg_data[7] << 6) | (((uint8_t)(reg_data[6] & 0xFE) >> 2)); // 14-bits

  /* Compensated Mag data in int16_t format */
  dev->mx = compensate_x(raw_datax, raw_data_r, trim_regs);
  dev->my = compensate_y(raw_datay, raw_data_r, trim_regs);
  dev->mz = compensate_z(raw_dataz, raw_data_r, trim_regs);

  return true;

  (void)imu_user_data;
}
