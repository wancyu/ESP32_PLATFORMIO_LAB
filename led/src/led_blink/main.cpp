/**
 * @file main.cpp
 * @author wancyu
 * @brief 14号引脚led闪烁
 * 
 */

#include <Arduino.h>


void setup() {
  pinMode(14,OUTPUT);
}

void loop() {
  digitalWrite(14,HIGH);
  delay(500);
  digitalWrite(14,LOW);
  delay(500);
}

