#ifndef _ZUMO_CONFIG_H
#define	_ZUMO_CONFIG_H

#include "PololuConfig.h"


// Drive parameters.
#define GEAR_RATIO	GEAR_RATIO_50_1
#define WHEEL_DIAMETER	WHEEL_ZUMO
#define WHEEL_CIRC	(3.141593 * WHEEL_DIAMETER)


static const short SPEED_MAX = 400;
static const short SPEED_MIN = -400;
static const short SPEED_DRIVE = 150;


#define MINIMUM_VOLTAGE_MV(ncells)	(ncells * 1100)

static const short MIN_VOLT4	= MINIMUM_VOLTAGE_MV(4);


#endif /* _ZUMO_CONFIG_H */
