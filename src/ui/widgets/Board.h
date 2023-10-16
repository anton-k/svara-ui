#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <vector>
#include <functional>

class GroupBoard : public juce::GroupComponent {
  public:
    void paint(juce::Graphics& g) override {
      g.fillAll(background);
      GroupComponent::paint(g);
    }

    void setBackground(juce::Colour _background) {
      if (_background != background) {
        background = _background;
        repaint();
      }
    }

    juce::Colour background;
};

class Board : public juce::Component {
  public:
    void paint(juce::Graphics& g) override {
      g.fillAll(background);
      Component::paint(g);
    }

    void setBackground(juce::Colour _background) {
      if (_background != background) {
        background = _background;
        repaint();
      }
    }

    juce::Colour background;
};

