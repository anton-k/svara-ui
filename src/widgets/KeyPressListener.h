#pragma once
#include <functional>
#include <set>
#include <juce_gui_extra/juce_gui_extra.h>

class KeyEvent
{
  public:
    KeyEvent(bool _isKeyDown, juce::KeyPress _key): isKeyDown(_isKeyDown), key(_key) {}
    bool isKeyDown;
    juce::KeyPress key;
    bool operator== (KeyEvent& that) const;
};

class KeyPressListener : public juce::Component
{
  public:
    KeyPressListener():
      onKeyEvent([](auto evt) { (void) evt; }),
      keysDown(std::set<juce::juce_wchar>()),
      lastPress(),
      lastRelease(true)
    {}

    std::function<void(KeyEvent)> onKeyEvent;

    bool keyStateChanged(bool isKeyDown) override;
    bool keyPressed(const juce::KeyPress& key) override;

  private:
    void logState();
    std::set<juce::juce_wchar> keysDown;
    juce::KeyPress lastPress;
    bool lastRelease;
};


