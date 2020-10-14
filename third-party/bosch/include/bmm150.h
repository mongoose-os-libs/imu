#include "mgos.h"
#include "mgos_imu_internal.h"

// internal API to obtain compensated magnetometer data in uT
// trim data structure
struct mgos_imu_bmm150_trim_registers {
  int8_t dig_x1;
  int8_t dig_y1;
  int8_t dig_x2;
  int8_t dig_y2;
  uint16_t dig_z1;
  int16_t dig_z2;
  int16_t dig_z3;
  int16_t dig_z4;
  uint8_t dig_xy1;
  int8_t dig_xy2;
  uint16_t dig_xyz1;
};

bool read_trim_registers(struct mgos_imu_mag *dev);
int16_t compensate_x(int16_t mag_data_x, uint16_t data_rhall,
                     const struct mgos_imu_bmm150_trim_registers *trim_data);
int16_t compensate_y(int16_t mag_data_y, uint16_t data_rhall,
                     const struct mgos_imu_bmm150_trim_registers *trim_data);
int16_t compensate_z(int16_t mag_data_z, uint16_t data_rhall,
                     const struct mgos_imu_bmm150_trim_registers *trim_data);
