/*
 * Copyright 2019 Vasily Kiniv
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
#include "mgos_imu_icm20948.h"

#define MGOS_ICM20948_ACC_BASE_ODR  1125.f
#define MGOS_ICM20948_GYRO_BASE_ODR 1100.f

static bool mgos_imu_icm20948_change_bank(struct mgos_i2c *i2c, uint8_t i2caddr, void *imu_user_data, uint8_t bank_no) {
  struct  mgos_imu_icm20948_userdata *iud = (struct mgos_imu_icm20948_userdata *)imu_user_data;
  uint8_t bank_addr = 0x00;

  if (!imu_user_data) {
    return false;
  }

  if(bank_no == iud->current_bank_no) {
    return true;
  }

  switch (bank_no) {
    case 0: bank_addr = 0x00; break;
    case 1: bank_addr = 0x10; break;
    case 2: bank_addr = 0x20; break;
    case 3: bank_addr = 0x30; break;
  }

  if(mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_ICM20948_REG0_BANK_SEL, bank_addr)) {
    iud->current_bank_no = bank_no;
    return true;
  }

  return false;
}

static bool mgos_imu_icm20948_detect(struct mgos_i2c *i2c, uint8_t i2caddr, void *imu_user_data) {
  struct mgos_imu_icm20948_userdata *iud = (struct mgos_imu_icm20948_userdata *)imu_user_data;
  int device_id;

  if (!i2c) {
    return false;
  }
  if(!imu_user_data) {
    return false;
  }
  if(iud->accgyro_initialized) {
    return true;
  }

  device_id = mgos_i2c_read_reg_b(i2c, i2caddr, MGOS_ICM20948_REG0_WHO_AM_I);
  if (device_id == MGOS_ICM20948_DEVID) {
    return true;
  }
  return false;
}

static bool mgos_imu_icm20948_accgyro_create(struct mgos_i2c *i2c, uint8_t i2caddr, void *imu_user_data) {
  if (!i2c) {
    return false;
  }

  if(!mgos_imu_icm20948_change_bank(i2c, i2caddr, imu_user_data, 0)) {
    return false;
  }

  // PWR_MGMNT_1: DEVICE_RESET=1; SLEEP=0; LP_EN=0; TEMP_DIS=0; CLKSEL=000;
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_ICM20948_REG0_PWR_MGMT_1, 0x80);
  mgos_usleep(11000);

  // PWR_MGMNT_1: DEVICE_RESET=0; SLEEP=0; LP_EN=0; TEMP_DIS=0; CLKSEL=001(auto clock source);
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_ICM20948_REG0_PWR_MGMT_1, 0x01);

  // INT_PIN_CFG: BYPASS_EN=1(enable slave bypass mode);
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_ICM20948_REG0_INT_PIN_CFG, 0x02);

  return true;
}

bool mgos_imu_icm20948_acc_detect(struct mgos_imu_acc *dev, void *imu_user_data) {
  return mgos_imu_icm20948_detect(dev->i2c, dev->i2caddr, imu_user_data);
}

bool mgos_imu_icm20948_acc_create(struct mgos_imu_acc *dev, void *imu_user_data) {
  struct mgos_imu_icm20948_userdata *iud = (struct mgos_imu_icm20948_userdata *)imu_user_data;

  if (!dev) {
    return false;
  }

  // Only initialize the ICM20948 if gyro hasn't done so yet
  if (!iud->accgyro_initialized) {
    if (!mgos_imu_icm20948_accgyro_create(dev->i2c, dev->i2caddr, imu_user_data)) {
      return false;
    }
    iud->accgyro_initialized = true;
  }

  if(!mgos_imu_icm20948_change_bank(dev->i2c, dev->i2caddr, imu_user_data, 2)) {
    return false;
  }

  // REG2_ACCEL_CONFIG: ACCEL_DLPFCFG=101(12Hz); ACCEL_FS_SEL=10(8g); ACCEL_FCHOICE=1(Enable accel DLPF);
  // ACCEL_SMPLRT_DIV_1: ACCEL_SMPLRT_DIV=0000(MSB);
  // ACCEL_SMPLRT_DIV_2: ACCEL_SMPLRT_DIV=00001010(LSB);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_REG2_ACCEL_CONFIG, 0x15);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_REG2_ACCEL_SMPLRT_DIV_1, 0x00);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_REG2_ACCEL_SMPLRT_DIV_2, 0x0a);

  dev->scale = 8.f / 32767.0f;
  return true;
}

bool mgos_imu_icm20948_acc_read(struct mgos_imu_acc *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }

  if(!mgos_imu_icm20948_change_bank(dev->i2c, dev->i2caddr, imu_user_data, 0)) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_ICM20948_REG0_ACCEL_XOUT_H, 6, data)) {
    return false;
  }

  dev->ax = (data[0] << 8) | (data[1]);
  dev->ay = (data[2] << 8) | (data[3]);
  dev->az = (data[4] << 8) | (data[5]);

  return true;
}

bool mgos_imu_icm20948_acc_get_scale(struct mgos_imu_acc *dev, void *imu_user_data, float *scale) {
  uint8_t fs = 0;

  if (!scale) {
    return false;
  }
  if(!mgos_imu_icm20948_change_bank(dev->i2c, dev->i2caddr, imu_user_data, 2)) {
    return false;
  }
  if (!mgos_i2c_getbits_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_REG2_ACCEL_CONFIG, 1, 2, &fs)) {
    return false;
  }

  switch (fs) {
    case 0: *scale = 2; break;
    case 1: *scale = 4; break;
    case 2: *scale = 8; break;
    case 3: *scale = 16; break;
  }

  return true;
}

bool mgos_imu_icm20948_acc_set_scale(struct mgos_imu_acc *dev, void *imu_user_data, float scale) {
  uint8_t fs = 0;

  if (scale <= 2) {
    fs = 0;
    scale = 2.f;
  } else if (scale <= 4) {
    fs = 1;
    scale = 4.f;
  } else if (scale <= 8) {
    fs = 2;
    scale = 8.f;
  } else if (scale <= 16) {
    fs = 3;
    scale = 16.f;
  } else {
    return false;
  }

  if(!mgos_imu_icm20948_change_bank(dev->i2c, dev->i2caddr, imu_user_data, 2)) {
    return false;
  }
  if (!mgos_i2c_setbits_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_REG2_ACCEL_CONFIG, 1, 2, fs)) {
    return false;
  }
  dev->opts.scale = scale;
  dev->scale = dev->opts.scale / 32767.0f;

  return true;
}

bool mgos_imu_icm20948_acc_get_odr(struct mgos_imu_acc *dev, void *imu_user_data, float *odr) {
  uint16_t div  = 0;

  if (!odr) {
    return false;
  }
  if(!mgos_imu_icm20948_change_bank(dev->i2c, dev->i2caddr, imu_user_data, 2)) {
    return false;
  }

  if(!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_ICM20948_REG2_ACCEL_SMPLRT_DIV_1, 2, (uint8_t *)&div)) {
    return false;
  }
  div = (div & 0x00ff) << 8 | (div & 0xff00) >> 8;

  // ODR is computed as follows:
  // 1.125 kHz/(1+ACCEL_SMPLRT_DIV[11:0])
  *odr = MGOS_ICM20948_ACC_BASE_ODR / (div + 1);
  return *odr >= 0;
}

bool mgos_imu_icm20948_acc_set_odr(struct mgos_imu_acc *dev, void *imu_user_data, float odr) {
  uint16_t div = 0;

  if(odr <= 0) {
    return false;
  }

  div = (MGOS_ICM20948_ACC_BASE_ODR / odr) - 1;

  if(div > 0xfff) {
    return false;
  }

  if(!mgos_imu_icm20948_change_bank(dev->i2c, dev->i2caddr, imu_user_data, 2)) {
    return false;
  }

  div = (div & 0x00ff) << 8 | (div & 0xff00) >> 8;
  if(!mgos_i2c_write_reg_n(dev->i2c, dev->i2caddr, MGOS_ICM20948_REG2_ACCEL_SMPLRT_DIV_1, 2, (uint8_t *)&div)) {
    return false;
  }

  dev->opts.odr = odr;
  return true;
}

bool mgos_imu_icm20948_gyro_detect(struct mgos_imu_gyro *dev, void *imu_user_data) {
  return mgos_imu_icm20948_detect(dev->i2c, dev->i2caddr, imu_user_data);
}

bool mgos_imu_icm20948_gyro_create(struct mgos_imu_gyro *dev, void *imu_user_data) {
  struct mgos_imu_icm20948_userdata *iud = (struct mgos_imu_icm20948_userdata *)imu_user_data;

  if (!dev) {
    return false;
  }

  // Only initialize the ICM20948 if acc hasn't done so yet
  if (!iud->accgyro_initialized) {
    if (!mgos_imu_icm20948_accgyro_create(dev->i2c, dev->i2caddr, imu_user_data)) {
      return false;
    }
    iud->accgyro_initialized = true;
  }

  if(!mgos_imu_icm20948_change_bank(dev->i2c, dev->i2caddr, imu_user_data, 2)) {
    return false;
  }

  // GYRO_CONFIG_1: GYRO_DLPFCFG=101(12Hz); GYRO_FS_SEL=11(2000dps); GYRO_FCHOICE=1(Enable gyro DLPF);
  // GYRO_SMPLRT_DIV: GYRO_SMPLRT_DIV=00000000;
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_REG2_GYRO_CONFIG_1, 0x2F);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_REG2_GYRO_SMPLRT_DIV, 0x0a);

  dev->scale = 2000 / 32767.0f;
  return true;
}

bool mgos_imu_icm20948_gyro_read(struct mgos_imu_gyro *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }

  if(!mgos_imu_icm20948_change_bank(dev->i2c, dev->i2caddr, imu_user_data, 0)) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_ICM20948_REG0_GYRO_XOUT_H, 6, data)) {
    return false;
  }
  dev->gx = (data[0] << 8) | (data[1]);
  dev->gy = (data[2] << 8) | (data[3]);
  dev->gz = (data[4] << 8) | (data[5]);

  return true;
}

bool mgos_imu_icm20948_gyro_get_scale(struct mgos_imu_gyro *dev, void *imu_user_data, float *scale) {
  uint8_t fs = 0;

  if (!scale) {
    return false;
  }
  if(!mgos_imu_icm20948_change_bank(dev->i2c, dev->i2caddr, imu_user_data, 2)) {
    return false;
  }
  if (!mgos_i2c_getbits_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_REG2_GYRO_CONFIG_1, 1, 2, &fs)) {
    return false;
  }
  switch (fs) {
    case 0: *scale = 250; break;
    case 1: *scale = 500; break;
    case 2: *scale = 1000; break;
    case 3: *scale = 2000; break;
  }

  return true;
}

bool mgos_imu_icm20948_gyro_set_scale(struct mgos_imu_gyro *dev, void *imu_user_data, float scale) {
  uint8_t fs = 0;

  if (scale <= 250) {
    fs = 0;
    scale = 250.f;
  } else if (scale <= 500) {
    fs = 1;
    scale = 500.f;
  } else if (scale <= 1000) {
    fs = 2;
    scale = 1000.f;
  } else if (scale <= 2000) {
    fs = 3;
    scale = 2000.f;
  } else {
    return false;
  }

  if(!mgos_imu_icm20948_change_bank(dev->i2c, dev->i2caddr, imu_user_data, 2)) {
    return false;
  }
  if (!mgos_i2c_setbits_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_REG2_GYRO_CONFIG_1, 1, 2, fs)) {
    return false;
  }
  dev->opts.scale = scale;
  dev->scale = dev->opts.scale / 32767.0f;

  return true;
}

bool mgos_imu_icm20948_gyro_get_odr(struct mgos_imu_gyro *dev, void *imu_user_data, float *odr) {
  uint8_t div  = 0;

  if (!odr) {
    return false;
  }
  if(!mgos_imu_icm20948_change_bank(dev->i2c, dev->i2caddr, imu_user_data, 2)) {
    return false;
  }
  if(!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_ICM20948_REG2_GYRO_SMPLRT_DIV, 1, &div)) {
    return false;
  }

  // ODR is computed as follows:
  // 1.1 kHz/(1+GYRO_SMPLRT_DIV[7:0])
  *odr = MGOS_ICM20948_GYRO_BASE_ODR / (div + 1);
  return *odr >= 0;
}

bool mgos_imu_icm20948_gyro_set_odr(struct mgos_imu_gyro *dev, void *imu_user_data, float odr) {
  uint8_t div = 0;

  if(odr <= 0 || odr > MGOS_ICM20948_GYRO_BASE_ODR) {
    return false;
  }
  div = (MGOS_ICM20948_GYRO_BASE_ODR / odr) - 1;

  if(!mgos_imu_icm20948_change_bank(dev->i2c, dev->i2caddr, imu_user_data, 2)) {
    return false;
  }
  if(!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_REG2_GYRO_SMPLRT_DIV, div)) {
    return false;
  }

  dev->opts.odr = odr;
  return true;
}

bool mgos_imu_icm20948_mag_detect(struct mgos_imu_mag *dev, void *imu_user_data) {
  int device_id;

  if (!dev->i2c) {
    return false;
  }

  device_id = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_WHO_AM_I_M);
  if (device_id == MGOS_ICM20948_DEVID_M) {
    return true;
  }
  return false;

  (void)imu_user_data;
}

bool mgos_imu_icm20948_mag_create(struct mgos_imu_mag *dev, void *imu_user_data) {
  if (!dev) {
    return false;
  }

  // CNTL3: SRST=1;
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_CNTL3_M, 0x01);

  // CNTL2: 01000(MODE4, 100Hz);
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_CNTL2_M, 0x08);

  dev->scale = 22.f / 32768.0;
  dev->bias[0] = 1.0;
  dev->bias[1] = 1.0;
  dev->bias[2] = 1.0;
  return true;

  (void)imu_user_data;
}

bool mgos_imu_icm20948_mag_read(struct mgos_imu_mag *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }

  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_ICM20948_HXL_M, 6, data)) {
    return false;
  }

  // It is required to read ST2 register after data reading.
  mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_ST2_M);

  dev->mx = (data[1] << 8) | (data[0]);
  dev->my = (data[3] << 8) | (data[2]);
  dev->mz = (data[5] << 8) | (data[4]);
  return true;

  (void)imu_user_data;
}

bool mgos_imu_icm20948_mag_get_odr(struct mgos_imu_mag *dev, void *imu_user_data, float *odr) {
  int16_t mode;

  if (!odr) {
    return false;
  }

  mode = mgos_i2c_read_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_CNTL2_M);
  if(mode == -1) {
    return false;
  }

  switch (mode) {
    case 0x02: *odr = 10; break;
    case 0x04: *odr = 20; break;
    case 0x06: *odr = 50; break;
    case 0x08: *odr = 100; break;
    default:   return false;
  }

  return true;
  (void)imu_user_data;
}

bool mgos_imu_icm20948_mag_set_odr(struct mgos_imu_mag *dev, void *imu_user_data, float odr) {
  uint8_t mode = 0;

  if(odr <= 10) {
    mode = 0x02;
  } else if (odr <= 20) {
    mode = 0x04;
  } else if (odr <= 50) {
    mode = 0x06;
  } else if (odr <= 100) {
    mode = 0x08;
  } else {
    return false;
  }
  if(!mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_ICM20948_CNTL2_M, mode)) {
    return false;
  }

  return false;
  (void)imu_user_data;
}

struct mgos_imu_icm20948_userdata *mgos_imu_icm20948_userdata_create(void) {
  struct mgos_imu_icm20948_userdata *iud;

  iud = calloc(1, sizeof(struct mgos_imu_icm20948_userdata));
  if (!iud) {
    return NULL;
  }
  iud->accgyro_initialized = false;
  iud->current_bank_no = -1;
  return iud;
}
