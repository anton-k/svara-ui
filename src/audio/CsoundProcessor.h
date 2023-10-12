#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "csound.hpp"
#include "csPerfThread.hpp"

// cabbage : Source/Audio/Plugins/CsoundPluginProcessor.h
class CsoundProcessor : public juce::AudioProcessor, public juce::AsyncUpdater
{
  public:
    CsoundProcessor(Csound* _csound): csound(_csound)
    {
      numCsoundInputs = csound->GetNchnlsInput();
      numCsoundOutputs = csound->GetNchnls();
    };

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    virtual void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;
    virtual void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    template<typename Type>
    void processSamples(juce::AudioBuffer<Type>& buffer, juce::MidiBuffer& midiMessages);

    virtual void processBlockBypassed (juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) override {
      juce::ignoreUnused(buffer, midiMessages);
    }

  private:
    bool hasFile() {
      return true; // TODO - check that Csound has successfuly compiled file
    }

    template<typename Type>
    void muteExtraChannels(int outputCount, juce::AudioBuffer<Type> &buffer);

    void performCsoundKsmps();
    void sendHostDataToCsound();

    Csound* csound;
    int numCsoundInputs;
    int numCsoundOutputs;
    int index = 0;
    int ksmps = 0;
    int polling = 1;
    int guiCycles = 0;
    int guiRefreshRate = 128;
};


