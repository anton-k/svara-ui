#pragma once

#include "../model/Model.h"

class Config {
  public:
    Config(): windowWidth(200), windowHeight(100) {};
    int windowWidth, windowHeight;
};

class App {
  public:
    App();
    App(Config* _config, State* _state): config(_config), state(_state) {}

    State* state;
    Config* config;
};
