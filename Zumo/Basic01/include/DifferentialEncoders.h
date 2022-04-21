// DifferentialEncoders.h stores data for a balanced pair of encoders,
// intended to represent a left and a right side. It assumes that both
// motors are identical apart from direction.

#ifndef _ZUMO_DIFFENC_H_
#define _ZUMO_DIFFENC_H_


#include <config.h>


namespace DifferentialEncoders {
	void	setup();
	void	run();

	void	reset();
	double	getLeft();
	double	getRight();

	float	difference();
	double	average();

}; // namespace DifferentialEncoders


#endif // _ZUMO_DIFFENC_H_
