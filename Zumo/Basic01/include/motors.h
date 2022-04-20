#ifndef _ZUMO_MOTORS_H_
#define _ZUMO_MOTORS_H_


#include <Zumo32U4Motors.h>


namespace motors {
	void	setup();
	void	kill();
	void	enable();
	void	run();
	void	stop();

	void	forward();
	void	backward();
	void	turnLeft();
	void	turnRight();

	short	getLeft();
	short	getRight();
} // namespace motors

	
#endif // _ZUMO_MOTORS_H_
