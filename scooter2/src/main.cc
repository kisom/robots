#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_MotorShield.h>
#include <Scheduler.h>
#include <Streaming.h>

#include <motors.h>
#include <HCSR04.h>


constexpr float			 ClearanceDistance = 15.0;
constexpr uint8_t		 DistanceLeft  = 0;
constexpr uint8_t		 DistanceFront = 1;
constexpr uint8_t		 DistanceRight = 2;
float				 distance[3];

sensors::HCSR04	 urs[3] = {
	sensors::HCSR04(10, 11), // Left
	sensors::HCSR04(8, 9),   // Forward
	sensors::HCSR04(6, 7),   // Backward
};


static void
senseRange()
{
	for (uint8_t sensor = 0; sensor < 3; sensor++) {
		if (urs[sensor].ready()) {
			distance[sensor] = urs[sensor].range();
		}
		yield();
	}
}


static void
printDistances()
{
	uint8_t	i = 0;
	while (true) {
		Serial << "DISTANCES" << endl;
		Serial << "Left: "  << distance[0] << " cm" << endl;
		Serial << "Front: " << distance[1] << " cm" << endl;
		Serial << "Right: " << distance[2] << " cm" << endl;
		
		for (i = 0; i < 250; i++) {
			delay(1);
			yield();
		}
	}
}


#ifdef DRIVE_TEST
static void
driveTest()
{
	Serial.print("set speed to ");
	Serial.println(Drive::DefaultSpeed, DEC);
	Drive::Throttle(Drive::DefaultSpeed);

	Serial.println("drive forward");
	Drive::Forward();
	delay(1000);

	Serial.println("turn left");
	Drive::TurnLeft();
	delay(1000);

	Serial.println("slow down");
	Drive::Throttle(Drive::DefaultSpeed >> 1);
	delay(500);

	Serial.println("drive backward");
	Drive::Backward();
	delay(1000);

	Serial.println("turn right");
	Drive::TurnRight();
	delay(1000);

	Serial.println("graceful stop");
	Drive::Stop();
	delay(1000);

	Serial.println("throttle up");
	Drive::Throttle(Drive::DefaultSpeed);
	Drive::Forward();
	delay(1000);

	Serial.println("hard stop");
	Drive::Stop(true);
}
#endif // DRIVE_TEST


static void
avoidObstacles()
{
	// If an object is detected, start avoiding by choosing whether
	// to go left or right. Keep turning until the front sensor
	// reports that it is clear of obstacles, then resume driving.
	if (distance[1] < ClearanceDistance) {
		if (distance[0] < distance[2]) {
			Drive::TurnRight();
		}
		else {
			Drive::TurnLeft();
		}

		// The turning delay needs to make sure to relinquish
		// control so that the sensors continue to update.
		while (distance[1] < ClearanceDistance) yield();

		Drive::Forward();
	}
}


void
setup()
{
	Serial.begin(9600);
	delay(1000);

	Serial.println("setting up motors");
	Drive::Begin();

	delay(1000);

#ifdef DRIVE_TEST
	driveTest();
#else //  DRIVE_TEST
	Scheduler.startLoop(senseRange);
	if (Serial) {
		Scheduler.startLoop(printDistances);
	}
	Scheduler.startLoop(avoidObstacles);
#endif // DRIVE_TEST
}


void
loop()
{
	// If an object is detected, start avoiding by choosing whether
	// to go left or right. Keep turning until the front sensor
	// reports that it is clear of obstacles, then resume driving.
	if (distance[DistanceFront] < 10.0) {
		if (distance[DistanceLeft] < distance[DistanceRight]) {
			Drive::TurnRight();
		}
		else {
			Drive::TurnLeft();
		}

		// The turning delay needs to make sure to relinquish
		// control so that the sensors continue to update.
		while (distance[DistanceFront] < 10.0) yield();

		Drive::Forward();
	}
}
