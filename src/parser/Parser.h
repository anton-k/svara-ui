#pragma once
#include "../../libs/yaml-cpp/include/yaml-cpp/yaml.h"
#include <string>
#include <juce_gui_extra/juce_gui_extra.h>
#include "../model/Model.h"
#include "../widgets/KeyPressListener.h"

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

  class Col
  {
    public:
      Col(): val("") {};
      Col(std::string v): val(v) {};

      std::string val;
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

  Expr<int> toIntExpr(Val<int> v, State* state);
  Expr<double> toDoubleExpr(Val<double> v, State* state);
  Expr<std::string> toStringExpr(Val<std::string> v, State* state);
  Expr<Col> toColExpr(Val<Col> v, State* state);

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

  juce::Justification toJustification(std::string str);
  std::string fromJustification(juce::Justification align);

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
        secondaryColor (Val<Col>(Col("navy"))),
        background (Val<Col>(Col("black"))),
        textSize (18),
        textAlign ("centered"),
        font (""),
        pad (Pad()),
        border(Border()) {}

      Val<Col> color, secondaryColor, background;
      Val<double> textSize;
      Val<std::string> textAlign;
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

  typedef Procedure Update;

  class InitVars : public IsYaml
  {
    public:
      virtual void intVar(std::string name, int val, bool needDebug) { (void)name; (void)val; (void)needDebug; };
      virtual void doubleVar(std::string name, double val, bool needDebug) { (void)name; (void)val; (void)needDebug; };
      virtual void stringVar(std::string name, std::string val, bool needDebug) { (void)name; (void)val; (void)needDebug; };

      void run(YAML::Node node);
  };

  class UpdateVars : public IsYaml
  {
    public:
      virtual void setInt(std::string trigger, std::string name, int val) { (void)trigger; (void)name; (void)val; };
      virtual void setDouble(std::string trigger, std::string name, double val) { (void)trigger; (void)name; (void)val; };
      virtual void setString(std::string trigger, std::string name, std::string val) { (void)trigger; (void)name; (void)val; };

      virtual void insertUpdater(std::string trigger, Update* setter) { (void) trigger; (void) setter; };

      virtual Type getType(std::string name) = 0;
      virtual Callback<int>* getSetInt(std::string name) = 0;
      virtual Callback<double>* getSetDouble(std::string name) = 0;
      virtual Callback<std::string>* getSetString(std::string name) = 0;

      Update* runUpdater(YAML::Node node);
      void run(YAML::Node node);
  };

  class KeypressUpdate
  {
    public:
      virtual void insertKey(KeyEvent key, Procedure* proc) { (void) key; (void) proc; };
      void run(UpdateVars* updater, YAML::Node node);
  };

  class InitState : public IsYaml
  {
    public:
      InitState() {};
      InitState(InitVars* _init, UpdateVars* _update, KeypressUpdate* _keypress):
        init(_init),
        update(_update),
        keypress(_keypress)
    {}

      void run(YAML::Node node);

      InitVars* init;
      UpdateVars* update;
      KeypressUpdate* keypress;
  };

  class Widget : public IsUi
  {
    public:
      enum Type
      {
        Input,
        Output,
        Through,
        Auto
      };

      virtual void knob(Style& style, Rect rect, std::string name) { (void) style, (void) rect; (void)name; };
      virtual void slider(Style& style, Rect rect, std::string name) { (void) style; (void) rect; (void)name; };
      virtual void bar(Style& style, Rect rect, std::string name, Widget::Type widgetType) { (void) style; (void) rect; (void)name; (void) widgetType; };
      // XYPad impl: https://github.com/seanlikeskites/SAFEJuceModule/blob/master/SAFE_juce_module/UIComponents/XYSlider.h
      virtual void xyPad(Style& style, Rect rect, std::string nameX, std::string nameY) { (void) style; (void) rect; (void)nameX; (void)nameY; };
      virtual void button(Style& style, Rect rect, std::string name, std::string title) { (void) style; (void) rect; (void)name; (void) title; };
      virtual void iconButton(Style& style, Rect rect, std::string name, std::string title) { (void) style; (void) rect; (void)name; (void) title; };
      virtual void toggle(Style& style, Rect rect, std::string name, std::string title) { (void) style; (void) rect; (void)name; (void)title; };
      virtual void pressButton(Style& style, Rect rect, std::string name, std::string title) { (void) style; (void) rect; (void)name; (void)title; };
      virtual void checkToggle(Style& style, Rect rect, std::string name, std::string title) { (void) style; (void) rect; (void)name; (void)title; };
      virtual void checkGroup(Style& style, Rect rect, std::string chan, std::vector<std::string> names, bool isVer) { (void) style; (void) rect; (void) chan; (void)names; (void) isVer; };
      virtual void buttonGroup(Style& style, Rect rect, std::string chan, std::vector<std::string> names, bool isVer) { (void) style; (void) rect; (void) chan; (void)names; (void) isVer; };
      // virtual void buttonRow(std::string name) { (void)name; };
      virtual void label(Style& style, Rect rect, std::string val) { (void)style; (void) rect; (void)val; };
      virtual void text(Style& style, Rect rect, std::string name) { (void)style; (void) rect; (void)name; };
      virtual void space(Rect rect) { (void)rect; };
      virtual void image(Style& style, Rect rect, std::string file) { (void)style; (void) rect; (void) file; };
      virtual void dot(Style& style, Rect rect) { (void) style; (void) rect; }
      virtual void barDisplay(Style& style, Rect rect, std::string chan) { (void) style; (void) rect; (void) chan; }
      virtual void dotMeter(Style& style, Rect rect, std::string chan, std::vector<Col> colors) { (void) style; (void) rect; (void) chan; (void) colors; };
      virtual void barMeter(Style& style, Rect rect, std::string chan, std::vector<Col> colors) { (void) style; (void) rect; (void) chan; (void) colors; };

      virtual void comboBox(Style& style, Rect rect, std::string chan, std::vector<std::string> names) { (void)style; (void)rect; (void) chan; (void) names; };

      // groups: set of items
      virtual void groupBegin(Style& style, Rect rect, std::string name) { (void) style; (void) rect; (void) name; };
      virtual void groupEnd() {};

      // panels. With panels we can toggle visibility of groups of widgets
      virtual void panelBegin(Style& style, Rect rect, std::string name) { (void) style; (void) rect; (void) name; };
      virtual void panelItemBegin() {};
      virtual void panelItemEnd() {};
      virtual void panelEnd(std::string name) { (void) name; };

      void run(YAML::Node node, Rect rect, Style style) override;
      void parseComboBox(YAML::Node node, Rect rect, Style style);
      void parseCheckGroup(YAML::Node node, Rect rect, Style style);
      void parseButtonGroup(YAML::Node node, Rect rect, Style style);
      void parseHorCheckGroup(YAML::Node node, Rect rect, Style style);
      void parseHorButtonGroup(YAML::Node node, Rect rect, Style style);
      void parseVerCheckGroup(YAML::Node node, Rect rect, Style style);
      void parseVerButtonGroup(YAML::Node node, Rect rect, Style style);
      void parseRadioGroupBy(std::string tag, std::function<void(Style&,Rect,std::string,std::vector<std::string>)> call, YAML::Node node, Rect rect, Style style);

      void parseMeterBy(std::string tag, std::function<void(Style&, Rect, std::string, std::vector<Col>)> call, YAML::Node node, Rect rect, Style style);
      void parseDotMeter(YAML::Node node, Rect rect, Style style);
      void parseBarMeter(YAML::Node node, Rect rect, Style style);
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

  class CsoundUi : public IsYaml
  {
    public:
      CsoundUi() {};
      virtual void initWriteChannel(std::string name) = 0; // { (void)name; };
      virtual void initReadChannel(std::string name) = 0 ;// {(void)name; };
      void run(YAML::Node node);
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
      // Window();
      Window(InitState* _state, Ui* _ui, Config* _config, CsoundUi* _csoundUi): state(_state), ui(_ui), config(_config), csoundUi(_csoundUi) {};

      void run(YAML::Node node);

      InitState* state;
      Ui* ui;
      Config* config;
      CsoundUi* csoundUi;
  };

  void run(Window win, YAML::Node node);
}
