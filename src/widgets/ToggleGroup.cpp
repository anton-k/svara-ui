#include "ToggleGroup.h"
#include <algorithm>
#include <vector>
#include <plog/Log.h>

void ToggleGroup::addItem(juce::Button* button) 
{
  addAndMakeVisible(button);
  size_t index = buttons.size();
  buttons.push_back(button);
  button->onStateChange = [this, index, button] {
    if (button->getState() == juce::Button::ButtonState::buttonDown) {      
      this->setValue(index);
    } else {
      this->buttons[selected]->setToggleState(true, juce::dontSendNotification);
    }
  };
}

void ToggleGroup::setValue(size_t choice) 
{
  this->buttons[choice]->setToggleState(true, juce::dontSendNotification);
  if (choice != selected) {
    this->buttons[selected]->setToggleState(false, juce::dontSendNotification);
    this->selected = choice;
    this->onChange(choice);
  }
}

size_t ToggleGroup::getValue() 
{
  return selected;
}

void ToggleGroup::resized() 
{
  size_t size = buttons.size();
  float dh = 1.0f;
  float dw = 1.0f;

  if (isVertical) {
    dh = 1.0f / size;
    size_t counter = 0;
    std::for_each(buttons.begin(), buttons.end(), [&counter, dh] (auto button) {
      button->setBoundsRelative(juce::Rectangle(0.0f, counter * dh, 1.0f, dh));
      counter++;
    });
  } else {
    dw = 1.0f / size;
    size_t counter = 0;
    std::for_each(buttons.begin(), buttons.end(), [&counter, dw] (auto button) {
      button->setBoundsRelative(juce::Rectangle(counter * dw, 0.0f, dw, 1.0f));
      counter++;
    });
  }
}
