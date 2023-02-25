#include "App.h"
#include "../parser/Parser.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include <plog/Log.h>

// ------------------------------------------------------------------------------------- 
// Palette

juce::Colour Palette::fromName(Parser::Col col) 
{
  if (palette.count(col.val)) {
    return palette[col.val];
  } else {
    return defaultColor;
  }
}

// ------------------------------------------------------------------------------------- 
// Atomic widget

void Widget::setBounds() 
{
  PLOG_DEBUG << "setBounds: " << widget->getName() << "  " << rect.toString();
  widget->setBoundsRelative(rect);
}

void Widget::append(std::function<void(juce::Component*)> add)
{
  add(widget);
}

// ------------------------------------------------------------------------------------- 
// Group widget

void Group::setBounds()
{
  PLOG_DEBUG << "GROUP SIZE " << children.size();
  group->setBoundsRelative(rect);
  std::for_each(children.begin(), children.end(), [this](auto box) { box->setBounds();} );
}

void Group::append(std::function<void(juce::Component*)> add)
{
  add(group);
  std::for_each(children.begin(), children.end(), [this](auto box) {
    box->append( [this](juce::Component* child) {
      group->addAndMakeVisible(child);
    });
  });
}

void Group::push_back(Box* box) 
{
  children.push_back(box);
}

// ------------------------------------------------------------------------------------- 
// Panel widget

void Panel::setBounds()
{
  std::for_each(panels.begin(), panels.end(), [](Group* item) { item->setBounds(); });
}

void Panel::append(std::function<void(juce::Component*)> add) 
{
  std::for_each(panels.begin(), panels.end(), [&add](Group* item) { item->append(add); });
}

void Panel::push_back(Box* box)
{
  if (panels.empty()) {
    initItem();
  }

  Group* lastPanel = panels.back();
  lastPanel->push_back(box);
}

void Panel::initItem() 
{
  auto group = new Group(rect, "");
  panels.push_back(group);
}

// ------------------------------------------------------------------------------------- 
// Scene

void Scene::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    std::for_each(widgets.begin(), widgets.end(), [](auto box) { box->setBounds(); });
    PLOG_DEBUG << "WIDGET SIZE: " << widgets.size();
};

void Scene::append(Box* box)
{
  if (groupStack.empty()) {
    widgets.push_back(box);
  } else {
    GroupBox* lastGroup = groupStack.back();
    lastGroup->push_back(box);
  }
};

void Scene::addWidget(juce::Component* comp, Parser::Rect rect)
{
  append(new Widget(comp, rect));
}

void Scene::setup(juce::Component* parent)
{
  auto call = [parent] (juce::Component* widget) { parent->addAndMakeVisible(widget); };
  std::for_each(widgets.begin(), widgets.end(), [parent, call] (Box* box) { box->append(call); });
}

void Scene::groupBegin(Parser::Rect rect, std::string name)
{
  auto groupBox = new Group(rect, name);
  groupStack.push_back(groupBox);
}

void Scene::groupEnd()
{
  GroupBox* lastGroup = groupStack.back();  
  lastGroup->end();
  groupStack.pop_back();
  append(lastGroup);
}

void Scene::panelBegin(Parser::Rect rect, std::string name)
{
  GroupBox* panel = new Panel(rect, name);
  groupStack.push_back(panel);
}

void Scene::panelItemBegin()
{
  try {
    Panel* lastGroup = dynamic_cast<Panel*>(groupStack.back()); 
    lastGroup->initItem();
  } catch (...) {
    PLOG_ERROR << "panel item begin: not a panel on groupStack";
  }
}

void Scene::panelItemEnd()
{
  panelItemBegin();
}

void Scene::panelEnd()
{
  groupEnd();
}

