#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <vector>
#include "../Style.h"

class BarMeter : public juce::Component /*, HasStyle */ {
  public:
    BarMeter(): val(0), colors() {}

    void paint(juce::Graphics& g) override;

    juce::Colour getColor() const;
    float getValue() const { return val; }
    void setValue(float _val) { val = _val; repaint(); }
    void setColors(std::vector<juce::Colour> _colors) { colors = _colors; repaint(); }

    /* TODO
    bool hasColor(HasStyle::ColorId colId) override {
      return colId == HasStyle::ColorId::First;
    }

    void setColor(HasStyle::ColorId colId, juce::Colour col) override {
      if (colId == HasStyle::ColorId::First) {
        widget->setColor(col);
      }
    }

    bool hasText() override { return false; }
    void setTextSize(double) override {}
    void setFont(juce::Font) override {}
    void setTextAlign(juce::Justification) override {}
    */

  private:
    juce::Colour getColor(float v) const;
    float val;
    std::vector<juce::Colour> colors;
};

class BarDisplay : public juce::Component, public HasStyle {
  public:
    BarDisplay(): val(0), color() {}

    void paint(juce::Graphics& g) override;

    juce::Colour getColor() const { return color;}
    void setColor(juce::Colour c) { color = c; repaint(); }

    float getValue() const { return val; }
    void setValue(float _val) { val = _val; repaint(); }

    bool hasColor(HasStyle::ColorId colId) override {
      return colId == HasStyle::ColorId::First;
    }

    void setColor(HasStyle::ColorId colId, juce::Colour col) override {
      if (colId == HasStyle::ColorId::First) {
        setColor(col);
      }
    }

    bool hasText() override { return false; }
    void setTextSize(double) override {}
    void setFont(juce::Font) override {}
    void setTextAlign(juce::Justification) override {}

  private:
    float val;
    juce::Colour color;
};
