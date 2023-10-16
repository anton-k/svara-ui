#include "Meter.h"
#include <algorithm>
#include <vector>
#include <plog/Log.h>

// ---------------------------------------------
//  Bar meter

juce::Colour BarMeter::getColor(float v) const
{
  size_t size = colors.size() - 1;
  size_t a = std::floor(size * v);
  size_t b = a + 1;
  float x = size * v - a;

  return colors[a].interpolatedWith(colors[b], x);
}


juce::Colour BarMeter::getColor() const
{
  return getColor(this->val);
}

void BarMeter::paint(juce::Graphics& g) 
{
  size_t granularity = 4;
  size_t steps = colors.size() * granularity;

  float width = getWidth();
  float height = getHeight();
  float dw = width / steps;
  float dh = height / steps;
  size_t amount = std::floor(steps * this->val);

  if (width > height) {
    for (size_t k=0; k <= amount; k++) {    
      g.setColour(this->getColor((float) k / (float) steps));
      g.fillRect(k * dw, 0.0f, dw + 0.05 * dw, height);    
    }
  } else {
  float v = this->val;
    for (size_t k=0; k <= amount; k++) {    
      g.setColour(this->getColor((float) k / (float) steps));
      g.fillRect(0.0f, (1 - v) * height + (amount - k) * dh, width, dh + 0.05 * dh);    
    }
  }
}

void BarDisplay::paint(juce::Graphics& g) 
{

  float width = getWidth();
  float height = getHeight();
  float diam = std::min (width, height);

  PLOG_DEBUG << "PAINT BAr";
  float v = this->val;
  g.setColour(this->color);
  g.drawRoundedRectangle(0.0f, 0.0f, width, height, diam * 0.2f, diam * 0.1f);
  if (width > height) {
    if (v < 0.99) {
      g.fillRect(0.2f * v * width, 0.0f, width * v * 0.8f, height);
      g.fillRoundedRectangle(0.0f, 0.0f, width * v * 0.5, height, diam * 0.2f);
    } else {
      g.fillRoundedRectangle(0.0f, 0.0f, width * v, height, diam * 0.2f);
    }

  } else {
    if (v < 0.99) {
      g.fillRect(0.0f, (1 - v) * height, width, v * 0.5f * height);
      g.fillRoundedRectangle(0.0f, (1 - v * 0.8f) * height, width, (v * 0.8f) * height , diam * 0.2f);
                  
   //   g.fillRect(0.0f, (1 - 0.2f * v) * height, width, height * (1 - v * 0.8f));
   //   g.fillRoundedRectangle(0.0f, 0.0f, width, height * v * 0.5, diam * 0.2f);
    } else {
      g.fillRoundedRectangle(0.0f, (1 - v) * height, width, v * height , diam * 0.2f);
    }
  }
}

