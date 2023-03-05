#include "Dot.h"
#include <algorithm>
#include <vector>
#include <plog/Log.h>

void Dot::paint(juce::Graphics& g) 
{
  float width = getWidth();
  float height = getHeight();
  float rad = std::min(width, height);
  float cX = width / 2.0f;
  float cY = height / 2.0f;

  g.setColour(color);
  g.fillEllipse ( cX - rad * 0.5f, cY - rad * 0.5f, rad, rad);
}

// ---------------------------------------------
//  Dot meter

juce::Colour DotMeter::getColor() const
{
  size_t size = colors.size() - 1;
  size_t a = std::floor(size * this->val);
  size_t b = a + 1;
  float x = size * this->val - a;

  return colors[a].interpolatedWith(colors[b], x);
}

void DotMeter::paint(juce::Graphics& g) 
{
  float width = getWidth();
  float height = getHeight();
  float rad = std::min(width, height);
  float cX = width / 2.0f;
  float cY = height / 2.0f;

  juce::Colour color = getColor();
  g.setColour(color);
  g.fillEllipse ( cX - rad * 0.5f, cY - rad * 0.5f, rad, rad);
}

