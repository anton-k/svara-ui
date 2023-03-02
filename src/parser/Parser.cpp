#include "Parser.h"
#include "Yaml.h"
#include <string>
#include <plog/Log.h>
#include "../model/Model.h"
#include "../widgets/KeyPressListener.h"

namespace Parser
{
/*
Expr<int> toIntExpr(Val<int> v, State* state)
{
  if (v.isChan()) {
    return readIntChan(v.getChan(), state);
  } else {
    return Expr<int>(v.getVal());
  }
}

Expr<double> toDoubleExpr(Val<double> v, State* state)
{
  if (v.isChan()) {
    return readDoubleChan(v.getChan(), state);
  } else {
    return Expr<double>(v.getVal());
  }
}

Expr<std::string> toIntExpr(Val<std::string> v, State* state)
{
  if (v.isChan()) {
    return readStringChan(v.getChan(), state);
  } else {
    return Expr<std::string>(v.getVal());
  }
}

Expr<Col> toColExpr(Val<Col> v, State* state)
{
  if (v.isChan()) {
    std::function<Col(std::string)> toCol = [](std::string str) { return Col(str); };
    return readStringChan(v.getChan(), state).map(toCol);
  } else {
    return Expr<Col>(v.getVal());
  }
}
*/

Expr<std::string> toStringExpr(Val<std::string> v, State* state)
{
  if (v.isChan()) {
    return readStringChan(v.getChan(), state);
  } else {
    return Expr<std::string>(v.getVal());
  }
}

Expr<Col> toColExpr(Val<Col> v, State* state)
{
  if (v.isChan()) {
    PLOG_DEBUG << "BOO CHAN " << v.getChan().name;
    return map<std::string, Col>([] (std::string str) { return Col(str);}, readStringChan(v.getChan(), state));
  } else {
    PLOG_DEBUG << "BOO 1 " << v.getVal().val;
    return Expr<Col>(v.getVal());
  }
}


// -------------------------------------------------------------------
// IsYaml

void IsYaml::onKey(YAML::Node node, std::string key)
{
  forKey(node, key, [this](auto x) { this->run(x); });
}

void IsUi::onKey(YAML::Node node, std::string key, Rect rect, Style style)
{
  forKey(node, key, [this, rect, style](auto x) { this->run(x, rect, style); });
}

// -------------------------------------------------------------------
// State

// If key name end on ! prefix we turn debug on for this variable
bool parseDebugFlag(std::string& str)
{
  if (str.size() != 0) {
    if (str[str.size() - 1] == '!') {
      str.erase(str.size() - 1);
      return true;
    }
  } 
  return false;
}

void InitVars::run(YAML::Node node) 
{
  forObject(node, [this](std::string key, YAML::Node x) {    
      bool needDebug = parseDebugFlag(key);
      if (isInt(x)) { this->intVar(key, getInt(x, 0), needDebug); 
      } else if (isDouble(x)) { this->doubleVar(key, getDouble(x, 0), needDebug); 
      } else {
        this->stringVar(key, getString(x, ""), needDebug);
      }
  });  
}

Update UpdateVars::runUpdater(YAML::Node node)
{  
  Procedure res;
  forObject(node, [&res,this](std::string key, YAML::Node x) {
    switch (this->getType(key)) 
    {
      case Type::Int : 
        {
          if (isInt(x)) {
            int n = getInt(x, 0);
            Callback<int> setter = this->getSetInt(key);
            auto proc = [this,setter,n] () { setter.apply(n); };
            res.append(Procedure(proc));
          } else {
            PLOG_ERROR << "update " << key << " is defined as int, but set to non int value";
          }
        }
      case Type::Double :
        {
          if (isDouble(x)) {
            double d = getDouble(x, 0);          
            Callback<double> setter = this->getSetDouble(key);
            auto proc = [this,setter,d] () { setter.apply(d); };
            res.append(Procedure(proc));
          } else {
            PLOG_ERROR << "update " << key << " is defined as double, but set to non double value";
          }
        }
      case Type::String :
        {
          std::string str = getString(x, "");
          Callback<std::string> setter = this->getSetString(key);
          auto proc = [this,setter,str] () { setter.apply(str); };
          res.append(Procedure(proc));
        }
    }
  });
  return res;
}

void UpdateVars::run(YAML::Node node)
{  
  forObject(node, [this](std::string trigger, YAML::Node triggerNode) {
    Procedure setter = this->runUpdater(triggerNode);
    insertUpdater(trigger, setter);
  });  
}

void InitState::run(YAML::Node node) 
{
  this->init->onKey(node, "init");
  this->update->onKey(node, "update");
  forKey(node, "keyboard", [this] (auto child) {
    PLOG_DEBUG << "Parse keyboard";
    this->keypress->run(this->update, child);    
  });
}

void runKey(KeypressUpdate* parent, UpdateVars* updater, std::string tag, KeyEvent key, YAML::Node& node)
{
  forKey(node, tag, [parent,updater,key](auto triggerNode) {
    PLOG_DEBUG << "KEY PARSE" << key.key.getTextDescription(); 
    Procedure setter = updater->runUpdater(triggerNode);
    parent->insertKey(key, setter);
  });  
}

/*
bool isPrefix(std::string& prefix, std::string& str)
{
  if prefix.size() > str.size() { return false; }
  return std::mismatch(prefix.begin(), prefix.end(),
    str.begin(), str.end()).first == prefix.end();
}
*/


bool parseKey(std::string& str, juce::KeyPress& key)
{
  key = juce::KeyPress::createFromDescription(str);
  // check if returned key is invalid. Invalid keys are constructed with empty constructor
  return key != juce::KeyPress();
}

void KeypressUpdate::run(UpdateVars* updater, YAML::Node node)
{
  forObject(node, [this, updater](std::string keyStr, YAML::Node triggerNode) {
    juce::KeyPress key;
    bool isOk = parseKey(keyStr, key);
    if (isOk) {
      runKey(this, updater, "down", KeyEvent(true, key), triggerNode);
      runKey(this, updater, "up",   KeyEvent(false, key), triggerNode);
    } else {
      PLOG_ERROR << "Failed to parse keyboard key: " << keyStr;
    }
  });
}

// -------------------------------------------------------------------
// Ui

Hint toHint(std::string str)
{
  if (str == "hover") { return Hover; }
  if (str == "top") { return Top; }
  if (str == "bottom") { return Bottom; }
  if (str == "left") { return Left; }
  if (str == "right") { return Right; }
  return None;
}

std::string fromHint(Hint hint) 
{
  switch (hint)
  {
    case None: return "none";
    case Hover: return "hover";
    case Top: return "top";
    case Bottom: return "bottom";
    case Left: return "left";
    case Right: return "right";
  }
  return "none";
}

void Layout::run(YAML::Node node) 
{
  (void) node;
}

std::string getWidgetName(YAML::Node node)
{
  std::string name = "";
  forString(node, "name", [&name](auto str) { 
    name = str; 
  });  
  return name;
}

void Widget::run(YAML::Node node, Rect rect, Style style) 
{
  std::string name = getWidgetName(node);
  forString(node, "knob", [this, rect, &style](auto chan) { this->knob(style, rect, chan);});
  forString(node, "slider", [this, rect, &style](auto chan) { this->slider(style, rect, chan); });
  forString(node, "bar", [this, rect, &style](auto chan) { this->bar(style, rect, chan); });
  forString(node, "button", [this, name, rect, &style](auto chan) { this->button(style, rect, chan, name); });
  forString(node, "toggle", [this, name, rect, &style](auto chan) { this->toggle(style, rect, chan, name); });
  forString(node, "press-button", [this, name, rect, &style](auto chan) { this->pressButton(style, rect, chan, name); });
  forString(node, "check-toggle", [this, name, rect, &style](auto chan) { this->checkToggle(style, rect, chan, name); });
  forString(node, "label", [this, rect, &style](auto val) { this->label(style, rect, val); });
  forString(node, "text", [this, rect, &style](auto chan) { this->text(style, rect, chan); });  
  forKey(node, "xy-pad", [this, rect](auto xyNode) {
    forString(xyNode, "x", [this, rect, &xyNode] (auto x) {
      forString(xyNode, "y", [this, rect, x](auto y) { this->xyPad(rect, x, y); });
    });
  });
  forKey(node, "space", [this, rect] (auto x) { (void)x; this->space(rect); });

}

std::string getLayoutListTag(bool isHor) 
{
  return isHor ? "hor" : "ver";
}

float getScale(YAML::Node node) 
{
  float res = 1.0;
  forDouble(node, "scale", [&res](double val) { res = val; });
  return res;
}

void foldBoxes(bool isHor, std::vector<std::pair<float, YAML::Node>>& boxes, Rect rect, Style style, Ui* parent) 
{
  float x = rect.getX();
  float y = rect.getY();
  float width = rect.getWidth();
  float height = rect.getHeight();
  
  float total = 0;
  std::for_each(boxes.begin(), boxes.end(), [&total](auto pair) { total = total + pair.first; });
  if (total == 0) { return; }

  float dw, dh;
  if (isHor) {
    dh = height;
    std::for_each(boxes.begin(), boxes.end(), [&](auto pair) {
        dw = width * pair.first / total;
        parent->run(pair.second, Rect(x, y, dw, dh), style);
        x = x + dw;
    });
  } else {
    dw = width;
    std::for_each(boxes.begin(), boxes.end(), [&](auto pair) {
        dh = height * pair.first / total;
        parent->run(pair.second, Rect(x, y, dw, dh), style);
        y = y + dh;
    });
  }
}

void forListLayout(bool isHor, Ui* parent, YAML::Node node, Rect rect, Style style) 
{
  std::string tag = getLayoutListTag(isHor);
  std::vector<std::pair<float,YAML::Node>> boxes;  

  forKey(node, tag, [&boxes](auto elems) {
      if (elems.IsSequence()) {
        forNodes(elems, [&boxes](auto elem) { 
            boxes.push_back(std::pair(getScale(elem), elem));
        });
      }
  });
  
  foldBoxes(isHor, boxes, rect, style, parent);
}

void runGroup(Ui* parent, YAML::Node node, Rect rect, Style style)
{
  forKey(node, "group", [parent, rect, &style] (auto x) {
    std::string groupName = getWidgetName(x);
    PLOG_DEBUG << "GROUP NAME " << groupName;

    parent->widget->groupBegin(style, rect, groupName);
    parent->run(x, Rect(0.0, 0.0, 1.0, 1.0), style);
    parent->widget->groupEnd();
  });
}

void runTabs(Ui* parent, YAML::Node node, Rect rect, Style style)
{
  forKey(node, "tabs", [parent, rect, &style] (auto tabNode) {
    if (tabNode["index"].IsScalar() && tabNode["items"].IsSequence()) {
      std::string name = "";
      forString(tabNode, "index", [&name] (auto str) { name = str; });
      
      PLOG_DEBUG << "TABS INDEX: " << name;
      parent->widget->panelBegin(style, rect, name);

      forNodes(tabNode["items"], [parent, &style] (YAML::Node x) {
        parent->widget->panelItemBegin();        
        parent->run(x, Rect(0.0, 0.0, 1.0, 1.0), style);
        parent->widget->panelItemEnd();        
      });

      parent->widget->panelEnd(name);    
    }
  });
}

void runLayout(Ui* parent, YAML::Node node, Rect rect, Style style)
{
  bool isHor = true;
  forListLayout(isHor,  parent, node, rect, style);
  forListLayout(!isHor, parent, node, rect, style);
  runGroup(parent, node, rect, style);
  runTabs(parent, node, rect, style);
}

void Ui::updateStyle(YAML::Node root, Style& style) 
{
  if (hasKey(root, "style")) {
    YAML::Node node = root["style"];
    forValColor(node, "color", [this,&style](auto col) { style.color = col; });
    forValColor(node, "background", [this,&style](auto col) { style.background = col; });
    forValColor(node, "secondary-color", [this,&style](auto col) { style.secondaryColor = col; });
    forInt(node, "text-size", [this,&style](auto size) { style.textSize = size; });
    forValString(node, "font", [this,&style](auto f) { style.font = f; });
    forString(node, "hints", [this,&style](auto x) { style.hint = toHint(x); });
    forDouble(node, "pad", [this,&style](double x) { style.pad = Pad(x, x, x, x); });
    forKey(node, "pad", [this,&style] (auto padNode) {
      if (padNode.IsMap()) {
        forDouble(padNode, "left", [this,&style](double x) { style.pad.left = x; });
        forDouble(padNode, "right", [this,&style](double x) { style.pad.right = x;  });
        forDouble(padNode, "bottom", [this,&style](double x) { style.pad.bottom = x;  });
        forDouble(padNode, "top", [this,&style](double x) { style.pad.top = x;  });
      }
    });
  }
}

void Ui::run(YAML::Node node, Rect rect, Style style) 
{ 
  PLOG_DEBUG << "UI::RUN";
  // std::cout << node << "\n";  // TODO how to print node body in logger?
  PLOG_DEBUG << "RECT: " << rect.toString() << "\n";
  updateStyle(node, style);
  this->widget->run(node, rect, style);
  runLayout(this, node, rect, style);
}

// -------------------------------------------------------------------
// Config

void Config::run(YAML::Node node) 
{
  forKey(node, "size", [this](auto x) {
    if (hasKey(x, "height") && hasKey(x, "width")) {
      this->windowSize(getInt(x["height"], 100), getInt(x["width"], 100));
    }
  });
}

// -------------------------------------------------------------------
// Window

void Window::run(YAML::Node node) 
{
  this->config->onKey(node, "config");
  this->state->onKey(node, "state");
  this->ui->onKey(node, "ui", Rect(0.0, 0.0, 1.0, 1.0), Style());
}


void run(Window* win, YAML::Node node)
{
  forKey(node, "main", [win](auto x) { win->run(x); });
}

}
