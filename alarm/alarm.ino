#include <pt.h>
// นิยามมาโคร PT_DELAY ไว้เพื่อจำลองการหน่วงเวลาแบบมัลติทาสกิ้ง
#define PT_DELAY(pt, ms, ts) \
    ts = millis(); \
    PT_WAIT_WHILE(pt, millis()-ts < (ms));
 
struct pt pt_taskBuzzer;
struct pt pt_taskLED;
 
///////////////////////////////////////////////////////
PT_THREAD(taskBuzzer(struct pt* pt))
{
  static uint32_t ts;
 
  PT_BEGIN(pt);
 
  while (1)
  {
    analogWrite(10, 00);
    PT_DELAY(pt, 1000, ts);
    analogWrite(10, 0);
    PT_DELAY(pt, 500, ts);
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
 
  PT_END(pt);
}
 
///////////////////////////////////////////////////////
void setup()
{
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
 
  PT_INIT(&pt_taskBuzzer);
  PT_INIT(&pt_taskLED);
}
 
///////////////////////////////////////////////////////
void loop()
{
  taskBuzzer(&pt_taskBuzzer);
  taskLED(&pt_taskLED);
}
