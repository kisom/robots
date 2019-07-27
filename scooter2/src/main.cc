#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

#include <Adafruit_MotorShield.h>
#include <Scheduler.h>
#include <Servo.h>
#include <Streaming.h>

#include <motors.h>
#include <HCSR04.h>


constexpr float			 ClearanceDistance = 30.0;
constexpr uint8_t		 maxSteps = 9;
uint8_t				 steps[maxSteps+1] = {
	30, 45, 60, 75, 90, 105, 120, 135, 150,
};
float				 distance[maxSteps];
#define distanceIndex(deg)	 (distance[(deg) / 15])

Servo				 ursPan;
sensors::HCSR04			 urs(6, 7);

constexpr uint8_t		 sdCS = 8;
static bool			 loggingEnabled = false;

#define logFile			 SEROUT


static void
logDistance()
{
	if (!loggingEnabled) {
		return;
	}

	auto now = millis();
	logFile << now << ",DIST";
	for (uint8_t i = 0; i < maxSteps; i++) {
		logFile << "," << distance[i];
	}
	logFile << endl;
}


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

	logDistance();
}


static void
logGuess(char direction, float distance)
{
	if (!loggingEnabled) {
		return;
	}
	auto now = millis();
	logFile << now << ",GUESS," << direction << "," << distance << endl;
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
		logGuess('F', average);
		return average;
	}
	logGuess('F', minDist);
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
		logGuess('L', average);
		return average;
	}
	logGuess('L', minDist);
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
		logGuess('R', average);
		return average;
	}
	logGuess('R', minDist);
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


const uint8_t	STATE_NORMAL = 0;
const uint8_t	STATE_AVOID = 1;
const uint8_t	STATE_BACK = 2;
const uint8_t	STATE_STOP = 3;
const uint8_t	STATE_TURNLEFT = 4;
const uint8_t	STATE_TURNRIGHT = 5;


static void
logStateChange(uint8_t state)
{
	if (!loggingEnabled) {
		return;
	}

	auto now = millis();

	logFile << now << "," << "STATE,";
	switch (state) {
	case STATE_NORMAL:
		logFile << "DRIVE";
		break;
	case STATE_AVOID:
		logFile << "AVOID";
		break;
	case STATE_BACK:
		logFile << "BACK";
		break;
	case STATE_STOP:
		logFile << "STOP";
		break;
	case STATE_TURNLEFT:
		logFile << "TURN_LEFT";
		break;
	case STATE_TURNRIGHT:
		logFile << "TURN_RIGHT";
		break;
	default:
		logFile << "UNKNOWN_" << int(state);
	}

	logFile << endl;
}


void
setup()
{
	Serial.begin(9600);
	delay(1000);

	Drive::Begin();

	loggingEnabled = true;

	delay(1000);

#ifdef DRIVE_TEST
	driveTest();
#else //  DRIVE_TEST
	ursPan.attach(9);
	Scheduler.startLoop(senseRange);
	Drive::SetSpeed(Drive::DefaultSpeed);
	Drive::Forward();
	logStateChange(STATE_NORMAL);
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
		logStateChange(STATE_AVOID);

		// Hard stop, backup, and make sure we turn quickly.
		Drive::Stop();
		logStateChange(STATE_STOP);
		delay(500);

		Drive::SetSpeed(2 * Drive::DefaultSpeed);
		logStateChange(STATE_BACK);
		Drive::Backward();
		delay(250);

		Drive::Stop();
		logStateChange(STATE_STOP);
		Drive::SetSpeed(Drive::DefaultSpeed);

		// Pick the direction with the most room.
		if (Serial) Serial << "AVOID" << endl;
		if (guessLeftDistance() < guessRightDistance()) {
			Serial << "turn right" << endl;
			Drive::TurnRight();
			logStateChange(STATE_TURNRIGHT);
		}
		else {
			Serial << "turn left" << endl;
			Drive::TurnLeft();
			logStateChange(STATE_TURNLEFT);
		}

		// The turning delay needs to make sure to relinquish
		// control so that the sensors continue to update.
		while (guessFrontDistance() < 2 * ClearanceDistance) {
			yield();
		}

		Drive::SetSpeed(Drive::DefaultSpeed);
		Drive::Forward();
		logStateChange(STATE_NORMAL);
	}
	yield();
}
