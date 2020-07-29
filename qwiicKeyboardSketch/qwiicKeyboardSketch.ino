/*
  Qwiic Capacitive SoundBoard Project
  By: Marcus Stevenson
  SparkFun Electronics
  Date: January 14th, 2020
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).


  Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/15165

  For hardware hookup instructions, read this blog post:
*/

#include <Wire.h> //Needed for I2C

#include <SparkFun_CAP1203_Registers.h>
#include <SparkFun_CAP1203_Types.h>

#include "SparkFun_Qwiic_MP3_Trigger_Arduino_Library.h" //http://librarymanager/All#SparkFun_MP3_Trigger
#include "SparkFun_Qwiic_Twist_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_Twist

TWIST twist;
MP3TRIGGER mp3;

CAP1203 sensor010203;  //cap slider connected to mux port 4, keys 1, 2, 3 (left pad, middle pad, right pad)
CAP1203 sensor040506;  //cap slider connected to mux port 5, keys 4, 5, 6 (left pad, middle pad, right pad)
CAP1203 sensor071112;  //cap slider connected to mux port 5, keys 7, 11, 12 (left pad, middle pad, right pad)
CAP1203 sensor100809;  //cap slider connected to mux port 5, keys 10, 9, 8 (left pad, middle pad, right pad)
   
#define MUX_ADDR 0x70 //7-bit unshifted default I2C Address

//Enables a specific port number
boolean enableMuxPort(byte portNumber)
{
  if(portNumber > 7) portNumber = 7;

  //Read the current mux settings
  Wire.requestFrom(MUX_ADDR, 1);
  if(!Wire.available()) return(false); //Error
  byte settings = Wire.read();

  //Set the wanted bit to enable the port
  settings |= (1 << portNumber);

  Wire.beginTransmission(MUX_ADDR);
  Wire.write(settings);
  Wire.endTransmission();

  return(true);
}

//Disables a specific port number
boolean disableMuxPort(byte portNumber)
{
  if(portNumber > 7) portNumber = 7;

  //Read the current mux settings
  Wire.requestFrom(MUX_ADDR, 1);
  if(!Wire.available()) return(false); //Error
  byte settings = Wire.read();

  //Clear the wanted bit to disable the port
  settings &= ~(1 << portNumber);

  Wire.beginTransmission(MUX_ADDR);
  Wire.write(settings);
  Wire.endTransmission();

  return(true);
}


void setup()
{
  Serial.begin(9600);

  Wire.begin();

  //Check to see if Qwiic MP3 is present on the bus
  if (mp3.begin() == false)
  {
    Serial.println("Qwiic MP3 failed to respond. Please check wiring and possibly the I2C address. Freezing...");
    while (1);
  }

  mp3.setVolume(5); //Volume can be 0 (off) to 31 (max)
  
  enableMuxPort(3); //initialize qwiic twist
  twist.begin();
  twist.setCount(0);
  twist.setLimit(35);
  disableMuxPort(3);

  
  enableMuxPort(4);  //initialize cap touch sensors
  sensor010203.begin();
  disableMuxPort(4);
  
  enableMuxPort(5);
  sensor040506.begin();
  disableMuxPort(5);
  
  enableMuxPort(6);
  sensor071112.begin();
  disableMuxPort(6);
  
  enableMuxPort(7);
  sensor100809.begin();
  disableMuxPort(7);

}

void loop()
{
  int x = 0; //variable used to set track number w/qwiic twist knob
  
  enableMuxPort(3);
  
  if (twist.isPressed()){// stop the current playing mp3 when knob pressed
    mp3.stop();
  }
  
  int twistCount = twist.getCount();
  
  if(0<= twistCount && twistCount <5){//---------------> red = tracks 1-12
    twist.setColor(255, 0, 0); //red
    x = 0;
  } else if (5<= twistCount && twistCount <10){//------> green = tracks 13-24
    twist.setColor(128, 255, 0); //green
    x = 12;
  } else if (10<= twistCount && twistCount <15){//-----> violet = tracks 25-36
    twist.setColor(128, 0, 255); //violet
    x = 24;
  } else if (15<= twistCount && twistCount <20){//-----> yellow = tracks 37-48
    twist.setColor(255, 255, 0); //yellow
    x = 36;
  } else if (20<= twistCount && twistCount <25){//-----> pink = tracks 49-60
    twist.setColor(128, 0, 255); //pink
    x = 48;
  } else if (25<= twistCount && twistCount <30){//-----> blue = tracks 61-72
    twist.setColor(0, 255, 255); //blue
    x = 60;
  } else if (30<= twistCount && twistCount <35){//-----> orange = tracks 73-84
    twist.setColor(255, 100, 0); //orange
    x = 72;
  }
  disableMuxPort(3);

  
  enableMuxPort(4);
  if (sensor010203.isLeftTouched() == true) { //----------------> Key 1
    Serial.println("Touch1 left");
    mp3.playFile((1+x));
  } else if (sensor010203.isMiddleTouched() == true){//---------> Key 1
    Serial.println("Touch1 center");
    mp3.playFile((2+x));
  } else if (sensor010203.isRightTouched() == true){//----------> Key 3
    Serial.println("Touch1 right");
    mp3.playFile((3+x));
  }
  disableMuxPort(4);
  enableMuxPort(5);
  //bool reading2 = sensor010203.isTouched();
  if (sensor040506.isLeftTouched() == true) { //----------------> Key 4
    Serial.println("Touch2 left");
    mp3.playFile((4+x));
  } else if (sensor040506.isMiddleTouched() == true){//---------> Key 5
    Serial.println("Touch2 center");
    mp3.playFile((5+x));
  } else if (sensor040506.isRightTouched() == true){//----------> Key 6
    Serial.println("Touch2 right");
    mp3.playFile((6+x));
  }
  disableMuxPort(5);
  enableMuxPort(6);
  //bool reading3 = sensor010203.isTouched();
  if (sensor071112.isLeftTouched() == true) { //----------------> Key 7
    Serial.println("Touch3 left");
    mp3.playFile((7+x));
  } else if (sensor071112.isMiddleTouched() == true){//---------> Key 12
    Serial.println("Touch3 center");
    mp3.playFile((12+x));
  } else if (sensor071112.isRightTouched() == true){//----------> Key 11
    Serial.println("Touch3 right");
    mp3.playFile((11+x));
  }
  disableMuxPort(6);
  enableMuxPort(7);
  //bool reading4 = sensor010203.isTouched();
  if (sensor100809.isLeftTouched() == true) { //----------------> Key 10
    Serial.println("Touch1 left");
    mp3.playFile((10+x));
  } else if (sensor100809.isMiddleTouched() == true){//---------> Key 9
    Serial.println("Touch1 center");
    mp3.playFile((9+x));
  } else if (sensor100809.isRightTouched() == true){//----------> Key 8
    Serial.println("Touch1 right");
    mp3.playFile((8+x));
  }
  disableMuxPort(7);
  delay(0.2);//------------------------------------------------> pseudo debounce
}
