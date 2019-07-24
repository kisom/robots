#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_MotorShield.h>
#include <HCSR04.h>
#include <Scheduler.h>
#include <Streaming.h>

#include <motors.h>


double				 distance[3];
UltraSonicDistanceSensor	 urs[3] = {
	UltraSonicDistanceSensor(10, 11), // Left
	UltraSonicDistanceSensor(8, 9),   // Forward
	UltraSonicDistanceSensor(6, 7),   // Backward
};


static void
senseRange()
{
	for (uint8_t sensor = 0; sensor < 3; sensor++) {
		distance[sensor] = urs[sensor].measureDistanceCm();
		delay(66);
		yield();
	}
	yield();
}


static void
printDistances()
{
	Serial << "DISTANCES" << endl;
	Serial << "Left: "  << distance[0] << " cm" << endl;
	Serial << "Front: " << distance[1] << " cm" << endl;
	Serial << "Right: " << distance[2] << " cm" << endl;
}


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


void
setup()
{
	Serial.begin(9600);
	delay(1000);

	Serial.println("setting up motors");
	Drive::Begin();

	delay(1000);
	driveTest();

	Scheduler.startLoop(senseRange);
}


void
loop()
{
	printDistances();
	delay(250);
}
