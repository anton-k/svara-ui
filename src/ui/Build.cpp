//------------------------------------------------------------------------------------- 
// Build App from YAML file

#include "App.h"
#include <string>
#include <iostream>
#include <plog/Log.h>
#include "csound.hpp"
#include <juce_gui_extra/juce_gui_extra.h>

#include <Icons.h>
#include "Style.h"
#include "model/Model.h"
#include "parser/Parser.h"
#include "Value.h"

#include "widgets/ToggleGroup.h"
#include "widgets/Dot.h"
#include "widgets/Meter.h"
#include "widgets/XYPad.h"
#include "widgets/FadIcons.h"
#include "widgets/Board.h"
#include "widgets/Knob.h"
#include "widgets/Slider.h"
#include "widgets/Bar.h"
#include "widgets/Button.h"
#include "widgets/Toggle.h"
#include "widgets/IconButton.h"
#include "widgets/IconToggleButton.h"
#include "widgets/PressButton.h"
#include "widgets/CheckToggle.h"
#include "widgets/ButtonGroup.h"
#include "widgets/Label.h"
#include "widgets/ComboBox.h"

// Build Application from YAML-file

template<class T>
void printVar(std::string name, T val)
{
  PLOG_INFO << name << ": " << val << "\n";
}

Parser::Col readCol(Parser::Val<Parser::Col> val, App* app) {
  if (val.isChan()) {
    return Parser::Col(app->state->getString(val.getChan().name));
  } else {
    return val.getVal();
  }  
}

//------------------------------------------------------------------------------------- 
// Build state
class BuildInits : public Parser::InitVars {
  public:
    BuildInits() {};
    BuildInits(App* _app): app(_app) {};

    void intVar(std::string name, int val, bool needDebug) override { 
      if (needDebug) { PLOG_INFO << "Insert int: " << name << " by " << val; }
      app->state->insertInt(name, val, needDebug); 
    };
    
    void doubleVar(std::string name, double val, bool needDebug) override { 
      if (needDebug) { PLOG_INFO << "Insert double: " << name << " by " << val; }
      app->state->insertDouble(name, val, needDebug); 
    };

    void stringVar(std::string name, std::string val, bool needDebug) override { 
      if (needDebug) { PLOG_INFO << "Insert string: " << name << " by " << val; }
      app->state->insertString(name, val, needDebug); 
    }
  
  private:
    App* app;
};

class BuildUpdates : public Parser::UpdateVars {
  public:
    BuildUpdates() {};
    BuildUpdates(App* _app): app(_app) {};

    void setInt(std::string trigger, std::string name, int val) override 
    { 
      app->state->appendSetterInt(trigger,name, val);
    };

    void setDouble(std::string trigger, std::string name, double val) override 
    { 
      app->state->appendSetterDouble(trigger, name, val);
    };
    
    void setString(std::string trigger, std::string name, std::string val) override 
    { 
      app->state->appendSetterString(trigger, name, val);
    };

    Type getType(std::string name) override
    {
      return app->state->getType(name);
    }

    void insertUpdater(std::string trigger, Procedure* setter) override
    {
      app->state->appendSetter(trigger, setter);
    }

    Callback<int>* getSetInt(std::string name) override
    {
      auto setter =  [this,name] (int val) { this->app->state->setInt(name, val) ;};
      return new Callback<int>(setter);
    }

    Callback<double>* getSetDouble(std::string name) override
    {
      auto setter =  [this,name] (double val) { this->app->state->setDouble(name, val) ;};
      return new Callback<double>(setter);
    }

    Callback<std::string>* getSetString(std::string name) override
    {
      auto setter = [this,name] (std::string val) { this->app->state->setString(name, val) ;};
      return new Callback<std::string>(setter);
    }

  private:
    App* app;
};

class BuildKeypress : public Parser::KeypressUpdate {
  public:
    BuildKeypress() {};
    BuildKeypress(App* _app): app(_app) {};

    void insertKey(KeyEvent key, Procedure* update) override 
    {
      PLOG_DEBUG << "Add key listener: " << key.key.getTextDescription();
      app->scene->appendKeyListener(key, update);
    }

  private:
    App* app;
};

//------------------------------------------------------------------------------------- 
// Build config

class BuildConfig : public Parser::Config {
  public:
    BuildConfig(App* _app): app(_app) {}

    void windowSize(int height, int width) override 
    {
      PLOG_DEBUG << "set sizes\n";
      app->config->windowHeight = height;
      app->config->windowWidth = width;
    }
  
  private:
    App* app;
};

//------------------------------------------------------------------------------------- 
// Build Csound UI

class CsoundChannelReader : public juce::Timer {
  public:
    CsoundChannelReader(App* _app, CsdModel* _csound): 
        app(_app), csound(_csound), 
        doubleChannels(std::vector<std::pair<std::string, Get<MYFLT>>>()),
        intChannels(std::vector<std::pair<std::string, Get<MYFLT>>>()) {}

    void timerCallback() override {
      // update doubles
      std::for_each(doubleChannels.begin(), doubleChannels.end(), [this](auto chan) {
        this->app->state->setDouble(chan.first, (double) chan.second());
      });

      // update ints
      std::for_each(intChannels.begin(), intChannels.end(), [this](auto chan) {
        this->app->state->setInt(chan.first, floor(chan.second()));
      });
    };

    void insertDouble(std::string name) {
      doubleChannels.push_back(std::pair(name, csound->getChannel(name)));
    }

    void insertInt(std::string name) {
      intChannels.push_back(std::pair(name, csound->getChannel(name)));
    }

  private:
    App* app;
    CsdModel* csound;
    std::vector<std::pair<std::string, Get<MYFLT>>> doubleChannels;
    std::vector<std::pair<std::string, Get<MYFLT>>> intChannels;
};

class BuildCsoundUi : public Parser::CsoundUi {
  public:
    BuildCsoundUi(App* _app, CsdModel* _csound, CsoundChannelReader* _channelReader): 
        app(_app), csound(_csound), channelReader(_channelReader) {}
    
    void initWriteChannel(std::string name) override {
      PLOG_DEBUG << "Init csound write channel: " << name;
      switch (app->state->getType(name)) 
      {
        case Type::Int: 
          {
            Set<MYFLT> chan = this->csound->setChannel(name);
            Callback<int>* setter = new Callback<int>(
              [this, chan](int val) { chan((MYFLT) val); }
            );
            app->state->appendCallbackInt(name, setter);
            break;
          }
        case Type::Double:
          {
            Set<MYFLT> chan = this->csound->setChannel(name);
            Callback<double>* setter = new Callback<double>(
              [this, chan](double val) { chan((MYFLT) val); }
            );
            app->state->appendCallbackDouble(name, setter);
            break;
          }
 
        case Type::String: 
          {
            Set<std::string> chan = this->csound->setStringChannel(name);
            Callback<std::string>* setter = new Callback<std::string>(
              [this, chan](std::string val) { chan(val); }
            );
            app->state->appendCallbackString(name, setter);
            break;
          }
      }
    };

    void initReadChannel(std::string name) override {
      PLOG_DEBUG << "Init csound read channel: " << name;
      if (app->state->getType(name) == Type::Double) {
        channelReader->insertDouble(name);
      }
      if (app->state->getType(name) == Type::Int) {
        channelReader->insertInt(name);
      }
      if (app->state->getType(name) == Type::String) {
        PLOG_ERROR << "String read channels are not supported for Csound";
      }
    };

    void initScore(std::string name, Set<int> score) override {
      PLOG_DEBUG << "Init csound score on channel: " << name;
      app->state->appendCallbackInt(name, new Callback<int>([score](int val) { score(val); }));      
    };

    Type getType(std::string name) override
    {
      return app->state->getType(name);
    }
      
    Get<int> getterInt(std::string name) override {
      return [name, this]() { return this->app->state->getInt(name); };
    }

    Get<double> getterDouble(std::string name) override {
      return [name, this]() { return this->app->state->getDouble(name); };
    }

    Get<std::string> getterString(std::string name) override {
      return [name, this]() { return this->app->state->getString(name); };
    }

    void sendScore(std::string sco) override {
      csound->readScore(sco);
    }

  private:
    App* app;
    CsdModel* csound;  
    CsoundChannelReader* channelReader;
};


//------------------------------------------------------------------------------------- 
// Build UI

void setFont(App* app, Parser::Val<std::string> typeface, std::function<juce::Font(void)> getFont, std::function<void(juce::Font)> setFont)
{
  if (typeface.isVal()) {
    juce::Font font = getFont();
    font.setTypefaceName(typeface.getVal());
    setFont(font);
  } else {
    Chan chan = typeface.getChan();
    app->state->appendCallbackString(chan.name, new Callback<std::string>([&getFont,&setFont] (auto face) {
      juce::Font font = getFont();
      font.setTypefaceName(face);
      setFont(font);
    ;}));
  }
}

class BuildWidget : public Parser::Widget {
  public:
    BuildWidget(App* _app): app(_app) {}
    
    void addWidget(Parser::Style &style, juce::Component* widget, juce::Rectangle<float> rect) {
      app->addWidget(style, widget, rect);    
      auto widgetStyle = dynamic_cast<HasStyle*>(widget);
      if (widgetStyle) {
        widgetStyle->setStyle(app, style);
      }
    }

    void knob(Parser::Style& style, Parser::Rect rect, std::string name) override 
    { 
      Knob* knob = new Knob();
      knob->setName(name);
      knob->linkValue(app, name);
      PLOG_DEBUG << "make knob: widget name: " << name << " value: " << app->state->getDouble(name);
      addWidget(style, knob, rect);
    }
    
    void slider(Parser::Style& style, Parser::Rect rect, std::string name) override 
    {
      Slider* slider = new Slider(rect);
      slider->setName(name);
      slider->linkValue(app, name);
      PLOG_DEBUG << "make slider: widget name: " << name << " value: " << app->state->getDouble(name);
      addWidget(style, slider, rect);
    };
    
    void bar(Parser::Style& style, Parser::Rect rect, std::string name, Parser::Widget::Type widgetType) override 
    {
      Bar* slider = new Bar(rect);
      slider->setName(name);
      slider->linkValue(app, name);
      PLOG_DEBUG << "make bar: widget name: " << name << " value: " << app->state->getDouble(name);
      addWidget(style, slider, rect);
    };

    void xyPad(Parser::Style& style, Parser::Rect rect, std::string nameX, std::string nameY) override 
    { 
      (void) nameX; (void) nameY;
      XYPad* widget = new XYPad();

      widget->setValue(juce::Point<float>(app->state->getDouble(nameX), app->state->getDouble(nameY)), false);
      widget->onValueChange = [this, widget, nameX, nameY] (juce::Point<float> p) {
        this->app->state->setDouble(nameX, p.x);
        this->app->state->setDouble(nameY, p.y);
      };

      this->app->state->appendCallbackDouble(nameX, new Callback<double>([widget] (double x) {
        float currentX = widget->getX();
        float choiceX = (float) x;
        if (currentX != choiceX) {
          widget->setX(choiceX, true);
        }
      }));

      this->app->state->appendCallbackDouble(nameY, new Callback<double>([widget] (double y) {
        float currentY = widget->getY();
        float choiceY = (float) y;
        if (currentY != choiceY) {
          widget->setY(choiceY, true);
        }
      }));

      addWidget(style, widget, rect);
    };

    // TODO: see nvim examples/Plugins/AUv3SynthPluginDemo.h
    // JUCE example on how to change font size
    void button(Parser::Style& style, Parser::Rect rect, std::string name, std::string title) override 
    { 
      Button* widget = new Button(title);
      widget->linkValue(app, name);

      PLOG_DEBUG << "make button: " << name << " with text: " << title;
      addWidget(style, widget, rect);
    };

    void toggle(Parser::Style& style, Parser::Rect rect, std::string name, std::string title) override 
    { 
      Toggle* widget = new Toggle(title);
      widget->setToggleState(this->app->state->getInt(name) == 1, juce::dontSendNotification);
     
      auto onColor = style.color;
      auto offColor = style.secondaryColor;
      
      widget->onStateChange = [this,name,widget, onColor, offColor] { 
        if (widget->getState() == juce::Button::ButtonState::buttonDown) {
          int newVal = 1 - this->app->state->getInt(name);
          this->app->state->setInt(name, newVal);
          // we invert toggle state because it was inverted on callback call of state->int
          // without this call UI and state will go out of sync
          widget->setToggleState(!widget->getToggleState(), juce::dontSendNotification);
        }
      };

      this->app->state->appendCallbackInt(name, new Callback<int>([widget](int val) {        
        bool tog = widget->getToggleState();
        widget->setToggleState(val == 1, juce::dontSendNotification);
      }));
      
      addWidget(style, widget, rect);
    };
    
    void iconButton(Parser::Style& style, Parser::Rect rect, std::string name, std::string title) override 
    {
      (void) name; (void) title;
      IconButton* widget = new IconButton(title, style.icon);
      widget->linkValue(app, name);

      PLOG_DEBUG << "make icon toggle button: " << name << " with text: " << title;
      addWidget(style, widget, rect);
    };
     
    void iconToggleButton(Parser::Style& style, Parser::Rect rect, std::string name, std::string title) override 
    {
      (void) name; (void) title;
      IconToggleButton* widget = new IconToggleButton(title, style.icon, style.secondaryIcon);

      widget->setToggleState(this->app->state->getInt(name) == 1, juce::dontSendNotification);
      widget->onStateChange = [&style,this,name,widget] { 
        if (widget->getState() == juce::Button::ButtonState::buttonDown) {
          this->app->state->setInt(name, 1 - this->app->state->getInt(name));
        }
      };
     
      PLOG_DEBUG << "make icon toggle button: " << name << " with text: " << title;
      addWidget(style, widget, rect);
    };
     

    void pressButton(Parser::Style& style, Parser::Rect rect, std::string name, std::string title) override 
    { 
      PressButton* widget = new PressButton(title);
      
      widget->onStateChange = [&style,this,name,widget] { 
        if (widget->getState() == juce::Button::ButtonState::buttonDown) {
          this->app->state->setInt(name, 1); 
        } else if (widget->getState() == juce::Button::ButtonState::buttonOver && this->app->state->getInt(name) == 1) {
          this->app->state->setInt(name, 0); 
        }
      };

      PLOG_DEBUG << "make press button: " << name << " with text: " << title;
      addWidget(style, widget, rect);
    };

    void checkToggle(Parser::Style& style, Parser::Rect rect, std::string name, std::string title) override 
    { 
      CheckToggle* widget = new CheckToggle(title);
      widget->setToggleState(this->app->state->getInt(name) == 1, juce::dontSendNotification);
      
      widget->onStateChange = [&style,this,name,widget] { 
        if (widget->getState() == juce::Button::ButtonState::buttonDown) {
          this->app->state->setInt(name, 1 - this->app->state->getInt(name));
          if (widget->getToggleState()) {
              widget->setColour(
                  juce::ToggleButton::tickColourId, 
                  this->app->findColor(readCol(style.secondaryColor, this->app)));
          } else {
              widget->setColour(
                  juce::ToggleButton::tickColourId, 
                  this->app->findColor(readCol(style.color, this->app)));
          }
        }
      };
      addWidget(style, widget, rect);
    };

    void checkGroup(Parser::Style& style, Parser::Rect rect, std::string chan, std::vector<std::string> names, bool isVertical) override
    { 
      ToggleGroup* widget = new ToggleGroup(isVertical);
      std::for_each(names.begin(), names.end(), [this, widget] (auto name) {
        juce::ToggleButton* button = new juce::ToggleButton(name);
        widget->addItem(button);
      });

      widget->setValue((size_t) app->state->getInt(chan));
      app->state->appendCallbackInt(chan, new Callback<int>([this, widget] (auto n) {
        size_t current = widget->getValue();
        size_t choice = (size_t) n;
        if (choice != current) {
          widget->setValue((size_t) n);
        }
      }));

      widget->onChange = [this,widget, chan] (size_t n) {
        this->app->state->setInt(chan, n);
      };

      addWidget(style, widget, rect);
    };

    void buttonGroup(Parser::Style& style, Parser::Rect rect, std::string chan, std::vector<std::string> names, bool isVertical) override
    { 
      ButtonGroup* widget = new ButtonGroup(isVertical, app->state->getInt(chan), names);

      app->state->appendCallbackInt(chan, new Callback<int>([this, widget] (auto n) {
        size_t current = widget->getValue();
        size_t choice = (size_t) n;
        if (choice != current) {
          widget->setValue((size_t) n);
        }
      }));

      widget->onChange = [this,widget, chan] (size_t n) {
        this->app->state->setInt(chan, n);
      };

      addWidget(style, widget, rect);
    };

   
    void label(Parser::Style& style, Parser::Rect rect, std::string val) override 
    {
      Label* widget = new Label(val);
      addWidget(style, widget, rect);
    };

    // todo: background images
    void image(Parser::Style& style, Parser::Rect rect, std::string file) 
    { 
      juce::Image image = juce::ImageFileFormat::loadFrom(juce::File(file));
      juce::ImageComponent* widget = new juce::ImageComponent(file);
      widget->setImage(image); 
      addWidget(style, widget, rect);
    };

    void dot(Parser::Style& style, Parser::Rect rect)
    {
      Dot* widget = new Dot();
      addWidget(style, widget, rect);
    }

    void barDisplay(Parser::Style& style, Parser::Rect rect, std::string chan)
    {
      BarDisplay* widget = new BarDisplay();

      app->state->appendCallbackDouble(chan, new Callback<double>([widget] (double x) {
        widget->setValue((float) x);
      }));
      
      addWidget(style, widget, rect);
    }

    // TODO: make rainbow range of colors configurable
    virtual void dotMeter(Parser::Style& style, Parser::Rect rect, std::string chan, std::vector<Parser::Col> colors) 
    { 
      DotMeter* widget = new DotMeter();
      std::vector<juce::Colour> cols;
      std::for_each(colors.begin(), colors.end(), [this, &cols] (auto c) {
        cols.push_back(this->app->findColor(c));
      });
      widget->setColors(cols);
      widget->setValue((float) app->state->getDouble(chan));
      app->state->appendCallbackDouble(chan, new Callback<double>([widget] (double v) { widget->setValue((float) v);}));
      addWidget(style, widget, rect);
    };

    // TODO: make rainbow range of colors configurable
    virtual void barMeter(Parser::Style& style, Parser::Rect rect, std::string chan, std::vector<Parser::Col> colors) 
    { 
      BarMeter* widget = new BarMeter();
      std::vector<juce::Colour> cols;
      std::for_each(colors.begin(), colors.end(), [this, &cols] (auto c) {
        cols.push_back(this->app->findColor(c));
      });
      widget->setColors(cols);
      widget->setValue((float) app->state->getDouble(chan));
      app->state->appendCallbackDouble(chan, new Callback<double>([widget] (double v) { widget->setValue((float) v);}));
      addWidget(style, widget, rect);
    };


    void text(Parser::Style& style, Parser::Rect rect, std::string name) override 
    {
      Label* widget = new Label(name);      

      addWidget(style, widget, rect);
      
      widget->setEditable(true);
      widget->onTextChange = [this, name, widget] {
        this->app->state->setString(name, widget->getText().toStdString());
      };
      app->state->appendCallbackString(name, new Callback<std::string>([widget](std::string str) {
        juce::String jstr = (const std::string & ) str;
        if (jstr.compare(widget->getText()) != 0) {
          widget->setText(jstr, juce::dontSendNotification);
        }
      }));
    };

    void comboBox(Parser::Style& style, Parser::Rect rect, std::string chan, std::vector<std::string> names) override
    { 
      ComboBox* widget = new ComboBox(chan, names);

      widget->onChange = [this, widget, chan] {
        this->app->state->setInt(chan, widget->getSelectedItemIndex());
      };
      widget->setSelectedItemIndex(app->state->getInt(chan));
      app->state->appendCallbackInt(chan, new Callback<int>([chan, this, widget] (int choice) {
        int current = widget->getSelectedItemIndex();
        PLOG_DEBUG << "set combo " << chan << ": choice: " << choice << ", index: " << current;
        if (current != choice) {
          widget->setSelectedItemIndex(choice);
        }
      })); 

      addWidget(style, widget, rect);      
    };

    void space(Parser::Rect rect) override { (void) rect; };

    void groupBegin(Parser::Style& style, Parser::Rect rect, std::string name) override
    {
      Group* group = app->groupBegin(style, rect, name);
      HasStyle* widget = dynamic_cast<HasStyle*>(group->getGroupWidget());
      if (widget) {
        widget->setStyle(app, style);
      }
    }

    void groupEnd(Parser::Style &style) override
    {
      app->groupEnd(style);
    }

    void panelBegin(Parser::Style& style, Parser::Rect rect, std::string name) override
    {
      PLOG_DEBUG << "panelBegin: " << rect.toString() << " , name: " << name;
      Panel* panel = app->panelBegin(style, rect, name);    
      app->state->appendCallbackInt(name, new Callback<int>([panel] (int n) { panel->selectVisible((size_t) n); }));
    }

    void panelEnd(Parser::Style &style, std::string name) override
    {
      PLOG_DEBUG << "panelEnd: " << name;
      Panel* panel = app->panelEnd(style);
      panel->selectVisible((size_t) app->state->getInt(name));
    }

    void panelItemBegin() override
    {
      PLOG_DEBUG << "panelItemBegin";
      app->panelItemBegin();
    }

    void panelItemEnd(Parser::Style &style) override
    {
      PLOG_DEBUG << "panelItemEnd";
      app->panelItemEnd(style);
    }

  private:
    App* app;
};

class BuildLayout : public Parser::Layout {
  public:
    BuildLayout(App* _app): app(_app) {}
    void horBegin() override { printVar("hor", "begin"); };
    void horEnd() override { printVar("hor", "end"); };
    void verBegin() override { printVar("ver", "begin"); };
    void verEnd() override { printVar("ver", "end"); };
    void gridBegin() override { printVar("grid", "begin"); };
    void gridEnd() override { printVar("grid", "end"); };
    void scale(Parser::Val<double> val) override { printVar("scale", val.getVal()); };
  private:
    App* app;
};

class BuildUi : public Parser::Ui {
  public:
    BuildUi(App* _app, Parser::Widget* widget, Parser::Layout* layout) : 
        Parser::Ui(widget, layout), 
        app(_app) 
    {}

    void begin(Parser::Style &style, juce::Rectangle<float> rect) override {
      Group* group = app->groupBegin(style, rect, "");
      HasStyle* widget = dynamic_cast<HasStyle*>(group->getGroupWidget());      
      if (widget) {
        widget->setStyle(app, style);
      }
    }

    void end(Parser::Style &style) override {
      app->groupEnd(style);
    }

  private:
    App* app;
};

//------------------------------------------------------------------------------------- 
// Build App from YAML file

void initApp(App* app, CsdModel* csound, YAML::Node node) 
{
  // assemble builders
  Parser::InitVars* buildInits = new BuildInits(app);  
  Parser::UpdateVars* buildUpdates = new BuildUpdates(app);
  Parser::KeypressUpdate* buildKeypress = new BuildKeypress(app);
  
  Parser::InitState* buildState = new Parser::InitState(buildInits, buildUpdates, buildKeypress);
  Parser::Config* buildConfig = new BuildConfig(app);
  Parser::Widget* buildWidget = new BuildWidget(app);
  Parser::Layout* buildLayout = new BuildLayout(app);
  Parser::Ui* buildUi = new BuildUi(app, buildWidget, buildLayout);
  CsoundChannelReader* csoundChannelReader = new CsoundChannelReader(app, csound);
  Parser::CsoundUi* buildCsoundUi = new BuildCsoundUi(app, csound, csoundChannelReader);
  Parser::Window* buildWindow = new Parser::Window(buildState, buildUi, buildConfig, buildCsoundUi);
  buildWindow->run(node);
  csoundChannelReader->startTimerHz(20);
}
