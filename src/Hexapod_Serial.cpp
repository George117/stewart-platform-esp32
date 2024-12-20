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

#include <Hexapod_Serial.h>

extern angle_t servo_angles[NB_SERVOS];
extern Hexapod_Servo hx_servo;

/**
 *
 */
Hexapod_Serial::Hexapod_Serial()
{
}

/**
 * Prepares the input buffer to receive a new message
 * and tells the serial connected device it is ready for more.
 */
void Hexapod_Serial::ready()
{
    sofar = 0;              // Clear input buffer.
    Serial.println(F(">")); // Signal ready to receive input.
}

/**
 *
 */
void Hexapod_Serial::setupSerial()
{
    Serial.begin(BAUD_RATE);
    ready();
}

/**
 * write a string followed by a double to the serial line.  Convenient for debugging.
 * @input code the string.
 * @input val the double.
 */
void Hexapod_Serial::output(const char code, double val)
{
    Serial.print(code);
    Serial.print(val);
    Serial.print(" ");
}

/**
 * Read the input buffer and find any recognized commands.
 * One G or M command per line.
 */
void Hexapod_Serial::processCommand()
{
    return;
}

/**
 * Look for character /code/ in the buffer and read the double that immediately follows it.
 * @return the value found.  If nothing is found, /val/ is returned.
 * @input code the character to look for.
 * @input val the return value if /code/ is not found.
 **/
double Hexapod_Serial::parseNumber(const char code, double val)
{
    char *ptr = buffer; // start at the beginning of buffer
    while ((long)ptr > 1 && (*ptr) && (long)ptr < (long)buffer + sofar)
    { // walk to the end
        if (*ptr == code)
        {                         // if you find code on your walk,
            return atof(ptr + 1); // convert the digits that follow into a double and return it
        }
        ptr = strchr(ptr, ' ') + 1; // take a step from here to the letter after the next space
    }
    return val; // end reached, nothing found, return default val.
}


/**
 *
 */
void Hexapod_Serial::serialRead()
{
    while (Serial.available() > 0)
    {
        char c = Serial.read();
        if (sofar < MAX_BUF - 1)
            buffer[sofar++] = c;
        if (c == '\n')
        {
            // End the buffer so string functions work right.
            buffer[sofar] = 0;

            x_axis = (buffer[0]<<8) | (buffer[1]);
            scaleAndConstrain(&x_axis, X_RAN);

            y_axis = (buffer[2]<<8) | (buffer[3]);
            scaleAndConstrain(&y_axis, Y_RAN);

            z_axis = (buffer[4]<<8) | (buffer[5]);
            scaleAndConstrain(&z_axis, Z_RAN);
           
            roll = (buffer[6]<<8) | (buffer[7]);
            scaleAndConstrain(&roll, ROLL_RAN);

            pitch = (buffer[8]<<8) | (buffer[9]);
            scaleAndConstrain(&pitch, PITCH_RAN);

            yaw = (buffer[10]<<8) | (buffer[11]);
            scaleAndConstrain(&yaw, YAW_RAN);

            logAxes();

            sofar = 0;

            uint8_t movOK = hx_servo.calcServoAngles({(double)x_axis, (double)y_axis, (double)z_axis, roll*DEG_TO_RAD, pitch*DEG_TO_RAD, yaw*DEG_TO_RAD}, servo_angles);
            hx_servo.updateServos(movOK);
        }
    }
}

/**
 *
 */
void Hexapod_Serial::printSplashScreen()
{
    Serial.println("\n\n##########################");
    Serial.print(F("PROJECT NAME:     "));
    Serial.println(PROJECT_NAME);
    Serial.print(F("PROJECT VERSION:  "));
    Serial.println(PROJECT_VERSION);
    Serial.print(F("FILE NAME:        "));
    Serial.println(__FILE__);
    Serial.print(F("PROJECT PATH:     "));
    Serial.println(PROJECT_PATH);
    Serial.print(F("COMPILATION DATE: "));
    Serial.println(COMPILATION_DATE);
    Serial.print(F("COMPILATION TIME: "));
    Serial.println(COMPILATION_TIME);
    Serial.print(F("PYTHON VERSION:   "));
    Serial.println(PYTHON_VERSION);
    Serial.print(F("PYTHON PATH:      "));
    Serial.println(PYTHON_PATH);
    Serial.print(F("HEXAPOD_CONFIG:   "));
    Serial.println(HEXAPOD_CONFIG);
    Serial.println("##########################\n\n");
}


void Hexapod_Serial::scaleAndConstrain(double *value, int range) 
{
    *value = (*value / 100) - 40;
    if (*value > range) {
        *value = range;
    }
    if (*value < -range) {
        *value = -range;
    }
}

void Hexapod_Serial::logAxes(void)
{
    sprintf(str, "X: %+04f", x_axis);
    Serial.println(str);
    memset(str, 0, sizeof(str));

    sprintf(str, "Y: %+04f", y_axis);
    Serial.println(str);
    memset(str, 0, sizeof(str));

    sprintf(str, "Z: %+04f", z_axis);
    Serial.println(str);
    memset(str, 0, sizeof(str));

    sprintf(str, "Roll: %+04f", roll);
    Serial.println(str);
    memset(str, 0, sizeof(str));

    sprintf(str, "Pitch: %+04f", pitch);
    Serial.println(str);
    memset(str, 0, sizeof(str));

    sprintf(str, "Yaw: %+04f", yaw);
    Serial.println(str);
    memset(str, 0, sizeof(str));

    Serial.println("");
    memset(buffer, 0, sizeof(buffer));
}