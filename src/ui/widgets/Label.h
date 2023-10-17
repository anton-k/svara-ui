#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "../Style.h"

class Label : public juce::Label, public HasStyle {
  public:
    Label(std::string val): juce::Label() {
      setName(val);
      setText(val, juce::dontSendNotification);
      juce::Label::setFont(juce::Font(16.0f, juce::Font::plain));
    }

    bool hasColor(HasStyle::ColorId colId) override {
      return colId == HasStyle::ColorId::First;
    }

    void setColor(HasStyle::ColorId colId, juce::Colour col) override {
      if (colId == HasStyle::ColorId::First) {
        setColour(juce::Label::textColourId, col);
      }
    }

    bool hasText() override { return true; }

    void setTextAlign(juce::Justification justType) override {
      setJustificationType (justType);
    }
    void setTextSize(double size) override {
      juce::Font f = getFont();
      f.setHeight((float) size);
      setFont(f);
    }

    void setFont(juce::Font f) override {
      juce::Label::setFont(f);
    }
};
