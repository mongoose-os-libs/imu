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
#include "mgos_imu_lsm6dsl.h"

static bool mgos_imu_lsm6dsl_detect(struct mgos_i2c *i2c, uint8_t i2caddr) {
  int device_id;

  if (!i2c) {
    return false;
  }

  device_id = mgos_i2c_read_reg_b(i2c, i2caddr, MGOS_LSM6DSL_REG_WHO_AM_I);
  if (device_id == MGOS_LSM6DSL_DEVID) {
    return true;
  }
  return false;
}

static bool mgos_imu_lsm6dsl_accgyro_create(struct mgos_i2c *i2c, uint8_t i2caddr, bool no_rst) {
  if (!i2c) {
    return false;
  }

  // Reload trimmming data and reset (procedure as described in AN5040 5.7).
  if (!no_rst) {
    // Power down gyro and accel
    mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_LSM6DSL_REG_CTRL2_G, 0);
    mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_LSM6DSL_REG_CTRL1_XL, 0);
    // Reload trimming values (takes 15 ms)
    mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_LSM6DSL_REG_CTRL3_C, 0x80);
    mgos_usleep(15000);
    // Perform SW reset (bit auto-clears).
    mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_LSM6DSL_REG_CTRL3_C, 1);
    while (mgos_i2c_read_reg_b(i2c, i2caddr, MGOS_LSM6DSL_REG_CTRL3_C) & 1) {
      ;
    }
  }

  // CTRL3_C: BOOT=0; BDU=1; H_LACTIVE=1; PP_OD=0; SIM=0; IF_INC=1; BLE=0; SW_RESET=0;
  return mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_LSM6DSL_REG_CTRL3_C, 0x44);
}

bool mgos_imu_lsm6dsl_acc_detect(struct mgos_imu_acc *dev, void *imu_user_data) {
  return mgos_imu_lsm6dsl_detect(dev->i2c, dev->i2caddr);

  (void)imu_user_data;
}

bool mgos_imu_lsm6dsl_acc_create(struct mgos_imu_acc *dev, void *imu_user_data) {
  struct mgos_imu_lsm6dsl_userdata *iud = (struct mgos_imu_lsm6dsl_userdata *)imu_user_data;

  if (!dev) {
    return false;
  }

  // Only initialize the LSM6DSL if gyro hasn't done so yet
  if (!iud->initialized) {
    if (!mgos_imu_lsm6dsl_accgyro_create(dev->i2c, dev->i2caddr, dev->opts.no_rst)) {
      return false;
    }
    iud->initialized = true;
  }

  return true;
}

bool mgos_imu_lsm6dsl_acc_read(struct mgos_imu_acc *dev, void *imu_user_data) {
  int16_t data[3];

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_LSM6DSL_REG_OUTX_L_XL, 6, (uint8_t *)data)) {
    return false;
  }
  dev->ax = data[0];
  dev->ay = data[1];
  dev->az = data[2];
  return true;

  (void)imu_user_data;
}

bool mgos_imu_lsm6dsl_acc_get_scale(struct mgos_imu_acc *dev, void *imu_user_data, float *scale) {
  uint8_t fs = 0;

  if (!mgos_i2c_getbits_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM6DSL_REG_CTRL1_XL, 2, 2, &fs)) {
    return false;
  }
  switch (fs) {
  case 0: *scale = 2; break;

  case 1: *scale = 16; break;

  case 2: *scale = 4; break;

  case 3: *scale = 8; break;
  }

  return true;

  (void)imu_user_data;
}

bool mgos_imu_lsm6dsl_acc_set_scale(struct mgos_imu_acc *dev, void *imu_user_data, float scale) {
  uint8_t fs = 0;

  if (scale <= 2) {
    fs = 0;
    scale = 2.f;
  } else if (scale <= 4) {
    fs = 2;
    scale = 4.f;
  } else if (scale <= 8) {
    fs = 3;
    scale = 8.f;
  } else if (scale <= 16) {
    fs = 1;
    scale = 16.f;
  } else {
    return false;
  }
  if (!mgos_i2c_setbits_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM6DSL_REG_CTRL1_XL, 2, 2, fs)) return false;
  dev->opts.scale = scale;
  dev->scale = dev->opts.scale / 32767.0f;
  return true;

  (void)imu_user_data;
}

static float mgos_imu_lsm6dsl_odr_to_hz(uint8_t lsm6_odr) {
  switch (lsm6_odr) {
  case 0: return 0.0f;

  case 1: return 12.5f;

  case 2: return 26.0f;

  case 3: return 52.0f;

  case 4: return 104.0f;

  case 5: return 208.0f;

  case 6: return 416.0f;

  case 7: return 833.0f;

  case 8: return 1666.0f;

  case 9: return 3333.0f;

  case 10: return 6666.0f;

  case 11: return 1.6f;
  }
  return -1;
}

bool mgos_imu_lsm6dsl_acc_get_odr(struct mgos_imu_acc *dev, void *imu_user_data, float *odr) {
  uint8_t odr_xl = 0;

  if (!mgos_i2c_getbits_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM6DSL_REG_CTRL1_XL, 4, 4, &odr_xl)) {
    return false;
  }
  *odr = mgos_imu_lsm6dsl_odr_to_hz(odr_xl);
  return *odr >= 0;

  (void)imu_user_data;
}

static uint8_t mgos_imu_lsm6dsl_hz_to_odr(float odr) {
  if (odr == 0) {
    return 0;
  }
  if (odr <= 1) {
    return 11;              // 1.6
  }
  if (odr <= 12) {
    return 1;               // 12.5
  }
  if (odr <= 26) {
    return 2;
  }
  if (odr <= 52) {
    return 3;
  }
  if (odr <= 104) {
    return 4;
  }
  if (odr <= 208) {
    return 5;
  }
  if (odr <= 416) {
    return 6;
  }
  if (odr <= 833) {
    return 7;
  }
  if (odr <= 1666) {
    return 8;
  }
  if (odr <= 3333) {
    return 9;
  }
  if (odr <= 6666) {
    return 10;
  }
  return 0xff;
}

bool mgos_imu_lsm6dsl_acc_set_odr(struct mgos_imu_acc *dev, void *imu_user_data, float odr) {
  uint8_t lsm6_odr = mgos_imu_lsm6dsl_hz_to_odr(odr);

  if (lsm6_odr == 0xff) {
    return false;
  }

  if (!mgos_i2c_setbits_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM6DSL_REG_CTRL1_XL, 4, 4, lsm6_odr)) return false;
  dev->opts.odr = odr;
  return true;

  (void)imu_user_data;
}

bool mgos_imu_lsm6dsl_gyro_detect(struct mgos_imu_gyro *dev, void *imu_user_data) {
  return mgos_imu_lsm6dsl_detect(dev->i2c, dev->i2caddr);

  (void)imu_user_data;
}

bool mgos_imu_lsm6dsl_gyro_create(struct mgos_imu_gyro *dev, void *imu_user_data) {
  struct mgos_imu_lsm6dsl_userdata *iud = (struct mgos_imu_lsm6dsl_userdata *)imu_user_data;

  if (!dev) {
    return false;
  }

  // Only initialize the LSM6DSL if acc hasn't done so yet
  if (!iud->initialized) {
    if (!mgos_imu_lsm6dsl_accgyro_create(dev->i2c, dev->i2caddr, false /* no_rst */)) {
      return false;
    }
    iud->initialized = true;
  }

  // CTRL2_G: ODR_XL=0100 (104Hz ODR); FS_XL=11 (2000dps); FS_125=0; 0
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM6DSL_REG_CTRL2_G, 0x4c);

  dev->scale = 2000.f * .035 * 1e-3;
  return true;
}

bool mgos_imu_lsm6dsl_gyro_read(struct mgos_imu_gyro *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_LSM6DSL_REG_OUTX_L_G, 6, data)) {
    return false;
  }
  dev->gx = (data[1] << 8) | (data[0]);
  dev->gy = (data[3] << 8) | (data[2]);
  dev->gz = (data[5] << 8) | (data[4]);

  return true;

  (void)imu_user_data;
}

struct mgos_imu_lsm6dsl_userdata *mgos_imu_lsm6dsl_userdata_create(void) {
  struct mgos_imu_lsm6dsl_userdata *iud;

  iud = calloc(1, sizeof(struct mgos_imu_lsm6dsl_userdata));
  if (!iud) {
    return NULL;
  }
  iud->initialized = false;
  return iud;
}

bool mgos_imu_lsm6dsl_get_and_clear_ints(struct mgos_imu *imu, uint32_t *res) {
  uint32_t ints = 0;
  // Query all the possible interrupt sources.
  uint8_t regs[4] = { 0 };

  if (!mgos_i2c_read_reg_n(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_WAKE_UP_SRC, 4, regs)) {
    return false;
  }
  uint8_t wake_up_src = regs[0];
  if (wake_up_src & 0b00100000) {
    ints |= MGOS_LSM6DSL_INT_FF;
  }
  if (wake_up_src & 0b00001000) {
    ints |= MGOS_LSM6DSL_INT_WU;
  }
  uint8_t tap_src = regs[1];
  if (tap_src & 0b00100000) {
    ints |= MGOS_LSM6DSL_INT_TAP;
  }
  if (tap_src & 0b00010000) {
    ints |= MGOS_LSM6DSL_INT_DBL_TAP;
  }
  uint8_t d6d_src = regs[2];
  if (d6d_src & 0b01000000) {
    ints |= MGOS_LSM6DSL_INT_D6D;
  }
  uint8_t status_reg = regs[3];
  if (status_reg & 0b00000001) {
    ints |= MGOS_LSM6DSL_INT_DRDY_XL;
  }
  if (status_reg & 0b00000010) {
    ints |= MGOS_LSM6DSL_INT_DRDY_G;
  }
  if (status_reg & 0b00000100) {
    ints |= MGOS_LSM6DSL_INT2_DRDY_TEMP;
  }
  int fifo_status2 = mgos_i2c_read_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_FIFO_STATUS2);
  if (fifo_status2 < 0) {
    return false;
  }
  if (fifo_status2 & 0b10000000) {
    ints |= MGOS_LSM6DSL_INT_FIFO_THR;
  }
  if (fifo_status2 & 0b01000000) {
    ints |= MGOS_LSM6DSL_INT_FIFO_OVR;
  }
  if (fifo_status2 & 0b00100000) {
    ints |= MGOS_LSM6DSL_INT_FIFO_FULL;
  }
  if (!mgos_i2c_read_reg_n(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_FUNC_SRC1, 2, regs)) {
    return false;
  }
  uint8_t func_src1 = regs[0];
  if (func_src1 & 0b10000000) {
    ints |= MGOS_LSM6DSL_INT2_STEP_DELTA;
  }
  if (func_src1 & 0b01000000) {
    ints |= MGOS_LSM6DSL_INT1_SIGN_MOT;
  }
  if (func_src1 & 0b00100000) {
    ints |= MGOS_LSM6DSL_INT_TILT;
  }
  if (func_src1 & 0b00010000) {
    ints |= MGOS_LSM6DSL_INT1_STEP_DET;
  }
  if (func_src1 & 0b00001000) {
    ints |= MGOS_LSM6DSL_INT2_STEP_OVR;
  }
  /* Not sure what to do about MGOS_LSM6DSL_INT1_BOOT. Meh. */
  *res = ints;
  return true;
}

static void mgos_imu_lsm6dsl_irq(int pin, void *arg) {
  struct mgos_imu_lsm6dsl_userdata *iud;
  struct mgos_imu *imu = (struct mgos_imu *)arg;

  if (!imu || !imu->acc || !imu->user_data) {
    return;
  }
  iud = (struct mgos_imu_lsm6dsl_userdata *)imu->user_data;

  uint32_t ints = 0;
  if (!mgos_imu_lsm6dsl_get_and_clear_ints(imu, &ints)) {
    return;
  }
  // Callback to user
  if (iud->int_cb) {
    iud->int_cb(imu, ints, iud->int_cb_user_data);
  }

  (void)pin;
}

bool mgos_imu_lsm6dsl_set_int_handler(struct mgos_imu *imu, int int1_gpio, int int2_gpio, mgos_imu_lsm6dsl_int_cb cb, void *user_data) {
  struct mgos_imu_lsm6dsl_userdata *iud;

  if (!imu || !imu->acc || !imu->user_data) {
    return false;
  }

  iud = (struct mgos_imu_lsm6dsl_userdata *)imu->user_data;

  iud->int_cb           = cb;
  iud->int_cb_user_data = user_data;

  if (int1_gpio >= 0) {
    mgos_gpio_setup_input(int1_gpio, MGOS_GPIO_PULL_DOWN);
    mgos_gpio_set_int_handler(int1_gpio, MGOS_GPIO_INT_EDGE_POS, mgos_imu_lsm6dsl_irq, imu);
    mgos_gpio_clear_int(int1_gpio);
    mgos_gpio_enable_int(int1_gpio);
  }
  if (int2_gpio >= 0) {
    bool int2_on_int1 = (int2_gpio == int1_gpio);
    mgos_i2c_setbits_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_CTRL4_C, 5, 1, int2_on_int1);
    if (!int2_on_int1) {
      mgos_gpio_setup_input(int2_gpio, MGOS_GPIO_PULL_DOWN);
      mgos_gpio_set_int_handler(int2_gpio, MGOS_GPIO_INT_EDGE_POS, mgos_imu_lsm6dsl_irq, imu);
      mgos_gpio_clear_int(int2_gpio);
      mgos_gpio_enable_int(int2_gpio);
    }
  }

  // Set latched mode for ints.
  mgos_i2c_setbits_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_TAP_CFG, 0, 1, 1);

  return true;
}

bool mgos_imu_lsm6dsl_int1_enable(struct mgos_imu *imu, uint32_t int1_mask) {
  int int1_ctrl = mgos_i2c_read_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_INT1_CTRL);
  int md1_cfg   = mgos_i2c_read_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_MD1_CFG);

  if (int1_ctrl < 0 || md1_cfg < 0) {
    return false;
  }
  int1_ctrl |= (uint8_t)int1_mask;
  md1_cfg   |= (uint8_t)(int1_mask >> 8);
  if (md1_cfg != 0) {
    if (!mgos_i2c_setbits_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_TAP_CFG, 7, 1, 1)) {
      return false;
    }
  }
  return mgos_i2c_write_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_INT1_CTRL, int1_ctrl) &&
         mgos_i2c_write_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_MD1_CFG, md1_cfg);
}

bool mgos_imu_lsm6dsl_int1_disable(struct mgos_imu *imu, uint32_t int1_mask) {
  int int1_ctrl = mgos_i2c_read_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_INT1_CTRL);
  int md1_cfg   = mgos_i2c_read_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_MD1_CFG);

  if (int1_ctrl < 0 || md1_cfg < 0) {
    return false;
  }
  int1_ctrl &= ~((uint8_t)int1_mask);
  md1_cfg   &= ~((uint8_t)(int1_mask >> 8));
  return mgos_i2c_write_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_INT1_CTRL, int1_ctrl) &&
         mgos_i2c_write_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_MD1_CFG, md1_cfg);
}

bool mgos_imu_lsm6dsl_int2_enable(struct mgos_imu *imu, uint32_t int2_mask) {
  int int2_ctrl = mgos_i2c_read_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_INT2_CTRL);
  int md2_cfg   = mgos_i2c_read_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_MD2_CFG);

  if (int2_ctrl < 0 || md2_cfg < 0) {
    return false;
  }
  int2_ctrl |= (uint8_t)(int2_mask | (int2_mask >> 16));
  md2_cfg   |= (uint8_t)(int2_mask >> 8);
  if (md2_cfg != 0) {
    if (!mgos_i2c_setbits_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_TAP_CFG, 7, 1, 1)) {
      return false;
    }
  }
  return mgos_i2c_write_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_INT2_CTRL, int2_ctrl) &&
         mgos_i2c_write_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_MD2_CFG, md2_cfg);
}

bool mgos_imu_lsm6dsl_int2_disable(struct mgos_imu *imu, uint32_t int2_mask) {
  int int2_ctrl = mgos_i2c_read_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_INT2_CTRL);
  int md2_cfg   = mgos_i2c_read_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_MD2_CFG);

  if (int2_ctrl < 0 || md2_cfg < 0) {
    return false;
  }
  int2_ctrl &= ~((uint8_t)(int2_mask | (int2_mask >> 16)));
  md2_cfg   &= ~((uint8_t)(int2_mask >> 8));
  return mgos_i2c_write_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_INT2_CTRL, int2_ctrl) &&
         mgos_i2c_write_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_MD2_CFG, md2_cfg);
}
