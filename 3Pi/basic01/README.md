basic01
=======

This is a dumb program designed to test basic sensors and task scheduling.
All collision response is based on timing, e.g. the robot backs up for X
milliseconds and turns for Y milliseconds.

Lessons learned
- Scheduler
- Basic robot control
- IR-based bump sensor limitations

Next steps;
- IMU integration
  - change heading based on compass heading
  - use accelerometer to assist in detecting collisions
  - use accelerometer to assist in speed control
  - use gyro to detect changes in pitch and roll (falling into a pit)
  - use gryo to detect changes in yaw to assist in speed control
- motor control
  - change speeds smoothly, not as an instant change
- distance sensing for obstacle avoidance
  - HC-SR04
  - LIDAR / ToF sensors
