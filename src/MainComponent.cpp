#include "MainComponent.h"
#include "model/Model.h"
#include "parser/Parser.h"

// Beautiful colors from:
// https://clrs.cc/
/*
class Palette
{
public:  
  const juce::Colour blue = juce::Colour (0xff0074d9); 
  const juce::Colour red = juce::Colour (0xffff4136);
  const juce::Colour yellow = juce::Colour (0xffFFDC00);
  const juce::Colour teal = juce::Colour (0xff39CCCC);
  const juce::Colour navy = juce::Colour (0xff001f3f);
};

HouseComponent::HouseComponent () {}
FloorComponent::FloorComponent () {}
SunComponent::SunComponent () {}

void HouseComponent::paint(juce::Graphics& g)
{
    auto p = new Palette ();

    g.setFont (juce::Font (26.0f));
    g.setColour (p->blue);
    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);

    g.fillCheckerBoard (getLocalBounds().toFloat(), 30, 10,
                            juce::Colours::sandybrown, juce::Colours::saddlebrown);

    g.setColour (juce::Colours::red);
 
        juce::Path roof;
        roof.addTriangle (0.0f, (float) getHeight(), (float) getWidth(), (float) getHeight(), (float) getWidth() / 2.0f, 0.0f);
        g.fillPath (roof);
};
void HouseComponent::resized()
{
};

void FloorComponent::paint(juce::Graphics& g)
{
   g.setColour (juce::Colours::green);
   g.drawLine (0.0f, (float) getHeight() / 2.0f, (float) getWidth(), (float) getHeight() / 2.0f, 5.0f);
};
void FloorComponent::resized()
{
};

SceneComponent::SceneComponent()
  {
      addAndMakeVisible (floor);
      addAndMakeVisible (house);
      addAndMakeVisible (smallHouse);
      addAndMakeVisible(sun);
  };

void SunComponent::paint (juce::Graphics& g) 
    {
        g.setColour (juce::Colours::yellow);
 
        auto lineThickness = 3.0f;
        g.drawEllipse (lineThickness * 0.5f,
                       lineThickness * 0.5f,
                       (float) getWidth()  - lineThickness * 2,
                       (float) getHeight() - lineThickness * 2,
                       lineThickness);
    }

void SunComponent::resized(){};

void SceneComponent::paint(juce::Graphics& g)
{
  g.fillAll (juce::Colours::lightblue);
};

void SceneComponent::resized()
{
  floor.setBounds (10, 297, 580, 5);
  house.setBounds (300, 70, 200, 220);
  smallHouse.setBounds (20, 10, 50, 70);
  sun.setBounds (530, 10,  60, 60);
};
*/
//==============================================================================

MainComponent::MainComponent()
  {
    
    Parser::check_parser ();
    /*
    addAndMakeVisible (checkTheTimeButton);
    checkTheTimeButton.setButtonText ("Check the time...");
    // check_model();
 
    addAndMakeVisible (timeLabel);
    timeLabel.setColour (juce::Label::backgroundColourId, juce::Colours::black);
    timeLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    timeLabel.setJustificationType (juce::Justification::centred);
    checkTheTimeButton.onClick = [this] { checkTime(); };
 
    setSize (600, 310);
    */
    YAML::Node node = YAML::LoadFile("config.yaml");
    initApp(&app, node);
    std::cout << "sizes:  "<< app.config->windowWidth  << "  " << app.config->windowHeight << "\n";
    app.scene->setup([this] (juce::Component* widget) { this->addAndMakeVisible(widget); });
    setSize(app.config->windowWidth, app.config->windowHeight);
    std::cout << "Widget size " << app.scene->widgets.size() << "\n";
  
  };


void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
  
  // checkTheTimeButton.setBounds (10, 100, getWidth() - 50, 50);
  // timeLabel.setBounds (10, 10, getWidth() - 50, 50);
  setBounds (0, 0, getWidth(), getHeight());
  app.resized();
};

/*
void MainComponent::checkTime () 
{
  auto currentTime = juce::Time::getCurrentTime();                                          // [4]

  auto includeDate = true;
  auto includeTime = true;
  auto currentTimeString = currentTime.toString (includeDate, includeTime);           // [5]

  timeLabel.setText (currentTimeString, juce::dontSendNotification);                        // [6]
  std::cout << "\n";
  Parser::check_parser ();
};
*/
