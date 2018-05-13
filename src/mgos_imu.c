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
  if (imu->acc->detect) {
    if (!imu->acc->detect(imu->acc)) {
      LOG(LL_ERROR, ("Could not detect accelerometer type %d (%s) at I2C 0x%02x", type, mgos_imu_get_accelerometer_name(imu), i2caddr));
      mgos_imu_destroy_accelerometer(imu);
      return false;
    } else {
      LOG(LL_DEBUG, ("Successfully detected accelerometer type %d (%s) at I2C 0x%02x", type, mgos_imu_get_accelerometer_name(imu), i2caddr));
    }
  }

  if (imu->acc->create) {
    if (!imu->acc->create(imu->acc)) {
      LOG(LL_ERROR, ("Could not create accelerometer type %d (%s) at I2C 0x%02x", type, mgos_imu_get_accelerometer_name(imu), i2caddr));
      mgos_imu_destroy_accelerometer(imu);
      return false;
    } else {
      LOG(LL_DEBUG, ("Successfully created accelerometer type %d (%s) at I2C 0x%02x", type, mgos_imu_get_accelerometer_name(imu), i2caddr));
    }
  }

  return true;
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

  if (imu->mag->detect) {
    if (!imu->mag->detect(imu->mag)) {
      LOG(LL_ERROR, ("Could not detect magnetometer type %d (%s) at I2C 0x%02x", type, mgos_imu_get_magnetometer_name(imu), i2caddr));
      mgos_imu_destroy_magnetometer(imu);
      return false;
    } else {
      LOG(LL_DEBUG, ("Successfully detected magnetometer type %d (%s) at I2C 0x%02x", type, mgos_imu_get_magnetometer_name(imu), i2caddr));
    }
  }

  if (imu->mag->create) {
    if (!imu->mag->create(imu->mag)) {
      LOG(LL_ERROR, ("Could not create magnetometer type %d (%s) at I2C 0x%02x", type, mgos_imu_get_magnetometer_name(imu), i2caddr));
      mgos_imu_destroy_magnetometer(imu);
      return false;
    } else {
      LOG(LL_DEBUG, ("Successfully created magnetometer type %d (%s) at I2C 0x%02x", type, mgos_imu_get_magnetometer_name(imu), i2caddr));
    }
  }

  return true;
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
  return (imu->acc != NULL);
}

bool mgos_imu_has_gyroscope(struct mgos_imu *imu) {
  if (!imu) {
    return false;
  }
  return (imu->gyro != NULL);
}

bool mgos_imu_has_magnetometer(struct mgos_imu *imu) {
  if (!imu) {
    return false;
  }
  return (imu->mag != NULL);
}

const char *mgos_imu_get_gyroscope_name(struct mgos_imu *imu) {
  if (!imu || !imu->gyro) return "VOID";

  switch (imu->gyro->type) {
  case GYRO_NONE: return "NONE";
  case GYRO_MPU9250: return "MPU9250";
  default: return "UNKNOWN";
  }
}

const char *mgos_imu_get_magnetometer_name(struct mgos_imu *imu) {
  if (!imu || !imu->mag) return "VOID";

  switch (imu->mag->type) {
  case MAG_NONE: return "NONE";
  case MAG_AK8963: return "AK8963";
  default: return "UNKNOWN";
  }
}

const char *mgos_imu_get_accelerometer_name(struct mgos_imu *imu) {
  if (!imu || !imu->acc) return "VOID";

  switch (imu->acc->type) {
  case ACC_NONE: return "NONE";
  case ACC_MPU9250: return "MPU9250";
  default: return "UNKNOWN";
  }
}

bool mgos_imu_read(struct mgos_imu *imu) {
  if (!imu) return false;

  if (imu->gyro && imu->gyro->read) {
    if (!imu->gyro->read(imu->gyro))
      LOG(LL_ERROR, ("Could not read from gyroscope"));
  }
  if (imu->acc && imu->acc->read) {
    if (!imu->acc->read(imu->acc))
      LOG(LL_ERROR, ("Could not read from accelerometer"));
  }
  if (imu->mag && imu->mag->read) {
    if (!imu->mag->read(imu->mag))
      LOG(LL_ERROR, ("Could not read from magnetometer"));
  }
  return true;
}

bool mgos_imu_init(void) {
  return true;
}

// Public functions end
