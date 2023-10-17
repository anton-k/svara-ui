// How to link UI and update of the state
#pragma once
#include "../general/Fun.h"
#include "App.h"
#include "parser/Parser.h"

// float compare resolution
inline const float EPS = 0.0001;

inline bool equalFloats(float a, float b) {
  return abs(a - b) < EPS;
}

class HasValue {
  public:
  virtual void setValueChange(Proc) = 0;
  virtual double getCurrentValue() = 0;
  virtual void setCurrentValue(double) = 0;
  virtual void setValueRange(double, double) = 0;
  virtual Parser::Widget::Type getWidgetType() = 0;

  void linkValue(App* app, std::string name) {
    this->setValueRange(0, 1.0);
    this->setCurrentValue(app->state->getDouble(name));
    auto widgetType = this->getWidgetType();

    // Callback to update channel value on change in slider
    if (widgetType != Parser::Widget::Output) {
      setValueChange([app, name, this]() { app->state->setDouble(name, this->getCurrentValue()); });
    }

    // Callback to update slider on value change
    if (widgetType != Parser::Widget::Input) {
      app->state->appendCallbackDouble(name, new Callback<double>([this](double val) {
        float v = val;
        if (!equalFloats(this->getCurrentValue(), v)) {
          this->setCurrentValue(v);
          }
        }));
    }
  }
};

class HasClick {
  public:
    virtual void setOnClick(Proc) = 0;
    virtual void triggerClick() = 0;
    virtual bool isDownClick() = 0;

    void linkValue(App* app, std::string name) {
      // we use a trick to ensure that note is not retriggered when state
      // is change automatically
      setOnClick([this, app, name] {
        if (this->isUser) {
          if (this->isDownClick()) {
            this->counter = this->counter + 1;
            app->state->setInt(name, this->counter);
          }
        } else {
          this->isUser = true;
        }
      });

      app->state->appendCallbackInt(name, new Callback<int>(
        [this](int val) {
          this->isUser = false;
          this->triggerClick();
        }
      ));
    }

  private:
    int counter = 0;
    bool isUser = true;
};

