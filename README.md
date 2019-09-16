# Smart Home Security System
This project provides a security solution for houses using an arduino kit.

# Motivation
Today nearly everything is smart and secure. So why not add doors to this line-up.

# Build Status
Successful build.

# Code Style
C/C++ for Arduino.

# IDE used
Arduino IDE

# Features
1. Generate a new random pin on boot and send it to you via an email.
2. Gives the ability to reset pin, if you forgot the pin.
3. Manually change and set a new pin.
4. Open door based on a pin.
5. On continuous use, recognise user's pattern of pin creation and suggest a new pin based on that pattern.

# Hardware required
1. Arduino Uno
2. ESP8266
3. Membrane Keypad (4x4)
4. 1602 LCD
5. Breadboard
6. Potentiometer
7. Push button
8. Jumper wires
9. Servo Motor

# Software and Services used
1. Arduino IDE
2. ThingSpeak
3. IFTTT webhook

# Prerequisite
1. The hardware
2. ThingSpeak account
3. IFTTT account

# How to use
1. Update the wifi ssid, password and your thingspeak api in the code.
2. Compile and upload the code to arduino using the ethernet cable.
3. Set up the motor and put the housing in a secure place.
4. Power on the device and check he pin from mail.
5. Enter the pin to unlock.
6. Press 'C' in open door to change the pin.
7. Press '\*' to close the door.
8. Press 'B' to reset the pin via mail.

# Credits
Sachin Sharma sir -> For giving the opportunity to work on an IoT project.

# Project Contributers
Abhinav Sharma  
Rohan Deoli  
Tanya Goel  
Samridhi Bhatt
