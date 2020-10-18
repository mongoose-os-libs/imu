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
#include "mgos_imu_internal.h"
#include "mgos_imu_mpu925x.h"
#include "mgos_imu_l3gd20.h"
#include "mgos_imu_itg3205.h"
#include "mgos_imu_lsm9ds1.h"
#include "mgos_imu_lsm6dsl.h"
#include "mgos_imu_mpu60x0.h"
#include "mgos_imu_mpu6886.h"
#include "mgos_imu_icm20948.h"

static struct mgos_imu_gyro *mgos_imu_gyro_create(void) {
  struct mgos_imu_gyro *gyro;

  gyro = calloc(1, sizeof(struct mgos_imu_gyro));
  if (!gyro) {
    return NULL;
  }
  memset(gyro, 0, sizeof(struct mgos_imu_gyro));

  gyro->opts.type      = GYRO_NONE;
  gyro->orientation[0] = 1.f;
  gyro->orientation[1] = 0.f;
  gyro->orientation[2] = 0.f;
  gyro->orientation[3] = 0.f;
  gyro->orientation[4] = 1.f;
  gyro->orientation[5] = 0.f;
  gyro->orientation[6] = 0.f;
  gyro->orientation[7] = 0.f;
  gyro->orientation[8] = 1.f;
  return gyro;
}

static bool mgos_imu_gyro_destroy(struct mgos_imu_gyro **gyro, void *imu_user_data) {
  if (!*gyro) {
    return false;
  }
  if ((*gyro)->destroy) {
    (*gyro)->destroy(*gyro, imu_user_data);
  }
  if ((*gyro)->user_data) {
    free((*gyro)->user_data);
  }
  free(*gyro);
  *gyro = NULL;
  return true;
}

bool mgos_imu_gyroscope_destroy(struct mgos_imu *imu) {
  bool ret;

  if (!imu || !imu->gyro) {
    return false;
  }
  ret       = mgos_imu_gyro_destroy(&(imu->gyro), imu->user_data);
  imu->gyro = NULL;
  return ret;
}

const char *mgos_imu_gyroscope_get_name(struct mgos_imu *imu) {
  if (!imu || !imu->gyro) {
    return "VOID";
  }

  switch (imu->gyro->opts.type) {
  case GYRO_NONE: return "NONE";

  case GYRO_MPU9250: return "MPU9250";

  case GYRO_MPU9255: return "MPU9255";

  case GYRO_L3GD20: return "L3GD20";

  case GYRO_L3GD20H: return "L3GD20H";

  case GYRO_ITG3205: return "ITG3205";

  case GYRO_LSM9DS1: return "LSM9DS1";

  case GYRO_LSM6DSL: return "LSM6DSL";

  case GYRO_MPU6000: return "MPU6000";

  case GYRO_MPU6050: return "MPU6050";

  case GYRO_MPU6886: return "MPU6886";

  case GYRO_ICM20948: return "ICM20948";

  default: return "UNKNOWN";
  }
}

bool mgos_imu_gyroscope_get(struct mgos_imu *imu, float *x, float *y, float *z) {
  if (!imu->gyro || !imu->gyro->read) {
    return false;
  }

  if (!imu->gyro->read(imu->gyro, imu->user_data)) {
    LOG(LL_ERROR, ("Could not read from gyroscope"));
    return false;
  }
  // LOG(LL_DEBUG, ("Raw: gx=%d gy=%d gz=%d", imu->gyro->gx, imu->gyro->gy, imu->gyro->gz));
  if (x) {
    *x = (imu->gyro->scale *
          (imu->gyro->gx * imu->gyro->orientation[0] + imu->gyro->gy * imu->gyro->orientation[1] + imu->gyro->gz * imu->gyro->orientation[2])
          ) + imu->gyro->offset_gx;
  }
  if (y) {
    *y = (imu->gyro->scale *
          (imu->gyro->gx * imu->gyro->orientation[3] + imu->gyro->gy * imu->gyro->orientation[4] + imu->gyro->gz * imu->gyro->orientation[5])
          ) + imu->gyro->offset_gy;
  }
  if (z) {
    *z = (imu->gyro->scale *
          (imu->gyro->gx * imu->gyro->orientation[6] + imu->gyro->gy * imu->gyro->orientation[7] + imu->gyro->gz * imu->gyro->orientation[8])
          ) + imu->gyro->offset_gz;
  }
  return true;
}

bool mgos_imu_gyroscope_create_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, const struct mgos_imu_gyro_opts *opts) {
  if (!imu || !i2c || !opts) {
    return false;
  }
  if (imu->gyro) {
    mgos_imu_gyroscope_destroy(imu);
  }
  imu->gyro = mgos_imu_gyro_create();
  if (!imu->gyro) {
    false;
  }
  imu->gyro->i2c     = i2c;
  imu->gyro->i2caddr = i2caddr;
  imu->gyro->opts    = *opts;
  switch (opts->type) {
  case GYRO_MPU6000:
  case GYRO_MPU6050:
    imu->gyro->detect    = mgos_imu_mpu60x0_gyro_detect;
    imu->gyro->create    = mgos_imu_mpu60x0_gyro_create;
    imu->gyro->read      = mgos_imu_mpu60x0_gyro_read;
    imu->gyro->get_scale = mgos_imu_mpu60x0_gyro_get_scale;
    imu->gyro->set_scale = mgos_imu_mpu60x0_gyro_set_scale;
    if (!imu->user_data) {
      imu->user_data = mgos_imu_mpu60x0_userdata_create();
    }
    break;

  case GYRO_MPU6886:
    imu->gyro->detect    = mgos_imu_mpu6886_gyro_detect;
    imu->gyro->create    = mgos_imu_mpu60x0_gyro_create;
    imu->gyro->read      = mgos_imu_mpu60x0_gyro_read;
    imu->gyro->get_scale = mgos_imu_mpu60x0_gyro_get_scale;
    imu->gyro->set_scale = mgos_imu_mpu60x0_gyro_set_scale;
    if (!imu->user_data) {
      imu->user_data = mgos_imu_mpu60x0_userdata_create();
    }
    break;

  case GYRO_LSM6DSL:
    imu->gyro->detect = mgos_imu_lsm6dsl_gyro_detect;
    imu->gyro->create = mgos_imu_lsm6dsl_gyro_create;
    imu->gyro->read   = mgos_imu_lsm6dsl_gyro_read;
    if (!imu->user_data) {
      imu->user_data = mgos_imu_lsm6dsl_userdata_create();
    }
    break;

  case GYRO_LSM9DS1:
    imu->gyro->detect = mgos_imu_lsm9ds1_gyro_detect;
    imu->gyro->create = mgos_imu_lsm9ds1_gyro_create;
    imu->gyro->read   = mgos_imu_lsm9ds1_gyro_read;
    if (!imu->user_data) {
      imu->user_data = mgos_imu_lsm9ds1_userdata_create();
    }
    break;

  case GYRO_ITG3205:
    imu->gyro->detect = mgos_imu_itg3205_detect;
    imu->gyro->create = mgos_imu_itg3205_create;
    imu->gyro->read   = mgos_imu_itg3205_read;
    break;

  case GYRO_L3GD20:
  case GYRO_L3GD20H:
    imu->gyro->detect = mgos_imu_l3gd20_detect;
    imu->gyro->create = mgos_imu_l3gd20_create;
    imu->gyro->read   = mgos_imu_l3gd20_read;
    break;

  case GYRO_MPU9250:
  case GYRO_MPU9255:
    imu->gyro->detect    = mgos_imu_mpu925x_gyro_detect;
    imu->gyro->create    = mgos_imu_mpu925x_gyro_create;
    imu->gyro->read      = mgos_imu_mpu925x_gyro_read;
    imu->gyro->get_scale = mgos_imu_mpu925x_gyro_get_scale;
    imu->gyro->set_scale = mgos_imu_mpu925x_gyro_set_scale;
    if (!imu->user_data) {
      imu->user_data = mgos_imu_mpu925x_userdata_create();
    }
    break;

  case GYRO_ICM20948:
    imu->gyro->detect    = mgos_imu_icm20948_gyro_detect;
    imu->gyro->create    = mgos_imu_icm20948_gyro_create;
    imu->gyro->read      = mgos_imu_icm20948_gyro_read;
    imu->gyro->get_odr   = mgos_imu_icm20948_gyro_get_odr;
    imu->gyro->set_odr   = mgos_imu_icm20948_gyro_set_odr;
    imu->gyro->get_scale = mgos_imu_icm20948_gyro_get_scale;
    imu->gyro->set_scale = mgos_imu_icm20948_gyro_set_scale;
    if (!imu->user_data) {
      imu->user_data = mgos_imu_icm20948_userdata_create();
    }
    break;

  default:
    LOG(LL_ERROR, ("Unknown gyroscope type %d", opts->type));
    mgos_imu_gyroscope_destroy(imu);
    return false;
  }
  if (imu->gyro->detect) {
    if (!imu->gyro->detect(imu->gyro, imu->user_data)) {
      LOG(LL_ERROR, ("Could not detect gyroscope type %d (%s) at I2C 0x%02x",
                     opts->type, mgos_imu_gyroscope_get_name(imu), i2caddr));
      mgos_imu_gyroscope_destroy(imu);
      return false;
    } else {
      LOG(LL_DEBUG, ("Successfully detected gyroscope type %d (%s) at I2C 0x%02x",
                     opts->type, mgos_imu_gyroscope_get_name(imu), i2caddr));
    }
  }

  if (imu->gyro->create) {
    if (!imu->gyro->create(imu->gyro, imu->user_data)) {
      LOG(LL_ERROR, ("Could not create gyroscope type %d (%s) at I2C 0x%02x",
                     opts->type, mgos_imu_gyroscope_get_name(imu), i2caddr));
      mgos_imu_gyroscope_destroy(imu);
      return false;
    } else {
      LOG(LL_DEBUG, ("Successfully created gyroscope type %d (%s) at I2C 0x%02x",
                     opts->type, mgos_imu_gyroscope_get_name(imu), i2caddr));
    }
  }
  if (imu->gyro->set_scale) {
    imu->gyro->set_scale(imu->gyro, imu->user_data, opts->scale);
  }

  if (imu->gyro->set_odr) {
    imu->gyro->set_odr(imu->gyro, imu->user_data, opts->odr);
  }


  return true;
}

bool mgos_imu_gyroscope_set_offset(struct mgos_imu *imu, float x, float y, float z) {
  if (!imu || !imu->gyro) {
    return false;
  }
  imu->gyro->offset_gx = x;
  imu->gyro->offset_gy = y;
  imu->gyro->offset_gz = z;
  return true;
}

bool mgos_imu_gyroscope_get_offset(struct mgos_imu *imu, float *x, float *y, float *z) {
  if (!imu || !imu->gyro) {
    return false;
  }
  if (x) {
    *x = imu->gyro->offset_gx;
  }
  if (y) {
    *y = imu->gyro->offset_gy;
  }
  if (z) {
    *z = imu->gyro->offset_gz;
  }
  return true;
}

bool mgos_imu_gyroscope_get_orientation(struct mgos_imu *imu, float v[9]) {
  if (!imu || !imu->gyro || !v) {
    return false;
  }
  memcpy(v, imu->gyro->orientation, sizeof(float) * 9);
  return true;
}

bool mgos_imu_gyroscope_set_orientation(struct mgos_imu *imu, float v[9]) {
  if (!imu || !imu->gyro || !v) {
    return false;
  }
  memcpy(imu->gyro->orientation, v, sizeof(float) * 9);
  return true;
}

bool mgos_imu_gyroscope_get_scale(struct mgos_imu *imu, float *scale) {
  if (!imu || !imu->gyro || !imu->gyro->get_scale || !scale) {
    return false;
  }
  return imu->gyro->get_scale(imu->gyro, imu->user_data, scale);
}

bool mgos_imu_gyroscope_set_scale(struct mgos_imu *imu, float scale) {
  if (!imu || !imu->gyro || !imu->gyro->set_scale) {
    return false;
  }
  return imu->gyro->set_scale(imu->gyro, imu->user_data, scale);
}

bool mgos_imu_gyroscope_get_odr(struct mgos_imu *imu, float *hertz) {
  if (!imu || !imu->gyro || !imu->gyro->get_odr || !hertz) {
    return false;
  }
  return imu->gyro->get_odr(imu->gyro, imu->user_data, hertz);
}

bool mgos_imu_gyroscope_set_odr(struct mgos_imu *imu, float hertz) {
  if (!imu || !imu->gyro || !imu->gyro->set_odr) {
    return false;
  }
  return imu->gyro->set_odr(imu->gyro, imu->user_data, hertz);
}
