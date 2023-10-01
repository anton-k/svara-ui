#include "MainComponent.h"
#include "model/Model.h"
#include "parser/Parser.h"
#include <plog/Log.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Formatters/MessageOnlyFormatter.h>
#include "widgets/KeyPressListener.h"

//==============================================================================

MainComponent::MainComponent()
  {
    /*
    setWantsKeyboardFocus(true);
    plog::init<plog::MessageOnlyFormatter>(plog::verbose, plog::streamStdOut);
    PLOG_INFO << "Start app";
    
    YAML::Node node = YAML::LoadFile("examples/tabs.yaml");
    initApp(&app, node);

    app.scene->setup(this);
    setSize(app.config->windowWidth, app.config->windowHeight);

    onKeyEvent = [this](auto event) { app.scene->onKeyEvent(event); };
    */
    setSize(400, 600);
    demo = new juce::ButtonPage(true);
    addAndMakeVisible(demo);
  };

void MainComponent::resized()
{
  setBounds (0, 0, getWidth(), getHeight());
  demo->resized();
//  app.resized();
};