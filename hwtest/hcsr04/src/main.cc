#include <Arduino.h>
#include <HCSR04.h>
#include <Streaming.h>



UltraSonicDistanceSensor	usd(8, 9);


void
setup()
{
	Serial.begin(9600);
	while (!Serial) ;
}


void
loop()
{
	double distance = usd.measureDistanceCm();

	Serial << "Distance: " << distance << " cm." << endl;
	delay(250);
}
