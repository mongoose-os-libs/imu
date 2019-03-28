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

#include "mgos_imu_mpu60x0.h"
#include "mgos.h"
#include "mgos_i2c.h"

static bool mgos_imu_mpu60x0_detect(struct mgos_i2c *i2c, uint8_t i2caddr) {
  if (!i2c) {
    return false;
  }

  return MGOS_MPU60X0_DEVID ==
         mgos_i2c_read_reg_b(i2c, i2caddr, MGOS_MPU60X0_REG_WHO_AM_I);
}

static bool mgos_imu_mpu60x0_create(struct mgos_i2c *i2c, uint8_t i2caddr) {
  if (!i2c) {
    return false;
  }

  // Reset
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_MPU60X0_REG_PWR_MGMT_1, 0x80);
  mgos_usleep(80000);

  // Enable IMU sensors
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_MPU60X0_REG_PWR_MGMT_2, 0x00);

  // Set DPLF: -- EXT_SYNC_SET=000 DLPF_CFG=010 (94Hz accel, 98Hz gyro, Fs=1KHz)
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_MPU60X0_REG_CONFIG, 0x02);

  // Exit sleep mode
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_MPU60X0_REG_PWR_MGMT_1, 0x00);

  return true;
}

bool mgos_imu_mpu60x0_acc_detect(struct mgos_imu_acc *dev,
                                 void *imu_user_data) {
  return mgos_imu_mpu60x0_detect(dev->i2c, dev->i2caddr);

  (void)imu_user_data;
}

bool mgos_imu_mpu60x0_acc_create(struct mgos_imu_acc *dev,
                                 void *imu_user_data) {
  struct mgos_imu_mpu60x0_userdata *iud =
    (struct mgos_imu_mpu60x0_userdata *)imu_user_data;

  if (!dev) {
    return false;
  }

  // Only initialize the MPU60X0 if gyro hasn't done so yet
  if (!iud->initialized) {
    if (!mgos_imu_mpu60x0_create(dev->i2c, dev->i2caddr)) {
      return false;
    }
    iud->initialized = true;
  }
  // Accel Config: XA_ST=0 YG_AT=0 ZA_ST=0 FS_SEL=10 (8G) ---
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_MPU60X0_REG_ACCEL_CONFIG, 0x10);

  return true;
}

bool mgos_imu_mpu60x0_acc_read(struct mgos_imu_acc *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_MPU60X0_REG_ACCEL_XOUT_H, 6, data)) {
    return false;
  }
  dev->ax = (data[0] << 8) | (data[1]);
  dev->ay = (data[2] << 8) | (data[3]);
  dev->az = (data[4] << 8) | (data[5]);

  return true;

  (void)imu_user_data;
}

bool mgos_imu_mpu60x0_gyro_detect(struct mgos_imu_gyro *dev,
                                  void *imu_user_data) {
  return mgos_imu_mpu60x0_detect(dev->i2c, dev->i2caddr);

  (void)imu_user_data;
}

bool mgos_imu_mpu60x0_gyro_create(struct mgos_imu_gyro *dev,
                                  void *imu_user_data) {
  struct mgos_imu_mpu60x0_userdata *iud =
    (struct mgos_imu_mpu60x0_userdata *)imu_user_data;

  if (!dev) {
    return false;
  }

  // Only initialize the MPU60X0 if acc hasn't done so yet
  if (!iud->initialized) {
    if (!mgos_imu_mpu60x0_create(dev->i2c, dev->i2caddr)) {
      return false;
    }
    iud->initialized = true;
  }
  // Gyro Config: XG_ST=0 YG_ST=0 ZG_ST=0 FS_SEL=11 (2000 dps) ---
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_MPU60X0_REG_GYRO_CONFIG, 0x18);

  return true;
}

bool mgos_imu_mpu60x0_gyro_read(struct mgos_imu_gyro *dev,
                                void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_MPU60X0_REG_GYRO_XOUT_H, 6, data)) {
    return false;
  }
  dev->gx = (data[0] << 8) | (data[1]);
  dev->gy = (data[2] << 8) | (data[3]);
  dev->gz = (data[4] << 8) | (data[5]);

  return true;

  (void)imu_user_data;
}

struct mgos_imu_mpu60x0_userdata *mgos_imu_mpu60x0_userdata_create(void) {
  struct mgos_imu_mpu60x0_userdata *iud;

  iud = calloc(1, sizeof(struct mgos_imu_mpu60x0_userdata));
  if (!iud) {
    return NULL;
  }
  iud->initialized = false;
  return iud;
}

bool mgos_imu_mpu60x0_acc_get_scale(struct mgos_imu_acc *dev,
                                    void *imu_user_data, float *scale) {
  uint8_t sel;

  if (!mgos_i2c_getbits_reg_b(dev->i2c, dev->i2caddr, MGOS_MPU60X0_REG_ACCEL_CONFIG, 3, 2, &sel)) {
    return false;
  }
  switch (sel) {
  case 3: *scale = 16; break;

  case 2: *scale = 8; break;

  case 1: *scale = 4; break;

  default: *scale = 2; break;
  }

  return true;

  (void)imu_user_data;
}

bool mgos_imu_mpu60x0_acc_set_scale(struct mgos_imu_acc *dev,
                                    void *imu_user_data, float scale) {
  uint8_t sel;

  if (scale > 16) {
    return false;
  } else if (scale > 8) {
    sel   = 3; // 16G
    scale = 16.f;
  } else if (scale > 4) {
    sel   = 2; // 8G
    scale = 8.f;
  } else if (scale > 2) {
    sel   = 1; // 4G
    scale = 4.f;
  } else {
    sel   = 0; // 2G
    scale = 2.f;
  }
  if (!mgos_i2c_setbits_reg_b(dev->i2c, dev->i2caddr, MGOS_MPU60X0_REG_ACCEL_CONFIG, 3, 2, sel)) {
    return false;
  }
  dev->opts.scale = scale;
  dev->scale      = scale / 32768.0f;

  return true;

  (void)imu_user_data;
}

bool mgos_imu_mpu60x0_gyro_get_scale(struct mgos_imu_gyro *dev,
                                     void *imu_user_data, float *scale) {
  uint8_t sel;

  if (!mgos_i2c_getbits_reg_b(dev->i2c, dev->i2caddr, MGOS_MPU60X0_REG_GYRO_CONFIG, 3, 2, &sel)) {
    return false;
  }
  switch (sel) {
  case 3: *scale = 2000; break;

  case 2: *scale = 1000; break;

  case 1: *scale = 500; break;

  default: *scale = 250; break;
  }

  return true;

  (void)imu_user_data;
}

bool mgos_imu_mpu60x0_gyro_set_scale(struct mgos_imu_gyro *dev,
                                     void *imu_user_data, float scale) {
  uint8_t sel;

  if (scale > 2000) {
    return false;
  } else if (scale > 1000) {
    sel   = 3; // 2000DPS
    scale = 2000.f;
  } else if (scale > 500) {
    sel   = 2; // 1000DPS
    scale = 1000.f;
  } else if (scale > 250) {
    sel   = 1; // 500DPS
    scale = 500.f;
  } else {
    sel   = 0; // 250DPS
    scale = 250.f;
  }
  if (!mgos_i2c_setbits_reg_b(dev->i2c, dev->i2caddr, MGOS_MPU60X0_REG_GYRO_CONFIG, 3, 2, sel)) {
    return false;
  }
  dev->opts.scale = scale;
  dev->scale      = scale / 32768.0f;

  return true;

  (void)imu_user_data;
}
