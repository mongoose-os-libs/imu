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

#include "mgos_imu_mpu6886.h"
#include "mgos.h"
#include "mgos_i2c.h"
#include "mgos_imu_mpu60x0.h"

static bool mgos_imu_mpu6886_detect(struct mgos_i2c *i2c, uint8_t i2caddr) {
  if (!i2c) {
    return false;
  }

  if (MGOS_MPU6886_DEVID !=
      mgos_i2c_read_reg_b(i2c, i2caddr, MGOS_MPU60X0_REG_WHO_AM_I)) {
    return false;
  }
  LOG(LL_WARN, ("This driver is experimental and work in progress. Proceed "
                "with caution!"));
  return true;
}

bool mgos_imu_mpu6886_acc_detect(struct mgos_imu_acc *dev,
                                 void *imu_user_data) {
  return mgos_imu_mpu6886_detect(dev->i2c, dev->i2caddr);

  (void)imu_user_data;
}

bool mgos_imu_mpu6886_gyro_detect(struct mgos_imu_gyro *dev,
                                  void *imu_user_data) {
  return mgos_imu_mpu6886_detect(dev->i2c, dev->i2caddr);

  (void)imu_user_data;
}
