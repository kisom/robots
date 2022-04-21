#include <Arduino.h>
#include <config.h>
#include <motors.h>


namespace motors {

static short	left = 0;
static short	right = 0;
static short	tleft = 0;
static short	tright = 0;
static bool		enabled = false;
static Zumo32U4Motors	motors;

// correction factors
static short	cLeft = 0;
static short	cRight = 0;


static short
constrainMotor(short value)
{
	if (value > 0) {
		if (value > SPEED_MAX) {
			value = SPEED_MAX;
		} else if (value < SPEED_STALL) {
			value = SPEED_STALL;
		}
	} else {
		if (value < SPEED_MIN) {
			value = SPEED_MIN;
		} else if (value < -SPEED_STALL) {
			value = -SPEED_STALL;
		}
	}

	return value;
}


static short
rampValues(short value, short target)
{
	short	delta = 0;
	bool	invert = false;

	delta = target - value;
	if (delta == 0) {
		return value;
	}

	if (delta < 0) {
		delta = -delta;
		invert = true;
	}

	if (delta > MOTOR_BIG_DELTA) {
		delta = MOTOR_BIG_DELTA;
	} else {
		delta = 1;
	}

	if (invert) {
		return value - delta;
	}

	return value + delta;
}


void
setup()
{
	motors.setSpeeds(0, 0);
	enable();
}


void
kill()
{
	motors.setSpeeds(0, 0);
	enabled = false;
}


void
enable()
{
	motors.setSpeeds(0, 0);
	enabled = true;
}


void
run()
{
	if (!enabled) {
		motors.setSpeeds(0, 0);
		delay(1);
		return;
	}

	left = constrainMotor(rampValues(left, tleft));
	right = constrainMotor(rampValues(right, tright));
	motors.setSpeeds(left, right);
	delay(50);
}


void
stop()
{
	tleft = 0;
	tright = 0;
}


static void
setTargets(short l, short r)
{
	tleft = constrainMotor(l);
	tright = constrainMotor(r);
}


void
forward()
{
	setTargets(SPEED_DRIVE, SPEED_DRIVE);
}


void
backward()
{
	setTargets(-SPEED_DRIVE, -SPEED_DRIVE);
}


void
turnLeft()
{
	setTargets(-SPEED_DRIVE, SPEED_DRIVE);
}


void
turnRight()
{
	setTargets(SPEED_DRIVE, -SPEED_DRIVE);
}


short
getLeft()
{
	return left;
}


short
getRight()
{
	return right;
}


// Tweaks to account for variances in motors.
void
tweakLeftUp()
{
	cLeft++;
}


void
tweakLeftDown()
{
	cLeft--;
}


void
tweakRightUp()
{
	cRight++;
}


void
tweakRightDown()
{
	cRight--;
}


} // namespace motors
