void setup()
{
  Serial.begin(115200);
  delay(1000); // give me time to bring up serial monitor
  Serial.println("FireBeetle Board-ESP32 Touch Test");
}

void loop()
{
  Serial.println(touchRead(T2));  // get value using T0
  delay(1000);
}
