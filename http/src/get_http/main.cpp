/**
 * @file main.cpp
 * @author wancyu
 * @brief get 请求http页面
 * @date 2025-09-28
 * @copyright Copyright (c) 2025
 * 
 */

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>

// Wifi配置
char *wifiSsid = "chenyu";
char *wifiPwd = "123456789";

String url = "http://58.87.96.193:8000/playground";
WiFiClient wifiClient;
HTTPClient http;

// 连接wifi
void connectWifi()
{
  Serial.print(wifiSsid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid, wifiPwd);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
}

// Wifi掉线重连
void wifi_reconnect()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    connectWifi();
  }
}

//http请求
void http_get()
{
  //指定请求url
  http.begin(url);
  //http.begin("对应https网站",cert);
  //发送get请求，返回状态码
  int httpcode = http.GET();
  Serial.println(httpcode);
  String payload = http.getString();
  Serial.println(payload);
  http.end();
}



void setup()
{
  Serial.begin(115200);
  connectWifi();
  http_get();
}

void loop()
{
  wifi_reconnect();
}