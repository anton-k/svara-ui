#include "App.h"
#include "../parser/Parser.h"
#include <juce_gui_extra/juce_gui_extra.h>

juce::Colour Palette::fromName(Parser::Col col) 
{
  if (palette.count(col.val)) {
    return palette[col.val];
  } else {
    return defaultColor;
  }
}

void Box::setBounds() 
{
  widget->setBoundsRelative(rect);
}


void Scene::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    std::for_each(widgets.begin(), widgets.end(), [](auto box) { box->setBounds(); });
};



