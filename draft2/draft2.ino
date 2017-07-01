#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>

#include <PubSubClient.h>
#include <Msgflo.h>

#include "./Config.h"

struct Config {
  const String role = "public/toollock/1";

  const int ledPin = D4;
  const int lockPin = D0;

  const String wifiSsid = WIFI_SSID;
  const String wifiPassword = WIFI_PASSWORD;

  const char *mqttHost = "mqtt.bitraf.no";
  const int mqttPort = 1883;

  const char *mqttUsername = "myuser";
  const char *mqttPassword = "mypassword";
} cfg;

WiFiClient wifiClient;
PubSubClient mqttClient;
msgflo::Engine *engine;
msgflo::InPort *lockPort;
long nextButtonCheck = 0;

auto participant = msgflo::Participant("iot/TheLock", cfg.role);

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println();
  Serial.println();

  Serial.printf("Configuring wifi: %s\r\n", cfg.wifiSsid.c_str());
  WiFi.begin(cfg.wifiSsid.c_str(), cfg.wifiPassword.c_str());
  participant.icon = "lock";
  
  mqttClient.setServer(cfg.mqttHost, cfg.mqttPort);
  mqttClient.setClient(wifiClient);

  String clientId = cfg.role;
  clientId += WiFi.macAddress();

  engine = msgflo::pubsub::createPubSubClientEngine(participant, &mqttClient, clientId.c_str());

//  lockPort = engine->addOutPort("button-event", "any", cfg.role + "/event");

  lockPort = engine->addInPort("lock", "boolean", cfg.role+"/lock",
  [](byte *data, int length) -> void {
      const std::string in((char *)data, length);
      const boolean on = (in == "1" || in == "true");
      Serial.printf("data: %s/n", in.c_str());
      digitalWrite(cfg.lockPin, on);
  });

  Serial.printf("lock pin: %d\r\n", cfg.lockPin);

  pinMode(cfg.lockPin, OUTPUT);

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

// TODO: check for statechange. If changed, send right away. Else only send every 3 seconds or so
//  if (millis() > nextButtonCheck) {
//    const bool pressed = digitalRead(cfg.lockPin);
//    lockPort->send(pressed ? "true" : "false");
//    nextButtonCheck += 100;
//  }

//  digitalWrite(cfg.ledPin, !b);
}

