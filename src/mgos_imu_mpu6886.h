#pragma once

#include "mgos.h"
#include "mgos_imu_internal.h"

#define MGOS_MPU6886_DEFAULT_I2CADDR         (0x68)
#define MGOS_MPU6886_DEVID                   (0x19)

#define MGOS_MPU6886_GYRO_OFFSET             (0x13)
#define MGOS_MPU6886_ACCEL_CONFIG2           (0x1D)
#define MGOS_MPU6886_ACCEL_INTEL_CTRL        (0x69)


bool mgos_imu_mpu6886_acc_detect(struct mgos_imu_acc *dev, void *imu_user_data);

bool mgos_imu_mpu6886_gyro_detect(struct mgos_imu_gyro *dev,
                                  void *imu_user_data);
