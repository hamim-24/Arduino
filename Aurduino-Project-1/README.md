
# Smart Access & Detection System using Arduino

In this project we will make a autometic gate that can track object and ask for scanner card and then password. If both are correct then it opens the gate for 5 seconds


## Features

- Rotated eyes(searching for object)
- Stop, Red indicator turns on and beep sound when object found
- Ask for scan card and password in lcd
- Beep sound when scan wrong card or password
- When correct card or password green indicator turns on
- Timer(5s) for closing gate with beep sound 
- Then gate closes autometically

## Elements
- Arduino Uno - 1 piece
- Ultrasonic sensor - 1 piece
- Servo motor - 2 piece
- 16x2 I2C LED Display - 1 piece
- RFID RC522 - 1 piece
- Led - red, green
- Buz - 1 piece


## Library
- SPI.h (for RFID)
- MFRC522.h (for RFID)
- Servo.h (for Servo)
- Wire.h (for lcd)
- LiquidCrystal_I2C.h (for lcd)


    
## Connections

Ultrasonic Sensor
- GND - GND
- VCC - 5v
- TRIG - 8
- ECHO - 7

16x2 I2C LED Display
- GND - GND
- VCC - 5v
- SCL - A5
- SDA - A4

Servo(1, 2)
- Broun(GND) - GND
- Red(VCC) - 5v
- Yello(1) - 6
- Yello(2) - 3

LED(use resister)
- GND - GND
- Red(VCC) - 5
- Green(VCC) - 4

Buz
- GND - GND
- VCC - 2

RFID RC522
- SDA - 10
- SCK - 13
- MOSI - 11
- MISO - 12
- IRQ - not connected
- GND - GND
- RST - 9
- 3.3V - 3.3V
## Discription

Compile and upload code. You must connect with laptop while processing. Open the serial monitor(9600). Enter password in the monitor. The command will also show in the serial monitor.

