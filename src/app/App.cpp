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
  widget->setBoundsRelative(rect);
}

void Widget::append(std::function<void(juce::Component*)> add)
{
  add(widget);
}

void Widget::setVisible (bool isVisible)
{
  widget->setVisible(isVisible);
}

std::string Widget::getName() 
{
  return widget->getName().toStdString();
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

void Group::setVisible (bool isVisible)
{
  group->setVisible(isVisible);
}

void Group::push_back(Box* box) 
{
  PLOG_DEBUG << "Group push_back: " << box->getName();
  children.push_back(box);
}


std::string Group::getName() 
{
  return group->getName().toStdString();
}

// ------------------------------------------------------------------------------------- 
// Panel widget

void Panel::setBounds()
{
  std::for_each(panels.begin(), panels.end(), [](Group* item) { item->setBounds(); });
}

void Panel::append(std::function<void(juce::Component*)> add) 
{
  size_t index = 0;
  PLOG_DEBUG << "PANEL SIZE: " << panels.size();
  std::for_each(panels.begin(), panels.end(), [this,&add,&index](Group* item) { 
    item->append(add);     
    item->setVisible(index == this->selected);
    index++;
  });
}

void Panel::push_back(Box* box)
{
  if (panels.empty()) {
    initItem();
  }

  Group* lastPanel = panels.back();

  PLOG_DEBUG << "Panel push_back: " << box->getName() << " to panel " << panels.size();
  lastPanel->push_back(box);
}

std::string Panel::getGroupName()
{
  std::stringstream ss;
  ss << "tab-group-" << getName() << "-" << getSize();
  return ss.str();
}

void Panel::initItem() 
{
  auto group = new Group(rect, getGroupName(), false);
  panels.push_back(group);
}

void Panel::setVisiblePanel(size_t n, bool isVisible)
{
  panels[n]->setVisible(isVisible);
}

void Panel::setVisible (bool isVisible)
{
  panels[selected]->setVisible(isVisible);
}

void Panel::selectVisible(size_t choice)
{
  if (choice != selected && choice < panels.size()) {
    setVisiblePanel(selected, false);
    setVisiblePanel(choice, true);
    selected = choice;
  }
}

std::string Panel::getName()
{
  return name;
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
  auto groupBox = new Group(rect, name, true);
  groupStack.push_back(groupBox);
}

void Scene::groupEnd()
{
  GroupBox* lastGroup = groupStack.back();  
  lastGroup->end();
  groupStack.pop_back();
  append(lastGroup);
}

Panel* Scene::panelBegin(Parser::Rect rect, std::string name)
{
  Panel* panel = new Panel(rect, name);
  groupStack.push_back(panel);
  return panel;
}

Panel* toPanel(GroupBox* box, std::string errorMessage)
{
  try {
    return dynamic_cast<Panel*>(box); 
  } catch (...) {
    PLOG_ERROR <<  errorMessage;
  }
}

void Scene::panelItemBegin()
{
  // Panel* lastGroup = toPanel(groupStack.back(), "panel item begin: not a panel on groupStack");
  // lastGroup->initItem();
}

void Scene::panelItemEnd()
{
  Panel* lastGroup = toPanel(groupStack.back(), "panel item begin: not a panel on groupStack");
  lastGroup->initItem();
}

Panel* Scene::panelEnd()
{
  Panel* lastGroup = toPanel(groupStack.back(), "panel item end: not a panel on groupStack");  
  PLOG_DEBUG << "PANEL SIZE END PRE: " << lastGroup->getSize();
  lastGroup->end();
  PLOG_DEBUG << "PANEL SIZE END POST: " << lastGroup->getSize();
  groupStack.pop_back();
  append(lastGroup);
  return lastGroup;
}

void Scene::onKeyEvent(KeyEvent key) 
{
  onKey.apply(key);
}

void Scene::appendKeyListener(KeyEvent key, Procedure proc)
{
  auto react = [key, proc] (auto event) { 
      if (key == event) { 
        PLOG_DEBUG << "key " << (key.isKeyDown ? "down: " : "up  : ") 
          <<  key.key.getTextDescription();
        proc.apply();
      } 
    };
  onKey.append(react);
}

