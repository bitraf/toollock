#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>

#include <PubSubClient.h>
#include <Msgflo.h>

#include "./Config.h"
#include "./toollock.hpp"

struct Config {
  const String role = "public/toollock/1";

  const LockConfig lockConfig;

  const int sensorPin = D5;
  const int lockPin = D0;

  const String wifiSsid = WIFI_SSID;
  const String wifiPassword = WIFI_PASSWORD;

  const char *mqttHost = "mqtt.bitraf.no";
  //  const char *mqttHost = "10.13.37.127";
  const int mqttPort = 1883;

  const char *mqttUsername = "myuser";
  const char *mqttPassword = "mypassword";
} cfg;

struct State {
  LockState lock;
  long nextUpdateMessage = 0;
};

Input getInput(Config config, bool requestUnlock) {
  return {
    requestUnlock,
    digitalRead(config.sensorPin),
    millis(),
  };
}

bool setOutput(Config config, State state) {

  digitalWrite(config.lockPin, (state.lock.state == lockState::locked) ? HIGH : LOW);

  return true;
}

State updateState(const Config &config, State current, bool requestUnlock=false) {
  State next;
  Input input = getInput(config, requestUnlock);

  next.lock = nextState(config.lockConfig, current.lock, input);
  setOutput(cfg, next);

  if (next.lock.state != current.lock.state) {
    // TODO: send lock state
  }
  if (input.currentTime > current.nextUpdateMessage) {
    // TODO: send lock states
    next.nextUpdateMessage = input.currentTime + 30*1000; 
  }

  return next;
}

State state;

WiFiClient wifiClient;
PubSubClient mqttClient;
msgflo::Engine *engine;
msgflo::InPort *lockPort;
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

  // FIXME: send lock state
  //  lockPort = engine->addOutPort("button-event", "any", cfg.role + "/event");

  lockPort = engine->addInPort("unlock", "boolean", cfg.role + "/unlock",
  [](byte * data, int length) -> void {
    const std::string in((char *)data, length);
    Serial.printf("data: %s\n", in.c_str());
    const boolean requestUnlock = (in == "1" || in == "true");
    state = updateState(cfg, state, requestUnlock);
  });

  Serial.printf("lock pin: %d\r\n", cfg.lockPin);
  pinMode(cfg.lockPin, OUTPUT);
  pinMode(cfg.sensorPin, INPUT);

  digitalWrite(cfg.lockPin, LOW);
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

  state = updateState(cfg, state);
}

