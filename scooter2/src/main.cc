#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_MotorShield.h>
#include <Scheduler.h>
#include <Servo.h>
#include <Streaming.h>

#include <motors.h>
#include <HCSR04.h>


constexpr float			 ClearanceDistance = 30.0;
constexpr uint8_t		 maxSteps = 8;
uint8_t				 steps[maxSteps+1] = {
	30, 45, 60, 75, 90, 105, 120, 135, 150,
};
float				 distance[maxSteps];
#define distanceIndex(deg)	 (distance[(deg) / 15])

Servo				 ursPan;
sensors::HCSR04			 urs(6, 7);


static void
senseRange()
{
	for (uint8_t i = 0; i < maxSteps; i++) {
		// Pan & scan.
		ursPan.write(steps[i]);
		while (ursPan.read() != steps[i]) yield();

		distance[i] = urs.range();
		delay(sensors::HCSR04::updateDelay);
	}

	for (int8_t i = maxSteps; i >= 0; i--) {
		// Pan & scan.
		ursPan.write(steps[i]);
		while (ursPan.read() != steps[i]) yield();

		distance[i] += urs.range();
		distance[i] /= 2;
		delay(sensors::HCSR04::updateDelay);
	}
}


static float
guessFrontDistance(bool useAverage=false)
{
	float	average = 0.0;
	float	minDist = 0.0;

	average = distanceIndex(75) + distanceIndex(90) + distanceIndex(105);
	average /= 3;
	minDist = average;

	for (uint8_t i = 75; i <= 105; i += 15) {
		minDist = minDist < distanceIndex(i) ? minDist : distanceIndex(i);
	}

	if (useAverage) {
		return average;
	}
	return minDist;
}


static float
guessLeftDistance(bool useAverage=false)
{
	float	average = 0.0;
	float	minDist = 0.0;

	average = distanceIndex(30) +
		  distanceIndex(45) +
		  distanceIndex(60);
	average /= 3;
	minDist = average;

	for (uint8_t i = 30; i <= 60; i += 15) {
		minDist = minDist < distanceIndex(i) ? minDist : distanceIndex(i);
	}

	if (useAverage) {
		return average;
	}
	return minDist;
}


static float
guessRightDistance(bool useAverage=false)
{
	float	average = 0.0;
	float	minDist = 0.0;

	average = distanceIndex(120) +
		  distanceIndex(135) +
		  distanceIndex(150);
	average /= 3;
	minDist = average;

	for (uint8_t i = 120; i <= 150; i += 15) {
		minDist = minDist < distanceIndex(i) ? minDist : distanceIndex(i);
	}

	if (useAverage) {
		return average;
	}
	return minDist;
}


static void
printDistances()
{
	uint8_t	i = 0;
	while (true) {
		Serial << "DISTANCES" << endl;
		for (uint8_t i = 0; i < maxSteps; i++) {
			Serial << int(i * 15) << "º: " << distance[i] << endl;
		}
		
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
	ursPan.attach(9);
	Scheduler.startLoop(senseRange);
	if (Serial) {
		Scheduler.startLoop(printDistances);
		Serial << "driving at " << int(Drive::DefaultSpeed) << "..." << endl;
	}
	Drive::SetSpeed(Drive::DefaultSpeed);
	Drive::Forward();
	delay(1000); // gives the sensors time to start getting readings.
#endif // DRIVE_TEST

}


void
loop()
{
	// If an object is detected, start avoiding by choosing whether
	// to go left or right. Keep turning until the front sensor
	// reports that it is clear of obstacles, then resume driving.
	if (guessFrontDistance() < ClearanceDistance) {
		// Hard stop, backup, and make sure we turn quickly.
		Drive::Stop();
		delay(500);

		Drive::SetSpeed(2 * Drive::DefaultSpeed);
		Drive::Backward();
		delay(250);

		Drive::Stop();
		Drive::SetSpeed(2 * Drive::DefaultSpeed);

		// Pick the direction with the most room.
		if (Serial) Serial << "AVOID" << endl;
		if (guessLeftDistance() < guessRightDistance()) {
			Serial << "turn right" << endl;
			Drive::TurnRight();
		}
		else {
			Serial << "turn left" << endl;
			Drive::TurnLeft();
		}

		// The turning delay needs to make sure to relinquish
		// control so that the sensors continue to update.
		while (guessFrontDistance() < 2 * ClearanceDistance) {
			Serial << "turning" << endl;
			yield();
		}

		Drive::SetSpeed(Drive::DefaultSpeed);
		Drive::Forward();
	}
	yield();
}
