#pragma once

#include "../model/Model.h"
#include "../parser/Parser.h"
#include <juce_gui_extra/juce_gui_extra.h>

class Palette {
  public:
    Palette();

    juce::Colour fromName(Parser:: Col);
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

class Style {
  public:
    Style():
      color (Parser::Val<Parser::Col>(Parser::Col("blue"))),
      secondaryColor (Parser::Val<Parser::Col>(Parser::Col("blue"))),
      background (Parser::Val<Parser::Col>(Parser::Col("white"))),
      textSize (14),
      font (""),
      pad (Parser::Pad()),
      border(Parser::Border()) {}

    Parser::Val<Parser::Col> color, secondaryColor, background;
    Parser::Val<int> textSize;
    Parser::Val<std::string> font;
    Parser::Pad pad;
    Parser::Border border;
    Parser::Hint hint;
};


class Box {
  public:
    Box(juce::Rectangle<float> _rect, juce::Component* _widget): rect(_rect), widget(_widget) {};

    void setBounds();

    juce::Rectangle<float> rect;
    juce::Component* widget;
};

class Scene
{
public:
    //==============================================================================
    Scene(): widgets(std::vector<Box*>()) {};

    //==============================================================================
//    void paint (juce::Graphics&) override;
    void resized();

    void addWidget(juce::Component* widget, Parser::Rect rect)
    {
      Box* box = new Box(rect, widget);
      widgets.push_back(box);
    };

    void setup(std::function<void(juce::Component*)> call)
    {
      std::for_each(widgets.begin(), widgets.end(), [this, call] (Box* box) { call(box->widget); });
    }
    std::vector<Box*> widgets;

private:
    //==============================================================================
    // Your private member variables go here...

};


class App {
  public:
    App(): config(new Config()), style(new Style()), state(new State()), scene(new Scene()) {};
    App(Config* _config, State* _state): config(_config), state(_state) {}

    void setup(std::function<void(juce::Component*)> addAndMakeVisible)
    {
      // scene->setSize(config->windowWidth, config->windowHeight);
    }

    void resized()
    {
      scene->resized();
    };

    Config* config;
    Style* style;
    State* state;
    Scene* scene;
};

void initApp(App* app, YAML::Node node);
