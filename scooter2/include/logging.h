#ifndef __ROBOTS_SCOOTER2_LOGGING_H
#define __ROBOTS_SCOOTER2_LOGGING_H


#include <Arduino.h>
#include <SD.h>


static File	Logger;
static bool	LoggingEnabled;


void	SetupLogging(uint8_t cs);


#endif // __ROBOTS_SCOOTER2_LOGGING_H
