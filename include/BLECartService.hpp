#ifndef BLE_CART_SERVICE_HPP
#define BLE_CART_SERVICE_HPP

#define BLE_GATT_CPF_UNIT_UNITLESS 0x2700
#define BLE_GATT_CPF_FORMAT_UINT8 0x04
#define BLE_GATT_CPF_FORMAT_UTF8S 0x19

#include <ArduinoBLE.h>
#include <ESP32Servo.h>
#include <M5Unified.h>
namespace ble
{
    class BLECartService : public BLEService
    {
    private:
        const uint8_t pwr_format_[7] = {BLE_GATT_CPF_FORMAT_UINT8,
                                        0b0,
                                        (uint8_t)BLE_GATT_CPF_UNIT_UNITLESS,
                                        (uint8_t)(BLE_GATT_CPF_UNIT_UNITLESS >> 8),
                                        0x01,
                                        0x00,
                                        0x00};

        const uint8_t cmd_format_[7] = {BLE_GATT_CPF_FORMAT_UTF8S,
                                        0b0,
                                        (uint8_t)BLE_GATT_CPF_UNIT_UNITLESS,
                                        (uint8_t)(BLE_GATT_CPF_UNIT_UNITLESS >> 8),
                                        0x01,
                                        0x00,
                                        0x00};

    public:
        BLEByteCharacteristic left_wheel_power_char;
        BLEByteCharacteristic right_wheel_power_char;

        BLECharCharacteristic command_char;

        BLECartService()
            : BLEService("ed276ce0-99fe-41a3-9a59-886ca2398c7b"),
              command_char("ed276ce1-99fe-41a3-9a59-886ca2398c7b", BLERead | BLEWrite),
              left_wheel_power_char("ed276ce2-99fe-41a3-9a59-886ca2398c7b", BLERead | BLEWrite),
              right_wheel_power_char("ed276ce3-99fe-41a3-9a59-886ca2398c7b", BLERead | BLEWrite)

        {
            // NOTE: the following Android app sends a char value to the first characteristic.
            // BLE Controller – Arduino ESP32: https://play.google.com/store/apps/details?id=com.circuitmagic.iot
            addCharacteristic(command_char);
            addCharacteristic(left_wheel_power_char);
            addCharacteristic(right_wheel_power_char);

            // Add user description descriptors
            auto left_user_descriptor = BLEDescriptor("2901", "Left Wheel Power");
            left_wheel_power_char.addDescriptor(left_user_descriptor);
            auto right_user_descriptor = BLEDescriptor("2901", "Right Wheel Power");
            right_wheel_power_char.addDescriptor(right_user_descriptor);
            // F/B/R/L and A/B/X/Y like game pad buttons
            auto command_user_descriptor = BLEDescriptor("2901", "Command");
            command_char.addDescriptor(command_user_descriptor);

            // Add Format descriptors
            auto left_format_descriptor = BLEDescriptor("2904", pwr_format_, 7);
            left_wheel_power_char.addDescriptor(left_format_descriptor);
            auto right_format_descriptor = BLEDescriptor("2904", pwr_format_, 7);
            right_wheel_power_char.addDescriptor(right_format_descriptor);
            auto command_format_descriptor = BLEDescriptor("2904", cmd_format_, 7);
            command_char.addDescriptor(command_format_descriptor);

            left_wheel_power_char.writeValue((uint8_t)128);  // stop
            right_wheel_power_char.writeValue((uint8_t)128); // stop
        }

        int poll(Servo &left_wheel, Servo &right_wheel)
        {
            // the value is from -127 to 127, where 0 is stop
            // negative is reverse, positive is forward
            // map to 0-180 for motor power control, where 90 is stop

            int is_written = 0;
            if (command_char.written())
            {
                char cmd = command_char.value();
                switch (cmd)
                {
                case 'F': // forward
                case 'f':
                    left_wheel.write(90 + 20);
                    right_wheel.write(90 - 20);
                    break;
                case 'B': // backward
                case 'b':
                    left_wheel.write(90 - 20);
                    right_wheel.write(90 + 20);
                    break;
                case 'L': // left, ccw turn
                case 'l':
                    left_wheel.write(90 - 20);
                    right_wheel.write(90 - 20);
                    break;
                case 'R': // right
                case 'r':
                    left_wheel.write(90 + 20);
                    right_wheel.write(90 + 20);
                    break;
                case 'S': // stop
                case 's':
                    left_wheel.write(90);
                    right_wheel.write(90);
                    break;
                }
                is_written = 1;
            }
            if (left_wheel_power_char.written())
            {
                int8_t power = map(left_wheel_power_char.value(), 0, 255, 0U, 180U);
                left_wheel.write(power);
                is_written = 2;
            }
            if (right_wheel_power_char.written())
            {
                int8_t power = map(right_wheel_power_char.value(), 0, 255, 180U, 0U);
                right_wheel.write(power);
                is_written = 3;
            }

            return is_written;
        }
    };

} // namespace ble

#endif