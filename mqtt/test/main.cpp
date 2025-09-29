

// #include <WiFi.h>
// #include <WiFiUdp.h>
// #include <PubSubClient.h>
// #include <ArduinoJson.h>
// #include <Arduino.h>
// #include <stdio.h>

// const char* ssid = "SCU_Makers";
// const char* password = "iloveSCU";
// const char* mqttServer = "2fd4091ed1.st1.iotda-device.cn-north-4.myhuaweicloud.com";
// const int mqttPort = 1883;
// const char* clientId = "6765bb73ef99673c8ad7b905_openharmony_rabbit_0_0_2024122020";
// const char* mqttUser = "6765bb73ef99673c8ad7b905_openharmony_rabbit";
// const char* mqttPassword = "b8ce0449ed5aa65e24d1151c19b4448151891708b1d76be8640b9c22af772090";
// const char* topic_properties_report = "$oc/devices/6765bb73ef99673c8ad7b905_openharmony_rabbit/sys/properties/report";

// DHT dht(DHTPIN, DHTTYPE);
// WiFiClient espClient;
// PubSubClient client(espClient);

// void callback(char* topic, byte* message, unsigned int length) {
//   Serial.print("Message arrived on topic: ");
//   Serial.println(topic);
//   String receivedMessage;
//   for (int i = 0; i < length; i++) {
//     receivedMessage += (char)message[i];
//   }
//   Serial.print("Message: ");
//   Serial.println(receivedMessage);
// }

// void MQTT_Init() {
//   Serial.println("Booting");
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(3000);
//     Serial.println("Connecting to WiFi...");
//   }
//   Serial.println("Connected to the WiFi network");
//   client.setServer(mqttServer, mqttPort);
//   client.setKeepAlive(60);
//   while (!client.connected()) {
//     Serial.println("Connecting to MQTT...");
//     if (client.connect(clientId, mqttUser, mqttPassword)) {
//       Serial.println("Connected to MQTT");
//     } else {
//       Serial.print("Failed to connect, state: ");
//       Serial.println(client.state());
//       delay(6000);
//     }
//   }
//   Serial.println("Ready");
//   Serial.print("IP address: ");
//   Serial.println(WiFi.localIP());
//   client.setCallback(callback);
// }

// void MReport_Sensor_inform() {
//   String JSONmessageBuffer; 
//   StaticJsonDocument<256> doc;
//   JsonObject services_0 = doc["services"].createNestedObject();
//   services_0["service_id"] = "basic";
//   JsonObject services_0_properties = services_0.createNestedObject("properties");
//   services_0_properties["dht11_t_in"] = t;
//   services_0_properties["dht11_h_in"] = h;
//   serializeJson(doc, JSONmessageBuffer);
//   Serial.println("Sending message to MQTT topic..");
//   Serial.println(JSONmessageBuffer);
//   if (client.publish(topic_properties_report, JSONmessageBuffer.c_str())) {
//     Serial.println("Success sending message");
//   } else {
//     Serial.println("Error sending message");
//     if(WiFi.status() != WL_CONNECTED)
//     MQTT_Init();
//   }
// }

// void MQTT_response(char *topic) {
//   String responsed;
//   StaticJsonDocument<128> doc;
//   JsonObject response = doc.createNestedObject("response");
//   serializeJson(doc, responsed);
//   client.publish(topic, responsed.c_str());
//   Serial.println(responsed);
// }

// void reconnect() {
//   int attempt = 0;
//   while (!client.connected()) {
//     Serial.printf("Attempt %d: Reconnecting to MQTT...\n", ++attempt);
//     if (client.connect(clientId, mqttUser, mqttPassword)) {
//       Serial.println("Reconnected to MQTT");
//     } else {
//       Serial.printf("Failed to reconnect, state: %d. Retrying...\n", client.state());
//       delay(6000);
//     }
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   Serial.println("DHT11 sensor!");
//   delay(2000);
//   dht.begin();
//   MQTT_Init();
// }

// void loop() {  
//   h = dht.readHumidity();
//   t = dht.readTemperature();
//   if (isnan(h) || isnan(t)) {
//     Serial.println("Failed to read from DHT sensor!");
//     return;
//   }
//   Serial.print("Humidity: ");
//   Serial.print(h);
//   Serial.print("\n");
//   Serial.print("Temperature: ");
//   Serial.print(t);
//   Serial.print("\n\n");
//   delay(10000);
//   MReport_Sensor_inform();
//     if (!client.connected()) {
//     reconnect();
//   }
//   client.loop();
// }


// WiFiClient espClient;
// PubSubClient client(espClient);

// void setup() {
//   // 配置 MQTT 代理
//   client.setServer(mqtt_server, 1883);
// }

// void loop() {
//   if (!client.connected()) {
//     reconnect();
//   }
//   client.loop();
// }

// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("尝试连接到 MQTT 代理...");
//     if (client.connect("ESP32Client")) {
//       Serial.println("连接成功！");
//     } else {
//       Serial.print("连接失败, rc=");
//       Serial.print(client.state());
//       delay(5000);
//     }
//   }
// }