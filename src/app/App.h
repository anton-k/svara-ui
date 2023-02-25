#pragma once

#include "../model/Model.h"
#include "../parser/Parser.h"
#include <juce_gui_extra/juce_gui_extra.h>

class Palette {
  public:
    Palette();

    juce::Colour fromName(Parser::Col);
  private:
    std::map<std::string, juce::Colour> palette;
    juce::Colour defaultColor;
};

class Config {
  public:
    Config(): windowWidth(200), windowHeight(100), palette(Palette()) {};

    int windowWidth, windowHeight;
    Palette palette;
};


class Box {
  public:
    virtual void setBounds() {};

    virtual void append(std::function<void(juce::Component*)> call) { (void) call; };

    virtual Parser::Rect getRectangle() { return Parser::Rect(0.0, 0.0, 1.0, 1.0); }
  // virtual void setVisible(bool isVisible) { (void) isVisible; };
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
    // void setVisible()

  private:
    Parser::Rect rect;
    juce::Component* widget;
};

class GroupBox : public Box {
  public:
    virtual void push_back(Box* box) { (void) box; };
    virtual void end() {};
};

// Groups widgets and draws names border around them
class Group : public GroupBox {
  public:
    Group(juce::GroupComponent* _group, juce::Rectangle<float> _rect):
      group(_group),
      children(std::vector<Box*>()),
      rect(_rect) {}

    Group(Parser::Rect rect, std::string name)
    {
      auto group = new juce::GroupComponent();
      if (name.size() > 0) {
        group->setText(juce::String(name));
      }
      Group(group, rect);
    }

    void setBounds() override;

    void append(std::function<void(juce::Component*)> call) override;

    void push_back(Box* box) override;

    void end() override {};

    Parser::Rect getRectangle() { return rect; }

  private:
    Parser::Rect rect;
    juce::GroupComponent* group;
    std::vector<Box*> children;
};

// With panels we can switch visibility of group of widgets
// It is useful to implement tab like functionality.
class Panel : public GroupBox {
  public:
    Panel(Parser::Rect _rect):
      rect(_rect),
      selected(0),
      panels(std::vector<Group*>())
    {};

    Panel(Parser::Rect _rect, std::string _name):
      rect(_rect),
      selected(0),
      panels(std::vector<Group*>())
    {}

    void setBounds() override;

    void append(std::function<void(juce::Component*)> call) override;

    void push_back(Box* box) override;

    void end() override
    {
      panels.pop_back();
    }

    void initItem();

  private:
    Parser::Rect rect;
    int selected;
    std::vector<Group*> panels;
};

class Scene
{
public:
    //==============================================================================
    Scene():
      widgets(std::vector<Box*>()),
      groupStack(std::vector<GroupBox*>())
    {};

    //==============================================================================
//    void paint (juce::Graphics&) override;
    void resized();

    void addWidget(juce::Component* comp, Parser::Rect rect);

    void setup(juce::Component* parent);

    // groups
    void groupBegin(Parser::Rect rect, std::string name = "");
    void groupEnd();

    // panels
    void panelBegin(Parser::Rect rect, std::string name);
    void panelEnd();
    void panelItemBegin();
    void panelItemEnd();

private:
    //==============================================================================
    // Your private member variables go here...
    void append(Box* box);
    std::vector<Box*> widgets;
    std::vector<GroupBox*> groupStack;
};


class App {
  public:
    App(): config(new Config()), state(new State()), scene(new Scene()) {};
    App(Config* _config, State* _state): config(_config), state(_state) {}

    void setup(std::function<void(juce::Component*)> addAndMakeVisible)
    {
      // scene->setSize(config->windowWidth, config->windowHeight);
    }

    juce::Colour findColor(Parser::Col col)
    {
      return config->palette.fromName(col);
    };

    void resized()
    {
      scene->resized();
    };

    Config* config;
    State* state;
    Scene* scene;
};

void initApp(App* app, YAML::Node node);
