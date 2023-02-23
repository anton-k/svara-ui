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

void Widget::run(YAML::Node node) 
{
  forString(node, "knob", [this](auto chan) { this->knob(chan);});
  forString(node, "slider", [this](auto chan) { this->slider(chan); });
  forString(node, "button", [this](auto chan) { this->button(chan); });
  forString(node, "toggle", [this](auto chan) { this->toggle(chan); });
  forString(node, "label", [this](auto val) { this->label(val); });
  forString(node, "text", [this](auto chan) { this->text(chan); });
  forKey(node, "xy-pad", [this](auto xyNode) {
    forString(xyNode, "x", [this, &xyNode] (auto x) {
      forString(xyNode, "y", [this, x](auto y) { this->xyPad(x, y); });
    });
  });
}

void forListLayout(std::string tag, Ui* parent, Layout* layout, YAML::Node node, std::function<void(void)> begin, std::function<void(void)> end) 
{
  forKey(node, tag, [parent, layout, begin, end](auto elems) {
      if (elems.IsSequence()) {
        begin();    
        forNodes(elems, [parent, layout](auto x) { parent->run(x); });
        end();    
      }
  });
}

void runLayout(Ui* parent, YAML::Node node)
{
  forDouble(node, "scale", [parent] (auto x) { parent->layout->scale(x); });
  forKey(node, "space", [parent] (auto x) { (void) x; parent->layout->space(); });
  forListLayout("hor", parent, parent->layout, node, [parent] { parent->layout->horBegin(); }, [parent] { parent->layout->horEnd(); });
  forListLayout("ver", parent, parent->layout, node, [parent] { parent->layout->verBegin(); }, [parent] { parent->layout->verEnd(); });
}

void Ui::run(YAML::Node node) 
{  
  this->style->onKey(node, "style");
  runLayout(this, node);
  this->widget->run(node);
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
  this->ui->onKey(node, "ui");
}


void run(Window* win, YAML::Node node)
{
  forKey(node, "main", [win](auto x) { win->run(x); });
}

}
