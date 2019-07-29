#ifndef __ROBOTS_SCOOTER2_SENSORS_H
#define __ROBOTS_SCOOTER2_SENSORS_H


void	SenseRange();
void	SetupSensors();
float	GuessFrontDistance(bool useAverage=false);
float	GuessLeftDistance(bool useAverage=false);
float	GuessRightDistance(bool useAverage=false);
void	PrintDistances();
void	TestPanning();



#endif // __ROBOTS_SCOOTER2_SENSORS_H
