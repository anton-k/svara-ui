#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "../Style.h"

class Toggle : public juce::TextButton, public HasStyle {
  public:
    Toggle(std::string title) : juce::TextButton(title)
    {
      setToggleable(true);
      setClickingTogglesState(true);
    }

    bool hasColor(HasStyle::ColorId colId) override {
      return  colId == HasStyle::ColorId::First || colId == HasStyle::ColorId::Second;
    }

    void setColor(HasStyle::ColorId colId, juce::Colour col) override {
      if (colId == HasStyle::ColorId::First) {
        setColour(juce::TextButton::buttonColourId, col);
      }
      if (colId == HasStyle::ColorId::Second) {
        setColour(juce::TextButton::buttonOnColourId, col);
        setColour(juce::TextButton::textColourOffId, col);
      }
    }

    bool hasText() override { return true; }
    // TODO
    void setTextSize(double) override {}
    void setFont(juce::Font) override {}
    void setTextAlign(juce::Justification) override {}

    int counter = 0;
    bool isUser = true;
};
