#ifndef __ROBOTS_SCOOTER2_URS_H
#define __ROBOTS_SCOOTER2_URS_H


// HCSR04 defines an interface to a common, low-cost ultrasonic
// sensor. Useful to know:
//	+ Voltage: 5V (they are not tolerant to overvoltage, but seem to
//	  work at 3.3V)
//	+ Typical current draw: 15mA
//	+ Range: 2-400cm
//	+ Measuring angle: 15º

// TODO: write a class that uses a single pin.

namespace sensors {


// HCSR04 ultrasonic ranging sensor using two pins.
class HCSR04 {
public:
	// The HCSR04 is a 4-pin sensor: 5V, TTL, echo, and ground.
	HCSR04(uint8_t trigger, uint8_t echo);

	// Range returns the distance measurement from the sensor in
	// centimetres; a range of infinity is marked as 400. If the
	// sensor hasn't taken a measurement yet, it will return -1.0.
	double	range();

	// Ready returns true if enough time has elapsed since the last
	// update for the sensor to take another reading.
	bool	ready();


	// Each sensor should only be sampled once every 65 ms to prevent
	// crosstalk between the trigger and echo (e.g. not triggering
	// before an echo has completed).
	static const unsigned long	updateDelay = 65;
private:
	uint8_t		trigger;
	uint8_t		echo;
	double		distance;
	unsigned long	lastUpdate;
};


// HCSR04SinglePin is an HCSR04 using a single pin.
class HCSR04SinglePin {
public:
	// The HCSR04 is a 4-pin sensor: 5V, TTL, echo, and ground.
	HCSR04SinglePin(uint8_t pin);

	// Range returns the distance measurement from the sensor in
	// centimetres; a range of infinity is marked as 400. If the
	// sensor hasn't taken a measurement yet, it will return -1.0.
	double	range();

	// Ready returns true if enough time has elapsed since the last
	// update for the sensor to take another reading.
	bool	ready();


	// Each sensor should only be sampled once every 65 ms to prevent
	// crosstalk between the trigger and echo (e.g. not triggering
	// before an echo has completed).
	static const unsigned long	updateDelay = 65;
private:
	uint8_t		pin;
	double		distance;
	unsigned long	lastUpdate;
};



} // namespace sensors


#endif // __ROBOTS_SCOOTER2_URS_H
