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
#include "mgos_imu_lsm303d.h"

static bool mgos_imu_lsm303d_detect(struct mgos_i2c *i2c, uint8_t i2caddr, uint8_t *devid) {
  int device_id;

  if (!i2c || !devid) {
    return false;
  }

  device_id = mgos_i2c_read_reg_b(i2c, i2caddr, MGOS_LSM303D_REG_WHO_AM_I);
  switch (device_id) {
  case MGOS_LSM303D_DEVID:
    *devid = MGOS_LSM303D_DEVID;
    return true;

  case MGOS_LSM303DLM_DEVID:
    *devid = MGOS_LSM303DLM_DEVID;
    return true;

  default:
    break;
  }
  return false;
}

static bool mgos_imu_lsm303d_create(struct mgos_i2c *i2c, uint8_t i2caddr) {
  if (!i2c) {
    return false;
  }

  // Reset
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_LSM303D_REG_CTRL0, 0x80);
  mgos_usleep(5000);

  // Enable
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_LSM303D_REG_CTRL0, 0x00);

  return true;
}

bool mgos_imu_lsm303d_acc_detect(struct mgos_imu_acc *dev, void *imu_user_data) {
  uint8_t devid;

  if (mgos_imu_lsm303d_detect(dev->i2c, dev->i2caddr, &devid)) {
    if (devid == MGOS_LSM303D_DEVID) {
      dev->opts.type = ACC_LSM303D;
    } else{
      dev->opts.type = ACC_LSM303DLM;
    }
    return true;
  }
  return false;

  (void)imu_user_data;
}

bool mgos_imu_lsm303d_acc_create(struct mgos_imu_acc *dev, void *imu_user_data) {
  struct mgos_imu_lsm303d_userdata *iud = (struct mgos_imu_lsm303d_userdata *)imu_user_data;

  if (!dev) {
    return false;
  }

  // Only initialize the LSM303D if mag hasn't done so yet
  if (!iud->initialized) {
    if (!mgos_imu_lsm303d_create(dev->i2c, dev->i2caddr)) {
      return false;
    }
    iud->initialized = true;
  }

  // Accel register settings:
  // CTRL1: AODR=0110 (100 Hz ODR); BDU=1; A*EN=1 (enable all axes)
  // CTRL2: ABW=00 (773Hz); AFS=011 (8G); 0; AST=0; SIM=0
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM303D_REG_CTRL1, 0x6F);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM303D_REG_CTRL2, 0x18);

  dev->scale = 8.f / 32767.5;
  return true;
}

bool mgos_imu_lsm303d_acc_read(struct mgos_imu_acc *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }
  // Low register is single-read, high register is streaming read.
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_LSM303D_REG_OUT_X_L_A | 0x80, 6, data)) {
    return false;
  }
  dev->ax = (data[1] << 8) | (data[0]);
  dev->ay = (data[3] << 8) | (data[2]);
  dev->az = (data[5] << 8) | (data[4]);

  return true;

  (void)imu_user_data;
}

bool mgos_imu_lsm303d_mag_detect(struct mgos_imu_mag *dev, void *imu_user_data) {
  uint8_t devid;

  if (mgos_imu_lsm303d_detect(dev->i2c, dev->i2caddr, &devid)) {
    if (devid == MGOS_LSM303D_DEVID) {
      dev->opts.type = MAG_LSM303D;
    } else{
      dev->opts.type = MAG_LSM303DLM;
    }
    return true;
  }
  return false;

  (void)imu_user_data;
}

bool mgos_imu_lsm303d_mag_create(struct mgos_imu_mag *dev, void *imu_user_data) {
  struct mgos_imu_lsm303d_userdata *iud = (struct mgos_imu_lsm303d_userdata *)imu_user_data;

  if (!dev) {
    return false;
  }

  // Only initialize the LSM303D if acc hasn't done so yet
  if (!iud->initialized) {
    if (!mgos_imu_lsm303d_create(dev->i2c, dev->i2caddr)) {
      return false;
    }
    iud->initialized = true;
  }

  // Mag settings
  // CTRL5: TEMP_EN=0; M_RES=11 (hires); M_ODR=101 (100Hz); LIR2=0; LIR1=0
  // CTRL6: 0; MFS=11 (12gauss); 00000
  // CTRL7: AHPM=00; AFDS=0; T_ONLY=0; 0; MLP=0; MD=01 (singleshot)
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM303D_REG_CTRL5, 0xf4);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM303D_REG_CTRL6, 0x60);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM303D_REG_CTRL7, 0x00);

  dev->scale   = 12.f / 32767.5;
  dev->bias[0] = 1.0f;
  dev->bias[1] = 1.0f;
  dev->bias[2] = 1.0f;

  return true;
}

bool mgos_imu_lsm303d_mag_read(struct mgos_imu_mag *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }

  // Low register is single-read, high register is streaming read.
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_LSM303D_REG_OUT_X_L_M | 0x80, 6, data)) {
    return false;
  }
  dev->mx = (data[1] << 8) | (data[0]);
  dev->my = (data[3] << 8) | (data[2]);
  dev->mz = (data[5] << 8) | (data[4]);

  return true;

  (void)imu_user_data;
}

struct mgos_imu_lsm303d_userdata *mgos_imu_lsm303d_userdata_create(void) {
  struct mgos_imu_lsm303d_userdata *iud;

  iud = calloc(1, sizeof(struct mgos_imu_lsm303d_userdata));
  if (!iud) {
    return NULL;
  }
  iud->initialized = false;
  return iud;
}
