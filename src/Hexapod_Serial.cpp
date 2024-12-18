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
            y_axis = (buffer[2]<<8) | (buffer[3]);
            z_axis = (buffer[4]<<8) | (buffer[5]);
            roll = (buffer[6]<<8) | (buffer[7]);
            pitch = (buffer[8]<<8) | (buffer[9]);
            yaw = (buffer[10]<<8) | (buffer[11]);

            x_axis = (x_axis/100)-40;
            if(x_axis > X_RAN){
                x_axis = X_RAN;
            }
            if(x_axis<-X_RAN){
                x_axis = -X_RAN;
            }

            y_axis = (y_axis/100)-40;
            if(y_axis > Y_RAN){
                y_axis = Y_RAN;
            }
            if(y_axis<-Y_RAN){
                y_axis = -Y_RAN;
            }


            z_axis = (z_axis/100)-40;
            if(z_axis > Z_RAN){
                z_axis = Z_RAN;
            }
            if(z_axis<-Z_RAN){
                z_axis = -Z_RAN;
            }

            roll = (roll/100)-40;
            if(roll > ROLL_RAN){
                roll = ROLL_RAN;
            }
            if(roll<-ROLL_RAN){
                roll = -ROLL_RAN;
            }

            pitch = (pitch/100)-40;
            if(pitch > PITCH_RAN){
                pitch = PITCH_RAN;
            }
            if(pitch<-PITCH_RAN){
                pitch = -PITCH_RAN;
            }


            yaw = (yaw/100)-40;
            if(yaw > YAW_RAN){
                yaw = YAW_RAN;
            }
            if(yaw<-YAW_RAN){
                yaw = -YAW_RAN;
            }

             char str[20];
            // sprintf(str, "%04d", x_axis);
            // Serial.println(str);

            // sprintf(str, "%04d", y_axis);
            // Serial.println(str);

            // sprintf(str, "%04d", z_axis);
            // Serial.println(str);

            sprintf(str, "%04d", roll);
            Serial.println(str);

            // sprintf(str, "%04d", pitch);
            // Serial.println(str);

            // sprintf(str, "%04d", yaw);
            // Serial.println(str);

            memset(buffer, '0', sizeof(buffer));
            sofar = 0;

            uint8_t movOK = hx_servo.calcServoAngles({x_axis, y_axis, z_axis, roll*0.017453, pitch*0.017453, yaw*0.017453}, servo_angles);
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
