#include "MainComponent.h"
#include "model/Model.h"
#include "parser/Parser.h"
#include <plog/Log.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Formatters/MessageOnlyFormatter.h>
#include "widgets/KeyPressListener.h"

#include "csound.hpp"
#include "csPerfThread.hpp"

//==============================================================================

MainComponent::MainComponent(App* _app, Csound* _csound, CsoundPerformanceThread* _csoundPerformanceThread)
  : app(_app), csound(_csound), csoundPerformanceThread(_csoundPerformanceThread)
  {
    setWantsKeyboardFocus(true);
    plog::init<plog::MessageOnlyFormatter>(plog::verbose, plog::streamStdOut);
    PLOG_INFO << "Start app";

    app->scene->setup(this);
    setSize(app->config->windowWidth, app->config->windowHeight);

    onKeyEvent = [this](auto event) { app->scene->onKeyEvent(event); };
  };

void MainComponent::resized()
{
  setBounds (0, 0, getWidth(), getHeight());
  app->resized();
};
