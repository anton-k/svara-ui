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


class Group : public Box {
  public:
    Group(juce::GroupComponent* _group, juce::Rectangle<float> _rect):
      group(_group),
      children(std::vector<Box*>()),
      rect(_rect) {}

    void setBounds() override;

    void append(std::function<void(juce::Component*)> call) override;

    void push_back(Box* box);

    Parser::Rect getRectangle() { return rect; }

  private:
    Parser::Rect rect;
    juce::GroupComponent* group;
    std::vector<Box*> children;
};


class Scene
{
public:
    //==============================================================================
    Scene():
      widgets(std::vector<Box*>()),
      groupStack(std::vector<Group*>())
    {};

    //==============================================================================
//    void paint (juce::Graphics&) override;
    void resized();

    void addWidget(juce::Component* comp, Parser::Rect rect);

    void setup(juce::Component* parent);

    void groupBegin(Parser::Rect rect, std::string name = "");
    void groupEnd();

private:
    //==============================================================================
    // Your private member variables go here...
    void append(Box* box);
    std::vector<Box*> widgets;
    std::vector<Group*> groupStack;
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
