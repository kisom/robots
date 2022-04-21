#ifndef _ZUMO_MOTORS_H_
#define _ZUMO_MOTORS_H_


#include <Zumo32U4Motors.h>

#define MOTOR_BIG_DELTA	10

namespace motors {
	// Operational commands.
	void	setup();
	void	kill();
	void	enable();
	void	run();

	// Movement controls.
	void	stop();
	void	forward();
	void	backward();
	void	turnLeft();
	void	turnRight();

	// Tweaks to account for variances in motors.
	void	tweakLeftUp();
	void	tweakLeftDown();
	void	tweakRightUp();
	void	tweakRightDown();

	// Getting status information.
	short	getLeft();
	short	getRight();
} // namespace motors

	
#endif // _ZUMO_MOTORS_H_
