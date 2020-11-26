void setup(){
  Serial.begin(115200);
}

void loop(){
  static unsigned long i = 0;
  Serial.println(i++);
  delay(1000);
}

