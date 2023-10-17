#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "../Style.h"

class CheckToggle : public juce::TextButton, public HasStyle {
  public:
    CheckToggle(std::string title) : juce::TextButton(title)
    {
      setToggleable(true);
      setClickingTogglesState(true);
    }

    bool hasColor(HasStyle::ColorId colId) override {
      return  colId == HasStyle::ColorId::First || colId == HasStyle::ColorId::Second;
    }

    void setColor(HasStyle::ColorId colId, juce::Colour c) override {
      if (colId == HasStyle::ColorId::First) {
        setColour(juce::ToggleButton::tickColourId , c);
      }
      if (colId == HasStyle::ColorId::Second) {
        setColour(juce::ToggleButton::tickDisabledColourId, c);
        setColour(juce::ToggleButton::textColourId, c);
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
