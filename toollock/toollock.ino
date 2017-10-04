#include "./Config.h"
#include "./toollock.hpp"

#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>

#include <PubSubClient.h>
#include <Msgflo.h>

#include "./participant.hpp"

// Configuration
ToolLockParticipant participants[] = {
  ToolLockParticipant("a001", D5, D0, D6),
  ToolLockParticipant("a002", D1, D2, D3),
  ToolLockParticipant("a003", D4, D7, D8),
};

static const char *MQTT_PREFIX = "public/toollock/";

// Defaults
#ifndef MQTT_PORT 
#define MQTT_PORT 1883
#endif
#ifndef MQTT_USERNAME
#define MQTT_USERNAME nullptr
#endif
#ifndef MQTT_PASSWORD
#define MQTT_PASSWORD nullptr
#endif

WiFiClient wifiClient;
PubSubClient mqttClient;
msgflo::Engine *engine;

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();

  const char *ssid = WIFI_SSID;
  Serial.printf("Configuring wifi: %s\r\n", ssid);
  WiFi.begin(ssid, WIFI_PASSWORD);

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setClient(wifiClient);

  String clientId = MQTT_PREFIX;
  clientId += WiFi.macAddress();

  engine = msgflo::pubsub::createPubSubClientEngine(mqttClient);
  engine->setClientId(clientId.c_str());
  engine->setCredentials(MQTT_USERNAME, MQTT_PASSWORD);
  engine->setTopicPrefix(MQTT_PREFIX);

  for (auto &p : participants) {
    engine->addParticipant(p);
    p.setup();
  }
}

void loop() {
  static bool connected = false;

  if (WiFi.status() == WL_CONNECTED) {
    if (!connected) {
      Serial.printf("Wifi connected: ip=%s\r\n", WiFi.localIP().toString().c_str());
    }
    connected = true;
    engine->loop();
  } else {
    if (connected) {
      connected = false;
      Serial.println("Lost wifi connection.");
    }
  }

  for (auto &p : participants) {
    p.loop();
  }
}

