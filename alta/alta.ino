#include <pt.h>
#define PT_DELAY(pt, ms, ts) \
    ts = millis(); \
    PT_WAIT_WHILE(pt, millis()-ts < (ms));

#define PIN_ULTRA_TRIG 12
#define PIN_ULTRA_ECHO 11

struct pt pt_taskUltrasonic;
struct pt pt_taskSendSerial;
struct pt pt_taskReadSerial;
struct pt pt_taskBuzzer;
struct pt pt_taskLED;
struct pt pt_taskAccele;

int duration , distance;
int maximumRange = 200;
int minimumRange = 0;
int alert = 0;

PT_THREAD(taskUltrasonic(struct pt* pt)) {
  static uint32_t ts;

  PT_BEGIN(pt);

  while (1) {
    digitalWrite(PIN_ULTRA_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(PIN_ULTRA_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_ULTRA_TRIG, LOW);
    duration = pulseIn(PIN_ULTRA_ECHO, HIGH);
    distance = duration/58.2;
    if (distance >= maximumRange || distance <= minimumRange){
      distance = -1;
    }
    PT_DELAY(pt, 5000, ts);
  }

////////////////////////////////////////////////////////////////
  PT_END(pt);
}

PT_THREAD(taskReadSerial(struct pt* pt)){
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1){
    Serial.print("hello");
    if(Serial1.available()) {
      Serial.print("hello2");
      String str = Serial1.readStringUntil('\r');
      str.replace("\r","");
      str.replace("\n","");
      Serial.println(str);
      if(str == "alarm"){
        alert = 1;
      }
    }
    PT_YIELD(pt);
  }
  PT_END(pt);
}

////////////////////////////////////////////////////////////
PT_THREAD(taskAccele(struct pt* pt)) {
  static uint32_t ts;
  const int xPin = 0;
  const int yPin = 1;
  const int zPin = 2;
  int minVal = 265;
  int maxVal = 402;
  double x, y, z;
  int xRead, yRead, zRead, xAng, yAng, zAng;
  PT_BEGIN(pt);
  while(1) {
    xRead = analogRead(xPin);
    yRead = analogRead(yPin);
    zRead = analogRead(zPin);

    xAng = map(xRead, minVal, maxVal, -90, 90);
    yAng = map(yRead, minVal, maxVal, -90, 90);
    zAng = map(zRead, minVal, maxVal, -90, 90);

    x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);
    y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
    z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

    Serial.print("x: ");
    Serial.print(x);
    Serial.print(" | y: ");
    Serial.print(y);
    Serial.print(" | z: ");
    Serial.println(z);

    PT_DELAY(pt,100,ts);
  }
  PT_END(pt);
}

////////////////////////////////////////////////////////
PT_THREAD(taskSendSerial(struct pt* pt)){
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1){
    Serial1.println(distance);
    Serial.println(distance);
    PT_DELAY(pt, 5000, ts);
  }
  PT_END(pt);
}

///////////////////////////////////////////////////////
PT_THREAD(taskBuzzer(struct pt* pt))
{
  static uint32_t ts;

  PT_BEGIN(pt);

  while (1)
  {
    if(alert == 1) {
      analogWrite(10, 00);
      PT_DELAY(pt, 1000, ts);
      analogWrite(10, 0);
      PT_DELAY(pt, 500, ts);
    }
  }

  PT_END(pt);
}

///////////////////////////////////////////////////////
PT_THREAD(taskLED(struct pt* pt))
{
  static uint32_t ts;

  PT_BEGIN(pt);

  while (1)
  {
    if(alert == 1) {
      digitalWrite(11, HIGH);
      digitalWrite(12, HIGH);
      digitalWrite(13, HIGH);
      digitalWrite(14, HIGH);
      PT_DELAY(pt, 700, ts);
      digitalWrite(11, LOW);
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
      digitalWrite(14, LOW);
      PT_DELAY(pt, 300, ts);
    }
  }

  PT_END(pt);
}

///////////////////////////////////////////////////////
void setup() {
  pinMode(PIN_ULTRA_TRIG, OUTPUT);
  pinMode(PIN_ULTRA_ECHO, INPUT);
  pinMode(10, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(115200);
  PT_INIT(&pt_taskUltrasonic);
  PT_INIT(&pt_taskSendSerial);
  PT_INIT(&pt_taskReadSerial);
  PT_INIT(&pt_taskBuzzer);
  PT_INIT(&pt_taskLED);
  PT_INIT(&pt_taskAccele);
}

void loop() {
  taskUltrasonic(&pt_taskUltrasonic);
//  taskSendSerial(&pt_taskSendSerial);
  taskReadSerial(&pt_taskReadSerial);
  taskBuzzer(&pt_taskBuzzer);
  taskLED(&pt_taskLED);
  taskAccele(&pt_taskAccele);
}
