

struct Config {
  const int sensorPin;
  const int lockPin;
  const int statusPin;
  const LockConfig lockConfig;
};

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

bool setOutput(Config config, msgflo::OutPort *statePort, State state, bool sendState, bool keyPresent) {

  const bool unlock = (state.lock.state == lockState::takeTool);
  digitalWrite(config.lockPin, unlock);
  digitalWrite(config.statusPin, keyPresent);
  if (sendState) {
    statePort->send(stateNames[state.lock.state]);
  }

  return true;
}

State updateState(const Config &config, State current, msgflo::OutPort *statePort, bool requestUnlock=false) {
  State next = current;
  Input input = getInput(config, requestUnlock);

  next.lock = nextState(config.lockConfig, current.lock, input);

  bool sendState = false;
  if (next.lock.state != current.lock.state) {
    sendState = true;
  }
  if (input.currentTime >= current.nextUpdateMessage) {
    sendState = true;
    next.nextUpdateMessage = input.currentTime + 30*1000;
  }

  setOutput(config, statePort, next, sendState, input.keyPresent);

  return next;
}

class ToolLockParticipant : public msgflo::Participant {
public:
    msgflo::InPort *lockPort;
    msgflo::OutPort *errorPort;
    msgflo::OutPort *statePort;

    State state;
    const Config cfg;

public:
    ToolLockParticipant(const char *role, int sensor, int lock, int led)
        : Participant("bitraf/ToolLock", role)
        , cfg { sensor, lock, led }
    {
      this->icon = "lock";
      
      statePort = this->outport("state", "string");
      errorPort = this->outport("error", "string");
      lockPort = this->inport("unlock", "boolean", [this](byte * data, int length) -> void {
        const std::string in((char *)data, length);
        const boolean requestUnlock = (in == "1" || in == "true");
        if (!requestUnlock) {
           errorPort->send("Error: Unknown unlock message: " + String(in.c_str()));
           return;
        }
        if (state.lock.state != lockState::toolPresent) {
          errorPort->send("Error: Tool is not present, cannot unlock: " + String(stateNames[state.lock.state]));
          return;
        }
        state = updateState(cfg, state, statePort, requestUnlock);
      });
       
    }

  void setup() {
    pinMode(cfg.sensorPin, INPUT);
    pinMode(cfg.lockPin, OUTPUT);
    pinMode(cfg.statusPin, OUTPUT);
    digitalWrite(cfg.lockPin, LOW);
  }

  void loop() {
      state = updateState(cfg, state, statePort);
  }
};
