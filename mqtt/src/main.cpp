#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

// WiFi 配置
const char* ssid     = "chenyu";
const char* password = "123456789";

// MQTT 服务器配置
const char* host = "makerbee.scumaker.org";
const int port_plain = 1883;   // 明文 TCP 测试
const int port_tls   = 8883;   // TLS 测试

void setup() {
  Serial.begin(115200);
  delay(500);

  // 连接 WiFi
  WiFi.begin(ssid, password);
  Serial.print("连接 WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi 已连接");

  // 测试明文 TCP
  WiFiClient client_plain;
  Serial.print("测试 TCP 1883...");
  if(client_plain.connect(host, port_plain)) {
    Serial.println("成功！");
    client_plain.stop();
  } else {
    Serial.println("失败！");
  }

  // 测试 TLS 8883
  WiFiClientSecure client_tls;
  client_tls.setInsecure(); // 跳过证书验证
  Serial.print("测试 TLS 8883...");
  if(client_tls.connect(host, port_tls)) {
    Serial.println("成功！");
    client_tls.stop();
  } else {
    Serial.println("失败！");
  }
}

void loop() {
  // 不需要循环
}
