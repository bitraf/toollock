
#include "../toollock/toollock.hpp"

#include <assert.h>
#include <iostream>

int main() {

    const LockConfig config;
    const LockState initial;
    LockState state = initial;

    const Input keyPresent = { false, true, 0 };

    state = nextState(config, state, keyPresent);
    assert(state.state == lockState::locked);

    const Input unlockWithKeyPresent = { true, true, 100 };

    state = nextState(config, state, unlockWithKeyPresent);

    assert(state.state == lockState::takeTool);

    std::cout << "All test passed" << std::endl;
}
