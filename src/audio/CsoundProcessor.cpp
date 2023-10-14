#include "CsoundProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <algorithm>
#include <plog/Log.h>

void CsdProcessor::processBlock(juce::AudioBuffer< float >& buffer, juce::MidiBuffer& midiMessages)
{
  processSamples(buffer, midiMessages);
}

void CsdProcessor::processBlock(juce::AudioBuffer< double >& buffer, juce::MidiBuffer& midiMessages)
{
  processSamples(buffer, midiMessages);
}

template<typename Type>
void muteChannels(int from, int to, juce::AudioBuffer<Type> &buffer) 
{
  for (int channelsToClear = from; channelsToClear < to; ++channelsToClear) {
    buffer.clear(channelsToClear, 0, buffer.getNumSamples());
  }
}

template< typename Type >
void CsdProcessor::processSamples(juce::AudioBuffer<Type>& buffer, juce::MidiBuffer& midiMessages)
{
  const int numSamples = buffer.getNumSamples();
  const int outputCount = std::min(numCsoundOutputs, getTotalNumOutputChannels());
  const int inputCount = std::min(numCsoundInputs, getTotalNumInputChannels());

  //if no inputs are used clear buffer in case it's not empty..
  if (getTotalNumInputChannels() == 0) {
     buffer.clear();
  }
  juce::MidiBuffer::Iterator iter(midiMessages);

  if (hasFile()){
    muteChannels<Type>(outputCount, getTotalNumOutputChannels(), buffer);
      
    const int numOutputBuses = getBusCount(false);
    const int numInputBuses = getBusCount(true);

    for (int samplePosition = 0; samplePosition < numSamples; samplePosition++) {
      index->next();

      /*
      midi->writeInput(iter, samplePosition);
      // process input samples
      ioBuffer->setPos(index->getIndex() * inputCount);
      for (int busIndex = 0; busIndex < numInputBuses; busIndex++) {
          auto inputBus = getBusBuffer(buffer, true, busIndex);
          ioBuffer->writeInput(inputBus, samplePosition, index->isActive());
      }
      */

      // process output samples
      ioBuffer->setPos(index->getIndex() * outputCount);
      for (int busIndex = 0; busIndex < numOutputBuses; busIndex++) {
          auto outputBus = getBusBuffer(buffer, false, busIndex);
          ioBuffer->readOutput(outputBus, samplePosition, index->isActive());
      }
      
    }
  } //if not compiled just mute output
  else {
    muteChannels<Type>(0, getTotalNumOutputChannels(), buffer);
  }
#if JucePlugin_ProducesMidiOutput
  midi->readOutput(midiMessages);
#endif
}

void CsdProcessor::releaseResources() 
{
  stop();
}

void CsdProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) 
{
  csound->SetChannel("HOST_BUFFER_SIZE", samplesPerBlock);
  

  //const int outputs = getBus(false, 0)->getNumberOfChannels();
  if((this->csdSampleRate != sampleRate)
      || numCsoundInputs != getTotalNumInputChannels()
      || numCsoundOutputs != getTotalNumOutputChannels())
  {
    //if sampling rate is other than default or has been changed, recompile..
    this->csdSampleRate = (double)sampleRate;
    setup(csdFile);
  }

  if (preferredLatency == -1)
    this->setLatencySamples(0);
  else
    this->setLatencySamples(preferredLatency == 0 ? csound->GetKsmps() : preferredLatency);
}

void CsdProcessor::resetCsound()
{
   if (csound) {
     csound = nullptr;
     csoundParams = nullptr;
     editorBeingDeleted(this->getActiveEditor());
	   csound = std::make_unique<Csound> ();
	   csoundParams = nullptr;
	   csoundParams = std::make_unique<CSOUND_PARAMS> ();
   } else {
	   csound = std::make_unique<Csound> ();
	   csoundParams = std::make_unique<CSOUND_PARAMS> ();
   }
}

void CsdProcessor::setup(juce::File file)
{
  resetCsound();
  csdFile = file;
  
  compileCsdFile(csdFile);
  if (compileResult) {
    ioBuffer = new CsdBuffer(csound.get());
    index = new CsdIndex(csound.get());
    delete midi;
    midi = new CsdMidi();
    const int inputs = getTotalNumInputChannels() - numSideChainChannels;
    numCsoundInputs = inputs + numSideChainChannels;
    numCsoundOutputs = getTotalNumOutputChannels();
    
    // set csound
	  csound->SetHostImplementedMIDIIO(true);
	  csound->SetHostImplementedAudioIO(1, 0);
	  csound->SetHostData(this);
	  csound->CreateMessageBuffer(0);
	  csound->SetExternalMidiInOpenCallback(OpenMidiInputDevice);
	  csound->SetExternalMidiReadCallback(ReadMidiData);
	  csound->SetExternalMidiOutOpenCallback(OpenMidiOutputDevice);
	  csound->SetExternalMidiWriteCallback(WriteMidiData);
    csound->SetOption((char*)"-n");
    csound->SetOption((char*)"-d");
    csound->SetOption((char*)"-b0");
      
    // set params
	  csoundParams->displays = 0;

    bool hostRequestedMono = getBusesLayout().getMainOutputChannelSet() == juce::AudioChannelSet::mono();
    if(hostRequestedMono) {
        //this mode is for logic and cubase as they both allow weird mono/stereo configs
        numCsoundOutputs = 1;
        numCsoundInputs = 1;
        csoundParams->nchnls_override = 1;
        csoundParams->nchnls_i_override = 1;
    } else {
        csoundParams->nchnls_override = numCsoundOutputs;
        csoundParams->nchnls_i_override = numCsoundInputs;
        // csoundParams->nchnls_override = numCsoundOutputChannels;
        // csoundParams->nchnls_i_override = numCsoundInputChannels;
    }
	
	  csoundParams->sample_rate_override = csdSampleRate;
    if(preferredLatency == -1)
        csoundParams->ksmps_override = 1;
	  csound->SetParams(csoundParams.get());

	  csound->Start();	  
  } else {
    PLOG_ERROR << "Failed to compile Csound file";
  }
}

//------------------------------------------------------------------------------------- 

bool CsdProcessor::hasEditor() const {
     return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CsdProcessor::createEditor()
{
  return new CsdEditor (*this);
}

//======================== CSOUND MIDI FUNCTIONS ================================
int CsdProcessor::OpenMidiInputDevice (CSOUND* csound, void** userData, const char* /*devName*/)
{
  *userData = csoundGetHostData (csound);
  return 0;
}

//==============================================================================
// Opens MIDI output device, adding -QN to your CsOptions will causes this method to be called
// as soon as your plugin loads
//==============================================================================
int CsdProcessor::OpenMidiOutputDevice (CSOUND* csound, void** userData, const char* /*devName*/)
{
  *userData = csoundGetHostData (csound);
  return 0;
}

//==============================================================================
// Write MIDI data to plugin's MIDI output. Each time Csound outputs a midi message this
// method should be called. Note: you must have -Q set in your CsOptions
//==============================================================================
int CsdProcessor::WriteMidiData (CSOUND* /*csound*/, void* _userData,
                                          const unsigned char* mbuf, int nbytes)
{
    auto* userData = static_cast<CsdProcessor*>(_userData);

    if (userData) {
      juce::MidiMessage message (mbuf, nbytes, 0);
      userData->midi->midiOutputBuffer.addEvent (message, 0);
      return nbytes;
    }
    return 0;
}

//==============================================================================
// Reads MIDI input data from host, gets called every time there is MIDI input to our plugin
//==============================================================================
int CsdProcessor::ReadMidiData (CSOUND* /*csound*/, void* userData,
                                        unsigned char* mbuf, int nbytes)
{
  auto* midiData = static_cast<CsdProcessor*>(userData);

  if (userData) {
    int cnt = 0;
    if (!midiData->midi->midiBuffer.isEmpty() && cnt <= (nbytes - 3)) {
      for (const juce::MidiMessageMetadata messageData : midiData->midi->midiBuffer) {
          juce::MidiMessage message = messageData.getMessage();
          const juce::uint8* data = message.getRawData();
          *mbuf++ = *data++;
          *mbuf++ = *data++;
          cnt += 2;

          if (!(message.isChannelPressure() || message.isProgramChange())) {
            *mbuf++ = *data++;
            cnt  += 1;
          }
      }
      midiData->midi->midiBuffer.clear();
    }
    return cnt;
  } else {
    PLOG_DEBUG << "Invalid user data in ReadMidi data";
    return 0;
  }
}

//------------------------------------------------------------------------------------- 
// IO buffers

void CsdBuffer::setPos(int _pos) {
  pos = _pos;
}

template<typename Type>
void CsdBuffer::writeInput(juce::AudioBuffer<Type> &buffer, int samplePosition, bool isActive)
{
  if (!isActive) { return; }

  Type** inputBuffer = (Type**) buffer.getArrayOfWritePointers();

  for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
    //csdPos should go from 0 to (ksmps * number of channels)...
    if(inputBuffer[channel] != nullptr) {
      //DBG("Input Buffer Pos: +" + String(csndPosition) + "\n");
      spin[pos] = inputBuffer[channel][samplePosition] * scale;
    } else {
      spin[pos] = 0;
    }
    pos++;
  }
}

template<typename Type>
void CsdBuffer::readOutput(juce::AudioBuffer<Type> &outputBus, int samplePosition, bool isActive)
{
  Type** outputBuffer = (Type**) outputBus.getArrayOfWritePointers();
  
  if (isActive) {
    for (int channel = 0; channel < outputBus.getNumChannels(); channel++) {
      outputBuffer[channel][samplePosition] = (spout[pos] / scale);
      pos++;
    }
  } else {
    for (int channel = 0; channel < outputBus.getNumChannels(); channel++) {
      outputBuffer[channel][samplePosition] = 0;
      pos++;
    }
  }
}

//------------------------------------------------------------------------------------- 
// MIDI buffers

void CsdMidi::writeInput(juce::MidiBuffer::Iterator &iter, int samplePosition) {
  int samplePos = 0;
  juce::MidiMessage message;

  while (iter.getNextEvent(message, samplePos)) {
    // if current sample position matches time code for MIDI event, 
    // add it to buffer that gets sent to Csound as incoming MIDI...
    if (samplePos == samplePosition) {
      midiBuffer.addEvent(message, samplePos);
    }
  }
  iter.setNextSamplePosition(0);
}

void CsdMidi::readOutput(juce::MidiBuffer& midiMessages) {
  midiMessages.clear();
  if (!midiOutputBuffer.isEmpty()) {
    midiMessages.swapWith(midiOutputBuffer);
  }
}

//------------------------------------------------------------------------------------- 
// internal info storage

void CsdProcessor::getStateInformation (juce::MemoryBlock& destData)
{
  juce::ignoreUnused(destData);
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
}

void CsdProcessor::setStateInformation (const void* data, int sizeInBytes)
{
  juce::ignoreUnused(data, sizeInBytes);
  // You should use this method to restore your parameters from this memory block,
  // whose contents will have been created by the getStateInformation() call.
}

//------------------------------------------------------------------------------------- 
// UI Editor

CsdEditor::CsdEditor (CsdProcessor& p)
    : juce::AudioProcessorEditor (&p), csoundProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

// CsdEditor::~CsdEditor(){}

//==============================================================================
void CsdEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::white);
}

void CsdEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
