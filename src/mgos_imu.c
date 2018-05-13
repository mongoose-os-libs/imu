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

// Private functions follow
// Private functions end

// Public functions follow
struct mgos_imu *mgos_imu_create(void) {
  struct mgos_imu *imu;

  imu = calloc(1, sizeof(struct mgos_imu));
  if (!imu) {
    return NULL;
  }
  memset(imu, 0, sizeof(struct mgos_imu));
  return imu;
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
      imu->gyro->destroy = mgos_imu_mpu9250_gyro_destroy;
      imu->gyro->read = mgos_imu_mpu9250_gyro_read;
      break;
    default:
      LOG(LL_ERROR, ("Unknown gyroscope type %d", type));
      mgos_imu_destroy_gyroscope(imu);
      return false;
  }
  return false;
}

bool mgos_imu_create_accelerometer_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, enum mgos_imu_acc_type type) {
  if (!imu) return false;
  if (imu->acc) mgos_imu_destroy_accelerometer(imu);
  imu->acc=mgos_imu_acc_create();
  if (!imu->acc) false;
  imu->acc->i2c=i2c;
  imu->acc->i2caddr=i2caddr;
  imu->acc->type=type;
  switch(type) {
    case ACC_MPU9250:
      imu->acc->detect = mgos_imu_mpu9250_acc_detect;
      imu->acc->create = mgos_imu_mpu9250_acc_create;
      imu->acc->destroy = mgos_imu_mpu9250_acc_destroy;
      imu->acc->read = mgos_imu_mpu9250_acc_read;
      break;
    default:
      LOG(LL_ERROR, ("Unknown accelerometer type %d", type));
      mgos_imu_destroy_accelerometer(imu);
      return false;
  }
  return false;
}

bool mgos_imu_create_magnetometer_i2c(struct mgos_imu *imu, struct mgos_i2c *i2c, uint8_t i2caddr, enum mgos_imu_mag_type type) {
  if (!imu) return false;
  if (imu->mag) mgos_imu_destroy_magnetometer(imu);
  imu->mag=mgos_imu_mag_create();
  if (!imu->mag) false;
  imu->mag->i2c=i2c;
  imu->mag->i2caddr=i2caddr;
  imu->mag->type=type;
  switch(type) {
    default:
      LOG(LL_ERROR, ("Unknown magnetometer type %d", type));
      mgos_imu_destroy_magnetometer(imu);
      return false;
  }
  return false;
}

bool mgos_imu_destroy_gyroscope(struct mgos_imu *imu) {
  bool ret;
  if (!imu || !imu->gyro) return false;
  ret=mgos_imu_gyro_destroy(&(imu->gyro));
  imu->gyro=NULL;
  return ret;
}

bool mgos_imu_destroy_accelerometer(struct mgos_imu *imu) {
  bool ret;
  if (!imu || !imu->acc) return false;
  ret=mgos_imu_acc_destroy(&(imu->acc));
  imu->acc=NULL;
  return ret;
}

bool mgos_imu_destroy_magnetometer(struct mgos_imu *imu) {
  bool ret;
  if (!imu || !imu->mag) return false;
  ret=mgos_imu_mag_destroy(&(imu->mag));
  imu->mag=NULL;
  return ret;
}

void mgos_imu_destroy(struct mgos_imu **imu) {
  if (!*imu) {
    return;
  }
  mgos_imu_destroy_gyroscope(*imu);
  mgos_imu_destroy_accelerometer(*imu);
  mgos_imu_destroy_magnetometer(*imu);

  free(*imu);
  *imu = NULL;
  return;
}

bool mgos_imu_has_accelerometer(struct mgos_imu *imu) {
  if (!imu) {
    return false;
  }
  return imu->capabilities & MGOS_IMU_CAP_ACCELEROMETER;
}

bool mgos_imu_has_gyroscope(struct mgos_imu *imu) {
  if (!imu) {
    return false;
  }
  return imu->capabilities & MGOS_IMU_CAP_GYROSCOPE;
}

bool mgos_imu_has_magnetometer(struct mgos_imu *imu) {
  if (!imu) {
    return false;
  }
  return imu->capabilities & MGOS_IMU_CAP_MAGNETOMETER;
}

bool mgos_imu_has_thermometer(struct mgos_imu *imu) {
  if (!imu) {
    return false;
  }
  return imu->capabilities & MGOS_IMU_CAP_THERMOMETER;
}

bool mgos_imu_init(void) {
  return true;
}

// Public functions end
