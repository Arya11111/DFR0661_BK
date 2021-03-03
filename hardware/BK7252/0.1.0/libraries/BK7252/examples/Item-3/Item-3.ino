/*!
 * @file Item-3.ino
 * @brief 串口UART数据收发实验。
 * @n 检测并接收USB收到的字符数据，并发送给UART的TX引脚，UART接收到TX发送的数据，
 * @n 去除掉字符串中所有字符O，并将处理后的数据发送到USB串口打印
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2020-12-2
 * @get from https://www.dfrobot.com
 * @url
 */
 
/****************************
 *DFRobot: Hi,DFRobot!
 ****************************/


void setup() {
    /*USB CDC串口波特率初始化*/
    Serial.begin(115200);
    /*UART串口波特率初始化*/
    Serial1.begin(115200);
    
    /*等待串口监视器COM口打开*/
    while(!Serial){
    }
}

void loop() {
    /*如果未检测到USB串口有数据，则等待*/
    while(!Serial.available());
    while(Serial.available()){
        /*将USB接收到的数据发送到UART TX引脚*/
        char p= Serial.read();
        Serial1.print(p);
        delay(2);
    }
    /*如果UART RX检测到数据*/
    while(Serial1.available()){
        /*偷看接收到的字符是不是字符O*/
        char c = Serial1.peek();
        if(c == 'o' || c == 'O'){
            /*如果UART接收到字符O，则去掉*/
            Serial1.read();
        }else{
            Serial.print((char)Serial1.read());
        }
    }
}
