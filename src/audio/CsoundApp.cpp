#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "CsoundApp.h"

CsdApp::CsdApp(CsdProcessor* csdProcessor) : mainProcessor(csdProcessor) {
  auto inputDevice  = juce::MidiInput::getDefaultDevice();
  auto outputDevice = juce::MidiOutput::getDefaultDevice();

  deviceManager.initialiseWithDefaultDevices (2, 2);                          // [1]
  deviceManager.addAudioCallback (&player);                                   // [2]
  deviceManager.setMidiInputDeviceEnabled (inputDevice.identifier, true);
  deviceManager.addMidiInputDeviceCallback (inputDevice.identifier, &player); // [3]
  deviceManager.setDefaultMidiOutputDevice (outputDevice.identifier);

  player.setProcessor (mainProcessor);                                  // [4]

  setSize (600, 400);
}

CsdApp::~CsdApp() 
{
  auto device = juce::MidiInput::getDefaultDevice();

  deviceManager.removeAudioCallback (&player);
  deviceManager.setMidiInputDeviceEnabled (device.identifier, false);
  deviceManager.removeMidiInputDeviceCallback (device.identifier, &player);
};
