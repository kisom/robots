#include <Arduino.h>

#include "urs.h"


namespace URS {


HCSR04::HCSR04(uint8_t trigger, uint8_t echo) :
	trigger(trigger), echo(echo), distance(-1.0)
{
	pinMode(trigger, OUTPUT);
	pinMode(echo, INPUT);
	digitalWrite(trigger, LOW);
}


double
HCSR04::range()
{
	// TODO: support delay between readings, returning cached
	// distance in between.
	
	// Verify we get a clean signal by holding the trigger LOW
	// for 1ms to stabilise.
	digitalWrite(this->trigger, LOW);
	delayMicroseconds(1);

	// A reading is triggered by a 10µs TTL.
	digitalWrite(this->trigger, HIGH);
	delayMicroseconds(10);
	digitalWrite(this->trigger, LOW);

	// Read the time for the echo pulse to be returned, accounting
	// for ToF.
	unsigned long echoDuration = pulseIn(this->echo, HIGH) >> 1;
	this->distance = static_cast<double>(echoDuration) / 58.0;

	if (this->distance < 2.1) {
		this->distance = 0.0;
	} else if (this->distance > 390) {
		this->distance = -1.0;
	}

	return this->distance;
}


} // namespace URS
