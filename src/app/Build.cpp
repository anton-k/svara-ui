#include "App.h"
#include "../parser/Parser.h"
#include <string>
#include <iostream>

// Build Application from YAML-file

template<class T>
void printVar(std::string name, T val)
{
  std::cout << name << ": " << val << "\n";
}

//------------------------------------------------------------------------------------- 
// Build state
class BuildInits : public Parser::InitVars {
  public:
    BuildInits() {};
    BuildInits(App* _app): app(_app) {};

    void intVar(std::string name, int val) override { app->state->insertInt(name, val); };
    
    void doubleVar(std::string name, double val) override { app->state->insertDouble(name, val); };

    void stringVar(std::string name, std::string val) override { app->state->insertString(name, val); }
  
  private:
    App* app;
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
  
  private:
    App* app;
};

//------------------------------------------------------------------------------------- 
// Build UI

class BuildStyle : public Parser::Style {
  public:
    BuildStyle(App* _app): app(_app) {}
    void color(Parser::Val<Parser::Col> col) override { app->style->color = col;  };
    void background(Parser::Val<Parser::Col> col) override { app->style->background = col; };
    void secondaryColor(Parser::Val<Parser::Col> col) override { app->style->secondaryColor = col; };
    void textSize(Parser::Val<int> size) override { app->style->textSize = size; };
    void font(Parser::Val<std::string> name) override { app->style->font = name; };
    void pad(Parser::Pad pad) override { app->style->pad = pad; };
    void border(Parser::Border border) override { app->style->border = border; };
    void hints(Parser::Hint val) override { app->style->hint = val; };
  private:
    App* app;
};

class BuildWidget : public Parser::Widget {
  public:
    BuildWidget(App* _app): app(_app) {}
    void knob(std::string name) override { printVar("knob", name); };
    void slider(std::string name) override { printVar("slider", name); };
    void xyPad(std::string nameX, std::string nameY) override { std::cout << "xy-pad: " << nameX << " " << nameY << "\n"; };
    void button(std::string name) override { printVar("button", name); };
    void toggle(std::string name) override { printVar("toggle", name); };
    // void buttonRow(std::string name) override { (void)name; };
    void label(std::string val) override { printVar("label", val); };
    void text(std::string name) override { printVar("text", name); };
  private:
    App* app;
};

class BuildLayout : public Parser::Layout {
  public:
    BuildLayout(App* _app): app(_app) {}
    void horBegin() override { printVar("hor", "begin"); };
    void horEnd() override { printVar("hor", "end"); };
    void verBegin() override { printVar("ver", "begin"); };
    void verEnd() override { printVar("ver", "end"); };
    void gridBegin() override { printVar("grid", "begin"); };
    void gridEnd() override { printVar("grid", "end"); };
    void scale(Parser::Val<double> val) override { printVar("scale", val.getVal()); };
    void space() override { printVar("space", ""); };
  private:
    App* app;
};

//------------------------------------------------------------------------------------- 
// Build App from YAML file

App* initApp(YAML::Node node) 
{
  // init empty app
  Config* config = new Config();
  State* state = new State();
  App* app = new App(config, state);

  // assemble builders
  
  Parser::InitVars* buildInits = new BuildInits(app);  
  Parser::UpdateVars* buildUpdates = new BuildUpdates(app);
  
  Parser::State* buildState = new Parser::State(buildInits, buildUpdates);
  Parser::Config* buildConfig = new BuildConfig(app);
  Parser::Widget* buildWidget = new BuildWidget(app);
  Parser::Layout* buildLayout = new BuildLayout(app);
  Parser::Style* buildStyle = new BuildStyle(app);
  Parser::Ui* buildUi = new Parser::Ui(buildWidget, buildLayout, buildStyle);
  Parser::Window* buildWindow = new Parser::Window(buildState, buildUi, buildConfig);
  buildWindow->run(node);

  return app;
}

