#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "../Style.h"

class Button : public juce::TextButton, public HasStyle, public HasClick {
  public:
    Button(std::string title) : juce::TextButton(title)
    {}

    // state update

    bool isDownClick() override {
      return getState() == juce::Button::ButtonState::buttonDown;
    }

    void setOnClick(Proc proc) override {
      onStateChange = proc;
    }
    void triggerClick() override { juce::Button::triggerClick(); }

    // style

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
};
