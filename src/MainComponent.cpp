#include "MainComponent.h"
#include "model/Model.h"
#include "parser/Parser.h"
#include <plog/Log.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Formatters/MessageOnlyFormatter.h>

//==============================================================================

MainComponent::MainComponent()
  {
    plog::init<plog::MessageOnlyFormatter>(plog::info, plog::streamStdOut);
    PLOG_INFO << "Start app";
    
    YAML::Node node = YAML::LoadFile("examples/config.yaml");
    initApp(&app, node);

    app.scene->setup(this);
    setSize(app.config->windowWidth, app.config->windowHeight);
  };


void MainComponent::resized()
{
  setBounds (0, 0, getWidth(), getHeight());
  app.resized();
};
