#include <pt.h>
#define PT_DELAY(pt, ms, ts) \
    ts = millis(); \
    PT_WAIT_WHILE(pt, millis()-ts < (ms));
    
#define PIN_ULTRA_TRIG 12
#define PIN_ULTRA_ECHO 11

struct pt pt_taskUltrasonic;
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
      Serial.println("-1");
    } 
    else {
      Serial.println(distance);
    }
    PT_DELAY(pt, 3000, ts);
  }
 
  PT_END(pt);
}

void setup() {
  pinMode(PIN_ULTRA_TRIG, OUTPUT);
  pinMode(PIN_ULTRA_ECHO, INPUT);
  Serial.begin(9600);
  PT_INIT(&pt_taskUltrasonic);
}
void loop() {
  taskUltrasonic(&pt_taskUltrasonic);
}
