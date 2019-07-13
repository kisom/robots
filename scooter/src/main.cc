#include <Arduino.h>
#include <Scheduler.h>
#include <Streaming.h>
#include <HCSR04.h>
#include <SCMD.h>
#include <SCMD_config.h>


SCMD				scmd;

UltraSonicDistanceSensor	usdFront(8, 2);
UltraSonicDistanceSensor	usdLeft(9, 3);
UltraSonicDistanceSensor	usdRight(10, 4);
double				distances[3];

uint8_t				targetSpeeds[2];
uint8_t				currentSpeeds[2];


static double	collisionThreshold = 10.0;
static double	collisonClearance = 45.0;


void
forward()
{
	scmd.setDrive(0, 0, 127);
	scmd.setDrive(1, 1, 112);
}


void
backward()
{
	scmd.setDrive(0, 1, 127);
	scmd.setDrive(1, 0, 112);
}


void
turnLeft()
{
	scmd.setDrive(0, 1, 127);
	scmd.setDrive(1, 1, 112);
}


void
turnRight()
{
	scmd.setDrive(0, 0, 127);
	scmd.setDrive(1, 0, 112);
}


void
stop()
{
	scmd.setDrive(0, 0, 0);
	scmd.setDrive(1, 0, 0);
}


static void
clampDistance(double &distance)
{
	if (distance <= 2.0) {
		distance = 0;
	}
}

void
senseLoop()
{
	distances[0] = usdFront.measureDistanceCm();
	delay(65);
	yield();

	distances[1] = usdLeft.measureDistanceCm();
	delay(65);
	yield();

	distances[2] = usdRight.measureDistanceCm();
	delay(65);
	yield();

	SerialUSB << "C: " << distances[0] << endl;
	SerialUSB << "L: " << distances[1] << endl;
	SerialUSB << "R: " << distances[2] << endl;
}


void
setup()
{
	uint8_t	scmdCheck;

	SerialUSB.begin(9600);

	SerialUSB << "setting up SCMD" << endl;
	scmd.settings.commInterface = I2C_MODE;
	scmd.settings.I2CAddress = 0x58;

	SerialUSB << "begin" << endl;
	scmdCheck = scmd.begin();
	while ((scmdCheck = scmd.begin()) != 0xA9) {
		SerialUSB << scmdCheck << " is invalid" << endl;
		delay(500);
	}

	SerialUSB << "wait for ready" << endl;
	while (!scmd.ready()) ;
	scmd.enable();

	Scheduler.startLoop(senseLoop);

	SerialUSB << "BOOT OK" << endl;
	forward();
}


void
loop()
{
	static bool	avoiding = false;
	static uint8_t	step = 0;

	if (step < 10) {
		step++;
		delay(10);
		return;
	}

	step = 0;

	if (avoiding) {
		if (distances[0] > collisonClearance) {
			SerialUSB << "collision avoidance cleared" << endl;
			avoiding = false;
			forward();
		}
		return;
	}

	yield();
	if (distances[0] < (collisionThreshold / 2)) {
		stop();
		delay(100);
		return;
	}

	if (distances[0] <= collisionThreshold) {
		SerialUSB << "collision avoidance active" << endl;
		avoiding = true;
		if (distances[1] > distances[2]) {
			SerialUSB << distances[1] << " (L) has more room than " << distances[2] << " (R), turning left" << endl;
			turnLeft();	
		}
		else {
			SerialUSB << distances[2] << " (R) has more room than " << distances[1] << " (L), turning left" << endl;
			turnRight();
		}
		return;
	}

	delay(10);
}
