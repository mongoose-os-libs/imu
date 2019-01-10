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

static bool mgos_imu_lsm6dsl_accgyro_create(struct mgos_i2c *i2c, uint8_t i2caddr) {
  if (!i2c) {
    return false;
  }

  // Reset acc/gyro
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_LSM6DSL_REG_CTRL3_C, 0x81);
  mgos_usleep(10000);

  // CTRL3_C: BOOT=0; BDU=1; H_LACTIVE=1; PP_OD=0; SIM=0; IF_INC=1; BLE=0; SW_RESET=0;
  mgos_i2c_write_reg_b(i2c, i2caddr, MGOS_LSM6DSL_REG_CTRL3_C, 0x44);

  return true;

  (void)i2caddr;
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
    if (!mgos_imu_lsm6dsl_accgyro_create(dev->i2c, dev->i2caddr)) {
      return false;
    }
    iud->initialized = true;
  }

  // CTRL1_XL: ODR_XL=0100 (104Hz ODR); FS_XL=11 (8G); LPF1_BW_SEL=0; BW0_XL=0
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM6DSL_REG_CTRL1_XL, 0x4c);

  /*
   #define SENSITIVITY_ACCELEROMETER_1  0.0000305
   #define SENSITIVITY_ACCELEROMETER_2  0.000061
   #define SENSITIVITY_ACCELEROMETER_4  0.000122
   #define SENSITIVITY_ACCELEROMETER_8  0.000244
   #define SENSITIVITY_ACCELEROMETER_16 0.000488
   */
  dev->scale = (8.f * 0.0000305 * G2MSS);
  return true;
}

bool mgos_imu_lsm6dsl_acc_read(struct mgos_imu_acc *dev, void *imu_user_data) {
  uint8_t data[6];

  if (!dev) {
    return false;
  }
  if (!mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_LSM6DSL_REG_OUTX_L_XL, 6, data)) {
    return false;
  }
  dev->ax = (data[1] << 8) | (data[0]);
  dev->ay = (data[3] << 8) | (data[2]);
  dev->az = (data[5] << 8) | (data[4]);
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
    if (!mgos_imu_lsm6dsl_accgyro_create(dev->i2c, dev->i2caddr)) {
      return false;
    }
    iud->initialized = true;
  }

  // CTRL2_G: ODR_XL=0100 (104Hz ODR); FS_XL=11 (2000dps); FS_125=0; 0
  mgos_i2c_write_reg_b(dev->i2c, dev->i2caddr, MGOS_LSM6DSL_REG_CTRL2_G, 0x4c);

  dev->scale = 2000.f * .035 * 1e-3 * DEG2RAD;
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

static void mgos_imu_lsm6dsl_irq(int pin, void *arg) {
  struct mgos_imu_lsm6dsl_userdata *iud;
  struct mgos_imu *imu = (struct mgos_imu *)arg;
  int val;

  if (!imu || !imu->acc || !imu->user_data) {
    return;
  }
  iud = (struct mgos_imu_lsm6dsl_userdata *)imu->user_data;

  LOG(LL_DEBUG, ("Interrupt on GPIO %d", pin));

  // TODO(rojer) FUNC_SRC1: contains significant motion detection -- Its the one I think we want.
  // TODO(rojer) WAKE_UP_SRC, TAP_SRC, D6D_SRC are sources of motion.
  // TODO(rojer) test which one of those has the interrupt bit we're looking for
  // TODO(rojer) ensure that reading from the registers clears the interrupt?
  val = mgos_i2c_read_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_FUNC_SRC1);
  LOG(LL_DEBUG, ("FUNC_SRC1=0x%02x", val));

  // Callback to user
  if (iud->int_cb) {
    iud->int_cb(imu, iud->int_cb_user_data);
  }
  return;
}

bool mgos_imu_lsm6dsl_set_int_handler(struct mgos_imu *imu, int gpio, mgos_imu_lsm6dsl_int_cb cb, void *user_data) {
  struct mgos_imu_lsm6dsl_userdata *iud;

  if (!imu || !imu->acc || !imu->user_data) {
    return false;
  }

  iud = (struct mgos_imu_lsm6dsl_userdata *)imu->user_data;

  // INT1_CTLR: STEP_DETECT=0; SIGN_MOT=1; FULL=0; FIFO_OVR=0; FTH=0; BOOT=0; DRDY_G=0; DRDY_XL=0
  mgos_i2c_write_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_INT1_CTRL, 0x40);

  // CTRL10_C: WRIST=0; 0; TIMER=0; PEDO=0; TILT=0; FUNC=1; PEDO_RST=0; SIGN_MOTION=1;
  mgos_i2c_write_reg_b(imu->acc->i2c, imu->acc->i2caddr, MGOS_LSM6DSL_REG_CTRL10_C, 0x05);

  LOG(LL_INFO, ("Installing interrupt handler on GPIO %d", gpio));
  mgos_gpio_set_mode(gpio, MGOS_GPIO_MODE_INPUT);
  mgos_gpio_set_pull(gpio, MGOS_GPIO_PULL_UP);
  mgos_gpio_set_int_handler(gpio, MGOS_GPIO_INT_EDGE_NEG, mgos_imu_lsm6dsl_irq, imu);
  mgos_gpio_clear_int(gpio);
  mgos_gpio_enable_int(gpio);

  iud->int_cb           = cb;
  iud->int_cb_user_data = user_data;
  iud->int_gpio         = gpio;
  return true;
}
