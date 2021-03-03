void setup() {
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0;i<10;i++){
    dacWrite(D2,i*25);
    delay(200);
  }
  for(int j=10;j>0;j--){
    dacWrite(D2,j*25);
    delay(200);
  }
}
