#include <Hexapod_IMU.h>
#include <Hexapod_Serial.h>



void Hexapod_IMU::setupIMU(void)
{
    if(!myMPU6500.init()){
        Serial2.println("MPU6500 does not respond");
    }
    else{
        Serial2.println("MPU6500 is connected");
    }
    Serial2.println("Position you MPU6500 flat and don't move it - calibrating...");

    delay(1000);

    myMPU6500.autoOffsets();

    Serial2.println("Done!");

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
  gValue = myMPU6500.getGValues();
  gyr = myMPU6500.getGyrValues();
  resultantG = myMPU6500.getResultantG(gValue);

  static unsigned int counter = 0;

  counter++;

  if(counter%10==1){
    Serial2.println("Acceleration in g (x,y,z):");
    Serial2.print(gValue.x);
    Serial2.print("   ");
    Serial2.print(gValue.y);
    Serial2.print("   ");
    Serial2.println(gValue.z);
    Serial2.print("Resultant g: ");
    Serial2.println(resultantG);

    Serial2.println("Gyroscope data in degrees/s: ");
    Serial2.print(gyr.x);
    Serial2.print("   ");
    Serial2.print(gyr.y);
    Serial2.print("   ");
    Serial2.println(gyr.z);
    Serial2.println("\n");
  }
}

void Hexapod_IMU::sendSerialIMU(void)
{
  unsigned int x_axis, y_axis, z_axis, roll, pitch, yaw = 0;

  gValue = myMPU6500.getGValues();
  gyr = myMPU6500.getGyrValues();

  x_axis = (round((gValue.x + 100) * 100));
  y_axis = (round((gValue.y + 100) * 100));
  z_axis = (round((gValue.z + 100) * 100));

  roll = (round((gyr.x + 1000) * 10));
  pitch = (round((gyr.y + 1000) * 10));
  yaw = (round((gyr.z + 1000) * 10));


  Serial.write((x_axis & 0xff00) >> 8);
  Serial.write(x_axis & 0xff);

  Serial.write((y_axis & 0xff00) >> 8);
  Serial.write(y_axis & 0xff);

  Serial.write((z_axis & 0xff00) >> 8);
  Serial.write(z_axis & 0xff);

  Serial.write((roll & 0xff00) >> 8);
  Serial.write(roll & 0xff);

  Serial.write((pitch & 0xff00) >> 8);
  Serial.write(pitch & 0xff);

  Serial.write((yaw & 0xff00) >> 8);
  Serial.write(yaw & 0xff);

  Serial.write("\n");

  Serial2.print("X: ");
  Serial2.println(gValue.x);

  Serial2.print("Y: ");
  Serial2.println(gValue.y);

  Serial2.print("Z: ");
  Serial2.println(gValue.z);

  Serial2.print("R: ");
  Serial2.println(gyr.x);

  Serial2.print("P: ");
  Serial2.println(gyr.y);

  Serial2.print("Y: ");
  Serial2.println(gyr.z);
  
  Serial2.println("\n");

  delay(50);
  return;
}