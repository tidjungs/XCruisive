#include <pt.h>
#define PT_DELAY(pt, ms ,ts) \
  ts = millis(); \
  PT_WAIT_WHILE(pt, millis()-ts < (ms));

struct pt pt_taskAccele;

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

void setup(){
  PT_INIT(&pt_taskAccele);
}


void loop(){
  taskAccele(&pt_taskAccele);
}
