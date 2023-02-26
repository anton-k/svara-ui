#include "App.h"
#include "../parser/Parser.h"
#include <string>
#include <iostream>
#include <plog/Log.h>

// Build Application from YAML-file

template<class T>
void printVar(std::string name, T val)
{
  std::cout << name << ": " << val << "\n";
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
      std::cout << "set sizes\n";
      app->config->windowHeight = height;
      app->config->windowWidth = width;
    }
  
  private:
    App* app;
};

//------------------------------------------------------------------------------------- 
// Build UI

void padRect(Parser::Rect& rect, Parser::Pad pad) 
{
  float x = rect.getX(), 
        y = rect.getY(), 
        w = rect.getWidth(),
        h = rect.getHeight();

  x = x + pad.left * w;
  y = y + pad.top * h;
  w = w * (1 - pad.left - pad.right);
  h = h * (1 - pad.top - pad.bottom);

  rect.setX(x);
  rect.setY(y);
  rect.setHeight(h);
  rect.setWidth(w);
}

void setColor(App* app, Parser::Val<Parser::Col> col, std::function<void(juce::Colour)> setter)
{
  if (col.isVal()) {
    juce::Colour c = app->findColor(col.getVal());
    setter(c);
  } else {
    std::cout << "SETTING COLOR\n";
    Parser::Chan chn = col.getChan();
    setter(app->findColor(app->state->getString(chn.name)));
    app->state->appendCallbackString(chn.name, [app,setter](auto newCol) { 
      juce::Colour c = app->findColor(newCol);
      setter(c);
    });               
  }
}

void setTextSize(App* app, Parser::Val<double> textSize, std::function<juce::Font(void)> getFont, std::function<void(juce::Font)> setFont)
{
  if (textSize.isVal()) {
    juce::Font font = getFont();
    font.setHeight(textSize.getVal());
    setFont(font);
  } else {
    Parser::Chan chan = textSize.getChan();
    app->state->appendCallbackDouble(chan.name, [&getFont,&setFont] (auto size) {
      juce::Font font = getFont();
      font.setHeight(size);
      setFont(font);
    ;} );
  }
}

void setFont(App* app, Parser::Val<std::string> typeface, std::function<juce::Font(void)> getFont, std::function<void(juce::Font)> setFont)
{
  if (typeface.isVal()) {
    juce::Font font = getFont();
    font.setTypefaceName(typeface.getVal());
    setFont(font);
  } else {
    Parser::Chan chan = typeface.getChan();
    app->state->appendCallbackString(chan.name, [&getFont,&setFont] (auto face) {
      juce::Font font = getFont();
      font.setTypefaceName(face);
      setFont(font);
    ;} );
  }
}


void setSlider(App* app, juce::Slider* widget, Parser::Style& style, std::string name, juce::Slider::ColourIds colourId)
{
  widget->setRange(0, 1.0);
  widget->setValue(app->state->getDouble(name));
  PLOG_DEBUG << "setSlider: widget name: " << app->state->getDouble(name);
  
  // Callback to update channel value on change in slider
  widget->onValueChange = [app, name, widget] { app->state->setDouble(name, widget->getValue()); };
  
  // Callback to update slider on value change
  app->state->appendCallbackDouble(name, [widget](double val) {
      float v = val;
      if (widget->getValue() != v) {
        widget->setValue(v);
      }
  });

  setColor(app, style.color, [widget, colourId] (auto c) {
    widget->setColour(colourId, c);
    widget->setColour(juce::Slider::thumbColourId, c);          
  });
}


class BuildWidget : public Parser::Widget {
  public:
    BuildWidget(App* _app): app(_app) {}
    void knob(Parser::Style& style, Parser::Rect rect, std::string name) override 
    { 
      padRect(rect, style.pad);
      juce::Slider* knob = new juce::Slider(juce::Slider::SliderStyle::Rotary, juce::Slider::TextEntryBoxPosition::NoTextBox);
      knob->setName(name);
      setSlider(app, knob, style, name, juce::Slider::rotarySliderFillColourId);
      app->scene->addWidget(knob, rect);
    }
    
    void slider(Parser::Style& style, Parser::Rect rect, std::string name) override 
    {
      padRect(rect, style.pad);
      juce::Slider::SliderStyle sliderStyle = (rect.getWidth() < rect.getHeight()) 
          ? juce::Slider::SliderStyle::LinearVertical
          : juce::Slider::SliderStyle::LinearHorizontal;
      juce::Slider* slider = new juce::Slider(sliderStyle, juce::Slider::TextEntryBoxPosition::NoTextBox);
      slider->setName(name);
      setSlider(app, slider, style, name, juce::Slider::trackColourId);
      app->scene->addWidget(slider, rect);
    };
    
    void bar(Parser::Style& style, Parser::Rect rect, std::string name) override 
    {
      padRect(rect, style.pad);
      juce::Slider::SliderStyle sliderStyle = (rect.getWidth() < rect.getHeight()) 
          ? juce::Slider::SliderStyle::LinearBarVertical 
          : juce::Slider::SliderStyle::LinearBar;
      juce::Slider* slider = new juce::Slider(sliderStyle, juce::Slider::TextEntryBoxPosition::NoTextBox);
      slider->setName(name);
      setSlider(app, slider, style, name, juce::Slider::trackColourId);
      app->scene->addWidget(slider, rect);
    };

    void xyPad(Parser::Rect rect, std::string nameX, std::string nameY) override { (void) rect; std::cout << "xy-pad: " << nameX << " " << nameY << "\n"; };
    void button(Parser::Rect rect, std::string name) override { (void) rect; printVar("button", name); };
    void toggle(Parser::Rect rect, std::string name) override { (void) rect; printVar("toggle", name); };
    // void buttonRow(std::string name) override { (void)name; };
    
    void label(Parser::Style& style, Parser::Rect rect, std::string val) override 
    {
      padRect(rect, style.pad);
      juce::Label* widget = new juce::Label();
      widget->setName(val);
      widget->setText(val, juce::dontSendNotification);
      widget->setFont(juce::Font(16.0f, juce::Font::plain));

      setTextSize(app, style.textSize, 
          [widget] { return widget->getFont(); },
          [widget] (auto font) { return widget->setFont(font); }
      );

      setColor(app, style.color, [widget] (auto c) {
        widget->setColour(juce::Label::textColourId, c);
      });
      widget->setJustificationType (juce::Justification::centred);
      app->scene->addWidget(widget, rect);
    };

    void text(Parser::Style& style, Parser::Rect rect, std::string name) override 
    {
      padRect(rect, style.pad);
      juce::Label* widget = new juce::Label();      
      widget->setName(name);
      widget->setText(app->state->getString(name), juce::dontSendNotification);
      widget->setFont(juce::Font(16.0f, juce::Font::plain));

      setTextSize(app, style.textSize, 
          [widget] { return widget->getFont(); },
          [widget] (auto font) { return widget->setFont(font); }
      );

      setColor(app, style.color, [widget] (auto c) {
        widget->setColour(juce::Label::textColourId, c);
      });
      widget->setJustificationType (juce::Justification::centred);
      app->scene->addWidget(widget, rect);
      
      widget->setEditable(true);
      widget->onTextChange = [this, name, widget] {
        this->app->state->setString(name, widget->getText().toStdString());
      };
      app->state->appendCallbackString(name, [widget](std::string str) {
        juce::String jstr = (const std::string & ) str;
        if (jstr.compare(widget->getText()) != 0) {
          widget->setText(jstr, juce::dontSendNotification);
        }
      });
    };
    void space(Parser::Rect rect) override { (void) rect; };

    void groupBegin(Parser::Style& style, Parser::Rect rect, std::string name) override
    {
      padRect(rect, style.pad);
      app->scene->groupBegin(rect, name);
    }

    void groupEnd() override
    {
      app->scene->groupEnd();
    }

    void panelBegin(Parser::Style& style, Parser::Rect rect, std::string name) override
    {
      PLOG_DEBUG << "panelBegin: " << rect.toString() << " , name: " << name;
      padRect(rect, style.pad);
      Panel* panel = app->scene->panelBegin(rect, name);    
      app->state->appendCallbackInt(name, [panel] (int n) { panel->selectVisible((size_t) n); });
    }

    void panelEnd(std::string name) override
    {
      PLOG_DEBUG << "panelEnd: " << name;
      Panel* panel = app->scene->panelEnd();
      panel->selectVisible((size_t) app->state->getInt(name));
    }

    void panelItemBegin() override
    {
      PLOG_DEBUG << "panelItemBegin";
      app->scene->panelItemBegin();
    }

    void panelItemEnd() override
    {
      PLOG_DEBUG << "panelItemEnd";
      app->scene->panelItemEnd();
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

//------------------------------------------------------------------------------------- 
// Build App from YAML file

void initApp(App* app, YAML::Node node) 
{
  // assemble builders
  
  Parser::InitVars* buildInits = new BuildInits(app);  
  Parser::UpdateVars* buildUpdates = new BuildUpdates(app);
  
  Parser::State* buildState = new Parser::State(buildInits, buildUpdates);
  Parser::Config* buildConfig = new BuildConfig(app);
  Parser::Widget* buildWidget = new BuildWidget(app);
  Parser::Layout* buildLayout = new BuildLayout(app);
  Parser::Ui* buildUi = new Parser::Ui(buildWidget, buildLayout);
  Parser::Window* buildWindow = new Parser::Window(buildState, buildUi, buildConfig);
  buildWindow->run(node);
}

