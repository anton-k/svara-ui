#pragma once

// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
#include <juce_gui_extra/juce_gui_extra.h>
#include "ui/App.h"
#include "csound.hpp"
#include "csPerfThread.hpp"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public KeyPressListener
{
public:
    //==============================================================================
    MainComponent(App* _app, Csound* _csound, CsoundPerformanceThread* _csoundPerformanceThread);


    //==============================================================================
    void resized() override;
    void checkTime ();
    juce::GroupComponent* group;

    // bool keyStateChanged(bool isKeyDown) override

private:
    //==============================================================================
    // Your private member variables go here...
    //  juce::TextButton checkTheTimeButton;
    // juce::Label timeLabel;
    App* app;
    Csound* csound;
    CsoundPerformanceThread* csoundPerformanceThread;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};



