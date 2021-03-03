//设置通道0
#define LEDC_CHANNEL_0     0

//设置13位定时器
#define LEDC_TIMER_13_BIT  13

//设置定时器频率位5000Hz
#define LEDC_BASE_FREQ     5000
//设置LED灯
#define LED_PIN            D9

int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

//设置led灯的亮度
void ledcAnalogWrite(uint32_t value, uint32_t valueMax = 255) {
  //计算占空比
  uint32_t duty = (LEDC_BASE_FREQ / valueMax) * min(value, valueMax);
  //设置占空比
  ledcWrite(LEDC_CHANNEL_0, duty);
}

void setup() {
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);
}

void loop() {
  ledcAnalogWrite(brightness);
  brightness += fadeAmount;

  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
  delay(30);
}
