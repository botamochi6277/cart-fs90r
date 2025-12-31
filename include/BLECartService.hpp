#ifndef BLE_CART_SERVICE_HPP
#define BLE_CART_SERVICE_HPP

#include <ArduinoBLE.h>
#include <ESP32Servo.h>
#include <M5Unified.h>
namespace ble
{
    class BLECartService : public BLEService
    {
    public:
        BLEByteCharacteristic left_wheel_power_char;
        BLEByteCharacteristic right_wheel_power_char;

        BLECartService()
            : BLEService("19b10000-e8f2-537e-4f6c-d104768a1214"),
              left_wheel_power_char("19b10001-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite),
              right_wheel_power_char("19b10002-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite)
        {
            addCharacteristic(left_wheel_power_char);
            addCharacteristic(right_wheel_power_char);

            // Add user description descriptors
            auto left_user_descriptor = BLEDescriptor("2901", "Left Wheel Power");
            left_wheel_power_char.addDescriptor(left_user_descriptor);
            auto right_user_descriptor = BLEDescriptor("2901", "Right Wheel Power");
            right_wheel_power_char.addDescriptor(right_user_descriptor);

            // Add Format descriptors
            uint8_t format_type = 0x04;            // unsigned 8-bit integer
            unsigned short unitless_unit = 0x2700; // unitless
            uint8_t pwr_format_[7] = {format_type,
                                      0,
                                      unitless_unit,
                                      (uint8_t)(unitless_unit >> 8U),
                                      0x01,
                                      0x00,
                                      0x00};
            auto left_format_descriptor = BLEDescriptor("2904", pwr_format_, 7);
            left_wheel_power_char.addDescriptor(left_format_descriptor);
            auto right_format_descriptor = BLEDescriptor("2904", pwr_format_, 7);
            right_wheel_power_char.addDescriptor(right_format_descriptor);

            left_wheel_power_char.writeValue((uint8_t)128);  // stop
            right_wheel_power_char.writeValue((uint8_t)128); // stop
        }
        bool poll(Servo &left_wheel, Servo &right_wheel)
        {
            // the value is from -127 to 127, where 0 is stop
            // negative is reverse, positive is forward
            // map to 0-180 for motor power control, where 90 is stop

            bool is_written = false;
            if (left_wheel_power_char.written())
            {
                int8_t power = map((int)left_wheel_power_char.value(), 0, 255, 0U, 180U);
                left_wheel.write(power);
                is_written = true;
            }
            if (right_wheel_power_char.written())
            {
                int8_t power = map((int)right_wheel_power_char.value(), 0, 255, 180U, 0U);
                right_wheel.write(power);
                is_written = true;
            }

            return is_written;
        }
    };

} // namespace ble

#endif