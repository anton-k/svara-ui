#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "CsoundProcessor.h"

using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
using Node = juce::AudioProcessorGraph::Node;

class CsdApp : public juce::Component
{
  public:
    CsdApp(CsdProcessor*);
    ~CsdApp();

  private:
    juce::AudioDeviceManager deviceManager;
    juce::AudioProcessorPlayer player;
    CsdProcessor* mainProcessor;
};


