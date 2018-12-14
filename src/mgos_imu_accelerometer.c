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
#include "mgos_imu_adxl345.h"

static struct mgos_imu_acc *mgos_imu_acc_create(void) {
  struct mgos_imu_acc *acc;

  acc = calloc(1, sizeof(struct mgos_imu_acc));
  if (!acc) {
    return NULL;
  }
  memset(acc, 0, sizeof(struct mgos_imu_acc));

  acc->type = ACC_NONE;
  return acc;
}

static bool mgos_imu_acc_destroy(struct mgos_imu_acc **acc, void *imu_user_data) {
  if (!*acc) {
    return false;
  }
  if ((*acc)->destroy) {
    (*acc)->destroy(*acc, imu_user_data);
  }
  if ((*acc)->user_data) {
    free((*acc)->user_data);
  }
  free(*acc);
  *acc = NULL;
  return true;
}

bool mgos_imu_accelerometer_destroy(struct mgos_imu *imu) {
  bool ret;

  if (!imu || !imu->acc) {
    return false;
  }
  ret      = mgos_imu_acc_destroy(&(imu->acc), imu->user_data);
  imu->acc = NULL;
  return ret;
}

const char *mgos_imu_accelerometer_get_name(struct mgos_imu *imu) {
  if (!imu || !imu->acc) {
    return "VOID";
  }

  switch (imu->acc->type) {
  case ACC_NONE: return "NONE";

  case ACC_MPU9250: return "MPU9250";

  case ACC_ADXL345: return "ADXL345";

  default: return "UNKNOWN";
  }
}

bool mgos_imu_accelerometer_get(struct mgos_imu *imu, float *x, float *y, float *z) {
  if (!imu->acc || !imu->acc->read) {
    return false;
  }

  if (!imu->acc->read(imu->acc, imu->user_data)) {
    LOG(LL_ERROR, ("Could not read from accelerometer"));
    return false;
  }
  if (x) {
    *x = (imu->acc->scale * imu->acc->ax) + imu->acc->offset_ax;
  }
  if (y) {
    *y = (imu->acc->scale * imu->acc->ay) + imu->acc->offset_ay;
  }
  if (z) {
    *z = (imu->acc->scale * imu->acc->az) + imu->acc->offset_az;
  }
  return true;
}

bool mgos_imu_accelerometer_create_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, enum mgos_imu_acc_type type) {
  if (!imu) {
    return false;
  }
  if (imu->acc) {
    mgos_imu_accelerometer_destroy(imu);
  }
  imu->acc = mgos_imu_acc_create();
  if (!imu->acc) {
    false;
  }
  imu->acc->i2c     = i2c;
  imu->acc->i2caddr = i2caddr;
  imu->acc->type    = type;
  switch (type) {
  case ACC_MPU9250:
    imu->acc->detect = mgos_imu_mpu9250_acc_detect;
    imu->acc->create = mgos_imu_mpu9250_acc_create;
    imu->acc->read   = mgos_imu_mpu9250_acc_read;
    if (!imu->user_data) {
      imu->user_data = mgos_imu_mpu9250_userdata_create();
    }
    break;

  case ACC_ADXL345:
    imu->acc->detect = mgos_imu_adxl345_detect;
    imu->acc->create = mgos_imu_adxl345_create;
    imu->acc->read   = mgos_imu_adxl345_read;
    break;

  default:
    LOG(LL_ERROR, ("Unknown accelerometer type %d", type));
    mgos_imu_accelerometer_destroy(imu);
    return false;
  }
  if (imu->acc->detect) {
    if (!imu->acc->detect(imu->acc, imu->user_data)) {
      LOG(LL_ERROR, ("Could not detect accelerometer type %d (%s) at I2C 0x%02x", type, mgos_imu_accelerometer_get_name(imu), i2caddr));
      mgos_imu_accelerometer_destroy(imu);
      return false;
    } else {
      LOG(LL_DEBUG, ("Successfully detected accelerometer type %d (%s) at I2C 0x%02x", type, mgos_imu_accelerometer_get_name(imu), i2caddr));
    }
  }

  if (imu->acc->create) {
    if (!imu->acc->create(imu->acc, imu->user_data)) {
      LOG(LL_ERROR, ("Could not create accelerometer type %d (%s) at I2C 0x%02x", type, mgos_imu_accelerometer_get_name(imu), i2caddr));
      mgos_imu_accelerometer_destroy(imu);
      return false;
    } else {
      LOG(LL_DEBUG, ("Successfully created accelerometer type %d (%s) at I2C 0x%02x", type, mgos_imu_accelerometer_get_name(imu), i2caddr));
    }
  }

  return true;
}

bool mgos_imu_accelerometer_set_offset(struct mgos_imu *imu, float x, float y, float z) {
  if (!imu || !imu->acc) {
    return false;
  }
  imu->acc->offset_ax = x;
  imu->acc->offset_ay = y;
  imu->acc->offset_az = z;
  return true;
}

bool mgos_imu_accelerometer_get_offset(struct mgos_imu *imu, float *x, float *y, float *z) {
  if (!imu || !imu->acc) {
    return false;
  }
  if (x) {
    *x = imu->acc->offset_ax;
  }
  if (y) {
    *y = imu->acc->offset_ay;
  }
  if (z) {
    *z = imu->acc->offset_az;
  }
  return true;
}
