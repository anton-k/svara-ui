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
    const std::vector<juce::Button*>& getItems() { return buttons; }

    size_t getValue();

    void setValue(size_t choice);

    std::function<void(size_t)> onChange;
    std::vector<juce::Button*> buttons;

  private:
    bool isVertical;
    size_t selected;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToggleGroup)
};
