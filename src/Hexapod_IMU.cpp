#include <Hexapod_IMU.h>




void Hexapod_IMU::setupIMU(void)
{
    if(!myMPU6500.init()){
        Serial.println("MPU6500 does not respond");
    }
    else{
        Serial.println("MPU6500 is connected");
    }
    Serial.println("Position you MPU6500 flat and don't move it - calibrating...");

    delay(1000);

    myMPU6500.autoOffsets();

    Serial.println("Done!");

    myMPU6500.enableGyrDLPF();
    myMPU6500.setGyrDLPF(MPU6500_DLPF_6);
    myMPU6500.setSampleRateDivider(5);
    myMPU6500.setGyrRange(MPU6500_GYRO_RANGE_250);
    myMPU6500.setAccRange(MPU6500_ACC_RANGE_2G);
    myMPU6500.enableAccDLPF(true);
    myMPU6500.setAccDLPF(MPU6500_DLPF_0);

    delay(200);
}

void Hexapod_IMU::printIMU(void)
{
  xyzFloat gValue = myMPU6500.getGValues();
  xyzFloat gyr = myMPU6500.getGyrValues();
  float resultantG = myMPU6500.getResultantG(gValue);

  static unsigned int counter = 0;

  counter++;

  if(counter%10==1){
    Serial.println("Acceleration in g (x,y,z):");
    Serial.print(gValue.x);
    Serial.print("   ");
    Serial.print(gValue.y);
    Serial.print("   ");
    Serial.println(gValue.z);
    Serial.print("Resultant g: ");
    Serial.println(resultantG);

    Serial.println("Gyroscope data in degrees/s: ");
    Serial.print(gyr.x);
    Serial.print("   ");
    Serial.print(gyr.y);
    Serial.print("   ");
    Serial.println(gyr.z);
    Serial.println("\n");
  }
}

void Hexapod_IMU::sendSerialIMU(void)
{
    return;
}