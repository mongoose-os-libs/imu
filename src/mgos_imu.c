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
#include <math.h>

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

void mgos_imu_destroy(struct mgos_imu **imu) {
  if (!*imu) {
    return;
  }
  mgos_imu_gyroscope_destroy(*imu);
  mgos_imu_accelerometer_destroy(*imu);
  mgos_imu_magnetometer_destroy(*imu);

  if ((*imu)->user_data) {
    free((*imu)->user_data);
  }

  free(*imu);
  *imu = NULL;
  return;
}

bool mgos_imu_accelerometer_present(struct mgos_imu *imu) {
  if (!imu) {
    return false;
  }
  return imu->acc != NULL;
}

bool mgos_imu_gyroscope_present(struct mgos_imu *imu) {
  if (!imu) {
    return false;
  }
  return imu->gyro != NULL;
}

bool mgos_imu_magnetometer_present(struct mgos_imu *imu) {
  if (!imu) {
    return false;
  }
  return imu->mag != NULL;
}

bool mgos_imu_init(void) {
  return true;
}

// Public functions end
