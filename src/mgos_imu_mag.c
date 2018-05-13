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
#include "mgos_imu_mag3110.h"

static struct mgos_imu_mag *mgos_imu_mag_create(void) {
  struct mgos_imu_mag *mag;
  mag = calloc(1, sizeof(struct mgos_imu_mag));
  if (!mag) return NULL;
  memset(mag, 0, sizeof(struct mgos_imu_mag));

  mag->type=MAG_NONE;
  return mag;
}

static bool mgos_imu_mag_destroy(struct mgos_imu_mag **mag) {
  if (!*mag) return false;
  if ((*mag)->destroy) (*mag)->destroy(*mag);
  if ((*mag)->user_data) free((*mag)->user_data);
  free(*mag);
  *mag=NULL;
  return true;
}

bool mgos_imu_magnetometer_destroy(struct mgos_imu *imu) {
  bool ret;
  if (!imu || !imu->mag) return false;
  ret=mgos_imu_mag_destroy(&(imu->mag));
  imu->mag=NULL;
  return ret;
}

const char *mgos_imu_magnetometer_get_name(struct mgos_imu *imu) {
  if (!imu || !imu->mag) return "VOID";

  switch (imu->mag->type) {
  case MAG_NONE: return "NONE";
  case MAG_AK8963: return "AK8963";
  case MAG_MAG3110: return "MAG3110";
  default: return "UNKNOWN";
  }
}

bool mgos_imu_magnetometer_get(struct mgos_imu *imu, float *x, float *y, float *z) {
  if (!imu->mag || !imu->mag->read) return false;

  if (!imu->mag->read(imu->mag)) {
    LOG(LL_ERROR, ("Could not read from magnetometer"));
    return false;
  }
  if (x) *x=imu->mag->scale * imu->mag->bias[0] * imu->mag->mx;
  if (y) *y=imu->mag->scale * imu->mag->bias[1] * imu->mag->my;
  if (z) *z=imu->mag->scale * imu->mag->bias[2] * imu->mag->mz;
  return true;
}

bool mgos_imu_magnetometer_create_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, enum mgos_imu_mag_type type) {
  if (!imu) return false;
  if (imu->mag) mgos_imu_magnetometer_destroy(imu);
  imu->mag=mgos_imu_mag_create();
  if (!imu->mag) false;
  imu->mag->i2c=i2c;
  imu->mag->i2caddr=i2caddr;
  imu->mag->type=type;
  switch(type) {
    case MAG_AK8963:
      imu->mag->detect = mgos_imu_ak8963_detect;
      imu->mag->create = mgos_imu_ak8963_create;
      imu->mag->read = mgos_imu_ak8963_read;
      break;
    case MAG_MAG3110:
      imu->mag->detect = mgos_imu_mag3110_detect;
      imu->mag->create = mgos_imu_mag3110_create;
      imu->mag->read = mgos_imu_mag3110_read;
      break;
    default:
      LOG(LL_ERROR, ("Unknown magnetometer type %d", type));
      mgos_imu_magnetometer_destroy(imu);
      return false;
  }

  if (imu->mag->detect) {
    if (!imu->mag->detect(imu->mag)) {
      LOG(LL_ERROR, ("Could not detect magnetometer type %d (%s) at I2C 0x%02x", type, mgos_imu_magnetometer_get_name(imu), i2caddr));
      mgos_imu_magnetometer_destroy(imu);
      return false;
    } else {
      LOG(LL_DEBUG, ("Successfully detected magnetometer type %d (%s) at I2C 0x%02x", type, mgos_imu_magnetometer_get_name(imu), i2caddr));
    }
  }

  if (imu->mag->create) {
    if (!imu->mag->create(imu->mag)) {
      LOG(LL_ERROR, ("Could not create magnetometer type %d (%s) at I2C 0x%02x", type, mgos_imu_magnetometer_get_name(imu), i2caddr));
      mgos_imu_magnetometer_destroy(imu);
      return false;
    } else {
      LOG(LL_DEBUG, ("Successfully created magnetometer type %d (%s) at I2C 0x%02x", type, mgos_imu_magnetometer_get_name(imu), i2caddr));
    }
  }

  return true;
}
