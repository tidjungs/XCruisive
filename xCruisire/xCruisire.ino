#include <pt.h>
#define PT_DELAY(pt, ms, ts) \
    ts = millis(); \
    PT_WAIT_WHILE(pt, millis()-ts < (ms));

#define PIN_ULTRA_TRIG 12
#define PIN_ULTRA_ECHO 11
#define SW1 2

struct pt pt_taskUltrasonic;
struct pt pt_taskSendSerial;
struct pt pt_taskReadSerial;
struct pt pt_taskBuzzer;
struct pt pt_taskLED;
struct pt pt_taskAccele;
struct pt pt_taskTemp;
struct pt pt_taskCancel;
struct pt pt_taskSmoke;
struct pt pt_taskTest;

#define adcPin A0
int adcValue = 0;

int duration , distance = 0;
int maximumRange = 200;
int minimumRange = 0;
int alert = 0;

const int xPin = A1;
const int yPin = A2;
const int zPin = A3;
int minVal = 265;
int maxVal = 402;
int x, y, z;
int xRead, yRead, zRead, xAng, yAng, zAng;
int temp = 0;


// check for update
bool shouldUpdate = false;
int lastDistance;
int lastTemp;
int lastAdcValue;
int lastX;
int lastY;

PT_THREAD(taskUltrasonic(struct pt* pt)) {
  static uint32_t ts;

  PT_BEGIN(pt);

  while (1) {
    Serial.println(distance);
    digitalWrite(PIN_ULTRA_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(PIN_ULTRA_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_ULTRA_TRIG, LOW);
    duration = pulseIn(PIN_ULTRA_ECHO, HIGH);
    lastDistance = distance;
    distance = duration/58.2;
    if (distance >= maximumRange || distance <= minimumRange){
      distance = -1;
    }
    if(distance == -1 && lastDistance == -1) {
      shouldUpdate = false;
    }
    else if (distance == lastDistance) {
      shouldUpdate = false;
    }
    else {
      shouldUpdate = true;
    }
    PT_DELAY(pt, 1000, ts);
  }

  PT_END(pt);
}
////////////////////////////////////////////////////////////////
PT_THREAD(taskTemp(struct pt* pt)){
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1){
    lastTemp = temp;
    temp = (25*analogRead(A2) - 2050)/100;
    if(abs(lastTemp - temp) >= 3) {
      shouldUpdate = true;
    }
    PT_DELAY(pt, 1000, ts);
  }
  PT_END(pt);
}
////////////////////////////////////////////////////////////////

PT_THREAD(taskReadSerial(struct pt* pt)){
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1){
    if(Serial1.available()) {
      String str = Serial1.readStringUntil('\r');
      str.replace("\r","");
      str.replace("\n","");
      if(str == "alert"){
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
  PT_BEGIN(pt);
  while(1) {
    lastX = xAng;
    lastY = yAng;
    
    xRead = analogRead(xPin);
    yRead = analogRead(yPin);
    zRead = analogRead(zPin);

    xAng = map(xRead, minVal, maxVal, -90, 90);
    yAng = map(yRead, minVal, maxVal, -90, 90);
    zAng = map(zRead, minVal, maxVal, -90, 90);
    if(abs(lastX - xAng) >= 30 || abs(lastY - yAng) >= 30) {
      shouldUpdate = true; 
    }
//    Serial.print("last ");
//    Serial.println(abs(lastX - xAng));
//    Serial.print("now ");
//    Serial.println(abs(lastY - yAng));
    PT_DELAY(pt,1000,ts);
  }
  PT_END(pt);
}

////////////////////////////////////////////////////////
PT_THREAD(taskSendSerial(struct pt* pt)){
  static uint32_t ts;
  PT_BEGIN(pt);
  while (1){
    if(shouldUpdate == true) {
       Serial.println(String(xAng) + "," + String(yAng) + "," + String(zAng)  + "," + String(temp) + "," + String(distance) + "," + String(adcValue));
       Serial1.println(String(xAng) + "," + String(yAng) + "," + String(zAng) + "," + String(temp) + "," + String(distance) + "," + String(adcValue));
       shouldUpdate = false;
    }
    PT_DELAY(pt, 1000, ts);
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
      analogWrite(10, 100);
      PT_DELAY(pt, 1000, ts);
      analogWrite(10, 0);
      PT_DELAY(pt, 500, ts);
    }
    PT_DELAY(pt, 100, ts);
  }

  PT_END(pt);
}
///////////////////////////////////////////////////////
PT_THREAD(taskCancel(struct pt* pt)) {
  static uint32_t ts;

  PT_BEGIN(pt);

  while (1)
  {
    if(digitalRead(SW1) == 0){
      alert = 0;
    }
    PT_DELAY(pt, 100, ts);
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
//      digitalWrite(11, HIGH);
      digitalWrite(12, HIGH);
      digitalWrite(13, HIGH);
      digitalWrite(14, HIGH);
      PT_DELAY(pt, 700, ts);
//      digitalWrite(11, LOW);
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
      digitalWrite(14, LOW);
      PT_DELAY(pt, 300, ts);
    }
    PT_DELAY(pt, 100, ts);
  }

  PT_END(pt);
}
////////////////////////////////////////////////////////
PT_THREAD(taskSmoke(struct pt* pt)){
  static uint32_t ts;
  PT_BEGIN(pt);
  while(1) {
    lastAdcValue = adcValue;
    adcValue = analogRead(adcPin);
    if(lastAdcValue <= 100 && adcValue > 100) {
      shouldUpdate = true;
    }
    else if ((lastAdcValue > 100 && lastAdcValue <= 300) && adcValue > 300) {
      shouldUpdate = true;
    }
    else if (lastAdcValue > 100 && adcValue < 100) {
      shouldUpdate = true;
    }
    else if (lastAdcValue > 300 && adcValue <= 300) {
      shouldUpdate = true;
    }
    else if (lastAdcValue == 0) {
      shouldUpdate = true;
    }
    Serial.print("last :");
    Serial.println(lastAdcValue);
    Serial.print("adc :");
    Serial.println(adcValue);
    PT_DELAY (pt, 1000, ts);
  }
  PT_END(pt);
}
////////////////////////////////////////////////////////
PT_THREAD(taskTest(struct pt* pt)){
  static uint32_t ts;
  PT_BEGIN(pt);
  while(1) {
    if(shouldUpdate == true) {
      Serial.println("update");
      shouldUpdate = false;
    }
    PT_DELAY (pt, 1000, ts);
  }
  PT_END(pt);
}
////////////////////////////////////////////////////////
void setup() {
  pinMode(PIN_ULTRA_TRIG, OUTPUT);
  pinMode(PIN_ULTRA_ECHO, INPUT);
  pinMode(10, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(A2, INPUT);
  pinMode(adcPin, INPUT);
  pinMode(SW1, INPUT);
  Serial.begin(9600);
  Serial1.begin(115200);
  PT_INIT(&pt_taskUltrasonic);
  PT_INIT(&pt_taskReadSerial);
  PT_INIT(&pt_taskBuzzer);
  PT_INIT(&pt_taskLED);
  PT_INIT(&pt_taskAccele);
  PT_INIT(&pt_taskSmoke);
  PT_INIT(&pt_taskTemp);
  PT_INIT(&pt_taskSendSerial);
  PT_INIT(&pt_taskCancel);
  PT_INIT(&pt_taskTest);
}

void loop() {
//  taskUltrasonic(&pt_taskUltrasonic);
//  taskSmoke(&pt_taskSmoke); 
//  taskTemp(&pt_taskTemp);
//  taskAccele(&pt_taskAccele);
//  taskSendSerial(&pt_taskSendSerial);
//  taskTest(&pt_taskTest);
//  taskReadSerial(&pt_taskReadSerial);
//  taskBuzzer(&pt_taskBuzzer);
//  taskLED(&pt_taskLED);
//  taskCancel(&pt_taskCancel);
}
