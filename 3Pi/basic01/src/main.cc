#include <Arduino.h>
#include <Pololu3piPlus32U4.h>
#include <Pololu3piPlus32U4IMU.h> // nb: draws in another 1400b
#include <Scheduler.h>

using namespace Pololu3piPlus32U4;

 
OLED		display;
Buzzer		buzzer;
ButtonA		buttonA;
ButtonB		buttonB;
ButtonC		buttonC;
LineSensors	lineSensors;
BumpSensors	bumpSensors;
Motors		motors;
Encoders	encoders;
IMU		imu;


const bool BUZZER_MUTE = false;


#define MOTOR_STOP	0
#define MOTOR_FORWARD	1
#define MOTOR_BACKWARD	2
#define MOTOR_LEFT	3
#define MOTOR_RIGHT	4

// The speed here is chosen as fast enough to be able to deflect the
// bump sensors, but not enough to damage the robot.
#define MOTOR_PSPEED	100
#define MOTOR_NSPEED	-100

#define ACTION_INACTIVE	0
#define ACTION_STOP	1
#define ACTION_EXPLORE	2
#define ACTION_BACKUP	3
#define ACTION_TURNR	4
#define ACTION_TURNL	5
#define ACTION_KILL	6


const char	alertSequence[] PROGMEM = "<C<D8R2<C<D8";
const char	alertTone[] PROGMEM = "<C<D8";
const char	motorForward[] PROGMEM = "<E8<F8<E8";
const char	motorBackward[] PROGMEM = "<F8<E8<F8";
const char	motorLeft[] PROGMEM = "<E8<F8<G8";
const char	motorRight[] PROGMEM = "<G8<F8<E8";


struct {
	uint8_t		motor;
	uint8_t		action;
	uint8_t		next;
	int		goal;
	int		heading;
} RobotState;


void
play(const char *progmemBuffer)
{
	if (BUZZER_MUTE) {
		return;
	}

	if (buzzer.isPlaying()) {
		buzzer.stopPlaying();
	}
	buzzer.playFromProgramSpace(progmemBuffer);
}


void
motorControl()
{
	uint8_t	current = MOTOR_STOP;

	while (true) {
		ledRed(0);
		if (current == RobotState.motor) {
			delay(1);
			continue;
		}
		current = RobotState.motor;

		int16_t	left = 0;
		int16_t	right = 0;
		switch (current) {
			case MOTOR_FORWARD:
				left = MOTOR_PSPEED;
				right = MOTOR_PSPEED;
				play(motorForward);
				break;
			case MOTOR_BACKWARD:
				left = MOTOR_NSPEED;
				right = MOTOR_NSPEED;
				play(motorBackward);
				break;
			case MOTOR_LEFT:
				left = MOTOR_NSPEED;
				right = MOTOR_PSPEED;
				play(motorLeft);
				break;
			case MOTOR_RIGHT:
				left = MOTOR_PSPEED;
				right = MOTOR_NSPEED;
				play(motorRight);
				break;
			case MOTOR_STOP:
				play(alertTone);
				// fallthrough
			default:
				// nothing to do
				break;
		}

		motors.setSpeeds(left, right);
		delay(1);
	}
}


void
runningLightBlink()
{
	while (true) {
		ledGreen(1);
		delay(100);
		ledGreen(0);
		delay(100);
		ledGreen(1);
		delay(100);
		ledGreen(0);
		delay(700);
	}
}


void
runningLightSetup()
{	
	ledGreen(0);
}


void
setupIMU()
{
	Wire.start();
	if (!imu.init()) {
		RobotState.action = ACTION_KILL;
		return;
	}
	img.configureForCompassHeading();
}


void
runIMU()
{
	if (imu.magDataReady()) {
		img.readMag();
	}
}


bool
between(unsigned long val, unsigned long low, unsigned long high)
{
	return (val > low && val < high);
}


void
eventTimer()
{

}


void
mainLoop()
{
	if (!BUZZER_MUTE && buzzer.isPlaying()) {
		buzzer.playCheck();
	}
	delay(1);

	switch (RobotState.action) {
	case ACTION_EXPLORE:
		RobotState.motors = MOTOR_FORWARD;
		break;
	case ACTION_STOP:
		RobotState.motors = MOTOR_STOP;
		break;
	case ACTION_BACKUP:
		if (millis() > (unsigned long)RobotState.goal) {
			RobotState.motors = MOTOR_STOP;
			RobotState.action = RobotState.next;
		}
		break;
	case ACTION_TURNR:
		break;
	case ACTION_TURNL:
		break;
	case ACTION_INACTIVE:
		// fallthrough
	case ACTION_KILL:
		RobotState.mtoors = MOTOR_STOP;
		if (!buzzer.isPlaying()) {
			play(alertSequence);
		}

		ledRed(1);
		delay(500);;
		ledRed(0);
		delay(500);
		break;
	case 
	}
}


void
setup()
{
	Serial.begin(9600);

	display.setLayout8x2();
	display.clear();
	display.print(F("BOOT OK"));
	display.display();
	delay(1000);

	display.clear();
	display.print(F("HELLO"));
	display.display();
	delay(1000);
	display.gotoXY(0, 1);
	display.print(F("  WORLD"));
	display.display();
	delay(1000);

	display.clear();
	display.print(F("PRESS B"));
	display.gotoXY(0, 1);
	display.print(F("TO START"));
	display.display();

	buttonB.waitForButton();
	display.clear();
	display.print(F(" STAND  "));
	display.display();
	delay(1000);
	display.gotoXY(0, 1);
	display.print(F(" CLEAR  "));
	display.display();
	delay(1000);
	
	buzzer.playMode(PLAY_CHECK);
	play(alertSequence);

	Scheduler.start(runningLightSetup, runningLightBlink);
	Scheduler.start(NULL, motorControl);	
	Scheduler.start(NULL, eventTimer);
	Scheduler.start(NULL, mainLoop);

	if (RobotState.action != ACTION_KILL) {
		RobotState.action = ACTION_EXPLORE;
	}
}


void
loop()
{
	delay(1);
}
