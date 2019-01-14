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

bool mgos_imu_adxl345_detect(struct mgos_imu_acc *dev, void *imu_user_data) {
  int device_id;

  if (!dev) {
    return false;
  }

  device_id = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_ADXL345_REG_WHO_AM_I);
  if (device_id == MGOS_ADXL345_DEVID) {
    return true;
  }
  return false;

  (void)imu_user_data;
}

bool mgos_imu_adxl345_create(struct mgos_imu_acc *dev, void *imu_user_data) {
  if (!dev) {
    return false;
  }

  // POWER_CTL: 00; link=0; auto_sleep=0; measure=1; sleep=0; wakeup=00 (8Hz)
  // DATA_FORMAT: SELF_TEST=0; SPI=0; INT_INVERT=0; 0; FULL_RES=1; Justify=0; Range=10 (8G)
  // BW_RATE: 000; LOW_POWER=0; Rate=1010 (100Hz ODR, 50Hz filter)
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ADXL345_REG_POWER_CTL, 0x08);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ADXL345_REG_DATA_FORMAT, 0x0A);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ADXL345_REG_BW_RATE, 0x0B);

  // FULL_RES: 3.9mg/LSB always
  dev->scale = 0.0039;
  return true;

  (void)imu_user_data;
}

bool mgos_imu_adxl345_read(struct mgos_imu_acc *dev, void *imu_user_data) {
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

  return true;

  (void)imu_user_data;
}
