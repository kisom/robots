#include <Arduino.h>
#include <Scheduler.h>
#include <Zumo32U4.h>
#include <math.h>
#include <avr/sleep.h>

#include <config.h>
#include <DifferentialEncoders.h>
#include <imu.h>
#include <motors.h>
#include <prox.h>


Zumo32U4OLED	oled;


typedef struct {
	float	x;
	float	y;
	float	z;
} Vector;

typedef struct {
	Vector	a;	// accelerometer
	Vector	g;	// gyro
	Vector	m;	// magnetometer
} IMUReading;

struct {
	IMUReading			imu;
	Zumo32U4ProximitySensors	prox;
	uint8_t				state;
} Zumo;


#define STATE_KILL	0
#define STATE_EXPLORE	1

#define AVOID_DISTANCE	4


static void
zumoKill()
{
	motors::kill();
	delay(100);

	sleep_enable();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_cpu();
	while (true) {
		ledRed(1);
		for (int i = 0; i < 1000; i++) {
			delayMicroseconds(500);
		}

		ledRed(0);
		for (int i = 0; i < 1000; i++) {
			delayMicroseconds(500);
		}
	}
}


static void
backupToAvoid()
{
	do {
		motors::backward();
		delay(1);
	} while (prox::front() <= (AVOID_DISTANCE+1));
}


static void
turnToAvoid()
{
	if (prox::left() > prox::right()) {
		motors::turnLeft();
		delay(1);
		return;
	} else if (prox::left() < prox::right()) {
		motors::turnRight();
		delay(1);
		return;
	} else {
		motors::turnLeft();
	}
}


void
avoidObstacles()
{
	uint8_t	front = prox::front();
	uint8_t left  = prox::left();
	uint8_t right = prox::right();

	if (Zumo.state != STATE_EXPLORE) {
		delay(1);
		return;
	}

	if (front < AVOID_DISTANCE) {
		if ((left >= AVOID_DISTANCE) || (right >= AVOID_DISTANCE)) {
			turnToAvoid();
		} else {
			backupToAvoid();
		}
	} else {
		motors::forward();
	}
	yield();
}


void
runningLightSetup()
{
	yield();
	return;	
}


void
healthCheck()
{
	static unsigned long	next = millis() + 500;

	while (true) {
		if (!usbPowerPresent() && (readBatteryMillivolts() < MIN_VOLT4)) {
			Zumo.state = STATE_KILL;
			motors::stop();
			ledRed(1);
			oled.clear();
			oled.gotoXY(0, 0);
			oled.print("BATTERY");
			oled.gotoXY(0, 1);
			oled.print("  LOW  ");
			oled.display();
			motors::stop();
			delay(5000);

			zumoKill();
		}

		if (millis() >= next) {
			oled.clear();
			switch (Zumo.state) {
			case STATE_KILL:
				oled.println(F("KILL"));
				break;
			case STATE_EXPLORE:
				oled.println(F("XPLORING"));
				break;
			default:
				oled.println(F("  ???  "));
				break;
			}

			oled.gotoXY(0, 1);
			oled.print("M ");
			oled.print(motors::getLeft());
			oled.print(" ");
			oled.print(motors::getRight());

			oled.gotoXY(0, 2);
			oled.print("P ");
			oled.print(uint8_t(prox::left()));
			oled.print(" ");
			oled.print(uint8_t(prox::front()));
			oled.print(" ");
			oled.print(uint8_t(prox::right()));

			oled.gotoXY(0, 3);
			oled.print(F("BAT: "));
			oled.print(readBatteryMillivolts());
			oled.print(F("V"));
			oled.display();
			delay(1);
			next = millis() + 500;
		}
		delay(1);
	}
}


void
runningLight()
{
	ledGreen(1);
	delay(100);
	ledGreen(0);
	delay(100);
	ledGreen(1);
	delay(100);
	ledGreen(0);
	delay(100);
	ledYellow(1);
	delay(100);
	ledYellow(0);
	delay(100);
	ledYellow(1);
	delay(100);
	ledYellow(0);
	delay(300);
}


void
zumoStart()
{
	Zumo.state = STATE_EXPLORE;
	yield();
}


void
setup()
{
	Serial.begin(9600);

	oled.clear();
	oled.gotoXY(0, 0);
	oled.println(F("HELLO"));
	oled.display();
	delay(1000);
	oled.gotoXY(0, 1);
	oled.print(F("  WORLD"));
	oled.display();
	delay(1000);
	oled.clear();

	oled.gotoXY(0, 0);
	oled.print(F("PRESS B"));
	oled.gotoXY(0, 1);
	oled.print(F("TO START"));
	oled.display();
	Zumo32U4ButtonB	b;
	b.waitForPress();
	oled.clear();
	oled.gotoXY(0, 0);
	oled.print("RUNNING");
	delay(1000);
	
	oled.clear();
	oled.setLayout11x4();

	Scheduler.start(runningLightSetup, runningLight);
	Scheduler.start(motors::setup, motors::run);
	Scheduler.start(prox::setup, prox::run);
	Scheduler.start(zumoStart, avoidObstacles);
	Scheduler.start(nullptr, healthCheck);
	Scheduler.start(IMU::setup, IMU::run);
	Scheduler.start(DifferentialEncoders::setup, DifferentialEncoders::run);
}


void
loop()
{
	double	dEncoders = DifferentialEncoders::difference();

	if (fabs(dEncoders) < 11.0) {
		if (dEncoders < 0.0) {
			motors::tweakRightUp();
		} else if (dEncoders < 0.0) {
			motors::tweakLeftUp();
		}
	}

	if (DifferentialEncoders::average() > 11.74) {
		motors::stop();
	} else if (DifferentialEncoders::average() >= 12.0) {
		motors::kill();
	}

	delay(1);

	if (millis() % 500 == 0) {
		Serial.print("enc left: ");
		Serial.println(DifferentialEncoders::getLeft());
		Serial.print("enc right: ");
		Serial.println(DifferentialEncoders::getRight());
	}
}
