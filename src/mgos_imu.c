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

// Private functions follow
// Private functions end

// Public functions follow
struct mgos_imu *mgos_imu_create_i2c(struct mgos_i2c *i2c) {
  struct mgos_imu *sensor;

  if (!i2c) {
    return NULL;
  }

  sensor = calloc(1, sizeof(struct mgos_imu));
  if (!sensor) {
    return NULL;
  }
  memset(sensor, 0, sizeof(struct mgos_imu));
  return sensor;
}

void mgos_imu_destroy(struct mgos_imu **sensor) {
  if (!*sensor) {
    return;
  }
  if ((*sensor)->gyro) mgos_imu_gyro_destroy(&((*sensor)->gyro));
  if ((*sensor)->acc) mgos_imu_acc_destroy(&((*sensor)->acc));
  if ((*sensor)->mag) mgos_imu_mag_destroy(&((*sensor)->mag));

  free(*sensor);
  *sensor = NULL;
  return;
}

bool mgos_imu_has_accelerometer(struct mgos_imu *sensor) {
  if (!sensor) {
    return false;
  }
  return sensor->capabilities & MGOS_IMU_CAP_ACCELEROMETER;
}

bool mgos_imu_has_gyroscope(struct mgos_imu *sensor) {
  if (!sensor) {
    return false;
  }
  return sensor->capabilities & MGOS_IMU_CAP_GYROSCOPE;
}

bool mgos_imu_has_magnetometer(struct mgos_imu *sensor) {
  if (!sensor) {
    return false;
  }
  return sensor->capabilities & MGOS_IMU_CAP_MAGNETOMETER;
}

bool mgos_imu_has_thermometer(struct mgos_imu *sensor) {
  if (!sensor) {
    return false;
  }
  return sensor->capabilities & MGOS_IMU_CAP_THERMOMETER;
}

bool mgos_imu_init(void) {
  return true;
}

// Public functions end
