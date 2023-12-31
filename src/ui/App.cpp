#include "App.h"
#include "parser/Parser.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include <plog/Log.h>
#include "../general/Fun.h"
#include "widgets/Board.h"

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
// init app argument list

InitApp::InitApp(juce::ArgumentList args)
{
  if (args.containsOption("--csound")) {
    csoundFile = std::string(args.getValueForOption("--csound").toRawUTF8());
    isUiMock = false;
  } else {
    isUiMock = true;
  }

  if (args.containsOption("--ui")) {
    uiFile = std::string(args.getValueForOption("--ui").toRawUTF8()); 
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

  forEach<Box*>(children, [this](auto box) { box->setBounds();} );
}

void Group::append(std::function<void(juce::Component*)> add)
{
  add(group);
  forEach<Box*>(children, [this](auto box) {
    box->append( [this](juce::Component* child) {
      group->addAndMakeVisible(child);
    });
  });
}

void Group::setVisible (bool isVisible)
{
  group->setVisible(isVisible);
}

void Group::push_back(App* app, Parser::Style &style, Box* box) 
{
  (void) app; (void) style;
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

void Panel::push_back(App* app, Parser::Style &style, Box* box)
{
  if (panels.empty()) {
    initItem(app, style);
  }

  Group* lastPanel = panels.back();

  PLOG_DEBUG << "Panel push_back: " << box->getName() << " to panel " << panels.size();
  lastPanel->push_back(app, style, box);
}

std::string Panel::getGroupName()
{
  std::stringstream ss;
  ss << "tab-group-" << getName() << "-" << getSize();
  return ss.str();
}

void Panel::initItem(App* app, Parser::Style &style) 
{
  bool hasBorder = false;
  Board* groupComponent = new Board();
  groupComponent->setStyle(app, style);
  groupComponent->setName(getGroupName());

  auto group = new Group(groupComponent, rect, hasBorder);
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

void Scene::append(App* app, Parser::Style &style, Box* box)
{
  if (groupStack.empty()) {
    widgets.push_back(box);
  } else {
    GroupBox* lastGroup = groupStack.back();
    lastGroup->push_back(app, style, box);
  }
};

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


void Scene::addWidget(App* app, Parser::Style &style, juce::Component* comp, Parser::Rect rect)
{
  padRect(rect, style.pad);
  append(app, style, new Widget(comp, rect));
}

void Scene::setup(juce::Component* parent)
{
  auto call = [parent] (juce::Component* widget) { parent->addAndMakeVisible(widget); };
  std::for_each(widgets.begin(), widgets.end(), [parent, call] (Box* box) { box->append(call); });
}

Group* App::groupBegin(Parser::Style &style, Parser::Rect rect, std::string name)
{
  bool hasBorder = style.border.width != 0 || name != "";
  juce::Component* group;
  if (hasBorder) {
    GroupBoard* widget = new GroupBoard();
    if (name.size() > 0) {
      widget->setText(juce::String(name));
      widget->setName(juce::String(name));
    }
    widget->setStyle(this, style);
    group = widget;
  } else {
    Board* widget = new Board();
    widget->setStyle(this, style);
    group = widget;
    group->setName(name);
  }

  auto groupBox = new Group(group, rect, hasBorder);
  scene->groupStack.push_back(groupBox);
  return groupBox;
}

void App::groupEnd(Parser::Style &style)
{
  GroupBox* lastGroup = scene->groupStack.back();  
  lastGroup->end();
  scene->groupStack.pop_back();
  scene->append(this, style, lastGroup);
}

Panel* App::panelBegin(Parser::Style style, Parser::Rect rect, std::string name)
{
  Panel* panel = new Panel(style, rect, name);
  scene->groupStack.push_back(panel);
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

void App::panelItemBegin()
{
  // Panel* lastGroup = toPanel(groupStack.back(), "panel item begin: not a panel on groupStack");
  // lastGroup->initItem();
}

void App::panelItemEnd(Parser::Style &style)
{
  Panel* lastGroup = toPanel(scene->groupStack.back(), "panel item begin: not a panel on groupStack");
  lastGroup->initItem(this, style);
}

Panel* App::panelEnd(Parser::Style& style)
{
  Panel* lastGroup = toPanel(scene->groupStack.back(), "panel item end: not a panel on groupStack");  
  PLOG_DEBUG << "PANEL SIZE END PRE: " << lastGroup->getSize();
  lastGroup->end();
  PLOG_DEBUG << "PANEL SIZE END POST: " << lastGroup->getSize();
  scene->groupStack.pop_back();
  scene->append(this, style, lastGroup);
  return lastGroup;
}

void Scene::onKeyEvent(KeyEvent key) 
{
  onKey.apply(key);
}

void Scene::appendKeyListener(KeyEvent key, Procedure* proc)
{
  auto react = [key, proc] (auto event) { 
      if (key == event) { 
        PLOG_DEBUG << "key " << (key.isKeyDown ? "down: " : "up  : ") 
          <<  key.key.getTextDescription();
        proc->apply();
      } 
    };
  onKey.append(react);
}


void App::setJustificationType (Parser::Val<std::string> val, std::function<void(juce::Justification)> setter)
{
  if (val.isChan()) {
    std::string name = val.getChan().name;
    setter(Parser::toJustification(this->state->getString(name)));
    this->state->appendCallbackString(name, new Callback<std::string>([this, setter] (auto str) {
      setter(Parser::toJustification(str));
    }));
  } else {
    setter(Parser::toJustification(val.getVal()));
  }
}

void App::setTextSize (Parser::Val<double> val, Set<double> setter)
{
  if (val.isChan()) {
    std::string name = val.getChan().name;
    setter(this->state->getDouble(name));
    this->state->appendCallbackDouble(name, new Callback<double>([this, setter] (auto v) {
      setter(v);
    }));
  } else {
    setter(val.getVal());
  }
}


void App::setColor(Parser::Val<Parser::Col> col, std::function<void(juce::Colour)> setter)
{
  if (col.isVal()) {
    juce::Colour c = this->findColor(col.getVal());
    setter(c);
  } else {
    PLOG_DEBUG << "SETTING COLOR\n";
    Chan chn = col.getChan();
    setter(this->findColor(this->state->getString(chn.name)));
    this->state->appendCallbackString(chn.name, new Callback<std::string>([this,setter](auto newCol) { 
      juce::Colour c = this->findColor(newCol);
      setter(c);
    }));               
  }
}

void App::setColor(std::optional<Parser::Val<Parser::Col>> col, std::function<void(juce::Colour)> setter) {
  forEach<Parser::Val<Parser::Col>>(col, [this, setter](auto c) { this->setColor(c, setter); });
}



