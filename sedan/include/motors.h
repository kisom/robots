#ifndef __ROBOTS_SCOOTER2_MOTORS_H
#define __ROBOTS_SCOOTER2_MOTORS_H


#include <stdint.h>


namespace Drive {

constexpr uint8_t		DefaultSpeed = 127;


enum class Direction : uint8_t {
	Stop = 0, // default value
	Forward = 1,
	Backward = 2,
	TurnLeft = 3,
	TurnRight = 4
};


struct Belief {
	uint8_t		speed;
	Direction	direction;
};


void Begin();
void Drive(Direction direction, uint8_t speed, bool throttle);
void Stop(bool hard = false);
void Forward();
void Backward();
void TurnLeft();
void TurnRight();
void SetSpeed(uint8_t);
void Throttle(uint8_t targetSpeed);
void WorldView(struct Belief *target);


} // namespace Drive


#endif // __ROBOTS_SCOOTER2_MOTORS_H
