#include <Arduino.h>

#include <HCSR04.h>


namespace sensors {


// The controller boots up at millis() == 0, so we need to be updateDelay
// millis behind 0...
constexpr unsigned long	initialLastUpdate = ~HCSR04::updateDelay - 1;


HCSR04::HCSR04(uint8_t trigger, uint8_t echo) :
	trigger(trigger), echo(echo), distance(-1.0),
	lastUpdate(initialLastUpdate)
{
	pinMode(trigger, OUTPUT);
	pinMode(echo, INPUT);
	digitalWrite(trigger, LOW);
}


double
HCSR04::range()
{
	// Datasheet:
	//   https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
	//
	// Timing diagram The Timing diagram is shown below. You only need
	// to supply a short 10uS pulse to the trigger input to start the
	// ranging, and then the module will send out an 8 cycle burst of
	// ultrasound at 40 kHz and raise its echo. The Echo is a distance
	// object that is pulse width and the range in proportion .You can
	// calculate the range through the time interval between sending
	// trigger signal and receiving echo signal. Formula: uS / 58 =
	// centimeters or uS / 148 =inch; or: the range = high level time *
	// velocity (340M/S) / 2; we suggest to use over 60ms measurement
	// cycle, in order to prevent trigger signal to the echo signal.

	// Enforce the update delay cycle, returning the last distance
	// measurement taken if it hasn't been long enough to take a new
	// measurement.
	if (!this->ready()) {
		return this->distance;
	}
	
	// Verify we get a clean signal by holding the trigger LOW
	// for 1ms to stabilise.
	digitalWrite(this->trigger, LOW);
	delayMicroseconds(1);

	// A reading is triggered by a 10µs TTL.
	digitalWrite(this->trigger, HIGH);
	delayMicroseconds(10);
	digitalWrite(this->trigger, LOW);

	// Read the time for the echo pulse to be returned.
	unsigned long echoDuration = pulseIn(this->echo, HIGH);
	this->distance = static_cast<double>(echoDuration) / 58.0;

	// There's a little fudge factor in these distances
	// TODO: figure out if this actually does what I think it does.
	// Narrator voice: it didn't.
	if (this->distance < 2.1) {
		this->distance = 0.0;
	} else if (this->distance > 399) {
		this->distance = 400.0;
	}

	return this->distance;
}


bool
HCSR04::ready()
{
	unsigned long	now = millis();

	if ((now - this->lastUpdate) <= HCSR04::updateDelay) {
		return false;
	}

	return true;
}


HCSR04SinglePin::HCSR04SinglePin(uint8_t pin) :
	pin(pin), distance(-1.0), lastUpdate(initialLastUpdate)
{

}


double
HCSR04SinglePin::range()
{
	// Datasheet:
	//   https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
	//
	// Timing diagram The Timing diagram is shown below. You only need
	// to supply a short 10uS pulse to the trigger input to start the
	// ranging, and then the module will send out an 8 cycle burst of
	// ultrasound at 40 kHz and raise its echo. The Echo is a distance
	// object that is pulse width and the range in proportion .You can
	// calculate the range through the time interval between sending
	// trigger signal and receiving echo signal. Formula: uS / 58 =
	// centimeters or uS / 148 =inch; or: the range = high level time *
	// velocity (340M/S) / 2; we suggest to use over 60ms measurement
	// cycle, in order to prevent trigger signal to the echo signal.

	// Enforce the update delay cycle, returning the last distance
	// measurement taken if it hasn't been long enough to take a new
	// measurement.
	if (!this->ready()) {
		return this->distance;
	}
	
	// Set the pin to output mode to trigger a pulse.
	pinMode(this->pin, OUTPUT);
	delayMicroseconds(1);

	// Verify we get a clean signal by holding the trigger LOW
	// for 1ms to stabilise.
	digitalWrite(this->pin, LOW);
	delayMicroseconds(1);

	// A reading is triggered by a 10µs TTL.
	digitalWrite(this->pin, HIGH);
	delayMicroseconds(10);
	digitalWrite(this->pin, LOW);

	pinMode(this->pin, INPUT);

	// Read the time for the echo pulse to be returned.
	unsigned long echoDuration = pulseIn(this->pin, HIGH);
	this->distance = static_cast<double>(echoDuration) / 58.0;

	// There's a little fudge factor in these distances
	// TODO: figure out if this actually does what I think it does.
	// Narrator voice: it didn't.
	if (this->distance < 2.1) {
		this->distance = 0.0;
	} else if (this->distance > 399) {
		this->distance = 400.0;
	}

	return this->distance;
}


bool
HCSR04SinglePin::ready()
{
	unsigned long	now = millis();

	if ((now - this->lastUpdate) <= HCSR04SinglePin::updateDelay) {
		return false;
	}

	return true;
}


} // namespace sensors
