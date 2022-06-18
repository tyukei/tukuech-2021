#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Wire.h>

// MPU-6050のアドレス、レジスタ設定値
#define MPU6050_WHO_AM_I     0x75  // Read Only
#define MPU6050_PWR_MGMT_1   0x6B  // Read and Write
#define MPU_ADDRESS  0x68
#define SENSOR_DURING_MS 4000

#define THRESHOLD_SHALE 5

#define DIN_PIN 13 // スイッチのピン

// DPPlayer
// https://www.wwwmaplesyrup-cs6.work/entry/2019/02/10/152415
SoftwareSerial mySoftwareSerial1(4, 5);
SoftwareSerial mySoftwareSerial2(6, 7);
SoftwareSerial mySoftwareSerial3(8, 9);
SoftwareSerial mySoftwareSerial4(10, 11);

DFRobotDFPlayerMini myDFPlayer1; 
DFRobotDFPlayerMini myDFPlayer2;
DFRobotDFPlayerMini myDFPlayer3; 
DFRobotDFPlayerMini myDFPlayer4;

unsigned long prev_time = 0;
unsigned long delta_time = 0;

int cnt_acc = 0;
int cnt_switch = 0;
int seq = 0;

void setup()
{
  // PCとの通信を開始
  Serial.begin(115200); //115200bps
  Serial.println("setup start");

  pinMode( DIN_PIN, INPUT_PULLUP );

  AccSensor_setup();
  
  DFPlayer_setup(&myDFPlayer1, &mySoftwareSerial1, 28);
  DFPlayer_setup(&myDFPlayer2, &mySoftwareSerial2, 28);
  DFPlayer_setup(&myDFPlayer3, &mySoftwareSerial3, 28);
  DFPlayer_setup(&myDFPlayer4, &mySoftwareSerial4, 28);

  prev_time = millis();

  Serial.println("setup end");
}


void loop()
{
  delta_time = millis() - prev_time;  
  if(delta_time < 0) return; // 約50日後にマイナスになる
  prev_time = millis();
 

  if (cnt_acc < 0){
    // 加速度判定   
    if (AccSensor_isShake()) {
        // 踏まれた時
        DFPlayer_play();
        cnt_acc = SENSOR_DURING_MS;         
    }
  }else{
    cnt_acc -= delta_time;
  }

  if (cnt_switch < 0){
    if(digitalRead( DIN_PIN )==LOW){
      DFPlayer_play();
      cnt_switch = SENSOR_DURING_MS;  
    }
  }else{
    cnt_switch -= delta_time;
  }
  

  DFPlayer_checkError(myDFPlayer1);
  DFPlayer_checkError(myDFPlayer2);
  DFPlayer_checkError(myDFPlayer3);
  DFPlayer_checkError(myDFPlayer4);
}

void DFPlayer_play(){
  // 乱数生成
  int rands = random(100);
  
  switch(seq){
    case 0:
      myDFPlayer1.play(rands%3+1);
      break;
    case 1:
      myDFPlayer2.play(rands%3+1);
      break;
    case 2:
      myDFPlayer3.play(rands%3+1);
      break;
    case 3:
      myDFPlayer4.play(rands%3+1);
      break;
    default:
      break;
  }
  seq = (seq+1)%4;
}


void DFPlayer_setup(DFRobotDFPlayerMini* df, SoftwareSerial* ss, int volume) {
  ss->begin(9600);
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)!"));
  if (!df->begin(*ss)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:1"));
    Serial.println(F("1.Please recheck the connection!1"));
    Serial.println(F("2.Please insert the SD card!1"));
    while (true) {
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online1."));
  df->volume(volume);  //Set volume value. From 0 to 30
}


void DFPlayer_checkError(DFRobotDFPlayerMini df) {
  if (df.available()) {
    DFPlayer_printDetail(df.readType(), df.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}

void DFPlayer_printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      //Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

// デバイス初期化時に実行される
void AccSensor_setup() {
  Wire.begin();
 
  // 初回の読み出し
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(MPU6050_WHO_AM_I);  //MPU6050_PWR_MGMT_1
  Wire.write(0x00);
  Wire.endTransmission();

  // 動作モードの読み出し
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(MPU6050_PWR_MGMT_1);  //MPU6050_PWR_MGMT_1レジスタの設定
  Wire.write(0x00);
  Wire.endTransmission();
}


bool AccSensor_isShake() {
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 14, true);
  while (Wire.available() < 14);
  int16_t axRaw, ayRaw, azRaw, gxRaw, gyRaw, gzRaw, Temperature;

  axRaw = Wire.read() << 8 | Wire.read();
  ayRaw = Wire.read() << 8 | Wire.read();
  azRaw = Wire.read() << 8 | Wire.read();
  Temperature = Wire.read() << 8 | Wire.read();
  gxRaw = Wire.read() << 8 | Wire.read();
  gyRaw = Wire.read() << 8 | Wire.read();
  gzRaw = Wire.read() << 8 | Wire.read();

  // 加速度値を分解能で割って加速度(G)に変換する
  float acc_x = axRaw / 16384.0;  //FS_SEL_0 16,384 LSB / g
  float acc_y = ayRaw / 16384.0;
  float acc_z = azRaw / 16384.0;

  // 角速度値を分解能で割って角速度(degrees per sec)に変換する
  float gyro_x = gxRaw / 131.0;//FS_SEL_0 131 LSB / (°/s)
  float gyro_y = gyRaw / 131.0;
  float gyro_z = gzRaw / 131.0;

  Serial.println(acc_x * acc_x + acc_y * acc_y + acc_z * acc_z); 

  return acc_x * acc_x + acc_y * acc_y + acc_z * acc_z > THRESHOLD_SHALE;
}
