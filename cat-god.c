#define CLAPIN  2             // pin must be interrupt-capable
#define CLAP_DELAY   1000      // max gap between claps to trigger
#define IN1 9 //9番ピンを出力ピンIN1と定義してます。
#define IN2 8 //8番ピンを出力ピンIN1と定義してます。
#define IN3 7
#define IN4 6

volatile boolean clap = false;                // clap detected state
boolean led_state = false;                    // LED on/off state
unsigned long clap_time, last_clap_time = 0;  // clap time records

void setup() {
  pinMode(CLAPIN, INPUT_PULLUP);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);     // control built-in LED by clapping
  Serial.begin(57600);
  Serial.println("# Clap sensor test ...");
  attachInterrupt(                  // register Interrupt Service Routine (ISR):
    digitalPinToInterrupt(CLAPIN),  //   pin to watch for interrupt
    heard_clap,                     //   void function to call on interrupt
    FALLING                         //   trigger interrupt on HIGH → LOW change
  );
  
}

void loop() {
  digitalWrite(LED_BUILTIN, led_state);  // set LED based on clap status
  
  if (clap) {                       // we heard a clap from ISR
    clap = false;                   // make sure we don't trigger again too soon
    last_clap_time = clap_time;     // store old clap time
    clap_time = millis();           // note current clap time
    if (clap_time - last_clap_time < CLAP_DELAY) {  // if two claps heard in ¼ s:
      Serial.println("clap clap!");                 //   notify

      clapclap();
      
      led_state = !led_state;                       //   and switch LED state
    }
    else {
      Serial.println("clap!");                      // notify of only one clap
    }
  }
}
  /*
  if (clap) {
  
    Serial.println("clap");
    digitalWrite(IN1,LOW);      
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);     
  digitalWrite(IN4,LOW);
  delay(2000);
  
  if (clap_time - last_clap_time < CLAP_DELAY) {
  */

void clapclap(){
    Serial.println("clapclap()"); // we heard a clap from ISR
  digitalWrite(IN1,LOW);      
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);     
  digitalWrite(IN4,LOW);
  delay(5000);
  
  digitalWrite(IN1,HIGH);     //どちらかがHIGHでモータが周ります。
  digitalWrite(IN2,HIGH);      //正転
  digitalWrite(IN3,HIGH);     
  digitalWrite(IN4,LOW);
  delay(1000);                //delayは適当に入れてください。

  digitalWrite(IN1,LOW);      
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);     
  digitalWrite(IN4,LOW);
  delay(2000);

  digitalWrite(IN1,HIGH); //両方ともHIGHにするとブレーキ
  digitalWrite(IN2,LOW);     //停止（ブレーキ）
  digitalWrite(IN3,HIGH);     
  digitalWrite(IN4,HIGH);     
  delay(500);

   digitalWrite(IN1,LOW);      
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);     
  digitalWrite(IN4,LOW);
  delay(2000);

  digitalWrite(IN1,LOW);      //上記のものと逆回転になります。
  digitalWrite(IN2,HIGH);     //逆転
  digitalWrite(IN3,HIGH);     
  digitalWrite(IN4,HIGH); 
  delay(500);

   digitalWrite(IN1,LOW);      
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);     
  digitalWrite(IN4,LOW);
  delay(2000);

  digitalWrite(IN1,HIGH);      //停止（惰性回転）
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);     
  digitalWrite(IN4,HIGH);
  delay(1000);

  digitalWrite(IN1,LOW);      
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);     
  digitalWrite(IN4,LOW);
  delay(2000);
}


void heard_clap() {
  clap = true;      // just set clap state in ISR
}