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

struct mgos_imu_mag *mgos_imu_mag_create(void) {
  struct mgos_imu_mag *mag;
  mag = calloc(1, sizeof(struct mgos_imu_mag));
  if (!mag) return NULL;
  memset(mag, 0, sizeof(struct mgos_imu_mag));

  mag->type=MAG_NONE;
  return mag;
}

bool mgos_imu_mag_destroy(struct mgos_imu_mag **mag) {
  if (!*mag) return false;
  free(*mag);
  *mag=NULL;
  return true;
}
