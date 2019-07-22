#include <Arduino.h>
#include <Scheduler.h>
#include <Streaming.h>

#include <Adafruit_NeoPixel.h>
#include <HCSR04.h>
#include <SCMD.h>
#include <SCMD_config.h>


#define DISABLE_MOTOR	1


// Motor control.
SCMD				scmd;
constexpr uint8_t		motorLeft = 0;
constexpr uint8_t		motorRight = 1;


// Ultrasonic sensors.
UltraSonicDistanceSensor	usdFront(8, 5);
UltraSonicDistanceSensor	usdLeft(9, 6);
UltraSonicDistanceSensor	usdRight(10, 7);
double				distances[3];

static double	collisionThreshold = 10.0;
static double	collisonClearance = 45.0;


// Miscellaneous.
Adafruit_NeoPixel strip(0, 44, NEO_GRB + NEO_KHZ800);
struct {
	bool	blinking;
	bool	on;
	uint8_t	color[3];
} NeoPixelStatus;


void
yieldDelay(int ms)
{
	int	waited = 0;

	while (waited < ms) {
		delay(10);
		yield();
		waited += 10;
	}
}


void
controlNeoPixel()
{
	strip.setPixelColor(0, NeoPixelStatus.color[0],
			       NeoPixelStatus.color[1],
			       NeoPixelStatus.color[2]);
	strip.show();
	yieldDelay(250);
}


void
neoPixelColor(uint8_t r, uint8_t g, uint8_t b)
{
	NeoPixelStatus.color[0] = r;
	NeoPixelStatus.color[1] = g;
	NeoPixelStatus.color[2] = b;
}

#define NEOPIXEL_RED	neoPixelColor(255, 0, 0)
#define NEOPIXEL_GREEN	neoPixelColor(0, 255, 0)
#define NEOPIXEL_BLUE	neoPixelColor(0, 0, 255)
#define NEOPIXEL_YELLOW	neoPixelColor(255, 255, 0)
#define NEOPIXEL_ORANGE	neoPixelColor(255, 0xa5, 0)


#ifdef DISABLE_MOTOR
void
motorForward(uint8_t id, uint8_t speed)
{

}


void
motorBackward(uint8_t id, uint8_t speed)
{

}


#else
void
motorForward(uint8_t id, uint8_t speed)
{
	scmd.setDrive(id, 0, speed);
}


void
motorBackward(uint8_t id, uint8_t speed)
{
	scmd.setDrive(id, 1, speed);
}
#endif


// Motor speeds compensate for weight imbalance.
void
forward()
{
	motorForward(motorLeft, 127);
	motorForward(motorRight, 112);
}


void
backward()
{
	motorBackward(motorLeft, 127);
	motorBackward(motorRight, 112);
}


void
turnLeft()
{
	motorBackward(motorLeft, 127);
	motorForward(motorRight, 112);
}


void
turnRight()
{
	motorForward(motorLeft, 127);
	motorBackward(motorRight, 112);
}


void
stop()
{
	scmd.setDrive(0, 0, 0);
	scmd.setDrive(1, 0, 0);
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

	if (SerialUSB) {
		SerialUSB << "C: " << distances[0] << endl;
		SerialUSB << "L: " << distances[1] << endl;
		SerialUSB << "R: " << distances[2] << endl;
	}
}


void
setup()
{
	uint8_t	scmdCheck;

	SerialUSB.begin(9600);
	while (!SerialUSB) ;

	SerialUSB << "setting up RGB LED" << endl;
	strip.begin();
	strip.setBrightness(255);
	NEOPIXEL_BLUE;
	Scheduler.startLoop(controlNeoPixel);

	#ifndef DISABLE_MOTOR
	if (SerialUSB) {
		SerialUSB << "setting up motor controller" << endl;
	}
	scmd.settings.commInterface = I2C_MODE;
	scmd.settings.I2CAddress = 0x58;

	scmdCheck = scmd.begin();
	while ((scmdCheck = scmd.begin()) != 0xA9) {
		delay(500);
	}

	while (!scmd.ready()) ;
	while (scmd.busy());
	scmd.inversionMode(1, 1);
	while (scmd.busy());
	scmd.enable();
	#endif

	Scheduler.startLoop(senseLoop);

	SerialUSB << "BOOT OK" << endl;
	forward();
	NEOPIXEL_GREEN;
}


void
turnBestDirection()
{
	if (distances[1] > distances[2]) {
		SerialUSB << distances[1] << " (L) has more room than "
			  << distances[2] << " (R), turning left" << endl;
		turnLeft();	
	}
	else {
		SerialUSB << distances[2] << " (R) has more room than " << distances[1]
			  << " (L), turning left" << endl;
		turnRight();
	}
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
			NEOPIXEL_GREEN;
			forward();
		}
		return;
	}

	yield();
	if ((distances[0] >= 0) && (distances[0] < (collisionThreshold / 2))) {
		NEOPIXEL_RED;
		
		// Stop and back up, giving time for the motors to
		// do things.
		stop();
		yieldDelay(500);

		backward();
		yieldDelay(500);

		stop();
		yieldDelay(500);

		turnBestDirection();
		yieldDelay(2500);		
	}

	if ((distances[0] >= 0) && (distances[0] <= collisionThreshold)) {
		SerialUSB << "collision avoidance active" << endl;
		avoiding = true;
		NEOPIXEL_YELLOW;
		turnBestDirection();
	}

	delay(10);
}
