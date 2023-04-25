#include "MatrixKeypad.h"
#include <stdint.h>
#include <math.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
MPU6050 mpu6050(Wire);
#define I2C_SLAVE_ADDR 0x04 // 4 in hexadecimal
#include <ctype.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(14, 27, 26, 25, 33, 32);
int left_motorspeed, right_motorspeed, servoangle,hash=0;
float distance =0,olddistance=0,deltaangle=0,oldangle=0;
long enc1_count,enc2_count,enc,enc1,oldenc;
float distance1,distance2,angle,angle1;
const uint8_t rown = 4; //4 rows
const uint8_t coln = 3; //3 columns
uint8_t rowPins[rown] = {15, 2, 0,4}; //frist row is connect to pin 10, second to 9...
uint8_t colPins[coln] = {16, 17, 5}; //frist column is connect to pin 6, second to 5...
char keymap[rown][coln] = 
  {{'1','2','3'}, //key of the frist row frist column is '1', frist row second column column is '2'
   {'4','5','6'}, //key of the second row frist column is '4', second row second column column is '5'
   {'7','8','9'},
   {'*','0','#'}};
MatrixKeypad_t *keypad; //keypad is the variable that you will need to pass to the other functions
char key,CmdLimit,Hope,tempDir,tempQuantFB,tempQuantLR,BreakLoop;
int count,countD,flag=0;
int DirectionEntered[25],DistanceEntered[25];
int ArrayCount,s;
int z = 1;
void setup() {
  Serial.begin(9600);
  keypad = MatrixKeypad_create((char*)keymap /* don't forget to do this cast */, rowPins, colPins, rown, coln); //creates the keypad object
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(1);
  lcd.begin(16, 2);
  lcd.print("Hello World");
  countD=0;
}

void loop(){

      for (z=1;z<=sizeof(DirectionEntered);z++)
      {
        DirectionEntered[z]=UserInputDirection();        
        DistanceEntered[z]=UserInputDistance();
        ArrayCount=ArrayCount+1;
        BreakLoop=EndOfArray();
        if (BreakLoop=='#')
          break; 
      }

      for (s=0;s<=ArrayCount;s++)
      {
        angle1=0;
        enc1=0;
        lcd.clear();
        int QuantFB=DistanceEntered[s]- '0';   
        if (DirectionEntered[s]=='2')
        {
          left_motorspeed=175;
          right_motorspeed=175;
          servoangle=90;
          lcd.print("Driving");
          transmit_to_arduino(left_motorspeed,right_motorspeed,servoangle);
          oldenc=encoder();
          while (abs(enc1)<(3*(round((10*QuantFB)/3))))
          {
            enc=encoder();
            enc1=enc-oldenc;
            Serial.println(enc1);
          }
        }
        else if (DirectionEntered[s]=='8')
        {
          left_motorspeed=-175;
          right_motorspeed=-175;
          servoangle=90;
          lcd.print("Driving");
          transmit_to_arduino(left_motorspeed,right_motorspeed,servoangle);
          oldenc=encoder();
          while (abs(enc1)<(3*(round((10*QuantFB)/3))))
          {
            enc=encoder();
            enc1=enc-oldenc;
          }
        }
        else if (DirectionEntered[s]=='4')
        {
          left_motorspeed=-150;
          right_motorspeed=150;
          servoangle=75;
          lcd.print("Turning");
          transmit_to_arduino(left_motorspeed,right_motorspeed,servoangle);
          while ((abs(angle1)<(QuantFB*90)))
          {
            angle=accelerometer();
            angle1=angle-oldangle;
          }
          oldangle=angle;
        }
        else if (DirectionEntered[s]=='6')
        {
          left_motorspeed=150;
          right_motorspeed=-150;
          servoangle=120;
          lcd.print("Turning");
          transmit_to_arduino(left_motorspeed,right_motorspeed,servoangle);
          while (abs(angle1)<(QuantFB*90))
          {
            angle=accelerometer();
            angle1=angle-oldangle;
          }
          oldangle=angle;
        }
        transmit_to_arduino(0,0,90);
        delay(1000);
      }
      Serial.println("Completed");    
}

float accelerometer()
{
  mpu6050.update();
  return (mpu6050.getAngleZ());
}

void transmit_to_arduino(int left_motorspeed, int right_motorspeed, int servoangle)
{
  Wire.beginTransmission(I2C_SLAVE_ADDR); // transmit to device #4
  Wire.write((byte)((left_motorspeed & 0x0000FF00) >> 8));    // first byte of x, containing bits 16 to 9
  Wire.write((byte)(left_motorspeed & 0x000000FF));           // second byte of x, containing the 8 LSB - bits 8 to 1
  Wire.write((byte)((right_motorspeed & 0x0000FF00) >> 8));   // first byte of y, containing bits 16 to 9
  Wire.write((byte)(right_motorspeed & 0x000000FF));          // second byte of y, containing the 8 LSB - bits 8 to 1
  Wire.write((byte)((servoangle & 0x0000FF00) >> 8));    // first byte of x, containing bits 16 to 9
  Wire.write((byte)(servoangle & 0x000000FF));
  Wire.endTransmission();   // stop transmitting
}

float encoder()
{
  Wire.requestFrom(I2C_SLAVE_ADDR, 4); // request 2 bytes from slave device
  uint8_t enc1_count16_9 = Wire.read();  // receive bits 16 to 9 of x (one byte)
  uint8_t enc1_count8_1 = Wire.read();   // receive bits 8 to 1 of x (one byte)
  uint8_t enc2_count16_9 = Wire.read();  // receive bits 16 to 9 of x (one byte)
  uint8_t enc2_count8_1 = Wire.read();   // receive bits 8 to 1 of x (one byte)
  enc1_count = (enc1_count16_9 << 8) | enc1_count8_1; // combine the two bytes into a 16 bit number
  enc2_count = (enc2_count16_9 << 8) | enc2_count8_1; // combine the two bytes into a 16 bit number
  return (enc1_count);
}

char UserInputDirection()
{
do
        {
          lcd.clear();
          lcd.setCursor(0,1);
          lcd.print("2=F 4=L 6=R 8=B");
          tempDir = MatrixKeypad_waitForKey(keypad); //blocks the program until the user presses a key 
          while ((tempDir!='2')&&(tempDir!='4')&&(tempDir!='6')&&(tempDir!='8'))
          {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Enter 2,4,6,8");
              lcd.setCursor(0,1);
              lcd.print("2=F 4=L 6=R 8=B");
              tempDir = MatrixKeypad_waitForKey(keypad); //blocks the program until the user presses a key 
          }
          do
          {  
              lcd.clear();
              lcd.setCursor(0,1);
              lcd.print("#=Ent *=clr");
              key = MatrixKeypad_waitForKey(keypad); //blocks the program until the user presses a key
              if (key=='#')
                  hash=0;
              else if (key=='*')
              {
                  hash=1;
                  lcd.clear();
                  lcd.setCursor(0,1);
                  lcd.print("Cleared");
              }
              else
                  hash=5;
          }
          while (hash>4);            
        }
        while (hash==1);
        return tempDir;  
}
char UserInputDistance()
{
        do
        {
          lcd.clear();
          lcd.setCursor(0,1);
          lcd.print("1=10cm 1=90deg");
          tempQuantFB= MatrixKeypad_waitForKey(keypad); //blocks the program until the user presses a key 
          while ((tempQuantFB=='#')|(tempQuantFB=='*'))
          {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Enter a num");
              lcd.setCursor(0,1);
              lcd.print("1=10cm 1=90deg");
              tempQuantFB = MatrixKeypad_waitForKey(keypad); //blocks the program until the user presses a key 
          }
          if ((tempDir=='4')|(tempDir=='6'))
          {
            while ((tempQuantFB!='1')&&(tempQuantFB!='2'))
            {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Only 1 or 2");
              lcd.setCursor(0,1);
              lcd.print("1=90 2=180");
              tempQuantFB = MatrixKeypad_waitForKey(keypad); //blocks the program until the user presses a key 
            }
          }
          do
          {  
              lcd.clear();
              lcd.setCursor(0,1);
              lcd.print("#=Ent *=clr");
              key = MatrixKeypad_waitForKey(keypad); //blocks the program until the user presses a key
              if (key=='#')
                  hash=0;
              else if (key=='*')
              {
                  hash=1;
                  lcd.clear();
                  lcd.setCursor(0,1);
                  lcd.print("Cleared");
              }
              else
                  hash=5;
          }
          while (hash>4);            
        }
        while (hash==1);
        return tempQuantFB;
}

char EndOfArray()
{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("More commands?");
        lcd.setCursor(0,1);
        lcd.print("#=No *=Yes");
        CmdLimit = MatrixKeypad_waitForKey(keypad);
        while ((CmdLimit!='#') && (CmdLimit!='*'))
        {
          lcd.setCursor(0,1);
          lcd.print("Only #=Yes *=No");
          CmdLimit = MatrixKeypad_waitForKey(keypad);
        }
        return CmdLimit;
}