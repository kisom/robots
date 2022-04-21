#include <Arduino.h>
#include <DifferentialEncoders.h>
#include <Zumo32U4Encoders.h>


namespace DifferentialEncoders {
static	Zumo32U4Encoders enc;
static	double	sLeft = 0.0;
static	double	sRight = 0.0;
static	float	cprScaled = GEAR_RATIO * ENCODER_CPR;
static	float	circ = WHEEL_CIRC;


void
setup()
{
	yield();
}


void
run()
{
	double	dLeft = 0.0;
	double	dRight = 0.0;

	if (enc.checkErrorLeft() || enc.checkErrorRight()) {
		reset();
	}

	dLeft = enc.getCountsAndResetLeft();	
	dRight = enc.getCountsAndResetRight();
	sLeft += ((double)dLeft / cprScaled) * circ;
	sRight += ((double)dRight / cprScaled) * circ;
	delay(1);
}


void
reset()
{
	sLeft = 0.0;
	sRight = 0.0;
}


double
getLeft()
{
	return sLeft;
}


double
getRight()
{
	return sRight;
}


float
difference()
{
	return sLeft - sRight;
}


double
average()
{
	return (sLeft + sRight) / 2.0;
}


} // namespace DifferentialEncoders

