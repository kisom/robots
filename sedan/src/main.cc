#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>

#include <Adafruit_MotorShield.h>
#include <HCSR04.h>
#include <Scheduler.h>
#include <Streaming.h>


constexpr uint8_t		DefaultSpeed = 0;
static Adafruit_MotorShield	 motors;
static Adafruit_DCMotor		*LeftMotor  = motors.getMotor(1);
static Adafruit_DCMotor		*RightMotor = motors.getMotor(2);

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
SetSpeed(uint8_t speed)
{
	LeftMotor->setSpeed(speed);
	RightMotor->setSpeed(speed);
}


static void
drive(uint8_t l, uint8_t r)
{
	LeftMotor->run(l);
	RightMotor->run(r);
}


static void
DriveForward()
{
	drive(FORWARD, FORWARD);
}


static void
DriveBackward()
{
	drive(BACKWARD, BACKWARD);
}


static void
TurnLeft()
{
	drive(BACKWARD, FORWARD);
}


static void
TurnRight()
{
	drive(FORWARD, BACKWARD);
}


static void
Stop()
{
	SetSpeed(0);
	drive(RELEASE, RELEASE);
}


static void
driveTest()
{
	Serial.println("drive forward");
	DriveForward();
	delay(1000);

	Serial.println("turn left");
	TurnLeft();
	delay(1000);

	Serial.println("drive backward");
	DriveBackward();
	delay(1000);

	Serial.println("turn right");
	TurnRight();
	delay(1000);

	Serial.println("stop");
	Stop();
	delay(1000);
}


void
setup()
{
	Serial.begin(9600);
	delay(1000);

	Serial.println("setting up motors");
	motors.begin();

	Serial.print("set speed to ");
	Serial.println(DefaultSpeed, DEC);
	SetSpeed(DefaultSpeed);
	delay(1000);

	Scheduler.startLoop(senseRange);
}


void
loop()
{
	printDistances();
	delay(250);
}
