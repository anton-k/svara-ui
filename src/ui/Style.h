#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "parser/Parser.h"
#include "App.h"

class HasStyle {
  public:
    enum ColorId {
      First,
      Second,
      Background
    };

    // colors
    virtual void setColor(ColorId, juce::Colour) = 0;
    virtual bool hasColor(ColorId) = 0;

    // font
    virtual bool hasText() = 0;
    virtual void setTextSize(double) = 0;
    virtual void setFont(juce::Font) = 0;
    virtual void setTextAlign(juce::Justification) = 0;

    void setStyle(App* app, Parser::Style &style);
};
