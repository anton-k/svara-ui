#pragma once
#include <juce_gui_extra/juce_gui_extra.h>

class Knob : public juce::Slider, public HasStyle {
  public:
    Knob() : juce::Slider(juce::Slider::SliderStyle::Rotary, juce::Slider::TextEntryBoxPosition::NoTextBox)
    {}

    bool hasColor(HasStyle::ColorId colId) override {
      return colId == HasStyle::ColorId::First;
    }

    void setColor(HasStyle::ColorId colId, juce::Colour col) override {
      if (colId == HasStyle::ColorId::First) {
        setColour(juce::Slider::rotarySliderFillColourId, col);
        setColour(juce::Slider::thumbColourId, col);
      }
    }

    bool hasText() override { return false; }
    void setTextSize(int) override {}
    void setFont(juce::Font) override {}
    void setTextAlign(juce::Justification) override {}
};

