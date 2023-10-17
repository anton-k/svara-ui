#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "../Style.h"

class Slider : public juce::Slider, public HasStyle, public HasValue {
  public:
    Slider(juce::Rectangle<float> rect) :
      juce::Slider(
          (rect.getWidth() < rect.getHeight())
          ? juce::Slider::SliderStyle::LinearVertical
          : juce::Slider::SliderStyle::LinearHorizontal,
          juce::Slider::TextEntryBoxPosition::NoTextBox)
    {}

    // state update
    void setValueChange(Proc proc) override { onValueChange = proc; }
    double getCurrentValue() override { return getValue(); }
    void setCurrentValue(double v) override { setValue(v); }
    void setValueRange(double a, double b) override { setRange(a, b); }
    Parser::Widget::Type getWidgetType() override { return Parser::Widget::Auto; }

    // style
    bool hasColor(HasStyle::ColorId colId) override {
      return colId == HasStyle::ColorId::First;
    }

    void setColor(HasStyle::ColorId colId, juce::Colour col) override {
      if (colId == HasStyle::ColorId::First) {
        setColour(juce::Slider::trackColourId, col);
        setColour(juce::Slider::thumbColourId, col);
      }
    }

    bool hasText() override { return false; }
    void setTextSize(double) override {}
    void setFont(juce::Font) override {}
    void setTextAlign(juce::Justification) override {}
};

