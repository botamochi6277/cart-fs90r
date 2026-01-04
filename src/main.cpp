#include <Arduino.h>
#include <M5Unified.h>
#include <ESP32Servo.h>

#include "BLECartService.hpp"

const int SERVO_L_PIN = 26;
const int SERVO_R_PIN = 32;

ble::BLECartService cart_service;

Servo left_servo;
Servo right_servo;

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

    auto cfg = M5.config();
    cfg.serial_baudrate = 115200;
    M5.begin(cfg);

    // BLE setup
    if (!BLE.begin())
    {
        Serial.println("starting BLE failed!");
        while (1)
            ;
    }
    Serial.println("starting BLE success!");

    String ble_address = BLE.address();
    String local_name = "Cart" + ble_address;
    BLE.setDeviceName(local_name.c_str());
    BLE.setLocalName("Cart");
    BLE.setAdvertisedService(cart_service);

    // add service
    BLE.addService(cart_service);
    // start advertising
    BLE.advertise();
}

void loop()
{
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

    BLE.poll();
    auto is_written = cart_service.poll(left_servo, right_servo);
    if (is_written > 0)
    {
        Serial.printf("Left value: %d, Right value: %d, Command: %c, written charactoristic: %d\n\r",
                      cart_service.left_wheel_power_char.value(),
                      cart_service.right_wheel_power_char.value(),
                      cart_service.command_char.value(),
                      is_written);
        Serial.printf("Left Power: %d, Right Power: %d", left_servo.read(), right_servo.read());
        Serial.println();
    }

    delay(1000); // wait for 0.1 second
}
