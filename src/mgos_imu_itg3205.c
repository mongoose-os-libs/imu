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
#include "mgos_imu_itg3205.h"

bool mgos_imu_itg3205_detect(struct mgos_imu_gyro *dev, void *imu_user_data) {
  int device_id;

  if (!dev) {
    return false;
  }

  device_id = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_ITG3205_REG_WHO_AM_I);
  if (device_id == MGOS_ITG3205_DEVID) {
    return true;
  }
  return false;

  (void)imu_user_data;
}

bool mgos_imu_itg3205_create(struct mgos_imu_gyro *dev, void *imu_user_data) {
  if (!dev) {
    return false;
  }

  // Reset
  if (!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ITG3205_REG_PWR_MGM, 0x80)) {
    return false;
  }
  mgos_usleep(2000);
  if (!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ITG3205_REG_PWR_MGM, 0x00)) {
    return false;
  }

  // SMPLRT_DIV: 00000111 (7, yielding 125Hz ODR with 8x oversampling)
  // DLPF_FS: 000; FS_SEL=11 (2000deg/sec); DLPF_CFG=011 (LPF=42Hz, samplerate=1kHz)
  if (!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ITG3205_REG_SMPLRT_DIV, 7)) {
    return false;
  }
  if (!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ITG3205_REG_DLPF_FS, 0x1B)) {
    return false;
  }

  // Datasheet says 2000dps is 14.375 LSB per deg/sec
  dev->scale = (1 / 14.375);

  return true;

  (void)imu_user_data;
}

bool mgos_imu_itg3205_read(struct mgos_imu_gyro *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }

  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_ITG3205_REG_GYRO_XOUT_H, 6, data)) {
    return false;
  }
  dev->gx = (int16_t)((data[0] << 8) | data[1]);
  dev->gy = (int16_t)((data[2] << 8) | data[3]);
  dev->gz = (int16_t)((data[4] << 8) | data[5]);

  return true;

  (void)imu_user_data;
}
