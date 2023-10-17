#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "../Style.h"
#include "../../general/Fun.h"

class ComboBox : public juce::ComboBox, public HasStyle {
  public:
    ComboBox(std::string chan, std::vector<std::string> names) : juce::ComboBox(chan) {
      int counter = 1;
      forEach<std::string>(names, [this, &counter] (auto name) {
        this->addItem(name, counter);
        counter++;
      });
    }

    bool hasColor(HasStyle::ColorId colId) override {
      return colId == HasStyle::ColorId::First
          || colId == HasStyle::ColorId::Background;
    }

    void setColor(HasStyle::ColorId colId, juce::Colour c) override {
      if (colId == HasStyle::ColorId::First) {
        setColour(juce::ComboBox::textColourId, c);
      }
      if (colId == HasStyle::ColorId::Background) {
        setColour(juce::ComboBox::backgroundColourId, c);
      }
    }

    bool hasText() override { return true; }
    void setTextAlign(juce::Justification align) override {
      setJustificationType(align);
    }
    // TODO
    void setTextSize(double) override {}
    void setFont(juce::Font) override {}
};
