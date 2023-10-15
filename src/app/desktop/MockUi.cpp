#include "MockUi.h"

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <plog/Log.h>

MockUi::MockUi(juce::File uiFile) {
  app = std::make_unique<App> ();
  csound = std::unique_ptr<MockCsdModel>(new MockCsdModel ());

  YAML::Node node = YAML::LoadFile(uiFile.getFullPathName().toRawUTF8());
  initApp(app.get(), csound.get(), node);

  juce::String result = "";

  setWantsKeyboardFocus(true);
  PLOG_INFO << "Start app";

  app->scene->setup(this);
  setSize(app->config->windowWidth, app->config->windowHeight);

  onKeyEvent = [this](auto event) { this->app->scene->onKeyEvent(event); };
}
    
void MockUi::paint(juce::Graphics& g) {
}
    
void MockUi::resized() {
  setBounds (0, 0, getWidth(), getHeight());
  app->resized();
}
