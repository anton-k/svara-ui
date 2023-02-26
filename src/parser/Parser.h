#pragma once
#include "../../libs/yaml-cpp/include/yaml-cpp/yaml.h"
#include <string>
#include <juce_gui_extra/juce_gui_extra.h>

namespace Parser
{
  void check_parser();

  typedef juce::Rectangle<float> Rect;

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
      bool isVal() { return !isChanFlag; }

    private:
      bool isChanFlag;
      Chan chan;
      T val;
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
      Pad(float _top, float _bottom, float _left, float _right): top(_top), bottom(_bottom), left(_left), right(_right) {};
      float top, bottom, left, right;
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


  class Style {
    public:
      Style():
        color (Val<Col>(Col("blue"))),
        secondaryColor (Val<Col>(Col("blue"))),
        background (Val<Col>(Col("navy"))),
        textSize (18),
        font (""),
        pad (Pad()),
        border(Border()) {}

      Val<Col> color, secondaryColor, background;
      Val<double> textSize;
      Val<std::string> font;
      Pad pad;
      Border border;
      Hint hint;
  };

  class IsYaml
  {
    public:
      virtual void run(YAML::Node node){ (void)node; };
      void onKey(YAML::Node node, std::string key);
  };

  class IsUi
  {
    public:
      virtual void run(YAML::Node node, Rect rect, Style style) { (void)node; (void) rect; (void)style; };
      void onKey(YAML::Node node, std::string key, Rect rect, Style style);
  };

  class InitVars : public IsYaml
  {
    public:
      virtual void intVar(std::string name, int val, bool needDebug) { (void)name; (void)val; (void)needDebug; };
      virtual void doubleVar(std::string name, double val, bool needDebug) { (void)name; (void)val; (void)needDebug; };
      virtual void stringVar(std::string name, std::string val, bool needDebug) { (void)name; (void)val; (void)needDebug; };

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

  class Widget : public IsUi
  {
    public:
      virtual void knob(Style& style, Rect rect, std::string name) { (void) style, (void) rect; (void)name; };
      virtual void slider(Style& style, Rect rect, std::string name) { (void) style; (void) rect; (void)name; };
      virtual void bar(Style& style, Rect rect, std::string name) { (void) style; (void) rect; (void)name; };
      // XYPad impl: https://github.com/seanlikeskites/SAFEJuceModule/blob/master/SAFE_juce_module/UIComponents/XYSlider.h
      virtual void xyPad(Rect rect, std::string nameX, std::string nameY) { (void) rect; (void)nameX; (void)nameY; };
      virtual void button(Rect rect, std::string name) { (void) rect; (void)name; };
      virtual void toggle(Rect rect, std::string name) { (void) rect; (void)name; };
      virtual void checkBox(Rect rect, std::string name) { (void) rect; (void)name; };
      // virtual void buttonRow(std::string name) { (void)name; };
      virtual void label(Style& style, Rect rect, std::string val) { (void)style; (void) rect; (void)val; };
      virtual void text(Style& style, Rect rect, std::string name) { (void)style; (void) rect; (void)name; };
      virtual void space(Rect rect) { (void)rect; };

      // groups: set of items
      virtual void groupBegin(Style& style, Rect rect, std::string name) { (void) style; (void) rect; (void) name; };
      virtual void groupEnd() {};

      // panels. With panels we can toggle visibility of groups of widgets
      virtual void panelBegin(Style& style, Rect rect, std::string name) { (void) style; (void) rect; (void) name; };
      virtual void panelItemBegin() {};
      virtual void panelItemEnd() {};
      virtual void panelEnd(std::string name) { (void) name; };

      void run(YAML::Node node, Rect rect, Style style) override;
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

      void run(YAML::Node node);
  };

  class Ui : public IsUi
  {
    public:
      Ui() {};
      Ui(Widget* _widget, Layout* _layout):
        widget(_widget),
        layout(_layout)
      {};

      void updateStyle(YAML::Node node, Style& style);

      Widget* widget;
      Layout* layout;
      void run(YAML::Node node, Rect rect, Style style) override;
  };

  class Config : public IsYaml
  {
    public:
      Config() {};

      virtual void windowSize(int width, int height) { (void)width; (void)height; };

      void run(YAML::Node node) override;
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
