#include "XYPad.h"

void XYPad::paint(juce::Graphics& g)
{
  float
    width = getWidth(), 
    height = getHeight(),
    diam = std::min(width, height),
    cursorSize = 0.04 * diam;
  
  g.setColour(cursorColor);
  g.drawLine(x * width, 0.0f + 0.017 * diam, x * width, height - 0.017 * diam, 0.01f * diam);
  g.drawLine(0.017 * diam, y * height, width - 0.017 * diam, y * height, 0.01f * diam);
  g.fillEllipse(x * width - cursorSize, y * height - cursorSize, cursorSize * 2, cursorSize * 2);
  
  g.setColour(frameColor);
  g.drawRoundedRectangle(0.0, 0.0, width, height, 0.035 * diam, 0.017 * diam);
}
    
void XYPad::setMousePoint(const juce::MouseEvent &event)
{
  setValue(juce::Point<float>(
    event.position.x / getWidth(), 
    event.position.y / getHeight()
  ), true);
}
    
void XYPad::mouseDown(const juce::MouseEvent &event)
{
  if (!isDrag) {
    setMousePoint(event);
  }
}
    
bool XYPad::isWithin(juce::Point<float> p)
{
  float
    width = getWidth(), 
    height = getHeight(),
    diam = std::min(width, height),
    cursorSize = 0.035 * diam;
  return 
       (std::abs(x * getWidth() - p.x) + std::abs(y * getHeight() - p.y) < (2 * cursorSize))
    && (p.x >= 0.0f && p.x <= getWidth())
    && (p.y >= 0.0f && p.y <= getHeight());
}

void XYPad::mouseDrag(const juce::MouseEvent &event)
{
  if (isWithin(event.position)) {
    setMousePoint(event);
  }  
}
