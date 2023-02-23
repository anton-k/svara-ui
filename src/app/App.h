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
    Config(): windowWidth(200), windowHeight(100) {};

    int windowWidth, windowHeight;
    Palette* palette;
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

class App {
  public:
    App();
    App(Config* _config, State* _state): config(_config), state(_state) {}

    Config* config;
    Style* style;
    State* state;
};
