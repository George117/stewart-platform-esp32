#include <MPU6500_WE.h>
#include <Wire.h>

#define MPU6500_ADDR 0x69

class Hexapod_IMU
{
private:
    MPU6500_WE myMPU6500 = MPU6500_WE(MPU6500_ADDR);
    
public:
    void setupIMU(void);
    void printIMU(void);
    void sendSerialIMU(void);

    xyzFloat gValue;
    xyzFloat gyr;
    float resultantG;
};