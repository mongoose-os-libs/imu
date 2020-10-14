#include "mgos_imu_mpu6886.h"
#include "mgos.h"
#include "mgos_i2c.h"
#include "mgos_imu_mpu60x0.h"

static bool mgos_imu_mpu6886_detect(struct mgos_i2c *i2c, uint8_t i2caddr) {
  if (!i2c) {
    return false;
  }

  if (MGOS_MPU6886_DEVID !=
      mgos_i2c_read_reg_b(i2c, i2caddr, MGOS_MPU60X0_REG_WHO_AM_I)) {
    return false;
  }
  LOG(LL_WARN, ("This driver is experimental and work in progress. Proceed "
                "with caution!"));
  return true;
}

bool mgos_imu_mpu6886_acc_detect(struct mgos_imu_acc *dev,
                                 void *imu_user_data) {
  return mgos_imu_mpu6886_detect(dev->i2c, dev->i2caddr);

  (void)imu_user_data;
}

bool mgos_imu_mpu6886_gyro_detect(struct mgos_imu_gyro *dev,
                                  void *imu_user_data) {
  return mgos_imu_mpu6886_detect(dev->i2c, dev->i2caddr);

  (void)imu_user_data;
}
