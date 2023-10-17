#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <vector>
#include <functional>
#include "../Style.h"
#include "../App.h"

class GroupBoard : public juce::GroupComponent, public HasStyle {
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

    bool hasColor(HasStyle::ColorId colId) override {
      return colId == HasStyle::ColorId::Background || colId == HasStyle::ColorId::First;
    }

    void setColor(HasStyle::ColorId colId, juce::Colour col) override {
      PLOG_DEBUG << "SET GROUP BOARD COLOR: " << colId << " " << col.toString();
      if (colId == HasStyle::ColorId::Background) {
        setBackground(col);
      }

      if (colId == HasStyle::ColorId::First) {
        this->setColour(juce::GroupComponent::outlineColourId, col);
        this->setColour(juce::GroupComponent::textColourId, col);
      }
    }

    bool hasText() override { return true; }
    void setTextSize(double) override {}
    void setFont(juce::Font) override {}

    void setTextAlign(juce::Justification align) override {
      this->setTextLabelPosition(align);
    }

    juce::Colour background;
};

class Board : public juce::Component, public HasStyle {
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

    bool hasColor(HasStyle::ColorId colId) override {
      return colId == HasStyle::ColorId::Background;
    }

    void setColor(HasStyle::ColorId colId, juce::Colour col) override {
      PLOG_DEBUG << "SET BOARD COLOR: " << colId << " " << col.toString();
      if (colId == HasStyle::ColorId::Background) {
        setBackground(col);
      }
    }

    bool hasText() override { return false; }
    void setTextSize(double) override {}
    void setFont(juce::Font) override {}
    void setTextAlign(juce::Justification) override {}

    juce::Colour background;
};

