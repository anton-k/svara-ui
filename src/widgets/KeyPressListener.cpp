#include "KeyPressListener.h"
#include <vector>
#include <algorithm>
#include <plog/Log.h>

bool KeyEvent::operator== (KeyEvent& that) const
{
  return (this->key == that.key) && (this->isKeyDown == that.isKeyDown);
}

void KeyPressListener::logState() 
{
  PLOG_DEBUG << "  state: last key     : " << lastPress.getTextDescription();
  PLOG_DEBUG << "         last release : " << lastRelease;
  std::for_each(keysDown.begin(), keysDown.end(), [this](auto n) {
      PLOG_DEBUG << n;
  });
}

bool KeyPressListener::keyPressed(const juce::KeyPress& key) 
{
  if (!keysDown.count(key.getKeyCode())) {
    if (key.getKeyCode() != lastPress.getKeyCode() || lastRelease) {
      // PLOG_DEBUG << "key down: " << key.getTextDescription();
      onKeyEvent(KeyEvent(true, key));
    }

    keysDown.insert(key.getKeyCode());
    lastPress = key;
  }
  lastRelease = false;
  return true;
}

bool KeyPressListener::keyStateChanged(bool isKeyDown) 
{
  if (isKeyDown) {
  } else {
    std::vector<juce::juce_wchar> removeKeys;
    std::for_each(keysDown.begin(), keysDown.end(), [this, &removeKeys](auto n) {
      if (not(juce::KeyPress::isKeyCurrentlyDown(n))) {
        removeKeys.push_back(n);
      }
    });

    std::for_each(removeKeys.begin(), removeKeys.end(), [this](auto n) {
      keysDown.erase(n);
      // PLOG_DEBUG << "key up  : " << juce::KeyPress(n).getTextDescription();
      this->onKeyEvent(KeyEvent(false, juce::KeyPress(n)));
    });            
    lastRelease = true;
  }
  return true;
}
