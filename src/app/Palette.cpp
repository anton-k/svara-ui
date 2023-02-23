// Beautiful palette constants
#include "App.h"
#include <juce_gui_extra/juce_gui_extra.h>

// Beautiful colors from:
// https://clrs.cc/
Palette::Palette() 
{
  palette["navy"]   = juce::Colour (0xff001f3f);
  palette["blue"]   = juce::Colour (0xff0074d9); 
  palette["aqua"]   = juce::Colour (0xff7FDBFF);
  palette["teal"]   = juce::Colour (0xff39CCCC);

  palette["purple"] = juce::Colour (0xffB10DC9);
  palette["fuchsia"]= juce::Colour (0xffF012BE);
  palette["maroon"] = juce::Colour (0xff85144b);

  palette["red"]    = juce::Colour (0xffff4136);
  palette["orange"] = juce::Colour (0xffFF851B);
  palette["yellow"] = juce::Colour (0xffFFDC00);

  palette["olive"]  = juce::Colour (0xff3D9970);
  palette["green"]  = juce::Colour (0xff2ECC40);
  palette["lime"]   = juce::Colour (0xff01FF70);

  palette["black"]  = juce::Colour (0xff111111);
  palette["gray"]   = juce::Colour (0xffAAAAAA);
  palette["silver"] = juce::Colour (0xffDDDDDD);
  palette["white"]  = juce::Colour (0xffFFFFFF);

  defaultColor = palette["blue"];
}



