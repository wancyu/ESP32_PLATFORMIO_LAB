/**
 * @file main.cpp
 * @author wancyu
 * @brief 按键切换led灯状态，阻塞式
 * 4,5号引脚对应led,
 * 6,7号引脚对应sw按键
 * 
 */

#include <Arduino.h>

#define led1 4 
#define led2 5
#define sw1 6
#define sw2 7



void Scan_keys()
{
    if (digitalRead(sw1) == LOW)    // 第一次检测按键是否被按下（LOW 表示按下）
    {
        delay(2);                   // 简单延时 2ms，做去抖动处理
        if (digitalRead(sw1) == LOW) // 再次确认，避免按键抖动误触发
        {
            digitalWrite(led1, !digitalRead(led1)); // 翻转 LED 状态
            while (digitalRead(sw1) == LOW);        // 等待：直到按键松开（由 LOW → HIGH 才会跳出）
        }
    }

    if (digitalRead(sw2) == LOW)    // 第一次检测按键是否被按下（LOW 表示按下）
    {
        delay(2);                   // 简单延时 2ms，做去抖动处理
        if (digitalRead(sw2) == LOW) // 再次确认，避免按键抖动误触发
        {
            digitalWrite(led2, !digitalRead(led2)); // 翻转 LED 状态
            while (digitalRead(sw2) == LOW);        // 等待：直到按键松开（由 LOW → HIGH 才会跳出）
        }
    }

}

void setup() {
    pinMode(led1,OUTPUT);
    pinMode(led2,OUTPUT);
    pinMode(sw1,OUTPUT);
    pinMode(sw2,OUTPUT);
    pinMode(sw1,INPUT_PULLUP);
    pinMode(sw2,INPUT_PULLUP);
}

void loop() {
    Scan_keys();
}

