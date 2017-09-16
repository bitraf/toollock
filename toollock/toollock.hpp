
// TODO: use a namespace


struct LockConfig {
  const int takeToolTime = 5*1000; 
};

// FIXME: use an enum class
enum lockState {
  locked = 0,
  takeTool,
  toolTaken,
  lockInit,
  n_states
};
const char *stateNames[n_states] = {
  "locked",
  "takeTool",
  "toolTaken",
  "lockInit"
};

struct LockState {
  lockState state = lockState::lockInit;
  int takeToolTimeout = 0;
};


struct Input {
  bool unlockRequested;
  bool keyPresent;
  long currentTime;
};

LockState nextState(const LockConfig &config, const LockState &current, const Input &input) {
  LockState state = current;
  // TODO: rewrite to switch on State?

  // Unlocking
  if (input.unlockRequested) {
    if (current.state == lockState::locked) {
      state.state = lockState::takeTool;
      state.takeToolTimeout = input.currentTime + config.takeToolTime;
    } else {
      // XXX: probably should treat as error?
    }
  }

  // Starting up
  if (current.state == lockState::lockInit) {
    if (input.keyPresent) {
      state.state = lockState::locked;
    } else {
      state.state = lockState::toolTaken;
    }
  }

  // Tool gets taken
  if (!input.keyPresent && current.state == lockState::takeTool) {
     state.state = lockState::toolTaken;
  }
  
  // Tool gets returned
  if (input.keyPresent && current.state == lockState::toolTaken){
    state.state = lockState::locked;
  }
      
  // 
  if (input.currentTime > current.takeToolTimeout && current.state == lockState::takeTool) {
    state.state = lockState::locked;
  }

  // TODO: check for statechange. If changed, send right away. Else only send every 5 seconds or so

  return state;
}




  

