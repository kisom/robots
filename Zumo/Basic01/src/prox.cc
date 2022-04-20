#include <Arduino.h>
#include <Zumo32U4ProximitySensors.h>


namespace prox {
Zumo32U4ProximitySensors	prox;
static const uint8_t		nbl = 6;

void
setup()
{
	prox.initThreeSensors();
}


void
run()
{
	prox.read();
	delay(20);
}


uint8_t
front()
{
	uint8_t		fl = nbl - prox.countsFrontWithLeftLeds();
	uint8_t		fr = nbl - prox.countsFrontWithRightLeds();
	return (fl + fr) / 2;
}


uint8_t
left()
{
	return nbl - prox.countsLeftWithLeftLeds();
}


uint8_t
right()
{
	return nbl - prox.countsRightWithRightLeds();
}


} // namespace prox


