#include <Arduino.h>
#include <Servo.h>
#include <Streaming.h>
#include <HCSR04.h>
#include <logging.h>
#include <sensors.h>


constexpr uint8_t		 maxSteps = 9;
static uint8_t			 steps[maxSteps] = {
	30, 45, 60, 75, 90, 105, 120, 135, 150,
};
static float			 distance[maxSteps];
#define distanceIndex(deg)	 (distance[((deg) / 15)-2])

static Servo			 ursPan;
static sensors::HCSR04		 urs(6, 7);


static void
printDistances()
{
	Serial << "DISTANCES" << endl;
	for (uint8_t i = 0; i < maxSteps; i++) {
		Serial << int(i++2) * 15) << "º: " << distance[i] << endl;
	}
}


static void
logDistance()
{
#if 0
	SEROUT << "logging distance... ";
	if (!LoggingEnabled) {
		SEROUT << "disabled" << endl;;
		return;
	}

	auto now = millis();
	Logger << now << ",DIST";
	for (uint8_t i = 0; i < maxSteps; i++) {
		Logger << "," << distance[i];
	}
	Logger << endl;
	Logger.flush();
	SEROUT << "OK" << endl;
#endif
}


void
SenseRange()
{
	Serial << "scanning" << endl;
	for (uint8_t i = 0; i < maxSteps; i++) {
		// Pan & scan.
		Serial << "pan: " << steps[i] << endl;
		ursPan.write(steps[i]);

		delay(sensors::HCSR04::updateDelay);
		distance[i] = urs.range();
	}

	for (int8_t i = maxSteps-1; i >= 0; i--) {
		// Pan & scan.
		Serial << "pan: " << steps[i] << endl;
		ursPan.write(steps[i]);

		delay(sensors::HCSR04::updateDelay);
		distance[i] += urs.range();
		distance[i] /= 2;
	}

	printDistances();
	logDistance();
}


void
TestPanning()
{
	while (true) {
		for (int8_t i = 0; i < maxSteps; i++) {
			ursPan.write(steps[i]);
			delay(60);
			Serial << steps[i] << "\t" << ursPan.read() << endl;
		}

		for (int8_t i = maxSteps-1; i >= 0; i--) {
			ursPan.write(steps[i]);
			delay(60);
			Serial << steps[i] << "\t" << ursPan.read() << endl;
		}
	}
}


void
SetupSensors()
{
	ursPan.attach(9);
	ursPan.write(90);
	delay(1000);
}


static void
logGuess(char direction, float distance)
{
	SEROUT << "GUESS(" << direction << "): " << distance << endl;
#if 0
	if (!LoggingEnabled) {
		return;
	}

	auto now = millis();
	Logger << now << ",GUESS," << direction << "," << distance << endl;
	Logger.flush();
#endif
}


float
GuessFrontDistance(bool useAverage)
{
	float	average = 0.0;
	float	minDist = 0.0;

	average = distance[3] +
		  distance[4] +
		  distance[5];
	average /= 3;
	minDist = average;

	for (uint8_t i = 3; i < 6; i++) {
		minDist = minDist < distance[i] ? minDist : distance[i];
	}

	if (useAverage) {
		logGuess('F', average);
		return average;
	}
	logGuess('F', minDist);
	return minDist;
}


float
GuessLeftDistance(bool useAverage)
{
	float	average = 0.0;
	float	minDist = 0.0;

	average = distance[0] +
		  distance[1] +
		  distance[2];
	average /= 3;
	minDist = average;

	for (uint8_t i = 0; i < 3; i++) {
		minDist = minDist < distance[i] ? minDist : distance[i];
	}

	if (useAverage) {
		logGuess('L', average);
		return average;
	}
	logGuess('L', minDist);
	return minDist;
}


float
GuessRightDistance(bool useAverage)
{
	float	average = 0.0;
	float	minDist = 0.0;

	average = distance[6] +
		  distance[7] +
		  distance[8];
	average /= 3;
	minDist = average;

	for (uint8_t i = 6; i < 9; i++) {
		minDist = minDist < distance[i] ? minDist : distance[i];
	}

	if (useAverage) {
		logGuess('R', average);
		return average;
	}
	logGuess('R', minDist);
	return minDist;
}


void
PrintDistances()
{
	uint8_t	i = 0;
	while (true) {
		printDistances();	
		for (i = 0; i < 250; i++) {
			delay(1);
			yield();
		}
	}
}
