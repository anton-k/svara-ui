#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "../Style.h"
#include "FadIcons.h"

class IconButton : public juce::DrawableButton, public HasStyle, public HasClick {
  public:
    IconButton(std::string title, std::string iconName) :
      icon(getIcon(iconName)),
      svg_xml(juce::XmlDocument::parse(icon.first)),
      juce::DrawableButton(title, juce::DrawableButton::ImageFitted)
    {
      iconImage = juce::Drawable::createFromSVG(*svg_xml); // GET THIS AS DRAWABLE
      iconHoverImage = juce::Drawable::createFromSVG(*svg_xml); // GET THIS AS DRAWABLE
      iconDownImage = juce::Drawable::createFromSVG(*svg_xml); // GET THIS AS DRAWABLE
      setImages(iconImage.get(), iconHoverImage.get(), iconDownImage.get());
    }

    // state update

    bool isDownClick() override {
      return getState() == juce::Button::ButtonState::buttonDown;
    }

    void setOnClick(Proc proc) override {
      onStateChange = proc;
    }
    void triggerClick() override { juce::Button::triggerClick(); }

    // style

    bool hasColor(HasStyle::ColorId colId) override {
      return  colId == HasStyle::ColorId::First;
    }

    void setColor(HasStyle::ColorId colId, juce::Colour col) override {
      if (colId == HasStyle::ColorId::First) {
        juce::Colour hoverCol = col.darker();
        juce::Colour downCol = hoverCol.darker();

        iconImage->replaceColour(iconImageColour, col);
        iconImageColour = col;

        iconHoverImage->replaceColour(iconHoverImageColour, hoverCol);
        iconHoverImageColour = hoverCol;

        iconDownImage->replaceColour(iconDownImageColour, downCol);
        iconDownImageColour = downCol;
        setImages(iconImage.get(), iconHoverImage.get(), iconDownImage.get());

        repaint();
      }
    }

    bool hasText() override { return false; }
    // TODO
    void setTextSize(double) override {}
    void setFont(juce::Font) override {}
    void setTextAlign(juce::Justification) override {}

  private:
    Icon icon;
    std::unique_ptr<juce::XmlElement> svg_xml = nullptr;
    std::unique_ptr<juce::Drawable> iconImage, iconHoverImage, iconDownImage;
    juce::Colour iconImageColour = juce::Colours::black;
    juce::Colour iconHoverImageColour = juce::Colours::black;
    juce::Colour iconDownImageColour = juce::Colours::black;
};
