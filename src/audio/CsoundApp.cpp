#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "CsoundApp.h"
#include <plog/Log.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Formatters/MessageOnlyFormatter.h>

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
  auto csoundProcessor = static_cast<CsdProcessor*>(mainProcessor);
  juce::String result = "";

  bool isOk = Parser::readUiDef (csoundProcessor->csdFile, result);
  std::cout << "Parse YAML: " << isOk << "\n";
  if (isOk) {
    std::cout << result << "\n";
    YAML::Node node = YAML::Load(result.toRawUTF8());
    initApp(csoundProcessor->app.get(), csoundProcessor->csound.get(), node);

    setWantsKeyboardFocus(true);
    plog::init<plog::MessageOnlyFormatter>(plog::verbose, plog::streamStdOut);
    PLOG_INFO << "Start app";

    csoundProcessor->app->scene->setup(this);
    setSize(csoundProcessor->app->config->windowWidth, csoundProcessor->app->config->windowHeight);

    // onKeyEvent = [this](auto event) { app->scene->onKeyEvent(event); };
  }
    
  setSize (400, 300);
}
    
void CsdApp::paint(juce::Graphics& g) {
  // editor->paint(g);
}
    
void CsdApp::resized() {
  auto csoundProcessor = static_cast<CsdProcessor*>(mainProcessor);
  setBounds (0, 0, getWidth(), getHeight());
  csoundProcessor->app->resized();
}

CsdApp::~CsdApp() 
{
  auto device = juce::MidiInput::getDefaultDevice();

  deviceManager.removeAudioCallback (&player);
  deviceManager.setMidiInputDeviceEnabled (device.identifier, false);
  deviceManager.removeMidiInputDeviceCallback (device.identifier, &player);
};
