#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "../../ui/App.h"
#include "../../ui/model/Model.h"

class MockUi : public KeyPressListener
{
  public:
    MockUi(juce::File);
    void resized() override;

  private:
    std::unique_ptr<App> app;
    std::unique_ptr<CsdModel> csound;
};

