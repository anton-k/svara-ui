#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "../Style.h"
#include "ToggleGroup.h"

class ButtonGroup : public ToggleGroup, public HasStyle {
  public:
    ButtonGroup(bool isVertical, int choice, std::vector<std::string> names) : ToggleGroup(isVertical)
    {
      std::for_each(names.begin(), names.end(), [this, choice] (auto name) {
        juce::TextButton* button = new juce::TextButton(name);
        button->setToggleable(true);
        button->setClickingTogglesState(true);
        button->setToggleState(choice == 1, juce::dontSendNotification);
        this->addItem(button);
      });
      this->setValue((size_t) choice);
    }

    bool hasColor(HasStyle::ColorId colId) override {
      return  colId == HasStyle::ColorId::First || colId == HasStyle::ColorId::Second;
    }

    void setColor(HasStyle::ColorId colId, juce::Colour col) override {
      if (colId == HasStyle::ColorId::First) {
        forEach<juce::Button*>(this->buttons, [col](auto button) {
          button->setColour(juce::TextButton::textColourOffId, col);
          button->setColour(juce::TextButton::buttonOnColourId, col);
        });
      }
      if (colId == HasStyle::ColorId::Second) {
        forEach<juce::Button*>(this->buttons, [col](auto button) {
          button->setColour(juce::TextButton::buttonColourId, col);
        });
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
