#include <Arduino.h>
#include <M5Unified.h>
#include <ESP32Servo.h>

const int SERVO_L_PIN = 26;
const int SERVO_R_PIN = 32;

Servo left_servo;
Servo right_servo;

// put function declarations here:
int myFunction(int, int);

void setup()
{
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  left_servo.setPeriodHertz(50);              // Standard 50hz servo
  right_servo.setPeriodHertz(50);             // Standard 50hz servo
  left_servo.attach(SERVO_L_PIN, 500, 2400);  // pin, min, max pulse width in microseconds
  right_servo.attach(SERVO_R_PIN, 500, 2400); // pin, min, max pulse width in microseconds

  M5.begin();
  Serial.begin(115200);
}

void loop()
{
  // put your main code here, to run repeatedly:

  M5.update();
  if (M5.BtnA.wasClicked())
  {
    // test driving
    left_servo.write(180);
    right_servo.write(180);
    delay(3000);

    left_servo.write(90);
    right_servo.write(90);
    delay(1000);

    left_servo.write(0);
    right_servo.write(0);
    delay(3000);

    left_servo.write(90);
    right_servo.write(90);
    delay(3000);
  }

  delay(100); // wait for 0.1 second
}

// put function definitions here:
int myFunction(int x, int y)
{
  return x + y;
}