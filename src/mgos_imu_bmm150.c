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

#include "mgos_imu_bmm150.h"
#include "mgos.h"
#include "mgos_i2c.h"

// internal API to obtain compensated magnetometer data in uT
// trim data structure
struct mgos_imu_bmm150_trim_registers {
  int8_t dig_x1;
  int8_t dig_y1;
  int8_t dig_x2;
  int8_t dig_y2;
  uint16_t dig_z1;
  int16_t dig_z2;
  int16_t dig_z3;
  int16_t dig_z4;
  uint8_t dig_xy1;
  int8_t dig_xy2;
  uint16_t dig_xyz1;
};

static bool read_trim_registers(struct mgos_imu_mag *dev);
static int16_t
compensate_x(int16_t mag_data_x, uint16_t data_rhall,
             const struct mgos_imu_bmm150_trim_registers *trim_data);
static int16_t
compensate_y(int16_t mag_data_y, uint16_t data_rhall,
             const struct mgos_imu_bmm150_trim_registers *trim_data);
static int16_t
compensate_z(int16_t mag_data_z, uint16_t data_rhall,
             const struct mgos_imu_bmm150_trim_registers *trim_data);

// external API
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

/* Following code changes license type.
Original code has been conveniently modified to fit current environment
requirements */

/**
 * Copyright (c) 2020 Bosch Sensortec GmbH. All rights reserved.
 *
 * BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

static bool read_trim_registers(struct mgos_imu_mag *dev) {
  struct mgos_imu_bmm150_trim_registers *trim_data =
      (struct mgos_imu_bmm150_trim_registers *)dev->user_data;
  uint8_t trim_x1y1[2];
  uint8_t trim_xyz_data[4];
  uint8_t trim_xy1xy2[10];

  /* Trim register values are read */
  if (mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_BMM150_DIG_X1, 2,
                          trim_x1y1) &&
      mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_BMM150_DIG_Z4_LSB, 4,
                          trim_xyz_data) &&
      mgos_i2c_read_reg_n(dev->i2c, dev->i2caddr, MGOS_BMM150_DIG_Z2_LSB, 10,
                          trim_xy1xy2)) {
    /* Trim data which is read is updated
     * in the device structure
     */
    uint16_t temp_msb = 0;
    trim_data->dig_x1 = (int8_t)trim_x1y1[0];
    trim_data->dig_y1 = (int8_t)trim_x1y1[1];
    trim_data->dig_x2 = (int8_t)trim_xyz_data[2];
    trim_data->dig_y2 = (int8_t)trim_xyz_data[3];
    temp_msb = ((uint16_t)trim_xy1xy2[3]) << 8;
    trim_data->dig_z1 = (uint16_t)(temp_msb | trim_xy1xy2[2]);
    temp_msb = ((uint16_t)trim_xy1xy2[1]) << 8;
    trim_data->dig_z2 = (int16_t)(temp_msb | trim_xy1xy2[0]);
    temp_msb = ((uint16_t)trim_xy1xy2[7]) << 8;
    trim_data->dig_z3 = (int16_t)(temp_msb | trim_xy1xy2[6]);
    temp_msb = ((uint16_t)trim_xyz_data[1]) << 8;
    trim_data->dig_z4 = (int16_t)(temp_msb | trim_xyz_data[0]);
    trim_data->dig_xy1 = trim_xy1xy2[9];
    trim_data->dig_xy2 = (int8_t)trim_xy1xy2[8];
    temp_msb = ((uint16_t)(trim_xy1xy2[5] & 0x7F)) << 8;
    trim_data->dig_xyz1 = (uint16_t)(temp_msb | trim_xy1xy2[4]);
    return true;
  }

  return false;
}

/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer X axis data(micro-tesla) in int16_t.
 */
static int16_t
compensate_x(int16_t mag_data_x, uint16_t data_rhall,
             const struct mgos_imu_bmm150_trim_registers *trim_data) {
  int16_t retval;
  uint16_t process_comp_x0 = 0;
  int32_t process_comp_x1;
  uint16_t process_comp_x2;
  int32_t process_comp_x3;
  int32_t process_comp_x4;
  int32_t process_comp_x5;
  int32_t process_comp_x6;
  int32_t process_comp_x7;
  int32_t process_comp_x8;
  int32_t process_comp_x9;
  int32_t process_comp_x10;

  /* Overflow condition check */
  if (mag_data_x != MGOS_BMM150_OVERFLOW_ADCVAL_XYAXES_FLIP) {
    if (data_rhall != 0) {
      /* Availability of valid data */
      process_comp_x0 = data_rhall;
    } else if (trim_data->dig_xyz1 != 0) {
      process_comp_x0 = trim_data->dig_xyz1;
    } else {
      process_comp_x0 = 0;
    }

    if (process_comp_x0 != 0) {
      /* Processing compensation equations */
      process_comp_x1 = ((int32_t)trim_data->dig_xyz1) * 16384;
      process_comp_x2 =
          ((uint16_t)(process_comp_x1 / process_comp_x0)) - ((uint16_t)0x4000);
      retval = ((int16_t)process_comp_x2);
      process_comp_x3 = (((int32_t)retval) * ((int32_t)retval));
      process_comp_x4 =
          (((int32_t)trim_data->dig_xy2) * (process_comp_x3 / 128));
      process_comp_x5 = (int32_t)(((int16_t)trim_data->dig_xy1) * 128);
      process_comp_x6 = ((int32_t)retval) * process_comp_x5;
      process_comp_x7 =
          (((process_comp_x4 + process_comp_x6) / 512) + ((int32_t)0x100000));
      process_comp_x8 =
          ((int32_t)(((int16_t)trim_data->dig_x2) + ((int16_t)0xA0)));
      process_comp_x9 = ((process_comp_x7 * process_comp_x8) / 4096);
      process_comp_x10 = ((int32_t)mag_data_x) * process_comp_x9;
      retval = ((int16_t)(process_comp_x10 / 8192));
      retval = (retval + (((int16_t)trim_data->dig_x1) * 8)) / 16;
    } else {
      retval = MGOS_BMM150_OVERFLOW_OUTPUT;
    }
  } else {
    /* Overflow condition */
    retval = MGOS_BMM150_OVERFLOW_OUTPUT;
  }

  return retval;
}

/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer Y axis data(micro-tesla) in int16_t.
 */
static int16_t
compensate_y(int16_t mag_data_y, uint16_t data_rhall,
             const struct mgos_imu_bmm150_trim_registers *trim_data) {
  int16_t retval;
  uint16_t process_comp_y0 = 0;
  int32_t process_comp_y1;
  uint16_t process_comp_y2;
  int32_t process_comp_y3;
  int32_t process_comp_y4;
  int32_t process_comp_y5;
  int32_t process_comp_y6;
  int32_t process_comp_y7;
  int32_t process_comp_y8;
  int32_t process_comp_y9;

  /* Overflow condition check */
  if (mag_data_y != MGOS_BMM150_OVERFLOW_ADCVAL_XYAXES_FLIP) {
    if (data_rhall != 0) {
      /* Availability of valid data */
      process_comp_y0 = data_rhall;
    } else if (trim_data->dig_xyz1 != 0) {
      process_comp_y0 = trim_data->dig_xyz1;
    } else {
      process_comp_y0 = 0;
    }

    if (process_comp_y0 != 0) {
      /* Processing compensation equations */
      process_comp_y1 =
          (((int32_t)trim_data->dig_xyz1) * 16384) / process_comp_y0;
      process_comp_y2 = ((uint16_t)process_comp_y1) - ((uint16_t)0x4000);
      retval = ((int16_t)process_comp_y2);
      process_comp_y3 = ((int32_t)retval) * ((int32_t)retval);
      process_comp_y4 = ((int32_t)trim_data->dig_xy2) * (process_comp_y3 / 128);
      process_comp_y5 = ((int32_t)(((int16_t)trim_data->dig_xy1) * 128));
      process_comp_y6 =
          ((process_comp_y4 + (((int32_t)retval) * process_comp_y5)) / 512);
      process_comp_y7 =
          ((int32_t)(((int16_t)trim_data->dig_y2) + ((int16_t)0xA0)));
      process_comp_y8 =
          (((process_comp_y6 + ((int32_t)0x100000)) * process_comp_y7) / 4096);
      process_comp_y9 = (((int32_t)mag_data_y) * process_comp_y8);
      retval = (int16_t)(process_comp_y9 / 8192);
      retval = (retval + (((int16_t)trim_data->dig_y1) * 8)) / 16;
    } else {
      retval = MGOS_BMM150_OVERFLOW_OUTPUT;
    }
  } else {
    /* Overflow condition */
    retval = MGOS_BMM150_OVERFLOW_OUTPUT;
  }

  return retval;
}

/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer Z axis data(micro-tesla) in int16_t.
 */
static int16_t
compensate_z(int16_t mag_data_z, uint16_t data_rhall,
             const struct mgos_imu_bmm150_trim_registers *trim_data) {
  int32_t retval;
  int16_t process_comp_z0;
  int32_t process_comp_z1;
  int32_t process_comp_z2;
  int32_t process_comp_z3;
  int16_t process_comp_z4;

  if (mag_data_z != MGOS_BMM150_OVERFLOW_ADCVAL_ZAXIS_HALL) {
    if ((trim_data->dig_z2 != 0) && (trim_data->dig_z1 != 0) &&
        (data_rhall != 0) && (trim_data->dig_xyz1 != 0)) {
      /*Processing compensation equations */
      process_comp_z0 = ((int16_t)data_rhall) - ((int16_t)trim_data->dig_xyz1);
      process_comp_z1 =
          (((int32_t)trim_data->dig_z3) * ((int32_t)(process_comp_z0))) / 4;
      process_comp_z2 = (((int32_t)(mag_data_z - trim_data->dig_z4)) * 32768);
      process_comp_z3 =
          ((int32_t)trim_data->dig_z1) * (((int16_t)data_rhall) * 2);
      process_comp_z4 = (int16_t)((process_comp_z3 + (32768)) / 65536);
      retval = ((process_comp_z2 - process_comp_z1) /
                (trim_data->dig_z2 + process_comp_z4));

      /* Saturate result to +/- 2 micro-tesla */
      if (retval > MGOS_BMM150_POSITIVE_SATURATION_Z) {
        retval = MGOS_BMM150_POSITIVE_SATURATION_Z;
      } else if (retval < MGOS_BMM150_NEGATIVE_SATURATION_Z) {
        retval = MGOS_BMM150_NEGATIVE_SATURATION_Z;
      }

      /* Conversion of LSB to micro-tesla */
      retval = retval / 16;
    } else {
      retval = MGOS_BMM150_OVERFLOW_OUTPUT;
    }
  } else {
    /* Overflow condition */
    retval = MGOS_BMM150_OVERFLOW_OUTPUT;
  }

  return (int16_t)retval;
}
