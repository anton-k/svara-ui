#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <vector>

class BarMeter : public juce::Component {
  public:
    BarMeter(): val(0), colors() {}

    void paint(juce::Graphics& g) override;

    juce::Colour getColor() const;
    float getValue() const { return val; }
    void setValue(float _val) { val = _val; repaint(); }
    void setColors(std::vector<juce::Colour> _colors) { colors = _colors; repaint(); }
  private:
    juce::Colour getColor(float v) const;
    float val;
    std::vector<juce::Colour> colors;
};

class BarDisplay : public juce::Component {
  public:
    BarDisplay(): val(0), color() {}

    void paint(juce::Graphics& g) override;

    juce::Colour getColor() const { return color;}
    void setColor(juce::Colour c) { color = c; repaint(); }

    float getValue() const { return val; }
    void setValue(float _val) { val = _val; repaint(); }
  private:
    float val;
    juce::Colour color;
};

