/**
 * @file main.cpp
 * @author wancyu
 * @brief 4,5,6,7号引脚led循环闪烁
 * 
 */

#include <Arduino.h>

const char pin[] = {4,5,6,7};

void Led_Running()
{
    for(int i=4;i<8;i++)
    {
        digitalWrite(i,HIGH);
        delay(500);
    }

    for(int i=4;i<8;i++)
    {
        digitalWrite(i,HIGH);
        delay(500);
        digitalWrite(i,LOW);
        delay(500);
    }
}

void setup() {
  for(int i=4;i<8;i++)
  {
    pinMode(i,OUTPUT);
  }
}

void loop() {
    Led_Running();
}

