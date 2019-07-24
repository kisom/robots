#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_MotorShield.h>
#include <Streaming.h>

#include "motors.h"


constexpr uint16_t		 pwmFrequency = 1600;
static Adafruit_MotorShield	 controller;
static Adafruit_DCMotor		*LeftMotor  = controller.getMotor(1);
static Adafruit_DCMotor		*RightMotor = controller.getMotor(2);


namespace Drive {


constexpr uint8_t	maxThrottleStep = 10;
constexpr uint8_t	throttleDelay = 20;
struct Belief		belief;


void
Begin()
{
	controller.begin(pwmFrequency);

	belief.speed = 0;
	belief.direction = Direction::Stop;
}


void
SetSpeed(uint8_t speed)
{
	LeftMotor->setSpeed(speed);
	RightMotor->setSpeed(speed);
	belief.speed = speed;
	belief.direction = Direction::Stop;
}


void
Throttle(uint8_t targetSpeed)
{
	if (targetSpeed == belief.speed) {
		Serial << "target = belief: nothing to do" << endl;
		return; // Nothing to do.
	}

	if (targetSpeed < belief.speed) {
		Serial << "target < belief: throttle down" << endl;
		while (targetSpeed < belief.speed) {
			uint8_t delta = belief.speed - targetSpeed;
			delta = delta > maxThrottleStep ? maxThrottleStep : delta;
			Serial << "speed = " << int(belief.speed)
			       << ",Δ = " << delta << endl;
			SetSpeed(belief.speed - delta);

			// need some amount of time for this to take effect.
			delay(throttleDelay);
			yield();
		}
	}
	else {
		Serial << "target > belief: throttle up" << endl;
		while (belief.speed < targetSpeed) {
			uint8_t delta = targetSpeed - belief.speed;
			delta = delta > maxThrottleStep ? maxThrottleStep : delta;
			Serial << "speed = " << int(belief.speed)
			       << ",Δ = " << delta << endl;
			SetSpeed(belief.speed + delta);

			// need some amount of time for this to take effect.
			delay(throttleDelay);
			yield();
		}
	}
}


void
Drive(Direction direction, uint8_t speed, bool throttle)
{
	if (throttle) {
		Throttle(speed);
	} else {
		SetSpeed(speed);
	}

	switch (direction) {
	case Direction::Forward:
		Forward();
		break;
	case Direction::Backward:
		Backward();
		break;
	case Direction::TurnLeft:
		TurnLeft();
		break;
	case Direction::TurnRight:
		TurnRight();
		break;
	default:
		// speed has already been set, just need to release the motors.
		Stop(true);
	}
}


static void
drive(uint8_t l, uint8_t r)
{
	LeftMotor->run(l);
	RightMotor->run(r);
}


void
Forward()
{
	belief.direction = Direction::Forward;
	drive(FORWARD, FORWARD);
}


void
Backward()
{
	belief.direction = Direction::Backward;
	drive(BACKWARD, BACKWARD);
}


void
TurnLeft()
{
	belief.direction = Direction::TurnLeft;
	drive(BACKWARD, FORWARD);
}


void
TurnRight()
{
	belief.direction = Direction::TurnRight;
	drive(FORWARD, BACKWARD);
}


void
Stop(bool hard)
{
	if (hard) {
		SetSpeed(0);
	}
	else {
		Throttle(0);
	}
	drive(RELEASE, RELEASE);
}


void
WorldView(struct Belief *target)
{
	target->speed = belief.speed;
	target->direction = belief.direction;
}


} // namespace Drive
