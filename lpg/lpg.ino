int adcPin = 0;
int adcValue = 0;
float v;
float rs,ppm;
void setup ()
{
  Serial.begin (9600);
  delay (2000);
}
void loop () {
  adcValue = analogRead(adcPin);
  v = adcValue*(5.00/1024);
  rs=(100-(20.00*v))/v;
  Serial.print("Volt output");
  Serial.println (v);
  Serial.print("RS=");
  Serial.println (rs);
  delay (1000);
}
