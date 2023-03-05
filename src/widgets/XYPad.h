#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <vector>
#include <functional>


class XYPad : public juce::Component
{
  public:
    XYPad():
      onValueChange([this](auto p) { (void) p; }),
      isDrag(false),
      x(0.5f), y(0.5f),
      cursorColor(juce::Colours::blue),
      frameColor(juce::Colours::orange)
    {}

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;

    std::function<void(juce::Point<float>)> onValueChange;

    void setFrameColor(juce::Colour col) { frameColor = col; }
    void setCursorColor(juce::Colour col) { cursorColor = col; }

    juce::Point<float> getValue() const { return juce::Point<float>(x, y); }

    void setValue(juce::Point<float> p, bool doNotify)
    {
      x = p.x;
      y = p.y;
      if (doNotify) {
        onValueChange(p);
      }
      repaint();
    }

    float getX() const { return x; }
    float getY() const { return y; }

    void setX(float newX, bool doNotify) { setValue(juce::Point<float>(newX, y), doNotify); }
    void setY(float newY, bool doNotify) { setValue(juce::Point<float>(x, newY), doNotify); }

  private:
    bool isWithin(juce::Point<float> p);
    bool isDrag;
    void setMousePoint(const juce::MouseEvent &event);

    float x, y;
    juce::Colour cursorColor, frameColor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYPad)
};
