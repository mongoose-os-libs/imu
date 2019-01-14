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
#include "mgos_imu_lsm9ds1.h"

static bool mgos_imu_lsm9ds1_detect(struct mgos_i2c *i2c, uint8_t i2caddr) {
  int device_id;

  if (!i2c) {
    return false;
  }

  device_id = mgos_i2c_read_reg_b(i2c, i2caddr, MGOS_LSM9DS1_REG_WHO_AM_I);
  if (device_id == MGOS_LSM9DS1_DEVID) {
    return true;
  }
  return false;
}

static bool mgos_imu_lsm9ds1_accgyro_create(struct mgos_i2c *i2c, uint8_t i2caddr) {
  if (!i2c) {
    return false;
  }

  // Reset acc/gyro
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_LSM9DS1_REG_CTRL_REG8, 0x81);
  mgos_usleep(10000);

  // FIFO_CTRL: FMODE=000 (FIFO off); FTH=00000
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_LSM9DS1_REG_FIFO_CTRL, 0x00);

  // CTRL_REG8: BOOT=0; BDU=1; H_LACTIVE=0; PP_OD=1; SIM=0; IF_ADD_INC=1; BLE=0; SW_RESET=0;
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_LSM9DS1_REG_CTRL_REG8, 0x54);

  // CTRL_REG9: 0; SLEEP_G=0; 0; FIFO_TEMP_EN=0; DRDY=1; I2C_DIS=0; FIFO_EN=0; STOP_ON_FTH=0;
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_LSM9DS1_REG_CTRL_REG9, 0x08);

  return true;

  (void)i2caddr;
}

bool mgos_imu_lsm9ds1_acc_detect(struct mgos_imu_acc *dev, void *imu_user_data) {
  return mgos_imu_lsm9ds1_detect(dev->i2c, dev->i2caddr);

  (void)imu_user_data;
}

bool mgos_imu_lsm9ds1_acc_create(struct mgos_imu_acc *dev, void *imu_user_data) {
  struct mgos_imu_lsm9ds1_userdata *iud = (struct mgos_imu_lsm9ds1_userdata *)imu_user_data;

  if (!dev) {
    return false;
  }

  // Only initialize the LSM9DS1 if gyro hasn't done so yet
  if (!iud->accgyro_initialized) {
    if (!mgos_imu_lsm9ds1_accgyro_create(dev->i2c, dev->i2caddr)) {
      return false;
    }
    iud->accgyro_initialized = true;
  }

  // CTRL_REG5_XL: DEC=00 (no decimation); [ZYX]en_XL=111; 000
  // CTRL_REG6_XL: ODR_XL=011 (ODR 119Hz) FS_XL=11 (8G); BW_SCAL=0; BW_XL=00 (408Hz bandwidth)
  // CTRL_REG7_XL: HR=0 (lores); DCF=00 (50Hz cutoff); 00; FDS=0 (filter off); 0; HPIS1=0 (filter on interrupt)
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_CTRL_REG7_XL, 0x00);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_CTRL_REG6_XL, 0x78);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_CTRL_REG5_XL, 0x68);

  /*
   #define SENSITIVITY_ACCELEROMETER_2  0.000061
   #define SENSITIVITY_ACCELEROMETER_4  0.000122
   #define SENSITIVITY_ACCELEROMETER_8  0.000244
   #define SENSITIVITY_ACCELEROMETER_16 0.000732
   */
  dev->scale = 0.000244;
  return true;
}

bool mgos_imu_lsm9ds1_acc_read(struct mgos_imu_acc *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_OUT_X_L_XL, 6, data)) {
    return false;
  }
  dev->ax = (data[1] << 8) | (data[0]);
  dev->ay = (data[3] << 8) | (data[2]);
  dev->az = (data[5] << 8) | (data[4]);
  return true;

  (void)imu_user_data;
}

bool mgos_imu_lsm9ds1_gyro_detect(struct mgos_imu_gyro *dev, void *imu_user_data) {
  return mgos_imu_lsm9ds1_detect(dev->i2c, dev->i2caddr);

  (void)imu_user_data;
  (void)imu_user_data;
}

bool mgos_imu_lsm9ds1_gyro_create(struct mgos_imu_gyro *dev, void *imu_user_data) {
  struct mgos_imu_lsm9ds1_userdata *iud = (struct mgos_imu_lsm9ds1_userdata *)imu_user_data;

  if (!dev) {
    return false;
  }

  // Only initialize the LSM9DS1 if acc hasn't done so yet
  if (!iud->accgyro_initialized) {
    if (!mgos_imu_lsm9ds1_accgyro_create(dev->i2c, dev->i2caddr)) {
      return false;
    }
    iud->accgyro_initialized = true;
  }

  // CTRL_REG1_G: ODR_G=011 (ODR 119Hz); FS_G=11 (2000dps); 0; BW_G=00 (14Hz bandwidth)
  // CTRL_REG2_G: 0000; INT_SEL=00; OUT_SEL=00;
  // CTRL_REG3_G: LP=0; HP_EN=0; HPCF_G=0000;
  // CTRL_REG4: 00; [ZYX]en_G=111; 0; LIR_XL1=0; 4D_XL1=0;
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_CTRL_REG1_G, 0x78);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_CTRL_REG2_G, 0x00);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_CTRL_REG3_G, 0x00);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_CTRL_REG4, 0x38);

  /*
   #define SENSITIVITY_GYROSCOPE_245    0.00875
   #define SENSITIVITY_GYROSCOPE_500    0.0175
   #define SENSITIVITY_GYROSCOPE_2000   0.07
   */
  dev->scale = 0.07f;
  return true;
}

bool mgos_imu_lsm9ds1_gyro_read(struct mgos_imu_gyro *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_OUT_X_L_G, 6, data)) {
    return false;
  }
  dev->gx = (data[1] << 8) | (data[0]);
  dev->gy = (data[3] << 8) | (data[2]);
  dev->gz = (data[5] << 8) | (data[4]);

  return true;

  (void)imu_user_data;
}

bool mgos_imu_lsm9ds1_mag_detect(struct mgos_imu_mag *dev, void *imu_user_data) {
  int device_id;

  if (!dev->i2c) {
    return false;
  }

  device_id = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_WHO_AM_I_M);
  if (device_id == MGOS_LSM9DS1_DEVID_M) {
    return true;
  }
  return false;

  (void)imu_user_data;
}

bool mgos_imu_lsm9ds1_mag_create(struct mgos_imu_mag *dev, void *imu_user_data) {
  if (!dev) {
    return false;
  }

  // Reset mag
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_CTRL_REG2_M, 0x0c);
  mgos_usleep(10000);


  // CTRL_REG1_M: TEMP_COMP=0; OM=10 (high performance); DO=110 (ODR 40Hz); FAST_ODR=0; ST=0
  // CTRL_REG2_M: 0; FS=10 (12Gauss); 0; REBOOT=0; SOFT_RST=0; 00
  // CTRL_REG3_M: I2C_DIS=0; 0; LP=0; 00; SIM=1 MD=00 (continuous)
  // CTRL_REG4_M: 0000; OMZ=10 (high performance); BLE=0; 0
  // CTRL_REG5_M: FAST_READ=0; BDU=1; 000000
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_CTRL_REG1_M, 0x58);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_CTRL_REG2_M, 0x40);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_CTRL_REG3_M, 0x04);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_CTRL_REG4_M, 0x08);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_CTRL_REG5_M, 0x40);

  /*
   #define SENSITIVITY_MAGNETOMETER_4   0.00014
   #define SENSITIVITY_MAGNETOMETER_8   0.00029
   #define SENSITIVITY_MAGNETOMETER_12  0.00043
   #define SENSITIVITY_MAGNETOMETER_16  0.00058
   */
  dev->scale   = 0.00043;
  dev->bias[0] = 1.f;
  dev->bias[1] = 1.f;
  dev->bias[2] = 1.f;
  return true;

  (void)imu_user_data;
}

bool mgos_imu_lsm9ds1_mag_read(struct mgos_imu_mag *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_LSM9DS1_REG_OUT_X_L_M, 6, data)) {
    return false;
  }
  dev->mx = (data[1] << 8) | (data[0]);
  dev->my = (data[3] << 8) | (data[2]);
  dev->mz = (data[5] << 8) | (data[4]);
  return true;

  (void)imu_user_data;
}

struct mgos_imu_lsm9ds1_userdata *mgos_imu_lsm9ds1_userdata_create(void) {
  struct mgos_imu_lsm9ds1_userdata *iud;

  iud = calloc(1, sizeof(struct mgos_imu_lsm9ds1_userdata));
  if (!iud) {
    return NULL;
  }
  iud->accgyro_initialized = false;
  return iud;
}
