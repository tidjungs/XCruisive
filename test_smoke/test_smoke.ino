struct pt pt_taskSmoke;
int adcPin = 0;
int adcValue = 0;
float v;
float rs,ppm;

PT_THREAD(taskSmoke(struct pt* pt)){
  static uint32_t ts;
  PT_BEGIN(pt);
  while(1) {
    adcValue = analogRead(adcPin);
    v = adcValue*(5.00/1024);
    rs=(100-(20.00*v))/v;
    Serial.print("Volt output");
    Serial.println (v);
    Serial.print("RS=");
    Serial.println (rs);
    PT_DELAY (pt, 1000, ts);
  }
  PT_END(pt);
}
void setup ()
{
  PT_INIT(&pt_taskSmoke);
}
void loop ()
{
    taskSmoke(&pt_taskSmoke);
}
