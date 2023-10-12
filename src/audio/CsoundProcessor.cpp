#include "CsoundProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <algorithm>

void CsoundProcessor::processBlock(juce::AudioBuffer< float >& buffer, juce::MidiBuffer& midiMessages)
{
  processSamples(buffer, midiMessages);
}

void CsoundProcessor::processBlock(juce::AudioBuffer< double >& buffer, juce::MidiBuffer& midiMessages)
{
  processSamples(buffer, midiMessages);
}

template<typename Type>
void muteExtraChannels(int outputCount, juce::AudioBuffer<Type> &buffer) 
{
  for (int channelsToClear = outputCount; channelsToClear < getTotalNumOutputChannels(); ++channelsToClear) {
    buffer.clear(channelsToClear, 0, buffer.getNumSamples());
  }
}

template< typename Type >
void CsoundProcessor::processSamples(juce::AudioBuffer<Type>& buffer, juce::MidiBuffer& midiMessages)
{
  juce::AudioBuffer<Type> mainInput = getBusBuffer(buffer, true, 0);
  juce::AudioBuffer<Type> mainOutput = getBusBuffer(buffer, false, 0);

  const int numSamples = buffer.getNumSamples();
  const int outputCount = std::min(numCsoundOutputs, getTotalNumOutputChannels());
  const int inputCount = std::min(numCsoundInputs, getTotalNumInputChannels());
  
  //if no inputs are used clear buffer in case it's not empty..
  if (getTotalNumInputChannels() == 0) {
     buffer.clear();
  }

  int samplePos = 0;
  juce::MidiMessage message;
  juce::MidiBuffer::Iterator iter (midiMessages);

  if (hasFile()){
    muteExtraChannels<Type>(outputCount, buffer);

    for (int i = 0; i < numSamples; i++, ++csndIndex) {
      
    }
  }
}


void CsoundProcessor::releaseResources() 
{
}

void CsoundProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) 
{
}


void CsoundProcessor::performCsoundKsmps()
{
  int result = csound->PerformKsmps();

  if (result == 0)
  {
    //slow down calls to these functions, no need for them to be firing at k-rate
    if(polling != 0)
    {
        if (guiCycles > guiRefreshRate)
        {
            guiCycles = 0;
            triggerAsyncUpdate();
        }
        else
            ++guiCycles;
    }
    else{
        //triggerAsyncUpdate();
    }
    //trigger any Csound score event on each k-boundary
    //triggerCsoundEvents();
    sendHostDataToCsound();
  }
  else
  {
    return; //return as soon as Csound has stopped
  }
}

void CsoundProcessor::sendHostDataToCsound()
{
  /* TODO
    if (juce::AudioPlayHead* const ph = getPlayHead())
    {
      juce::AudioPlayHead::CurrentPositionInfo hostPlayHeadInfo = {};

      if (ph->getCurrentPosition (hostPlayHeadInfo)) {
        csound->SetChannel (CabbageIdentifierIds::hostbpm.toUTF8(), hostPlayHeadInfo.bpm);
        csound->SetChannel (CabbageIdentifierIds::timeinseconds.toUTF8(), hostPlayHeadInfo.timeInSeconds);
        csound->SetChannel (CabbageIdentifierIds::isplaying.toUTF8(), hostPlayHeadInfo.isPlaying);
        csound->SetChannel (CabbageIdentifierIds::isrecording.toUTF8(), hostPlayHeadInfo.isRecording);
        csound->SetChannel (CabbageIdentifierIds::hostppqpos.toUTF8(), hostPlayHeadInfo.ppqPosition);
        csound->SetChannel (CabbageIdentifierIds::timeinsamples.toUTF8(), hostPlayHeadInfo.timeInSamples);
        csound->SetChannel (CabbageIdentifierIds::timeSigDenom.toUTF8(), hostPlayHeadInfo.timeSigDenominator);
        csound->SetChannel (CabbageIdentifierIds::timeSigNum.toUTF8(), hostPlayHeadInfo.timeSigNumerator);
      }
    }
  */
}



