#include <Arduino.h>

#include <Servo.h>

Servo servo;


void setup() {
	servo.attach(8);
	servo.write(90); // halfway

	pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
	digitalWrite(LED_BUILTIN, HIGH);
	delay(100);
	digitalWrite(LED_BUILTIN, LOW);
	delay(100);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(100);
	digitalWrite(LED_BUILTIN, LOW);
	delay(700);
}
