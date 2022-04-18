RobotController
==============

This is a Teensy-based robot controller, particularly designed as
a co-proceessor for Pololu's A-Star 32U4-based robots or in conjunction
with a Raspberry Pi.

Pinouts
=======

Pin		Function		Description
-----------------------------------------------------------------------------
0		RX			FTDI RX
1		TX			FTDI TX
2					URS 0 Trigger
3					URS 1 Trigger
4					URS 2 Trigger
5					URS 3 Trigger
6					URS 4 Trigger
7					URS 0 Echo
8					URS 1 Echo
9					URS 2 Echo
10					URS 3 Echo
11					URS 4 Echo
12					I/O 0
13					I/O 1
14					Sensor 0
15					Sensor 1
16					Sensor 2
17					Sensor 3
18					I2C SDA
19					I2C SCL
20					Sensor 4
21					Sensor 5
22					Sensor 6
23					Sensor 7

The power select pins are designed to support 3.3V and 5V
power input. Add a jumper across to the appropriate port
and run the power input to the unused PWRSEL pin. If Vcc
is 3.3V and 5V is needed as well, connect 5V to the 5V
power supply.

The FTDI so					Sensor 7
