/**
 * S T E W A R T    P L A T F O R M    O N    E S P 3 2
 *
 * Copyright (C) 2019  Nicolas Jeanmonod, ouilogique.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <Hexapod_Servo.h>
#include <Hexapod_GPIO.h>

#define COUNT_OF(x) ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))

extern angle_t servo_angles[NB_SERVOS];
extern Hexapod_GPIO hx_gpio;

/**
 *
 */
Hexapod_Servo::Hexapod_Servo()
{
}

/**
 *
 */
void Hexapod_Servo::setupServo()
{
    Wire.begin();          // Wire must be started first
    Wire.setClock(100000); // Supported baud rates are 100kHz, 400kHz, and 1000kHz

    pwmController.resetDevices();      // Software resets all PCA9685 devices on Wire line
    pwmController.init(B000001);       // Address pins A5-A0 set to B000000
    pwmController.setPWMFrequency(50); // Default is 200Hz, supports 24Hz to 1526Hz

    int8_t movOK = home(servo_angles);
    this->updateServos(movOK);
    delay(500);
}

/**
 * Set servo values to the angles in servo_angles[].
 */
void Hexapod_Servo::updateServosIncremental(int8_t movOK, unsigned long safetyWait_ms)
{
#if false
    static bool first_run = true;

    static angle_t _servo_angles_prev[NB_SERVOS];
    if (first_run)
    {
        for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
        {
            _servo_angles_prev[sid].us = servo_angles[sid].us;
        }
        first_run = false;
        return;
    }

    static angle_t _servo_angles_target[NB_SERVOS];
    for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
    {
        _servo_angles_target[sid].us = servo_angles[sid].us;
    }
    const int increment = 5;
    int8_t moving = B00111111;
    Serial2.println("### MOVING START ");

    Serial2.println("### ANGLE PREV ");
    for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
    {
        Serial2.printf("%4d ", _servo_angles_prev[sid].us);
    }
    Serial2.println("\n### ANGLE TARGET ");
    for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
    {
        Serial2.printf("%4d ", _servo_angles_target[sid].us);
    }
    Serial2.println("");

    while (moving)
    {
        for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
        {
            double err = _servo_angles_target[sid].us - _servo_angles_prev[sid].us;

            if (err >= increment)
                _servo_angles_prev[sid].us += increment;
            else if (err <= -increment)
                _servo_angles_prev[sid].us -= increment;
            else
            {
                _servo_angles_prev[sid].us = _servo_angles_target[sid].us;
                bitClear(moving, sid);
            }

            Serial2.printf("%4d ", _servo_angles_prev[sid].us);
        }
        // Write to servos.
        for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
        {
            servos[sid].writeMicroseconds(_servo_angles_prev[sid].us);
        }

        delay(5);

        Serial2.print("\n");
    }

    Serial2.println(" MOVING DONE ###");
#endif
}

/**
 * Set servo values to the angles in servo_angles[].
 */
void Hexapod_Servo::updateServos(int8_t movOK, unsigned long safetyWait_ms)
{
    // Statistics of errors.
    static double nbMov = 0;
    static double nbMovNOK = 0;
    static double NOKrate = 0;
    nbMov++;

    if (movOK == 0)
    {
        // Do not update too quickly. This helps a little to prevent
        // the servos from going crazy. safetyWait_ms must be set to 0
        // when going on a smooth trajectory with a lot of points.
        static unsigned long T1;
        while ((millis() - T1) < safetyWait_ms)
        {
        }
        T1 = millis();

        // Write to servos.
        static uint16_t pwms[NB_SERVOS];
        for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
        {
            pwms[sid] = servo_angles[sid].pwm_us;
        }
        pwmController.setChannelsPWM(0, NB_SERVOS, pwms);

#if SEND_PWM_US_TO_SERIAL
        // Write servo angles to Serial for debug.
        for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
        {
            Serial.printf("%4d ", servo_angles[sid].pwm_us);
        }
        Serial2.println("");
#endif
    }
    else
    {
        // Error handling.
        hx_gpio.setBuiltInLED();
        nbMovNOK++;
        NOKrate = (nbMovNOK / nbMov) * (double)100.0;
        Serial2.printf("%10lu", millis());
        Serial2.printf(" | BAD MOVE | movOK = %d", movOK);
        Serial2.printf(" | NB MOV = %10.0f", nbMov);
        Serial2.printf(" | NOK rate = %4.1f %%", NOKrate);
        Serial2.print("\n");
    }

    // Switch off LED.
    hx_gpio.clearBuiltInLEDDelayed(20);
}

/**
 *
 */
void Hexapod_Servo::printServoAngles()
{
    Serial2.print("\nSERVO COORD        = ");
    Serial2.print(getHX_X());
    Serial2.print(" ");
    Serial2.print(getHX_Y());
    Serial2.print(" ");
    Serial2.print(getHX_Z());
    Serial2.print(" ");
    Serial2.print(getHX_A());
    Serial2.print(" ");
    Serial2.print(getHX_B());
    Serial2.print(" ");
    Serial2.print(getHX_C());

    Serial2.print("\nSERVO_ANGLES (rad) = ");
    for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
    {
        Serial2.print(servo_angles[sid].deg);
        Serial2.print(" ");
    }

    Serial2.print("\nSERVO_ANGLES (us) = ");
    for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
    {
        Serial2.print(servo_angles[sid].us);
        Serial2.print(" ");
    }

    Serial2.print("\nSERVO_ANGLES (debug) = ");
    for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
    {
        Serial2.print(servo_angles[sid].debug);
        Serial2.print(" ");
    }
    Serial2.print("\n");
}

/**
 *
 */
void Hexapod_Servo::printJointAndServoAxisCoord()
{
    Serial2.println("P_COORDS");
    for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
    {
        Serial2.print(P_COORDS[sid][0]);
        Serial2.print("  ");
        Serial2.println(P_COORDS[sid][1]);
    }

    Serial2.println("\nB_COORDS");
    for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
    {
        Serial2.print(B_COORDS[sid][0]);
        Serial2.print("  ");
        Serial2.println(B_COORDS[sid][1]);
    }
}
