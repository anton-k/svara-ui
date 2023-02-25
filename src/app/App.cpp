#include "App.h"
#include "../parser/Parser.h"
#include <juce_gui_extra/juce_gui_extra.h>

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

void Widget::setBounds(Parser::Rect parentRect) 
{
  widget->setBoundsRelative(parentRect);
}

void Widget::append(std::function<void(juce::Component*)> addAndMakeVisible)
{
  addAndMakeVisible(widget);
}

// ------------------------------------------------------------------------------------- 
// Group widget

void Group::setBounds(Parser::Rect parentRect)
{
  group->setBoundsRelative(parentRect);
  std::for_each(children.begin(), children.end(), [this](auto box) { box->setBounds();} );
}

void Group::append(std::function<void(juce::Component*)> addAndMakeVisible)
{
  addAndMakeVisible(group);
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
// Scene

void Scene::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    std::for_each(widgets.begin(), widgets.end(), [](auto box) { box->setBounds(); });
};

void Scene::append(Box* box)
{
  if (groupStack.empty()) {
    widgets.push_back(box);
  } else {
    Group* lastGroup = groupStack.back();
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

void Scene::setGroup(juce::String name)
{
  auto group = new juce::GroupComponent(name, name);
  auto groupBox = new Group(group, widgets, Parser::Rect(0, 0, 1, 1));

  widgets = std::vector<Box*>();
  widgets.push_back(groupBox);
}

void Scene::groupBegin(Parser::Rect rect, std::string name)
{
  auto group = new juce::GroupComponent();
  if (name.size() > 0) {
    group->setText(juce::String(name));
  } 
  auto groupBox = new Group(group, widgets, rect);
  groupStack.push_back(groupBox);
}

void Scene::groupEnd()
{
  Group* lastGroup = groupStack.back();
  widgets.push_back(lastGroup);
}
