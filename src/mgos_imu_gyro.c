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
#include "mgos_imu_mpu9250.h"

static struct mgos_imu_gyro *mgos_imu_gyro_create(void) {
  struct mgos_imu_gyro *gyro;
  gyro = calloc(1, sizeof(struct mgos_imu_gyro));
  if (!gyro) return NULL;
  memset(gyro, 0, sizeof(struct mgos_imu_gyro));

  gyro->type=GYRO_NONE;
  return gyro;
}

static bool mgos_imu_gyro_destroy(struct mgos_imu_gyro **gyro) {
  if (!*gyro) return false;
  if ((*gyro)->destroy) (*gyro)->destroy(*gyro);
  if ((*gyro)->user_data) free((*gyro)->user_data);
  free(*gyro);
  *gyro=NULL;
  return true;
}

bool mgos_imu_destroy_gyroscope(struct mgos_imu *imu) {
  bool ret;
  if (!imu || !imu->gyro) return false;
  ret=mgos_imu_gyro_destroy(&(imu->gyro));
  imu->gyro=NULL;
  return ret;
}

const char *mgos_imu_get_gyroscope_name(struct mgos_imu *imu) {
  if (!imu || !imu->gyro) return "VOID";

  switch (imu->gyro->type) {
  case GYRO_NONE: return "NONE";
  case GYRO_MPU9250: return "MPU9250";
  default: return "UNKNOWN";
  }
}

bool mgos_imu_get_gyroscope(struct mgos_imu *imu, float *x, float *y, float *z) {
  if (!imu->gyro || !imu->gyro->read) return false;

  if (!imu->gyro->read(imu->gyro)) {
    LOG(LL_ERROR, ("Could not read from gyroscope"));
    return false;
  }
  if (x) *x=imu->gyro->scale * imu->gyro->gx;
  if (y) *y=imu->gyro->scale * imu->gyro->gy;
  if (z) *z=imu->gyro->scale * imu->gyro->gz;
  return true;
}

bool mgos_imu_create_gyroscope_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, enum mgos_imu_gyro_type type) {
  if (!imu) return false;
  if (imu->gyro) mgos_imu_destroy_gyroscope(imu);
  imu->gyro=mgos_imu_gyro_create();
  if (!imu->gyro) false;
  imu->gyro->i2c=i2c;
  imu->gyro->i2caddr=i2caddr;
  imu->gyro->type=type;
  switch(type) {
    case GYRO_MPU9250:
      imu->gyro->detect = mgos_imu_mpu9250_gyro_detect;
      imu->gyro->create = mgos_imu_mpu9250_gyro_create;
      imu->gyro->read = mgos_imu_mpu9250_gyro_read;
      break;
    default:
      LOG(LL_ERROR, ("Unknown gyroscope type %d", type));
      mgos_imu_destroy_gyroscope(imu);
      return false;
  }
  if (imu->gyro->detect) {
    if (!imu->gyro->detect(imu->gyro)) {
      LOG(LL_ERROR, ("Could not detect gyroscope type %d (%s) at I2C 0x%02x", type, mgos_imu_get_gyroscope_name(imu), i2caddr));
      mgos_imu_destroy_gyroscope(imu);
      return false;
    } else {
      LOG(LL_DEBUG, ("Successfully detected gyroscope type %d (%s) at I2C 0x%02x", type, mgos_imu_get_gyroscope_name(imu), i2caddr));
    }
  }

  if (imu->gyro->create) {
    if (!imu->gyro->create(imu->gyro)) {
      LOG(LL_ERROR, ("Could not create gyroscope type %d (%s) at I2C 0x%02x", type, mgos_imu_get_gyroscope_name(imu), i2caddr));
      mgos_imu_destroy_gyroscope(imu);
      return false;
    } else {
      LOG(LL_DEBUG, ("Successfully created gyroscope type %d (%s) at I2C 0x%02x", type, mgos_imu_get_gyroscope_name(imu), i2caddr));
    }
  }

  return true;
}
