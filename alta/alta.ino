#include <pt.h>
#define PT_DELAY(pt, ms, ts) \
    ts = millis(); \
    PT_WAIT_WHILE(pt, millis()-ts < (ms));
    
#define PIN_ULTRA_TRIG 12
#define PIN_ULTRA_ECHO 11

struct pt pt_taskUltrasonic;
struct pt pt_taskSendSerial;
int duration , distance;
int maximumRange = 200;
int minimumRange = 0;

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
 
  PT_END(pt);
}

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

void setup() {
  pinMode(PIN_ULTRA_TRIG, OUTPUT);
  pinMode(PIN_ULTRA_ECHO, INPUT);
  Serial.begin(9600);
  Serial1.begin(115200);
  PT_INIT(&pt_taskUltrasonic);
//  PT_INIT(&pt_taskSendSerial);
}
void loop() {
  taskUltrasonic(&pt_taskUltrasonic);
  taskSendSerial(&pt_taskSendSerial);
}
