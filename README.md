# Arduino-RC-Car-Project
This is a DIY Remote-Controlled Car built using Arduino, capable of forward/backward motion and left/right steering. It uses nRF24L01 wireless modules for communication between the transmitter (controller) and the receiver (car), and includes a servo motor for steering and DC motors for movement.
# Project Overview
Transmitter: Arduino UNO + Joystick + nRF24L01

Receiver: Arduino MEGA + nRF24L01 + L298N Motor Driver

Movement: 2 DC motors (rear wheels)

Steering: Servo motor (front wheels via LEGO setup)

Power Supply:

6x AA batteries (for Arduino Mega & servo)

9V battery (for DC motors via L298N)

# How It Works
Controller Side:

Reads joystick position.

Sends directional commands via nRF24L01.

Car Side:

Receives command via nRF24L01.

Translates joystick Y-axis to forward/backward motor control.

Translates joystick X-axis to servo angle for steering.
