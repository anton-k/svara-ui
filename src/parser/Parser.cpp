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

void IsUi::onKey(YAML::Node node, std::string key, Rect rect)
{
  forKey(node, key, [this, rect](auto x) { this->run(x, rect); });
}

// -------------------------------------------------------------------
// State


void InitVars::run(YAML::Node node) 
{
  forObject(node, [this](std::string key, YAML::Node x) {    
      if (isInt(x)) { this->intVar(key, getInt(x, 0)); 
      } else if (isDouble(x)) { this->doubleVar(key, getDouble(x, 0)); 
      } else {
        this->stringVar(key, getString(x, ""));
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

void Style::run(YAML::Node node) {
  forValColor(node, "color", [this](auto col) { this->color(col);});
  forValColor(node, "background", [this](auto col) { this->background(col);});
  forValColor(node, "secondary-color", [this](auto col) { this->secondaryColor(col);});
  forInt(node, "text-size", [this](auto size) { this->textSize(size); });
  forValString(node, "font", [this](auto f) { this->font(f); });
  forString(node, "hints", [this](auto x) { this->hints(toHint(x)); });
}

void Layout::run(YAML::Node node) 
{
  (void) node;
}

void Widget::run(YAML::Node node, Rect rect) 
{
  forString(node, "knob", [this, rect](auto chan) { this->knob(rect, chan);});
  forString(node, "slider", [this, rect](auto chan) { this->slider(rect, chan); });
  forString(node, "button", [this, rect](auto chan) { this->button(rect, chan); });
  forString(node, "toggle", [this, rect](auto chan) { this->toggle(rect, chan); });
  forString(node, "label", [this, rect](auto val) { this->label(rect, val); });
  forString(node, "text", [this, rect](auto chan) { this->text(rect, chan); });
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
  forDouble(node, "scale", [&res](int val) { res = val; });
  return res;
}

void foldBoxes(bool isHor, std::vector<std::pair<float, YAML::Node>>& boxes, Rect rect, Ui* parent) 
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
        parent->run(pair.second, Rect(x, y, dw, dh));
        x = x + dw;
    });
  } else {
    dw = width;
    std::for_each(boxes.begin(), boxes.end(), [&](auto pair) {
        dh = height * pair.first / total;
        parent->run(pair.second, Rect(x, y, dw, dh));
        y = y + dh;
    });
  }
}

void forListLayout(bool isHor, Ui* parent, YAML::Node node, Rect rect) 
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
  
  foldBoxes(isHor, boxes, rect, parent);
}

void runLayout(Ui* parent, YAML::Node node, Rect rect)
{
  bool isHor = true;
  forListLayout(isHor,  parent, node, rect);
  forListLayout(!isHor, parent, node, rect);
}

void Ui::run(YAML::Node node, Rect rect) 
{  
  this->style->onKey(node, "style");
  runLayout(this, node, rect);
  this->widget->run(node, rect);
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
  this->ui->onKey(node, "ui", Rect(0.0, 0.0, 1.0, 1.0));
}


void run(Window* win, YAML::Node node)
{
  forKey(node, "main", [win](auto x) { win->run(x); });
}

}
