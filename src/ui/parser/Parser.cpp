#include "Parser.h"
#include "Yaml.h"
#include <string>
#include <plog/Log.h>
#include "../model/Model.h"
#include "../widgets/KeyPressListener.h"

namespace Parser
{

// Reads YAML definition of UI from csound file. 
// Definition should be in XML-element <SvaraUi> ... </SvaraUi>.
bool readUiDef (juce::File csdFile, juce::String &result) {
  bool hasUi = false;
  bool collectLines = false;
  juce::StringArray def;

  if (csdFile.existsAsFile()) {
    juce::StringArray linesFromCsd;
    linesFromCsd.addLines(csdFile.loadFileAsString());

    for (const auto& line : linesFromCsd) {
      if (line.startsWith("</SvaraUi>")) {
        result = def.joinIntoString("\n");
        return collectLines; // if we stated to collect lines then result is ok
      }

      if (collectLines) {
        def.add(line);
      }

      if (!collectLines && line.startsWith("<SvaraUi>")) {
        collectLines = true;
      } 
    }
  }
  return hasUi;
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

Update* UpdateVars::runUpdater(YAML::Node node)
{  
  Procedure* res = new Procedure();
  forObject(node, [&res,this](std::string key, YAML::Node x) {
    switch (this->getType(key)) 
    {
      case Type::Int : 
        {
          if (isInt(x)) {
            int n = getInt(x, 0);
            Callback<int>* setter = this->getSetInt(key);
            auto proc = [this,setter,n] () { setter->apply(n); };
            res->append(Procedure(proc));
          } else {
            PLOG_ERROR << "update " << key << " is defined as int, but set to non int value";
          }
          break;
        }
      case Type::Double :
        {
          if (isDouble(x)) {
            double d = getDouble(x, 0);          
            Callback<double>* setter = this->getSetDouble(key);
            auto proc = [this,setter,d] () { setter->apply(d); };
            res->append(Procedure(proc));
          } else {
            PLOG_ERROR << "update " << key << " is defined as double, but set to non double value";
          }
          break;
        }
      case Type::String :
        {
          std::string str = getString(x, "");
          Callback<std::string>* setter = this->getSetString(key);
          auto proc = [this,setter,str] () { setter->apply(str); };
          res->append(Procedure(proc));
          break;
        }
    }
  });
  return res;
}

void UpdateVars::run(YAML::Node node)
{  
  forObject(node, [this](std::string trigger, YAML::Node triggerNode) {
    Procedure* setter = this->runUpdater(triggerNode);
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
    Procedure* setter = updater->runUpdater(triggerNode);
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

juce::Justification toJustification (std::string str)
{
  if (str == "left") { return juce::Justification(juce::Justification::left); }
  if (str == "right") { return juce::Justification::right; }
  if (str == "horizontally-centred") { return juce::Justification::horizontallyCentred; }
  if (str == "top") { return juce::Justification::top; }
  if (str == "bottom") { return juce::Justification::bottom; }
  if (str == "vertically-centred") { return juce::Justification::verticallyCentred; }
  if (str == "horizontally-justified") { return juce::Justification::horizontallyJustified; }
  if (str == "centred") { return juce::Justification::centred; }
  if (str == "centred-left") { return juce::Justification::centredLeft; }
  if (str == "centred-right") { return juce::Justification::centredRight; }
  if (str == "centred-top") { return juce::Justification::centredTop; }
  if (str == "centred-bottom") { return juce::Justification::centredBottom; }
  if (str == "top-left") { return juce::Justification::topLeft; }
  if (str == "top-right") { return juce::Justification::topRight; }
  if (str == "bottom-left") { return juce::Justification::bottomLeft; }
  if (str == "bottom-right") { return juce::Justification::bottomRight; }
  return juce::Justification(juce::Justification::centred);
}

/*   
TODO: from justification (do we really need it?)
     left
      right       
      horizontallyCentred
      top
      bottom
      verticallyCentred
      horizontallyJustified
      centred 
      centredLeft 
      centredRight 
      centredTop  
      centredBottom 
      topLeft  
      topRight 
      bottomLeft 
      bottomRight
*/

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

std::string getIconName(YAML::Node node)
{
  std::string name = "logojuce";
  forString(node, "icon", [&name](auto str) { 
    name = str; 
  });  
  return name;
}


std::vector<std::string> getNames(YAML::Node node)
{
  std::vector<std::string> names;
  forNodes(node["names"], [&names] (auto x) {
    names.push_back(getString(x, ""));      
  }); 
  return names;
}

void Widget::parseRadioGroupBy(std::string tag, std::function<void(Style&,Rect,std::string,std::vector<std::string>)> call, YAML::Node node, Rect rect, Style style)
{
  if (hasKey(node, tag) && hasKey(node, "names")) {
    std::vector<std::string> names = getNames(node);

    forString(node, tag, [this, rect, &style, names, call] (auto chan) {
      call(style, rect, chan, names);
    });
  }  
}

void Widget::parseComboBox(YAML::Node node, Rect rect, Style style) 
{
  this->parseRadioGroupBy(
      "combo-box", 
      [this] (Style& s, Rect r, std::string ch, std::vector<std::string> names) { this->comboBox(s, r, ch, names);}, 
      node, rect, style);
}

void Widget::parseCheckGroup(YAML::Node node, Rect rect, Style style) 
{
  this->parseRadioGroupBy(
      "check-group", 
      [this] (Style& s, Rect r, std::string ch, std::vector<std::string> names) { this->checkGroup(s, r, ch, names, r.getWidth() < r.getHeight());}, 
      node, rect, style);
}

void Widget::parseButtonGroup(YAML::Node node, Rect rect, Style style) 
{
  this->parseRadioGroupBy(
      "button-group", 
      [this] (Style& s, Rect r, std::string ch, std::vector<std::string> names) { this->buttonGroup(s, r, ch, names, r.getWidth() < r.getHeight());}, 
      node, rect, style);
}

void Widget::parseHorCheckGroup(YAML::Node node, Rect rect, Style style) 
{
  this->parseRadioGroupBy(
      "hor-check-group", 
      [this] (Style& s, Rect r, std::string ch, std::vector<std::string> names) { this->checkGroup(s, r, ch, names, false);}, 
      node, rect, style);
}

void Widget::parseHorButtonGroup(YAML::Node node, Rect rect, Style style) 
{
  this->parseRadioGroupBy(
      "hor-button-group", 
      [this] (Style& s, Rect r, std::string ch, std::vector<std::string> names) { this->buttonGroup(s, r, ch, names, false);}, 
      node, rect, style);
}

void Widget::parseVerCheckGroup(YAML::Node node, Rect rect, Style style) 
{
  this->parseRadioGroupBy(
      "ver-check-group", 
      [this] (Style& s, Rect r, std::string ch, std::vector<std::string> names) { this->checkGroup(s, r, ch, names, true);}, 
      node, rect, style);
}

void Widget::parseVerButtonGroup(YAML::Node node, Rect rect, Style style) 
{
  this->parseRadioGroupBy(
      "ver-button-group", 
      [this] (Style& s, Rect r, std::string ch, std::vector<std::string> names) { this->buttonGroup(s, r, ch, names, true);}, 
      node, rect, style);
}

Widget::Type toWidgetType (std::string str)
{
  if (str == "auto") { return Widget::Auto; }
  else if (str == "display") { return Widget::Output; }
  else if (str == "input") { return Widget::Input; }
  else if (str == "through") { return Widget::Through; }
  else { return Widget::Auto; }
}

Widget::Type getWidgetType(YAML::Node node)
{
  if (hasKey(node, "type")) {
    return toWidgetType(getString(node["type"], "auto"));
  } else {
    return Widget::Auto;
  }
}

void Widget::parseMeterBy(
  std::string tag, 
  std::function<void(Style&, Rect, std::string, std::vector<Col>)> call, 
  YAML::Node node, Rect rect, Style style)
{
  if (hasKey(node, tag)) {
    std::string chan = "";
    forString(node, tag, [&chan] (auto x) { chan = x; });

    std::vector<Col> colors;
    forNodes(node["colors"], [&colors] (auto col) {
      colors.push_back(Col(getString(col, "green")));
    });

    std::vector<Col> defColors;
    defColors.push_back(Col("olive"));
    defColors.push_back(Col("olive"));
    defColors.push_back(Col("olive"));
    defColors.push_back(Col("green"));
    defColors.push_back(Col("green"));
    defColors.push_back(Col("green"));
    defColors.push_back(Col("green"));
    defColors.push_back(Col("yellow"));
    defColors.push_back(Col("orange"));
    defColors.push_back(Col("red"));
    if (colors.size() == 0) {
      colors = defColors;
    }
    
    call(style, rect, chan, colors);
  }
}

void Widget::parseDotMeter(YAML::Node node, Rect rect, Style style)
{
  this->parseMeterBy("dot-meter", [this] (Style& s, Rect r, std::string chan, std::vector<Col> colors) { this->dotMeter(s, r, chan, colors);},
      node, rect, style);
}

void Widget::parseBarMeter(YAML::Node node, Rect rect, Style style)
{
  this->parseMeterBy("bar-meter", [this] (Style& s, Rect r, std::string chan, std::vector<Col> colors) { this->barMeter(s, r, chan, colors);},
      node, rect, style);
}


void Widget::run(YAML::Node node, Rect rect, Style style) 
{
  std::string name = getWidgetName(node);
  Widget::Type widgetType = getWidgetType(node);
  forString(node, "knob", [this, rect, &style](auto chan) { this->knob(style, rect, chan);});
  forString(node, "slider", [this, rect, &style](auto chan) { this->slider(style, rect, chan); });
  forString(node, "bar", [this, rect, widgetType, &style](auto chan) { this->bar(style, rect, chan, widgetType); });
  forString(node, "button", [this, name, rect, &style](auto chan) { this->button(style, rect, chan, name); });
  forString(node, "icon-button", [this, name, rect, &style](auto chan) { this->iconButton(style, rect, chan, name); });
  forString(node, "icon-toggle-button", [this, name, rect, &style](auto chan) { this->iconToggleButton(style, rect, chan, name); });
  forString(node, "toggle", [this, name, rect, &style](auto chan) { this->toggle(style, rect, chan, name); });
  forString(node, "press-button", [this, name, rect, &style](auto chan) { this->pressButton(style, rect, chan, name); });
  forString(node, "check-toggle", [this, name, rect, &style](auto chan) { this->checkToggle(style, rect, chan, name); });
  forString(node, "label", [this, rect, &style](auto val) { this->label(style, rect, val); });
  forString(node, "text", [this, rect, &style](auto chan) { this->text(style, rect, chan); });  
  forKey(node, "xy-pad", [this, rect, &style](auto xyNode) {
    forString(xyNode, "x", [this, rect, &xyNode, &style] (auto x) {
      forString(xyNode, "y", [this, rect, x, &style](auto y) { this->xyPad(style, rect, x, y); });
    });
  });
  forKey(node, "space", [this, rect] (auto x) { (void)x; this->space(rect); });
  forString(node, "image", [this, rect, &style](auto file) { this->image(style, rect, file); });  
  forKey(node, "dot", [this, rect, &style] (auto x) { (void) x; this->dot(style, rect); });
  forString(node, "bar-display", [this, rect, &style] (auto chan) { this->barDisplay(style, rect, chan); });

  this->parseComboBox(node, rect, style);
  this->parseCheckGroup(node, rect, style);
  this->parseButtonGroup(node, rect, style);
  this->parseHorCheckGroup(node, rect, style);
  this->parseHorButtonGroup(node, rect, style);
  this->parseVerCheckGroup(node, rect, style);
  this->parseVerButtonGroup(node, rect, style);
  this->parseDotMeter(node, rect, style);
  this->parseBarMeter(node, rect, style);
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

  forKey(node, tag, [parent, &style, rect, &boxes](auto elems) {
      parent->widget->groupBegin(style, rect, "");
      if (elems.IsSequence()) {
        forNodes(elems, [&boxes](auto elem) { 
            boxes.push_back(std::pair(getScale(elem), elem));
        });
      }
      parent->widget->groupEnd(style);
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
    parent->widget->groupEnd(style);
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
        parent->widget->panelItemEnd(style);        
      });

      parent->widget->panelEnd(style, name);    
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
    if (hasKey(node, "secondary-color")) {
      forValColor(node, "secondary-color", [this,&style](auto col) { style.secondaryColor = col; });
    } else {
      forValColor(node, "color", [this,&style](auto col) { style.secondaryColor = col; });
    }
    forInt(node, "text-size", [this,&style](auto size) { style.textSize = size; });
    forString(node, "text-align", [this, &style](auto align) { style.textAlign = align; });
    forString(node, "icon", [this, &style](auto icon) { style.icon = icon; });
    if (hasKey(node, "secondary-icon")) {
      forString(node, "secondary-icon", [this, &style](auto icon) { style.secondaryIcon = icon; });
    } else {
      forString(node, "icon", [this, &style](auto icon) { style.secondaryIcon = icon; });
    }
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
  PLOG_DEBUG << "RECT: " << rect.toString() << "\n";
  updateStyle(node, style);
  this->widget->run(node, rect, style);
  runLayout(this, node, rect, style);
}

// -------------------------------------------------------------------
// Csound UI

Set<int> errorScore() {
  return [](int n) {
    (void) n;
    PLOG_ERROR << "Failed to parse score expression";
  };
}

Get<std::string> CsoundUi::getScoreString(YAML::Node node) {
  bool isConst = true;
  std::vector<std::pair<Type, Val<std::string>>> vals;

  forNodes(node, [this, &isConst, &vals](auto valNode) {
      auto val = getValString(valNode, "");
      Type ty = Type::String;
      if (val.isChan()) {
        isConst = false;
        ty = this->getType(val.getChan().name);
      } else {
        if (isInt(valNode)) { ty = Type::Int; }
        if (isDouble(valNode)) { ty = Type::Double; }
      }
      vals.push_back(std::pair(ty, val));
  });

  if (isConst) {
    std::string res = "i";
    std::for_each(vals.begin(), vals.end(), [&res](auto val) {
      res.append(std::string(" "));
      if (val.first != Type::String) {
        res.append(val.second.getVal());
      } else {
        res.append("\"");
        res.append(val.second.getVal());
        res.append("\"");
      }      
    });
    return [res]() { return res; };
  } else {    
    return [this, vals]() {
      std::string res = "i";
      std::for_each(vals.begin(), vals.end(), [this, &res](auto val) {
        res.append(std::string(" "));
        if (val.second.isChan()) {
          std::string chanName = val.second.getChan().name;
          switch (val.first) {
            case Type::Int: { 
              res.append(std::to_string(this->getterInt(chanName)())); 
              break;
            }
            case Type::Double: { 
              res.append(std::to_string(this->getterDouble(chanName)())); 
              break;
            }
            case Type::String: {
                res.append("\"");              
                res.append(this->getterString(chanName)());
                res.append("\"");
                break;
              }
          }
        } else {
          if (val.first != Type::String) {
            res.append(val.second.getVal());
          } else {
            res.append("\"");
            res.append(val.second.getVal());
            res.append("\"");
          }      
        }
      });
      return res; 

    };
  }
  return []() { return "Error failed to parse notes"; };
}

Set<int> CsoundUi::parseSingleNoteScore (YAML::Node node) {
  Get<std::string> getter = this->getScoreString(node);
  return [this, getter](int val) { this->sendScore(getter()); };
}

Set<int> CsoundUi::parseManyNoteScore (YAML::Node node) {
  Set<int> res = emptySet<int>();
  forNodes(node, [this, &res](auto noteNode) {
    res = appendSet(res, this->parseSingleNoteScore(noteNode));
  });
  return res;
}

Set<int> CsoundUi::parseCaseScore (YAML::Node node) {
  Set<int> res = emptySet<int>();
  bool isOk = false;
  forKey(node, "case", [this, &isOk, &res](auto caseNode) {
    if (caseNode.IsSequence()) {
      int val = 0;
      forNodes(caseNode, [this, &val, &res](auto noteNode) {
        int currentVal = val;
        Set<int> current = this->parseNotes(noteNode);
        Set<int> withCond = [currentVal, current](int v) { if (v == currentVal) { current(v); }};
        res = appendSet(res, withCond);
        val++;
      });
    }
    isOk = true;
  });

  return isOk ? res : errorScore();
}

Set<int> CsoundUi::parseNotes(YAML::Node node) {
  if (node[0].IsScalar()) {
    return parseSingleNoteScore(node);
  } else if (node[0].IsSequence()) {
    return parseManyNoteScore(node);
  } 
  
  return errorScore();
}

Set<int> CsoundUi::parseScore(YAML::Node node) {
  if (node.IsSequence() && node.size() > 0) {
    return this->parseNotes(node);
  }

  if (node.IsMap() && hasKey(node, "case")) {
    return parseCaseScore(node);
  }

  return errorScore();
}

void CsoundUi::run(YAML::Node node)
{
  forKey(node, "write", [this](auto writeNode) {
    forNodes(writeNode, [this](auto channel) {
        this->initWriteChannel(getString(channel, "unknown-channel"));
    });       
  });
  
  forKey(node, "read", [this](auto readNode) {
    forNodes(readNode, [this](auto channel) {
        this->initReadChannel(getString(channel, "unknown-channel"));
    });       
  });

  forKey(node, "score", [this](auto scoreNode) {
    forObject(scoreNode, [this](std::string name, auto valNode) {
        this->initScore(name, this->parseScore(valNode));
    });
  });
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
  Style style;
  this->ui->updateStyle(node, style);
  juce::Rectangle<float> topLevelRect = juce::Rectangle<float>(0, 0, 1, 1);
  this->ui->begin(style, topLevelRect);
  this->ui->onKey(node, "ui", topLevelRect, style);
  this->ui->end(style);
  this->csoundUi->onKey(node, "csound");
}

void run(Window* win, YAML::Node node)
{
  forKey(node, "main", [win](auto x) { win->run(x); });
}

}
