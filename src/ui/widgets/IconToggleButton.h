#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "../Style.h"
#include "FadIcons.h"

class IconToggleButton : public juce::DrawableButton, public HasStyle {
  public:
    IconToggleButton(std::string title, std::string iconName, std::string secondIconName) :
      icon(getIcon(iconName)),
      svg_xml_on(juce::XmlDocument::parse(icon.first)),
      secondIcon(getIcon(secondIconName)),
      svg_xml(juce::XmlDocument::parse(secondIcon.first)),
      juce::DrawableButton(title, juce::DrawableButton::ImageFitted)
    {
      iconImage = juce::Drawable::createFromSVG(*svg_xml); // GET THIS AS DRAWABLE
      iconHoverImage = juce::Drawable::createFromSVG(*svg_xml); // GET THIS AS DRAWABLE
      iconDownImage = juce::Drawable::createFromSVG(*svg_xml); // GET THIS AS DRAWABLE
      iconDisabledImage = juce::Drawable::createFromSVG(*svg_xml); // GET THIS AS DRAWABLE
      iconImageOn = juce::Drawable::createFromSVG(*svg_xml_on); // GET THIS AS DRAWABLE
      iconHoverImageOn = juce::Drawable::createFromSVG(*svg_xml_on); // GET THIS AS DRAWABLE
      iconDownImageOn = juce::Drawable::createFromSVG(*svg_xml_on); // GET THIS AS DRAWABLE
      iconDisabledImageOn = juce::Drawable::createFromSVG(*svg_xml_on); // GET THIS AS DRAWABLE

      setIcons();
      setToggleable(true);
      setClickingTogglesState(true);
    }

    bool hasColor(HasStyle::ColorId colId) override {
      return colId == HasStyle::ColorId::First
          || colId == HasStyle::ColorId::Second
          || colId == HasStyle::ColorId::Background;
    }

    void setIcons() {
      setImages(
        iconImage.get(), iconHoverImage.get(), iconDownImage.get(), iconDisabledImage.get(),
        iconImageOn.get(), iconHoverImageOn.get(), iconDownImageOn.get(), iconDisabledImageOn.get());
    }

    void setColor(HasStyle::ColorId colId, juce::Colour col) override {
      if (colId == HasStyle::ColorId::First) {
        juce::Colour hoverCol = col.darker();
        juce::Colour downCol = hoverCol.darker();
        juce::Colour disabledCol = downCol.darker();

        iconImageOn->replaceColour(iconImageOnColour, col);
        iconImageOnColour = col;

        iconHoverImageOn->replaceColour(iconHoverImageOnColour, hoverCol);
        iconHoverImageOnColour = hoverCol;

        iconDownImageOn->replaceColour(iconDownImageOnColour, downCol);
        iconDownImageOnColour = downCol;

        iconDisabledImageOn->replaceColour(iconDisabledImageOnColour, disabledCol);
        iconDisabledImageOnColour = disabledCol;

        setIcons();
        repaint();
      }
      if (colId == HasStyle::ColorId::Second) {
        juce::Colour hoverCol = col.darker();
        juce::Colour downCol = hoverCol.darker();
        juce::Colour disabledCol = downCol.darker();

        iconImage->replaceColour(iconImageColour, col);
        iconImageColour = col;

        iconHoverImage->replaceColour(iconHoverImageColour, hoverCol);
        iconHoverImageColour = hoverCol;

        iconDownImage->replaceColour(iconDownImageColour, downCol);
        iconDownImageColour = downCol;

        iconDisabledImage->replaceColour(iconDisabledImageColour, disabledCol);
        iconDisabledImageColour = disabledCol;

        setIcons();
        repaint();
      } if (colId == HasStyle::ColorId::Background) {
        setColour(juce::DrawableButton::backgroundColourId, col);
        setColour(juce::DrawableButton::backgroundOnColourId, col);
        repaint();
      }
    }

    bool hasText() override { return false; }
    // TODO
    void setTextSize(double) override {}
    void setFont(juce::Font) override {}
    void setTextAlign(juce::Justification) override {}

    int counter = 0;
    bool isUser = true;
  private:
    Icon icon, secondIcon;
    std::unique_ptr<juce::XmlElement> svg_xml = nullptr;
    std::unique_ptr<juce::XmlElement> svg_xml_on = nullptr;
    std::unique_ptr<juce::Drawable>
        iconImage, iconHoverImage, iconDownImage, iconDisabledImage,
        iconImageOn, iconHoverImageOn, iconDownImageOn, iconDisabledImageOn;

    juce::Colour iconImageColour = juce::Colours::black;
    juce::Colour iconHoverImageColour = juce::Colours::black;
    juce::Colour iconDownImageColour = juce::Colours::black;
    juce::Colour iconDisabledImageColour = juce::Colours::black;

    juce::Colour iconImageOnColour = juce::Colours::black;
    juce::Colour iconHoverImageOnColour = juce::Colours::black;
    juce::Colour iconDownImageOnColour = juce::Colours::black;
    juce::Colour iconDisabledImageOnColour = juce::Colours::black;
};
