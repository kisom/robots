#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

#include <Adafruit_MotorShield.h>
#include <Scheduler.h>
#include <Streaming.h>

#include <motors.h>
#include <sensors.h>
#include <logging.h>


constexpr float			 ClearanceDistance = 30.0;
constexpr uint8_t		 sdCS = 8;


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
#if 0
	if (!LoggingEnabled) {
		return;
	}

	auto now = millis();

	Logger << now << "," << "STATE,";
	switch (state) {
	case STATE_NORMAL:
		Logger << "DRIVE";
		break;
	case STATE_AVOID:
		Logger << "AVOID";
		break;
	case STATE_BACK:
		Logger << "BACK";
		break;
	case STATE_STOP:
		Logger << "STOP";
		break;
	case STATE_TURNLEFT:
		Logger << "TURN_LEFT";
		break;
	case STATE_TURNRIGHT:
		Logger << "TURN_RIGHT";
		break;
	default:
		Logger << "UNKNOWN_" << int(state);
	}

	Logger << endl;
	Logger.flush();
#endif
}


void
setup()
{
	Serial.begin(9600);
	delay(1000);

	Drive::Begin();
	SetupSensors();
	SenseRange(); // populate distances initially
	SetupLogging(sdCS);

	if (LoggingEnabled) {
		SEROUT << "logging to SD is enabled" << endl;
	}

	delay(1000);

#ifdef DRIVE_TEST
	driveTest();
#else //  DRIVE_TEST
	SenseRange(); // populate distances initially
	Scheduler.startLoop(SenseRange);
	delay(1000); // gives the sensors time to start getting readings.

	Serial << "Starting motors..." << endl;
	Drive::SetSpeed(Drive::DefaultSpeed);
	Drive::Forward();
	logStateChange(STATE_NORMAL);
#endif // DRIVE_TEST

}


void
loop()
{
	Serial << "checking distances" << endl;
	yield();

	// If an object is detected, start avoiding by choosing whether
	// to go left or right. Keep turning until the front sensor
	// reports that it is clear of obstacles, then resume driving.
	if (GuessFrontDistance() < ClearanceDistance) {
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
		if (GuessLeftDistance() < GuessRightDistance()) {
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
		while (GuessFrontDistance() < 2 * ClearanceDistance) {
			yield();
		}

		Drive::SetSpeed(Drive::DefaultSpeed);
		Drive::Forward();
		logStateChange(STATE_NORMAL);
	}
}
