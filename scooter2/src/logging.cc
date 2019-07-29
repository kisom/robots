#include <Arduino.h>
#include <SD.h>
#include <logging.h>


void
SetupLogging(uint8_t cs)
{
#if 0
	if (cs == 0) {
		return;
	}

	pinMode(cs, OUTPUT);
	if (!SD.begin(cs)) {
		return;
	}

	Logger = SD.open("robot.log", FILE_WRITE);
	if (Logger) {
		LoggingEnabled = true;
		Serial.println("logging to SD card");
	}
#endif
}
