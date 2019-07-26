#include <Arduino.h>
#include <HCSR04.h>
#include <Streaming.h>



sensors::HCSR04_Single	usd(8);


void
setup()
{
	Serial.begin(9600);
	while (!Serial) ;
}


void
loop()
{
	double distance = usd.range();

	Serial << "Distance: " << distance << " cm." << endl;
	delay(250);
}
