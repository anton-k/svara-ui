#include "Parser.h"
#include "Yaml.h"
#include <iostream>

namespace Parser {
/*
template<class T>
void printVar(std::string name, T val)
{
  PLOG_DEBUG << name << ": " << val << "\n";
}

template<class T>
void printUpdateVar(std::string trigger, std::string name, T val)
{
  PLOG_DEBUG << trigger << " " << name << ": " << val << "\n";
}

class DebugInits : public InitVars {
  public:
    DebugInits() {};
    void intVar(std::string name, int val, bool needDebug) override { printVar(name, val); };
    
    void doubleVar(std::string name, double val, bool needDebug) override { printVar(name, val); };

    void stringVar(std::string name, std::string val, bool needDebug) override { printVar(name, val); }
};

class DebugUpdates : public UpdateVars {
  public:
    DebugUpdates() {};
    void setInt(std::string trigger, std::string name, int val) override { printUpdateVar(trigger, name, val); };

    void setDouble(std::string trigger, std::string name, double val) override { printUpdateVar(trigger, name, val); };
    
    void setString(std::string trigger, std::string name, std::string val) override { printUpdateVar(trigger, name, val); };
};

class DebugConfig : public Config {
  public:
    void windowSize(int height, int width) override 
    {
      PLOG_DEBUG << "window size" << "\n" << "  height: " << height << "\n" << "  width: " << width << "\n";
    }
};

class DebugWidget : public Widget {
  void knob(Style& style, Rect rect, std::string name) override { printVar("knob", name); };
  void slider(Style& style, Rect rect, std::string name) override { printVar("slider", name); };
  void bar(Style& style, Rect rect, std::string name) override { printVar("bar", name); };
  void xyPad(Rect rect, std::string nameX, std::string nameY) override { PLOG_DEBUG << "xy-pad: " << nameX << " " << nameY << "\n"; };
  void button(Rect rect, std::string name) override { printVar("button", name); };
  void toggle(Rect rect, std::string name) override { printVar("toggle", name); };
  // void buttonRow(std::string name) override { (void)name; };
  void label(Style& style, Rect rect, std::string val) override { printVar("label", val); };
  void text(Style& style, Rect rect, std::string name) override { printVar("text", name); };
  void space(Rect rect) override { printVar("space", ""); };
  void groupBegin(Parser::Style& style, Parser::Rect rect, std::string name) override {};
  void groupEnd() override {};
  // panels. With panels we can toggle visibility of groups of widgets
  void panelBegin(Style& style, Rect rect, std::string name) override { (void) style, rect, name; };
  void panelItemBegin()override  {};
  void panelItemEnd()override  {};
  void panelEnd()override {};
};

class DebugStyle : public StyleUpdate {
  void color(Parser::Val<Parser::Col> col) override { printVar("color", col.getVal().val);  };
  void background(Parser::Val<Parser::Col> col) override { printVar("background", col.getVal().val); };
  void secondaryColor(Parser::Val<Parser::Col> col) override { printVar("secondary-color", col.getVal().val); };
  void textSize(Val<int> size) override { printVar("text-size", size.getVal()); };
  void font(Parser::Val<std::string> name) override { printVar("font", name.getVal()); };
  void pad(Pad pad) override { PLOG_DEBUG << "pad: " << pad.top << " " << pad.bottom << " "<< pad.left << " " << pad.right << "\n"; };
  void border(Border border) override { PLOG_DEBUG << "border: " << border.width << " " << border.round << "\n"; };
  void hints(Hint val) override { printVar("hint", fromHint(val)); };
};

class DebugLayout : public Layout {
  void horBegin() override { printVar("hor", "begin"); };
  void horEnd() override { printVar("hor", "end"); };
  void verBegin() override { printVar("ver", "begin"); };
  void verEnd() override { printVar("ver", "end"); };
  void gridBegin() override { printVar("grid", "begin"); };
  void gridEnd() override { printVar("grid", "end"); };
  void scale(Parser::Val<double> val) override { printVar("scale", val.getVal()); };
};


void check_parser() 
{
  YAML::Node node = YAML::LoadFile("config.yaml");
  InitVars* debugInits = new DebugInits();  
  UpdateVars* debugUpdates = new DebugUpdates();
  
  State* debugState = new State(debugInits, debugUpdates);
  Config* debugConfig = new DebugConfig();
  Widget* debugWidget = new DebugWidget();
  Layout* debugLayout = new DebugLayout();
  StyleUpdate* debugStyle = new DebugStyle();
  Ui* debugUi = new Ui(debugWidget, debugLayout, debugStyle);
  Window* debugWindow = new Window(debugState, debugUi, debugConfig);
  debugWindow->run(node);
}
*/
}
