#include "Parser.h"
#include "Yaml.h"
#include <string>

namespace Parser
{

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

void UpdateVars::run(YAML::Node node)
{  
  forObject(node, [this](std::string trigger, YAML::Node triggerNode) {
    forObject(triggerNode, [this,trigger, triggerNode](std::string key, YAML::Node x) {
      if (isInt(x)) { 
        this->setInt(trigger, key, getInt(x, 0)); 
      }      
      else if (isDouble(x)) { 
        this->setDouble(trigger, key, getDouble(x, 0)); 
      }      
      else {
        this->setString(trigger, key, getString(x, ""));
      }
    });
  });  
}

void State::run(YAML::Node node) 
{
  this->init->onKey(node, "init");
  this->update->onKey(node, "update");
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

void StyleUpdate::update(YAML::Node node, Style& style) 
{
  forValColor(node, "color", [this,&style](auto col) { this->color(col); style.color = col; });
  forValColor(node, "background", [this,&style](auto col) { this->background(col); style.background = col; });
  forValColor(node, "secondary-color", [this,&style](auto col) { this->secondaryColor(col); style.secondaryColor = col; });
  forInt(node, "text-size", [this,&style](auto size) { this->textSize(size); style.textSize = size; });
  forValString(node, "font", [this,&style](auto f) { this->font(f); style.font = f; });
  forString(node, "hints", [this,&style](auto x) { this->hints(toHint(x)); style.hint = toHint(x); });
  forDouble(node, "pad", [this,&style](double x) { this->pad(Pad(x, x, x, x)); style.pad = Pad(x, x, x, x); });
  forKey(node, "pad", [this,&style] (auto padNode) {
    if (padNode.IsMap()) {
      forDouble(padNode, "left", [this,&style](double x) { style.pad.left = x; });
      forDouble(padNode, "right", [this,&style](double x) { style.pad.right = x;  });
      forDouble(padNode, "bottom", [this,&style](double x) { style.pad.bottom = x;  });
      forDouble(padNode, "top", [this,&style](double x) { style.pad.top = x;  });
    }
  });
}

void Layout::run(YAML::Node node) 
{
  (void) node;
}

void Widget::run(YAML::Node node, Rect rect, Style style) 
{
  forString(node, "knob", [this, rect, &style](auto chan) { this->knob(style, rect, chan);});
  forString(node, "slider", [this, rect, &style](auto chan) { this->slider(style, rect, chan); });
  forString(node, "bar", [this, rect, &style](auto chan) { this->bar(style, rect, chan); });
  forString(node, "button", [this, rect](auto chan) { this->button(rect, chan); });
  forString(node, "toggle", [this, rect](auto chan) { this->toggle(rect, chan); });
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

void runLayout(Ui* parent, YAML::Node node, Rect rect, Style style)
{
  bool isHor = true;
  forListLayout(isHor,  parent, node, rect, style);
  forListLayout(!isHor, parent, node, rect, style);
}

void Ui::updateStyle(YAML::Node node, Style& style) 
{
  if (hasKey(node, "style")) {
    styleUpdate->update(node["style"], style);
  }
}

void Ui::run(YAML::Node node, Rect rect, Style style) 
{ 
  updateStyle(node, style);
  runLayout(this, node, rect, style);
  this->widget->run(node, rect, style);
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
