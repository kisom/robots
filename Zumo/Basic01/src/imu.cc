#include <Arduino.h>
#include <Zumo32U4IMU.h>
#include <imu.h>


namespace IMU {
static Zumo32U4IMU	imu;

void
setup()
{
	imu.enableDefault();
}


void
run()
{
	imu.read();
	delay(10);
}


} // namespace IMU
