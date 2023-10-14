#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "CsoundApp.h"
#include <plog/Log.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Formatters/MessageOnlyFormatter.h>

CsdApp::CsdApp(CsdProcessor* csdProcessor) : mainProcessor(csdProcessor) {
  auto inputDevice  = juce::MidiInput::getDefaultDevice();
  auto outputDevice = juce::MidiOutput::getDefaultDevice();

  deviceManager.initialiseWithDefaultDevices (2, 2);                        
  deviceManager.addAudioCallback (&player);                                   
  deviceManager.setMidiInputDeviceEnabled (inputDevice.identifier, true);
  deviceManager.addMidiInputDeviceCallback (inputDevice.identifier, &player); 
  deviceManager.setDefaultMidiOutputDevice (outputDevice.identifier);

  player.setProcessor (mainProcessor);                                

  setSize (600, 400);
  juce::String result = "";

  bool isOk = Parser::readUiDef (mainProcessor->csdFile, result);
  std::cout << "Parse YAML: " << isOk << "\n";
  if (isOk) {
    std::cout << result << "\n";
    YAML::Node node = YAML::Load(result.toRawUTF8());
    initApp(mainProcessor->app.get(), mainProcessor->csound.get(), node);

    setWantsKeyboardFocus(true);
    plog::init<plog::MessageOnlyFormatter>(plog::verbose, plog::streamStdOut);
    PLOG_INFO << "Start app";

    mainProcessor->app->scene->setup(this);
    setSize(mainProcessor->app->config->windowWidth, mainProcessor->app->config->windowHeight);

    onKeyEvent = [this](auto event) { this->mainProcessor->app->scene->onKeyEvent(event); };
  }
}
    
void CsdApp::paint(juce::Graphics& g) {
}
    
void CsdApp::resized() {
  setBounds (0, 0, getWidth(), getHeight());
  mainProcessor->app->resized();
}

CsdApp::~CsdApp() 
{
  auto device = juce::MidiInput::getDefaultDevice();

  deviceManager.removeAudioCallback (&player);
  deviceManager.setMidiInputDeviceEnabled (device.identifier, false);
  deviceManager.removeMidiInputDeviceCallback (device.identifier, &player);
};
