#include <pt.h>

struct pt pt_taskAccele;
PT_THREAD(tashhAccel(struct pt* pt)) {
  static uint32_t ts;
  const int xPin = 0;
  const int yPin = 1;
  const int zPin = 2;
  int minVal = 265;
  int maxVal = 402;
  double x;
  double y;
  double z;
  PT_BEGIN(pt);
  while(1) {
    int xRead = analogRead(xPin);
    int yRead = analogRead(yPin);
    int zRead = analogRead(zPin);

    int xAng = map(xRead, minVal, maxVal, -90, 90);
    int yAng = map(yRead, minVal, maxVal, -90, 90);
    int zAng = map(zRead, minVal, maxVal, -90, 90);

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
