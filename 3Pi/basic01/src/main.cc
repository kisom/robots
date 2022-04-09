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


const char	alertSequence[] PROGMEM = "O2CD8R8CD8";
const char	alertTone[] PROGMEM = "O2CD8";
const char	motorForward[] PROGMEM = "O3E8F8E8";
const char	motorBackward[] PROGMEM = "O3F8E8F8";
const char	motorLeft[] PROGMEM = "O3E8F8G8";
const char	motorRight[] PROGMEM = "O3G8F8E8";


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
displayLines(const char *line1, const char *line2)
{
	display.clear();
	display.print(line1);
	display.gotoXY(0, 1);
	display.print(line2);
	display.display();
}


void
robotKill()
{
	// Most important thing: stop the motors.
	RobotState.motor = MOTOR_STOP;

	displayLines(" ROBOT  ", "  KILL  ");
	play(alertSequence);
	motors.setSpeeds(0, 0);
	RobotState.action = ACTION_KILL;

	while (true) {
		ledYellow(0);
		ledRed(1);
		delay(500);;
		ledRed(0);
		ledYellow(1);
		delay(500);
	}
}


void
motorControl()
{
	uint8_t	current = MOTOR_STOP;

	while (true) {
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
	Wire.begin();
	if (!imu.init()) {
		RobotState.action = ACTION_KILL;
		return;
	}
	imu.configureForCompassHeading();
}


void
runIMU()
{
	if (imu.magDataReady()) {
		imu.readMag();
	}
	delay(1);
}


void
setupBumpSensors()
{
	bumpSensors.calibrate();
}


void
runBumpSensors()
{
	while (true) {
		if (RobotState.action != ACTION_EXPLORE) {
			delay(1);
			continue;
		}

		bumpSensors.read();

		if (bumpSensors.leftChanged() || bumpSensors.rightChanged()) {
			if (bumpSensors.leftIsPressed()) {
				play(alertTone);
				RobotState.action = ACTION_BACKUP;
				RobotState.next = ACTION_TURNR;
				RobotState.goal = (int)(millis() + 250);
				displayLines(" BACKUP ", " ->RIGHT ");
			} else if (bumpSensors.rightIsPressed()) {
				play(alertTone);
				RobotState.action = ACTION_BACKUP;
				RobotState.next = ACTION_TURNL;
				RobotState.goal = (int)(millis() + 250);
				displayLines(" BACKUP ", " ->LEFT  ");
			}
		}

		delay(1);
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
	while (true) {
		delay(1);
	}
}


void
setupKillButton()
{
	// buttonA.waitForRelease();
}


void
runKillButton()
{
	while (true) {
		if (buttonA.getSingleDebouncedPress()) {
			robotKill();
		}
		delay(10);
	}
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
		RobotState.motor = MOTOR_FORWARD;
		break;
	case ACTION_STOP:
		RobotState.motor = MOTOR_STOP;
		break;
	case ACTION_BACKUP:
		ledRed(1);
		ledYellow(1);
		RobotState.motor = MOTOR_BACKWARD;
		if (millis() > (unsigned long)RobotState.goal) {
			RobotState.motor = MOTOR_STOP;
			delay(50);
			RobotState.action = RobotState.next;
			RobotState.goal = int(millis() + 1250);
		}
		break;
	case ACTION_TURNR:
		ledRed(1);
		ledYellow(0);
		RobotState.motor = MOTOR_RIGHT;
		if (millis() > (unsigned long)RobotState.goal) {
			RobotState.motor = MOTOR_STOP;
			delay(50);
			RobotState.action = ACTION_EXPLORE;
			ledRed(0);
			displayLines("EXPLORE", "");
		}
		break;
	case ACTION_TURNL:
		ledRed(0);
		ledYellow(1);
		RobotState.motor = MOTOR_LEFT;
		if (millis() > (unsigned long)RobotState.goal) {
			RobotState.motor = MOTOR_STOP;
			delay(50);
			RobotState.action = ACTION_EXPLORE;
			ledYellow(0);
			displayLines("EXPLORE", "");
		}
		break;
	case ACTION_INACTIVE:
		// fallthrough
	case ACTION_KILL:
		// fallthrough
	default:
		RobotState.motor = MOTOR_STOP;
		robotKill();
	}
	delay(1);
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
	Scheduler.start(setupBumpSensors, runBumpSensors);
	Scheduler.start(NULL, motorControl);	
	Scheduler.start(NULL, eventTimer);
	Scheduler.start(NULL, mainLoop);
	Scheduler.start(setupKillButton, runKillButton);

	if (RobotState.action != ACTION_KILL) {
		displayLines("EXPLORE", "");
		RobotState.action = ACTION_EXPLORE;
	}
}


void
loop()
{
	delay(1);
}
