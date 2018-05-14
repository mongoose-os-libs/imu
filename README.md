# Mongoose OS IMU Library

This library provides a simple API that describes inertial measurement units.
It implements various popular I2C and SPI IMUs behind that API. Callers of
the library can rely on the API returning consistently typed data regardless
of the choice of sensor.

# Anatomy

This library provides an API to expose various implementations of gyroscopes,
accelerometers, and magnetometers. It offers a suitable abstraction that 
presents the sensor data in a consistent way, so application authors need
not worry about the implementation details of the sensors.

Users create a `struct mgos_imu` object, to which they add sensors (gyroscope,
acellerometer and magnetometer) either via I2C or SPI. Some chips have all
three sensor types, some have only two (typically accelerometer and gyroscope)
and some have only one type. After successfully adding the sensors to the
`mgos_imu` object, reads can be performed. 

All implementation offer the calls described below in the `IMU API`. 
Some implementations offer chip-specific addendums, mostly setting sensitivity
and range options.

## IMU API

### High level

`struct mgos_imu *mgos_imu_create()` -- This call creates a new (opaque) object
which represents the IMU device. Upon success, a pointer to the object will be
returned. If the creation fails, NULL is returned.

`void mgos_imu_destroy()` -- This cleans up all resources associated with with
the IMU device. The caller passes a pointer to the object pointer. If any
sensors are attached (gyroscope, accelerometer, magnetometer), they will be
destroyed in turn using `mgos_imu_*_destroy()` calls. 

`bool mgos_imu_read()` -- This call schedules all sensors attached to the IMU
to be read from. It is not generally necessary to call this method directly,
as the `mgos_imu_*_get()` calls internally schedule reads from the sensors as
well.

`bool mgos_imu_has_accelerometer()` -- This returns `true` if the IMU has an
attached accelerometer sensor, or false otherwise.

`bool mgos_imu_has_gyroscope()` -- This returns `true` if the IMU has an
attached gyroscope sensor, or false otherwise.

`bool mgos_imu_has_magnetometer()` -- This returns `true` if the IMU has an
attached magnetometer, or false otherwise.

### Low level primitives

For each of ***accelerometer***, ***gyroscope*** and ***magnetometer***, the
following primitives exist:

`bool mgos_imu_*_create_i2c()` -- This attaches a sensor to the IMU
based on the `type` enum given, using the `i2c` bus and specified `i2caddr`
address. The function will return `true` upon success and `false` in case
either detection of the sensor, or creation of it failed.

`bool mgos_imu_*_create_spi()` -- This attaches a sensor to the IMU
based on the `type` enum given, using the `spi` bus and a specified cable select
in `cs_gpio`. The function will return `true` upon success and `false` in case
either detection of the sensor or creation of it failed.

`bool mgos_imu_*_destroy()` -- This detaches a sensor from the IMU if it exists.
It takes care of cleaning up all resources associated with the sensor, and
detaches it from the `i2c` or `spi` bus. The higher level `mgos_imu_destroy()`
call uses these lower level calls to clean up sensors as well.

`bool mgos_imu_accelerometer_get()` -- This call returns sensor data after
polling the sensor for the data. It returns `true` if the read succeeded, in
 which case the floats pointed to by `x`, `y` and `z` are filled in. If an
error occurred, `false` is returned and the contents of `x`, `y` and `z` are
unmodified. Note the units of the return values:

*   ***magnetometer*** returns units of `microTesla`.
*   ***accelerometer*** returns units of `m/s/s` (or standard G).
*   ***gyroscope*** returns units of `degrees per second`.

`const char *mgos_imu_*_get_name()` -- This returns a symbolic name of the
attached sensor, which is guaranteed to be less than or equal to 10 characters
and always exist. If there is no sensor of this type attached, `VOID` will be
returned. If the sensor is not known, `UNKNOWN` will be returned. Otherwise,
the chip manufacturer / type will be returned, for example `MPU9250` or
`ADXL345` or `MAG3110`.


## Supported devices

### Accelerometer

*   MPU9250 and MPU9255
*   ADXL345

### Gyroscope

*   MPU9250 and MPU9255

### Magnetometer

*   MAG3110
*   AK8963 (as found in MPU9250/MPU9255)
*   AK8975

## Adding devices

(TBD)

# Example Code

An example program that creates an IMU of type `MPU9250` (which has an
accelerometer, a gyroscope and a magnetometer all in one tiny package):

```
#include "mgos.h"
#include "mgos_i2c.h"
#include "mgos_imu.h"

static void imu_cb(void *user_data) {
  struct mgos_imu *imu = (struct mgos_imu *)user_data;
  float ax, ay, az;
  float gx, gy, gz;
  float mx, my, mz;

  if (!imu) return;

  if (mgos_imu_accelerometer_get(imu, &ax, &ay, &az))
    LOG(LL_INFO, ("type=%-10s Accel X=%.2f Y=%.2f Z=%.2f",
                  mgos_imu_accelerometer_get_name(imu), ax, ay, az));
  if (mgos_imu_gyroscope_get(imu, &gx, &gy, &gz))
    LOG(LL_INFO, ("type=%-10s Gyro  X=%.2f Y=%.2f Z=%.2f",
                  mgos_imu_gyroscope_get_name(imu), gx, gy, gz));
  if (mgos_imu_magnetometer_get(imu, &mx, &my, &mz))
    LOG(LL_INFO, ("type=%-10s Mag   X=%.2f Y=%.2f Z=%.2f",
                  mgos_imu_magnetometer_get_name(imu), mx, my, mz));
}

enum mgos_app_init_result mgos_app_init(void) {
  struct mgos_imu *imu;
  struct mgos_i2c *i2c;

  i2c=mgos_i2c_get_global();

  if (!i2c) {
    LOG(LL_ERROR, ("I2C bus missing, set i2c.enable=true in mos.yml"));
    return false;
  }

  imu=mgos_imu_create(); 
  if (!imu) {
    LOG(LL_ERROR, ("Cannot create IMU"));
    return false;
  }

  if (!mgos_imu_accelerometer_create_i2c(imu, i2c, 0x68, ACC_MPU9250))
    LOG(LL_ERROR, ("Cannot create accelerometer on IMU"));
  if (!mgos_imu_gyroscope_create_i2c(imu, i2c, 0x68, GYRO_MPU9250))
    LOG(LL_ERROR, ("Cannot create gyroscope on IMU"));
  if (!mgos_imu_magnetometer_create_i2c(imu, i2c, 0x0C, MAG_AK8963))
    LOG(LL_ERROR, ("Cannot create magnetometer on IMU"));

  mgos_set_timer(1000, true, imu_cb, imu);
  return true;
}
```

# Disclaimer

This project is not an official Google project. It is not supported by Google
and Google specifically disclaims all warranties as to its quality,
merchantability, or fitness for a particular purpose.
