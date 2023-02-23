#include "App.h"
#include "../parser/Parser.h"
#include <string>

// Build Application from YAML-file

//------------------------------------------------------------------------------------- 
// Build state
class BuildInits : public Parser::InitVars {
  public:
    BuildInits() {};
    BuildInits(State* _state): state(_state) {};

    void intVar(std::string name, int val) override { state->insertInt(name, val); };
    
    void doubleVar(std::string name, double val) override { state->insertDouble(name, val); };

    void stringVar(std::string name, std::string val) override { state->insertString(name, val); }
  
  private:
    State* state;
};

class BuildUpdates : public Parser::UpdateVars {
  public:
    BuildUpdates() {};
    BuildUpdates(App* _app): app(_app) {};

    void setInt(std::string trigger, std::string name, int val) override 
    { 
      app->state->appendSetterInt(trigger,name, val);
    };

    void setDouble(std::string trigger, std::string name, double val) override 
    { 
      app->state->appendSetterDouble(trigger, name, val);
    };
    
    void setString(std::string trigger, std::string name, std::string val) override 
    { 
      app->state->appendSetterString(trigger, name, val);
    };

  App* getApp() { return app; }

  private:
    App* app;
};

//------------------------------------------------------------------------------------- 
// Build config

class BuildConfig : public Parser::Config {
  public:
    BuildConfig(App* _app): app(_app) {}

    void windowSize(int height, int width) override 
    {
      app->config->windowHeight = height;
      app->config->windowWidth = width;
    }
  
    App* getApp() { return app; }

  private:
    App* app;
};

//------------------------------------------------------------------------------------- 
// Build UI

App* initApp() 
{
  Config* config = new Config();
  State* state = new State();
  App* app = new App(config, state);
  return app;
}

