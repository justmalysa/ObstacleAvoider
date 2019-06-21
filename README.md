# ObstacleAvoider
This repository contains firmware for the obstacle avoiding mobile robot.
The device is based on Atmega8 microcontroller. It is equipped with three ultrasonic 
distance sensors witch are used to detect obstacles in the proximity of the robot.
With each control loop iteration, implemented algorithm tries to choose most appropriate movement direction.
What is more, it is possible to steer the robot remotely by using serial commands.
These can be sent wirelessly by using Bluetooth module.