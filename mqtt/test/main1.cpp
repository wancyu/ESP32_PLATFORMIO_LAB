/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-09-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */


#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "mbedtls/aes.h"
#include "mbedtls/base64.h"


long lastMqttConn;          // 上次mqtt连接时间
long lastPublishMonitor;    // 上次发布监测数据时间
long lastTimerMonitor;      // 上次定时发布监测数据


PubSubClient mqttClient;
float rssi = 0;
char wumei_iv[17] = "wumei-smart-open";
int monitorCount = 0;
long monitorInterval = 1000;


// 设备信息配置
String deviceNum = "D1915D20370I4";
String userId = "1";
String productId = "93";
float firmwareVersion = 1.0;

// 经度和纬度可选，如果产品使用设备定位，则必须传
float latitude=0;
float longitude=0;

// Mqtt配置
char *mqttHost = "makerbee.scumaker.org";
int mqttPort = 1883;
char *mqttUserName = "FastBee";
char *mqttPwd = "P9W41T453X630HH2";
char mqttSecret[17] = "K2UK1VCQSJ6T7R35";
// 产品启用授权码，则授权码不能为空
String authCode="";



// 订阅的主题
String prefix = "/" + productId + "/" + deviceNum;
String sInfoTopic = prefix + "/info/get";
String sOtaTopic = prefix + "/ota/get";
String sNtpTopic = prefix + "/ntp/get";
String sPropertyTopic = prefix + "/property/get";
String sFunctionTopic = prefix + "/function/get";
String sPropertyOnline = prefix + "/property-online/get";
String sFunctionOnline = prefix + "/function-online/get";
String sMonitorTopic = prefix + "/monitor/get";
// 发布的主题
String pInfoTopic = prefix + "/info/post";
String pNtpTopic = prefix + "/ntp/post";
String pPropertyTopic = prefix + "/property/post";
String pFunctionTopic = prefix + "/function/post";
String pMonitorTopic = prefix + "/monitor/post";
String pEventTopic = prefix + "/event/post";





WiFiClient wifiClient;




// Wifi配置
char *wifiSsid = "chenyu";
char *wifiPwd = "123456789";


// 计算 Base64 编码长度
inline size_t base64_enc_len(size_t len) {
    return 4 * ((len + 2) / 3) + 1;
}

// 加密 (AES-CBC-128-pkcs5padding)
String encrypt(String plain_data, char *wumei_key, char *wumei_iv) {
    size_t len = plain_data.length();
    size_t n_blocks = len / 16 + 1;
    uint8_t n_padding = n_blocks * 16 - len;

    // 填充 PKCS#7
    uint8_t data[n_blocks * 16];
    memcpy(data, plain_data.c_str(), len);
    for (size_t i = len; i < n_blocks * 16; i++) {
        data[i] = n_padding;
    }

    // key / iv
    uint8_t key[16], iv[16];
    memcpy(key, wumei_key, 16);
    memcpy(iv, wumei_iv, 16);

    // AES 加密
    uint8_t crypt_data[n_blocks * 16];
    mbedtls_aes_context aes_ctx;
    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_enc(&aes_ctx, key, 128);
    mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, n_blocks * 16, iv, data, crypt_data);
    mbedtls_aes_free(&aes_ctx);

    // Base64 编码
    size_t enc_len = base64_enc_len(n_blocks * 16);
    unsigned char encoded_data[enc_len];
    size_t olen = 0;
    mbedtls_base64_encode(encoded_data, enc_len, &olen, crypt_data, n_blocks * 16);

    return String((char*)encoded_data);
}



//打印提示信息
void printMsg(String msg)
{
  Serial.print("\r\n[");
  Serial.print(millis());
  Serial.print("ms]");
  Serial.print(msg);
}



// 连接wifi
void connectWifi()
{
  printMsg("连接 ");
  Serial.print(wifiSsid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid, wifiPwd);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  printMsg("WiFi连接成功");
  printMsg("IP地址: ");
  Serial.print(WiFi.localIP());
}


// Wifi掉线重连
void wifi_reconnect()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    connectWifi();
  }
}



String g_time;
String ntpServer = "https://makerbee.scumaker.org/iot/tool/ntp?deviceSendTime=";

//  HTTP获取时间
String getTime()
{
  while (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    printMsg("获取时间...");

    if (http.begin(wifiClient, (ntpServer + (String)millis()).c_str()))
    {
      // 发送请求
      int httpCode = http.GET();
      if (httpCode > 0)
      {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
          g_time = http.getString();
          printMsg("获取时间成功，data:");
          Serial.print(g_time);
          return g_time;
        }
      }
      else
      {
        printMsg("获取时间失败，error:");
        Serial.printf(http.errorToString(httpCode).c_str());
      }
      http.end();
    }
    else
    {
      printMsg("连接Http失败");
    }
    delay(500);
  }
  return "1672524366000";
}




// 生成密码
String generationPwd()
{
  String jsonTime = getTime();
  printMsg("getTime()= " + jsonTime);

  // 128字节内存池容量
  StaticJsonDocument<128> doc;
  // 解析JSON
  DeserializationError error = deserializeJson(doc, jsonTime);
  if (error)
  {
    printMsg("Json解析失败：");
    Serial.print(error.f_str());
    return "";
  }
  // 获取当前时间
  float deviceSendTime = doc["deviceSendTime"];
  float serverSendTime = doc["serverSendTime"];
  float serverRecvTime = doc["serverRecvTime"];
  float deviceRecvTime = millis();
  float now = (serverSendTime + serverRecvTime + deviceRecvTime - deviceSendTime) / 2;
  // 过期时间 = 当前时间 + 1小时
  float expireTime = now + 1 * 60 * 60 * 1000;
  // 密码加密格式为：mqtt密码 & 过期时间 & 授权码（可选），如果产品启用了授权码就必须加上  
  String password="";
  if(authCode == ""){
    password = (String)mqttPwd + "&" + String(expireTime, 0);
  }else{
    password = (String)mqttPwd + "&" + String(expireTime, 0) + "&" + authCode;
  }
  printMsg("密码(未加密):" + password);

  return password;
}





// Mqtt回调
void callback(char *topic, byte *payload, unsigned int length)
{
  //blink();
  printMsg("接收数据：");
  String data = "";
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    data += (char)payload[i];
  }

  if (strcmp(topic, sOtaTopic.c_str()) == 0)
  {
    printMsg("订阅到设备升级指令...");
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    String newVersion = doc["version"];
    String downloadUrl = doc["downloadUrl"];
    printMsg("固件版本："+newVersion);
    printMsg("下载地址："+downloadUrl);
  }
  else if (strcmp(topic, sInfoTopic.c_str()) == 0)
  {
    printMsg("订阅到设备信息...");
    // 发布设备信息
    //publishInfo();
  }
  else if (strcmp(topic, sNtpTopic.c_str()) == 0)
  {
    printMsg("订阅到NTP时间...");
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    float deviceSendTime = doc["deviceSendTime"];
    float serverSendTime = doc["serverSendTime"];
    float serverRecvTime = doc["serverRecvTime"];
    float deviceRecvTime = millis();
    float now = (serverSendTime + serverRecvTime + deviceRecvTime - deviceSendTime) / 2;
    printMsg("当前时间：" + String(now, 0));
  }
  else if (strcmp(topic, sPropertyTopic.c_str()) == 0 || strcmp(topic, sPropertyOnline.c_str()) == 0)
  {
    printMsg("订阅到属性指令...");
    //processProperty(data);
  }
  else if (strcmp(topic, sFunctionTopic.c_str()) == 0 || strcmp(topic, sFunctionOnline.c_str()) == 0)
  {
    printMsg("订阅到功能指令...");
    //processFunction(data);
  }
  else if (strcmp(topic, sMonitorTopic.c_str()) == 0)
  {
    printMsg("订阅到实时监测指令...");
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    monitorCount = doc["count"];
    monitorInterval = doc["interval"];
  }
}


 // 连接mqtt
void connectMqtt()
{
  printMsg("连接Mqtt服务器...");
  // 生成mqtt认证密码(设备加密认证，密码加密格式为：mqtt密码 & 过期时间 & 授权码，其中授权码为可选)
  //String password = generationPwd();
  //String encryptPassword = encrypt(password, mqttSecret, wumei_iv);
  //printMsg("密码(已加密)：" + encryptPassword);
  mqttClient.setClient(wifiClient);
  mqttClient.setServer(mqttHost, mqttPort);
  mqttClient.setCallback(callback);
  mqttClient.setBufferSize(1024);
  mqttClient.setKeepAlive(10);
  //连接 设备mqtt客户端Id格式为：认证类型(E=加密、S=简单) & 设备编号 & 产品ID & 用户ID改为s测试用
  String clientId = "S&" + deviceNum + "&" + productId +"&" + userId;
  //bool connectResult = mqttClient.connect(clientId.c_str(), mqttUserName, encryptPassword.c_str());
  bool connectResult = mqttClient.connect("S&D1915D20370I4&93&19", mqttUserName, "P9W41T453X630HH2");
 
  if (connectResult)
  {
    printMsg("连接成功");

    /*

    // 订阅(OTA、NTP、属性、功能、实时监测、信息)
    mqttClient.subscribe(sInfoTopic.c_str(), 1);
    mqttClient.subscribe(sOtaTopic.c_str(), 1);
    mqttClient.subscribe(sNtpTopic.c_str(), 1);
    mqttClient.subscribe(sPropertyTopic.c_str(), 1);
    mqttClient.subscribe(sFunctionTopic.c_str(), 1);
    mqttClient.subscribe(sPropertyOnline.c_str(), 1);
    mqttClient.subscribe(sFunctionOnline.c_str(), 1);
    mqttClient.subscribe(sMonitorTopic.c_str(), 1);
    printMsg("订阅主题：" + sInfoTopic);
    printMsg("订阅主题：" + sOtaTopic);
    printMsg("订阅主题：" + sNtpTopic);
    printMsg("订阅主题：" + sPropertyTopic);
    printMsg("订阅主题：" + sFunctionTopic);
    printMsg("订阅主题：" + sPropertyOnline);
    printMsg("订阅主题：" + sFunctionOnline);
    printMsg("订阅主题：" + sMonitorTopic);
    // 发布设备信息
    publishInfo();

    */
  }
  else
  {
    printMsg("连接失败, rc=");
    Serial.print(mqttClient.state());
  }
}


void mqtt_reconnect()
{
  // 非阻塞Mqtt重连，间隔30秒
  if (WiFi.status() == WL_CONNECTED)
  {
    long now = millis();
    if (!mqttClient.connected())
    {
      if (now - lastMqttConn > 30000)
      {
        lastMqttConn = now;
        connectMqtt();
      }
    }
    else
    {
      mqttClient.loop();
    }
  }
}




/**
 * @brief 执行一次
 * 
 */
void setup()
{

  Serial.begin(115200);  //打开串行端口
  printMsg("device starting...");
  connectWifi();
  connectMqtt();
}

/**
 * @brief 循环执行
 */
void loop()
{
  // Wifi掉线重连
  wifi_reconnect();
  mqtt_reconnect();
}
