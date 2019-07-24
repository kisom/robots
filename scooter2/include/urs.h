#ifndef __ROBOTS_SCOOTER2_URS_H
#define __ROBOTS_SCOOTER2_URS_H


namespace URS {


// HCSR04 ultrasonic ranging sensor using two pins. Effective angle:
// 15º.
class HCSR04 {
public:
	Sensor(uint8_t trigger, uint8_t echo);
private:
	uint8_t	trigger;
	uint8_t	echo;
};


} // namespace URS


#endif // __ROBOTS_SCOOTER2_URS_H
