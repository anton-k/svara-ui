#pragma once

#include "../model/Model.h"
#include "parser/Parser.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include "widgets/KeyPressListener.h"
#include "csound.hpp"

// Palette is a map from string names to colors
class Palette {
  public:
    Palette();

    juce::Colour fromName(Parser::Col);
  private:
    std::map<std::string, juce::Colour> palette;
    juce::Colour defaultColor;
};

// Application config
class Config {
  public:
    Config(): windowWidth(200), windowHeight(100), palette(Palette()) {};

    int windowWidth, windowHeight;
    Palette palette;
};

class InitApp {
  public:
    InitApp(juce::ArgumentList args);

    std::string csoundFile;
    std::string uiFile;
};

// Box is a container for UI-widget. It has bounding rectangle and visibility
class Box {
  public:
    virtual std::string getName() = 0;

    virtual void setBounds() {};

    virtual void append(std::function<void(juce::Component*)> call) { (void) call; };

    virtual Parser::Rect getRectangle() { return Parser::Rect(0.0, 0.0, 1.0, 1.0); }
    virtual void setVisible(bool isVisible) { (void) isVisible; };
};

// Atomic juce widget
class Widget : public Box {
  public:
    Widget(juce::Component* _widget, juce::Rectangle<float> _rect):
      rect(_rect),
      widget(_widget) {};

    void setBounds() override;

    void append(std::function<void(juce::Component*)> call) override;

    Parser::Rect getRectangle() { return rect; }
    void setVisible(bool) override;

    std::string getName() override;

  private:
    Parser::Rect rect;
    juce::Component* widget;
};


// Container of widgets for layout
class GroupBox : public Box {
  public:
    virtual void push_back(Box* box) { (void) box; };
    virtual void end() {};
};

// Groups widgets and draws names border around them
class Group : public GroupBox {
  public:
    Group(juce::GroupComponent* _group, juce::Rectangle<float> _rect, bool _hasBorder):
      rect(_rect),
      group(_group),
      children(std::vector<Box*>()),
      hasBorder(_hasBorder)
    {}

    Group(Parser::Rect _rect, std::string name, bool _hasBorder)
    {
      hasBorder = _hasBorder;

      if (hasBorder) {
        juce::GroupComponent* widget = new juce::GroupComponent();
        if (name.size() > 0) {
          widget->setText(juce::String(name));
          widget->setName(juce::String(name));
        }
        group = widget;
      } else {
        group = new juce::Component();
        group->setName(name);
      }

      children = std::vector<Box*>();
      rect = _rect;
    }

    void setBounds() override;

    void append(std::function<void(juce::Component*)> call) override;

    void setVisible(bool) override;

    void push_back(Box* box) override;

    std::string getName() override;

    void end() override {};

    Parser::Rect getRectangle() { return rect; }

    juce::Component* getGroupWidget() { return group; }

  private:
    Parser::Rect rect;
    juce::Component* group;
    std::vector<Box*> children;
    bool hasBorder;
};

// With panels we can switch visibility of group of widgets
// It is useful to implement tab like functionality.
class Panel : public GroupBox {
  public:
    Panel(Parser::Rect _rect, std::string _name):
      rect(_rect),
      selected(0),
      panels(std::vector<Group*>()),
      name(_name)
    {};

    void setBounds() override;

    void append(std::function<void(juce::Component*)> call) override;

    void push_back(Box* box) override;

    void setVisible(bool) override;

    void end() override
    {
      panels.pop_back();
    }

    void initItem();

    void selectVisible(size_t choice);

    std::string getName() override;

    size_t getSize()
    {
      return panels.size();
    };

  private:
    void setVisiblePanel(size_t n, bool isVisible);
    std::string getGroupName();

    Parser::Rect rect;
    size_t selected;
    std::vector<Group*> panels;
    std::string name;
};

class Scene
{
public:
    friend class App;

    //==============================================================================
    Scene():
      widgets(std::vector<Box*>()),
      groupStack(std::vector<GroupBox*>()),
      onKey(Callback<KeyEvent>())
    {};

    //==============================================================================
//    void paint (juce::Graphics&) override;
    void resized();

    void addWidget(juce::Component* comp, Parser::Rect rect);

    void setup(juce::Component* parent);

    void appendKeyListener(KeyEvent event, Procedure*);
    void onKeyEvent(KeyEvent event);

private:
    //==============================================================================
    // Your private member variables go here...
    void append(Box* box);
    std::vector<Box*> widgets;
    std::vector<GroupBox*> groupStack;
    Callback<KeyEvent> onKey;
};


class App {
  public:
    App(): config(new Config()), state(new State()), scene(new Scene()) {};
    App(Config* _config, State* _state): config(_config), state(_state) {}

    juce::Colour findColor(Parser::Col col)
    {
      return config->palette.fromName(col);
    };

    Expr<juce::Colour> colorExpr(Parser::Val<Parser::Col> val)
    {
      return map<Parser::Col, juce::Colour>
          ([this] (auto col) { return this->findColor(col);},
           Parser::toColExpr(val, this->state));
    }

    // groups
    juce::Component* groupBegin(Parser::Rect rect, std::string name = "");
    void groupEnd();

    // panels
    Panel* panelBegin(Parser::Rect rect, std::string name);
    Panel* panelEnd();
    void panelItemBegin();
    void panelItemEnd();

    void setJustificationType (Parser::Val<std::string> val, std::function<void(juce::Justification)>);
    void setColor(Parser::Val<Parser::Col> col, std::function<void(juce::Colour)> setter);

    void resized()
    {
      scene->resized();
    };

    void setup(Csound* csound, YAML::Node &node);

    Config* config;
    State* state;
    Scene* scene;
};

void initApp(App* app, Csound* csound, YAML::Node &node);
