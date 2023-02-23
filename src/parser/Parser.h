#pragma once
#include "../../libs/yaml-cpp/include/yaml-cpp/yaml.h"
#include <string>

namespace Parser
{
  void check_parser();

  class IsYaml
  {
    public:
      virtual void run(YAML::Node node){ (void)node; };
      void onKey(YAML::Node node, std::string key);
  };

  class Var
  {
    public:
      void forInt(std::string name, int val);
      void forDouble(std::string name, double val);
      void forString(std::string name, std::string val);
  };

  class Chan {
    public:
      Chan(std::string _name): name(_name) {}
      std::string name;
  };

  template <class T>
  class Val
  {
    public:
      Val(): isChanFlag(false), chan(Chan("")), val() {};
      Val(Chan _chan): isChanFlag(true), chan(_chan), val() {};
      Val(T _val): isChanFlag(false), chan(Chan("")), val(_val) {};

      T getVal() { return val; };
      Chan getChan() { return chan; };

      bool isChan() { return isChanFlag; }

    private:
      bool isChanFlag;
      Chan chan;
      T val;
  };

  class InitVars : public IsYaml
  {
    public:
      virtual void intVar(std::string name, int val) { (void)name; (void)val; };
      virtual void doubleVar(std::string name, double val) { (void)name; (void)val; };
      virtual void stringVar(std::string name, std::string val) { (void)name; (void)val; };

      void run(YAML::Node node);
      /*
      void intRangeVar(std::string name, int val, int min, int max);
      void doubleRangeVar(std::string name, double val, double min, double max);
      void enumVar(std::string init, std::string defaultTag, std::vector<std::string> tags);
      */
  };

  class UpdateVars : public IsYaml
  {
    public:
      virtual void setInt(std::string trigger, std::string name, int val) { (void)trigger; (void)name; (void)val; };
      virtual void setDouble(std::string trigger, std::string name, double val) { (void)trigger; (void)name; (void)val; };
      virtual void setString(std::string trigger, std::string name, std::string val) { (void)trigger; (void)name; (void)val; };

      void run(YAML::Node node);
  };

  class State : public IsYaml
  {
    public:
      State() {};
      State(InitVars* _init, UpdateVars* _update): init(_init), update(_update) {}

      void run(YAML::Node node);

      InitVars* init;
      UpdateVars* update;
  };

  class Widget : public IsYaml
  {
    public:
      virtual void knob(std::string name) { (void)name; };
      virtual void slider(std::string name) { (void)name; };
      virtual void xyPad(std::string nameX, std::string nameY) { (void)nameX; (void)nameY; };
      virtual void button(std::string name) { (void)name; };
      virtual void toggle(std::string name) { (void)name; };
      // virtual void buttonRow(std::string name) { (void)name; };
      virtual void label(std::string val) { (void)val; };
      virtual void text(std::string name) { (void)name; };

      void run(YAML::Node node);
  };

  class Layout : public IsYaml
  {
    public:
      virtual void horBegin() {};
      virtual void horEnd() {};
      virtual void verBegin() {};
      virtual void verEnd() {};
      virtual void gridBegin() {};
      virtual void gridEnd() {};
      virtual void scale(Val<double> val) { (void)val; };
      virtual void space() {};

      void run(YAML::Node node);
  };

  class Col
  {
    public:
      Col(): val("") {};
      Col(std::string v): val(v) {};

      std::string val;
  };

  class Pad
  {
    public:
      Pad(): top(0), bottom(0), left(0), right(0) {};
      Pad(int _top, int _bottom, int _left, int _right): top(_top), bottom(_bottom), left(_left), right(_right) {};
      int top, bottom, left, right;
  };

  class Border
  {
    public:
      Border(): width(0), round(0) {};
      Border(int _width, int _round): width(_width), round(_round) {};
      int width, round;
  };

  enum Hint { None, Hover, Top, Right, Left, Bottom };
  Hint toHint(std::string str);
  std::string fromHint(Hint hint);


  class Font
  {
    public:
      Font(std::string _name): name(_name) {};
      std::string name;
  };

  class Style : public IsYaml
  {
    public:
      virtual void color(Val<Col> col) { (void)col; };
      virtual void background(Val<Col> col) { (void)col; };
      virtual void secondaryColor(Val<Col> col) { (void)col; };
      virtual void textSize(Val<int> size) { (void)size; };
      virtual void font(Val<std::string> name) { (void)name; };
      virtual void pad(Pad pad) { (void)pad; };
      virtual void border(Border border) {  (void)border; };
      virtual void hints(Hint val) { (void)val; };

      void run(YAML::Node node);
  };

  class Ui : public IsYaml
  {
    public:
      Ui() {};
      Ui(Widget* _widget, Layout* _layout, Style* _style): widget(_widget), layout(_layout), style(_style) {};

      Widget* widget;
      Layout* layout;
      Style* style;
      void run(YAML::Node node);
  };

  class Config : public IsYaml
  {
    public:
      Config() {};

      virtual void windowSize(int width, int height) { (void)width; (void)height; };

      void run(YAML::Node node);
  };

  class Window : public IsYaml
  {
    public:
      Window();
      Window(State* _state, Ui* _ui, Config* _config): state(_state), ui(_ui), config(_config) {};

      void run(YAML::Node node);

      State* state;
      Ui* ui;
      Config* config;
  };

  void run(Window win, YAML::Node node);
}
