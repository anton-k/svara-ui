#include "XYPad.h"

void XYPad::paint(juce::Graphics& g)
{
  float
    width = getWidth(), 
    height = getHeight(),
    diam = std::min(width, height),
    cursorSize = 0.036 * diam;
  
  float frameWidth = 0.017 * diam;
  g.setColour(cursorColor.darker());
  g.drawLine(x * width, 0.8 * frameWidth, x * width, height - frameWidth * 0.8, 0.01f * diam);
  g.drawLine(0.8 * frameWidth, y * height, width - frameWidth * 0.8, y * height, 0.01f * diam);
  g.setColour(cursorColor);
  g.fillEllipse(x * width - cursorSize, y * height - cursorSize, cursorSize * 2, cursorSize * 2);
  
  g.setColour(frameColor);
  g.drawRoundedRectangle(0.0, 0.0, width, height, 0.035 * diam, frameWidth);
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
