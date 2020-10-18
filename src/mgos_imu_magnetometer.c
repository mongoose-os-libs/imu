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
#include "mgos_imu_ak8963.h"
#include "mgos_imu_ak8975.h"
#include "mgos_imu_bmm150.h"
#include "mgos_imu_mag3110.h"
#include "mgos_imu_lsm303d.h"
#include "mgos_imu_hmc5883l.h"
#include "mgos_imu_lsm9ds1.h"
#include "mgos_imu_icm20948.h"

static struct mgos_imu_mag *mgos_imu_mag_create(void) {
  struct mgos_imu_mag *mag;

  mag = calloc(1, sizeof(struct mgos_imu_mag));
  if (!mag) {
    return NULL;
  }
  memset(mag, 0, sizeof(struct mgos_imu_mag));

  mag->opts.type      = MAG_NONE;
  mag->orientation[0] = 1.f;
  mag->orientation[1] = 0.f;
  mag->orientation[2] = 0.f;
  mag->orientation[3] = 0.f;
  mag->orientation[4] = 1.f;
  mag->orientation[5] = 0.f;
  mag->orientation[6] = 0.f;
  mag->orientation[7] = 0.f;
  mag->orientation[8] = 1.f;
  return mag;
}

static bool mgos_imu_mag_destroy(struct mgos_imu_mag **mag, void *imu_user_data) {
  if (!*mag) {
    return false;
  }
  if ((*mag)->destroy) {
    (*mag)->destroy(*mag, imu_user_data);
  }
  if ((*mag)->user_data) {
    free((*mag)->user_data);
  }
  free(*mag);
  *mag = NULL;
  return true;
}

bool mgos_imu_magnetometer_destroy(struct mgos_imu *imu) {
  bool ret;

  if (!imu || !imu->mag) {
    return false;
  }
  ret      = mgos_imu_mag_destroy(&(imu->mag), imu->user_data);
  imu->mag = NULL;
  return ret;
}

const char *mgos_imu_magnetometer_get_name(struct mgos_imu *imu) {
  if (!imu || !imu->mag) {
    return "VOID";
  }

  switch (imu->mag->opts.type) {
  case MAG_NONE: return "NONE";

  case MAG_AK8963: return "AK8963";

  case MAG_AK8975: return "AK8975";

  case MAG_BMM150: return "BMM150";

  case MAG_MAG3110: return "MAG3110";

  case MAG_LSM303D: return "LSM303D";

  case MAG_LSM303DLM: return "LSM303DLM";

  case MAG_HMC5883L: return "HMC5883L";

  case MAG_LSM9DS1: return "LSM9DS1";

  case MAG_ICM20948: return "ICM20948";

  default: return "UNKNOWN";
  }
}

bool mgos_imu_magnetometer_get(struct mgos_imu *imu, float *x, float *y, float *z) {
  float mxb, myb, mzb;

  if (!imu->mag || !imu->mag->read) {
    return false;
  }

  if (!imu->mag->read(imu->mag, imu->user_data)) {
    LOG(LL_ERROR, ("Could not read from magnetometer"));
    return false;
  }
  // LOG(LL_DEBUG, ("Raw: mx=%d my=%d mz=%d", imu->mag->mx, imu->mag->my, imu->mag->mz));
  mxb = imu->mag->bias[0] * imu->mag->mx * imu->mag->scale;
  myb = imu->mag->bias[1] * imu->mag->my * imu->mag->scale;
  mzb = imu->mag->bias[2] * imu->mag->mz * imu->mag->scale;
  if (x) {
    *x = (mxb * imu->mag->orientation[0] + myb * imu->mag->orientation[1] + mzb * imu->mag->orientation[2]);
  }
  if (y) {
    *y = (mxb * imu->mag->orientation[3] + myb * imu->mag->orientation[4] + mzb * imu->mag->orientation[5]);
  }
  if (z) {
    *z = (mxb * imu->mag->orientation[6] + myb * imu->mag->orientation[7] + mzb * imu->mag->orientation[8]);
  }
  return true;
}

bool mgos_imu_magnetometer_create_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, const struct mgos_imu_mag_opts *opts) {
  if (!imu || !i2c || !opts) {
    return false;
  }
  if (imu->mag) {
    mgos_imu_magnetometer_destroy(imu);
  }
  imu->mag = mgos_imu_mag_create();
  if (!imu->mag) {
    return false;
  }
  imu->mag->i2c     = i2c;
  imu->mag->i2caddr = i2caddr;
  imu->mag->opts    = *opts;
  switch (opts->type) {
  case MAG_LSM9DS1:
    imu->mag->detect = mgos_imu_lsm9ds1_mag_detect;
    imu->mag->create = mgos_imu_lsm9ds1_mag_create;
    imu->mag->read   = mgos_imu_lsm9ds1_mag_read;
    if (!imu->user_data) {
      imu->user_data = mgos_imu_lsm9ds1_userdata_create();
    }
    break;

  case MAG_HMC5883L:
    imu->mag->detect = mgos_imu_hmc5883l_detect;
    imu->mag->create = mgos_imu_hmc5883l_create;
    imu->mag->read   = mgos_imu_hmc5883l_read;
    break;

  case MAG_LSM303DLM:
  case MAG_LSM303D:
    imu->mag->detect = mgos_imu_lsm303d_mag_detect;
    imu->mag->create = mgos_imu_lsm303d_mag_create;
    imu->mag->read   = mgos_imu_lsm303d_mag_read;
    if (!imu->user_data) {
      imu->user_data = mgos_imu_lsm303d_userdata_create();
    }
    break;

  case MAG_AK8963:
    imu->mag->detect = mgos_imu_ak8963_detect;
    imu->mag->create = mgos_imu_ak8963_create;
    imu->mag->read   = mgos_imu_ak8963_read;
    break;

  case MAG_AK8975:
    imu->mag->detect = mgos_imu_ak8975_detect;
    imu->mag->create = mgos_imu_ak8975_create;
    imu->mag->read   = mgos_imu_ak8975_read;
    break;

  case MAG_BMM150:
    imu->mag->detect = mgos_imu_bmm150_detect;
    imu->mag->create = mgos_imu_bmm150_create;
    imu->mag->read   = mgos_imu_bmm150_read;
    break;

  case MAG_MAG3110:
    imu->mag->detect = mgos_imu_mag3110_detect;
    imu->mag->create = mgos_imu_mag3110_create;
    imu->mag->read   = mgos_imu_mag3110_read;
    break;

  case MAG_ICM20948:
    imu->mag->detect    = mgos_imu_icm20948_mag_detect;
    imu->mag->create    = mgos_imu_icm20948_mag_create;
    imu->mag->read      = mgos_imu_icm20948_mag_read;
    imu->mag->get_odr   = mgos_imu_icm20948_mag_get_odr;
    imu->mag->set_odr   = mgos_imu_icm20948_mag_set_odr;
    if (!imu->user_data) {
      imu->user_data = mgos_imu_icm20948_userdata_create();
    }
    break;

  default:
    LOG(LL_ERROR, ("Unknown magnetometer type %d", opts->type));
    mgos_imu_magnetometer_destroy(imu);
    return false;
  }

  if (imu->mag->detect) {
    if (!imu->mag->detect(imu->mag, imu->user_data)) {
      LOG(LL_ERROR, ("Could not detect magnetometer type %d (%s) at I2C 0x%02x",
                     opts->type, mgos_imu_magnetometer_get_name(imu), i2caddr));
      mgos_imu_magnetometer_destroy(imu);
      return false;
    } else {
      LOG(LL_DEBUG, ("Successfully detected magnetometer type %d (%s) at I2C 0x%02x",
                     opts->type, mgos_imu_magnetometer_get_name(imu), i2caddr));
    }
  }

  if (imu->mag->create) {
    if (!imu->mag->create(imu->mag, imu->user_data)) {
      LOG(LL_ERROR, ("Could not create magnetometer type %d (%s) at I2C 0x%02x",
                     opts->type, mgos_imu_magnetometer_get_name(imu), i2caddr));
      mgos_imu_magnetometer_destroy(imu);
      return false;
    } else {
      LOG(LL_DEBUG, ("Successfully created magnetometer type %d (%s) at I2C 0x%02x",
                     opts->type, mgos_imu_magnetometer_get_name(imu), i2caddr));
    }
  }
  if (imu->mag->set_scale) {
    imu->mag->set_scale(imu->mag, imu->user_data, opts->scale);
  }

  if (imu->mag->set_odr) {
    imu->mag->set_odr(imu->mag, imu->user_data, opts->odr);
  }

  return true;
}

bool mgos_imu_magnetometer_get_orientation(struct mgos_imu *imu, float v[9]) {
  if (!imu || !imu->mag || !v) {
    return false;
  }
  memcpy(v, imu->mag->orientation, sizeof(float) * 9);
  return true;
}

bool mgos_imu_magnetometer_set_orientation(struct mgos_imu *imu, float v[9]) {
  if (!imu || !imu->mag || !v) {
    return false;
  }
  memcpy(imu->mag->orientation, v, sizeof(float) * 9);
  return true;
}

bool mgos_imu_magnetometer_get_scale(struct mgos_imu *imu, float *scale) {
  if (!imu || !imu->mag || !imu->mag->get_scale || !scale) {
    return false;
  }
  return imu->mag->get_scale(imu->mag, imu->user_data, scale);
}

bool mgos_imu_magnetometer_set_scale(struct mgos_imu *imu, float scale) {
  if (!imu || !imu->mag || !imu->mag->set_scale) {
    return false;
  }
  return imu->mag->set_scale(imu->mag, imu->user_data, scale);
}

bool mgos_imu_magnetometer_get_odr(struct mgos_imu *imu, float *hertz) {
  if (!imu || !imu->mag || !imu->mag->get_odr || !hertz) {
    return false;
  }
  return imu->mag->get_odr(imu->mag, imu->user_data, hertz);
}

bool mgos_imu_magnetometer_set_odr(struct mgos_imu *imu, float hertz) {
  if (!imu || !imu->mag || !imu->mag->set_odr) {
    return false;
  }
  return imu->mag->set_odr(imu->mag, imu->user_data, hertz);
}
