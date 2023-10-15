// https://docs.juce.com/master/tutorial_audio_processor_graph.html
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "csound.hpp"
#include "../ui/App.h"

class CsdIndex {
  public:
    CsdIndex(Csound* _csound): index(0), ksmps(_csound->GetKsmps()), csound(_csound)
    {
      index = _csound->GetKsmps();
      ksmps = index;
    }

    void next() {
      index++;
      if (index >= ksmps) {
        index = 0;
        isActiveFlag = csound->PerformKsmps() == 0;
      }
    }

    int getIndex() {
      return index;
    }

    bool isActive() { return isActiveFlag; }

    void stop() { isActiveFlag = false; }

  private:
    bool isActiveFlag = true;
    int index = 0;
    int ksmps = 0;
    Csound* csound;
};

class CsdBuffer {
  public:
    CsdBuffer (Csound* csound)
    {
		  spout = csound->GetSpout();
		  spin = csound->GetSpin();
		  scale = csound->Get0dBFS();
		  pos = 0;
    };

    template<typename Type>
    void writeInput(juce::AudioBuffer<Type> &buffer, int samplePosition, bool isActive);

    template<typename Type>
    void readOutput(juce::AudioBuffer<Type> &buffer, int samplePosition, bool isActive);

    void setPos(int _pos);

  private:
    MYFLT *spin = nullptr;
    MYFLT *spout = nullptr;
    MYFLT scale = 1.0;
    int pos = 0;
};

class CsdMidi {
  public:
    CsdMidi() {};

    void writeInput(juce::MidiBuffer::Iterator &iter, int samplePosition);
    void readOutput(juce::MidiBuffer&);
    juce::MidiBuffer midiOutputBuffer = {};
    juce::MidiBuffer midiBuffer = {};
};


// cabbage : Source/Audio/Plugins/CsoundPluginProcessor.h
class CsdProcessor : public juce::AudioProcessor /*, public juce::AsyncUpdater */
{
  public:
    CsdProcessor() {};

    virtual juce::AudioProcessorEditor* createEditor() override;
    virtual bool hasEditor() const override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    virtual void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;
    virtual void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    template<typename Type>
    void processSamples(juce::AudioBuffer<Type>& buffer, juce::MidiBuffer& midiMessages);

    virtual void getStateInformation (juce::MemoryBlock& destData) override;
    virtual void setStateInformation (const void* data, int sizeInBytes) override;

    const juce::String getName() const override { return "Csound plugin"; }
    bool acceptsMidi() const override                      { return true; }
    bool producesMidi() const override                     { return true; }
    double getTailLengthSeconds() const override           { return 0; }

    int getNumPrograms() override                          { return 1; }
    int getCurrentProgram() override                       { return 0; }
    void setCurrentProgram (int) override                  {}
    const juce::String getProgramName (int) override             { return "None"; }
    void changeProgramName (int, const juce::String&) override   {}

    bool supportsSidechain = false;
    int numSideChainChannels = 0;

    void setup(juce::File);
    void resetCsound();
    void stop() { index->stop(); csound->Stop(); }

    //Csound API functions for deailing with midi input
    static int OpenMidiInputDevice (CSOUND* csnd, void** userData, const char* devName);
    static int OpenMidiOutputDevice (CSOUND* csnd, void** userData, const char* devName);
    static int ReadMidiData (CSOUND* csound, void* userData, unsigned char* mbuf, int nbytes);
    static int WriteMidiData (CSOUND* csound, void* userData, const unsigned char* mbuf, int nbytes);

    bool hasFile() {
      return compileResult;
    }

    template<typename Type>
    void muteExtraChannels(int outputCount, juce::AudioBuffer<Type> &buffer);

    void compileCsdFile (juce::File csoundFile)
    {
      compileResult = csound->Compile (csoundFile.getFullPathName().toUTF8().getAddress()) == 0;
    }

    std::unique_ptr<App> app;
    std::unique_ptr<Csound> csound;
    std::unique_ptr<CSOUND_PARAMS> csoundParams;
    std::unique_ptr<CsdModel> csoundModel;
    juce::File csdFile = {};

  private:
    int numCsoundInputs = 0;
    int numCsoundOutputs = 0;
    int preferredLatency = 32;
    CsdBuffer* ioBuffer;
    CsdMidi* midi;
    CsdIndex* index;
    int csdSampleRate = 44100;
    bool compileResult = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CsdProcessor)
};

class CsdEditor : public juce::AudioProcessorEditor {
 public:
     explicit CsdEditor (CsdProcessor&);
//     ~CsdEditor() override;

     //==============================================================================
     void paint (juce::Graphics&) override;
     void resized() override;

 private:
     CsdProcessor& csoundProcessor;

     JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CsdEditor)
};
