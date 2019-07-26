#ifndef __ROBOTS_SCOOTER2_URS_H
#define __ROBOTS_SCOOTER2_URS_H


namespace URS {


// HCSR04 ultrasonic ranging sensor using two pins. Effective angle:
// 15º.
class HCSR04 {
public:
	HCSR04(uint8_t trigger, uint8_t echo);

	float	range();
private:
	uint8_t	trigger;
	uint8_t	echo;
	float	distance;
};


} // namespace URS


#endif // __ROBOTS_SCOOTER2_URS_H
