#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <vector>
#include <functional>

class ToggleGroup : public juce::Component
{
  public:
    ToggleGroup(bool _isVertical):
      isVertical (_isVertical),
      selected(0),
      buttons()
    {}

    void resized() override;

    void addItem(juce::Button* button);

    size_t getValue();

    void setValue(size_t choice);

    std::function<void(size_t)> onChange;

  private:
    bool isVertical;
    size_t selected;
    std::vector<juce::Button*> buttons;
};
