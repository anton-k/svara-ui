#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <vector>
#include <functional>
#include "../Style.h"

class Dot : public juce::Component, public HasStyle {
  public:
    Dot(): color() {}
    Dot(juce::Colour _color): color(_color) {}

    void paint(juce::Graphics& g) override;

    juce::Colour getColor() const { return color;}
    void setColor(juce::Colour c) { color = c; repaint(); }

    bool hasColor(HasStyle::ColorId colId) override {
      return colId == HasStyle::ColorId::First;
    }

    void setColor(HasStyle::ColorId colId, juce::Colour col) override {
      if (colId == HasStyle::ColorId::First) {
        setColor(col);
      }
    }

    bool hasText() override { return false; }
    void setTextSize(int) override {}
    void setFont(juce::Font) override {}
    void setTextAlign(juce::Justification) override {}

  private:
    juce::Colour color;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dot)
};

class DotMeter : public juce::Component {
  public:
    DotMeter(): val(0), colors() {}

    void paint(juce::Graphics& g) override;

    juce::Colour getColor() const;
    float getValue() const { return val; }
    void setValue(float _val) { val = _val; repaint(); }
    void setColors(std::vector<juce::Colour> _colors) { colors = _colors; repaint(); }
  private:
    float val;
    std::vector<juce::Colour> colors;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DotMeter)
};

