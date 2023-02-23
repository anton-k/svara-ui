#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "../MainComponent.h"
#include "GUIParser.h"

//# https://github.com/biojppm/rapidyaml/blob/master/samples/quickstart.cpp
juce::Colour parse_color(std::string str) 
{
  if (str.compare("blue") == 0) { return juce::Colour (0xff0074d9); }
  if (str.compare("red") == 0) { return juce::Colour (0xffff4136); }
  return juce::Colour (0xff0074d9);
}

double get_double (YAML::Node node, double def) 
{
    double init = def;
    if (node.IsScalar()) {
      try {
        init = std::stod(node.as<std::string>());
      } catch (const std::invalid_argument &) {                                      
      } catch (const std::out_of_range &) {                                          
      }
    }
  return init;
}

std::string get_string (YAML::Node node, std::string def) {
  std::string col = def;
  if (node.IsScalar()) {
    col = node.as<std::string>();
  }
  return col;
}

double get_init (YAML::Node node) { return get_double (node["init"], 0); }

juce::Colour get_color (YAML::Node node) { return parse_color (get_string(node["color"], "blue")); }

bool has_key (YAML::Node node, std::string key) { return node[key].IsDefined(); }

class Chan {
  public:
    Chan() { name = ""; }
    Chan (std::string label) { name = label; }

    std::string getName() { return name; }
  private:
    std::string name;
};

Chan get_chan (YAML::Node node) { return Chan(get_string(node["chan"], "")); }

std::string get_text (YAML::Node node) { return get_string(node["text"], ""); }


// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
// Dial

class Dial : public Box
{
  public:
    Dial(double d, juce::Colour col, Chan ch) { init = d; colour = col; chan = ch; }

    void print() override
    {
      std::cout << "Dial (" << init << ", " << colour.toString() << ", " << chan.getName() << ")" << std::endl;      
    }   

  private:
    double init;
    juce::Colour colour;
    Chan chan;
};

bool is_dial (YAML::Node node) { return has_key(node, "dial"); }

Dial get_dial (YAML::Node tree)
  {
    YAML::Node params = tree["dial"];    
    return Dial(get_init(params), get_color(params), get_chan(params));
  }

// --------------------------------------------------------------------------
// Slider

class Slider : public Box
{
  public:
    Slider(double d, juce::Colour col, Chan ch) { init = d; colour = col; chan = ch; }

    void print() override
    {
      std::cout << "Slider (" << init << ", " << colour.toString() << ", " << chan.getName() << ")" << std::endl;      
    }   

  private:
    double init;
    juce::Colour colour;
    Chan chan;
};

bool is_slider (YAML::Node node) { return has_key(node, "slider"); }

Slider get_slider (YAML::Node tree)
  {
    YAML::Node params = tree["slider"];    
    return Slider(get_init(params), get_color(params), get_chan(params));
  }

// --------------------------------------------------------------------------
// Button

class Button : public Box
{
  public:
    Button(std::string str, juce::Colour col, Chan ch) { name = str; colour = col; chan = ch; }

    void print() override
    {
      std::cout << "Button (" << name << ", " << colour.toString() << ", " << chan.getName() << ")" << std::endl;      
    }   

  private:
    std::string name;
    juce::Colour colour; 
    Chan chan;
};

bool is_button (YAML::Node node) { return has_key(node, "button"); }

Button get_button (YAML::Node tree)
  {
    YAML::Node params = tree["button"];    
    return Button(get_text(params), get_color(params), get_chan(params));
  }

// --------------------------------------------------------------------------
//  Layout

std::vector<Box*> get_boxes (YAML::Node params) 
{
  std::vector<Box*> elements;

  for (std::size_t i=0; i < params.size(); i++) {
    YAML::Node elem = params[i];

    if (is_dial(elem))   { elements.push_back(new Dial(get_dial(elem))); }
    if (is_slider(elem)) { elements.push_back(new Slider(get_slider(elem))); }
    if (is_button(elem)) { elements.push_back(new Button(get_button(elem))); }
    if (is_hor(elem)) { elements.push_back(new Hor(get_hor(elem))); }
    if (is_ver(elem)) { elements.push_back(new Ver(get_ver(elem))); }
    if (is_space(elem)) { elements.push_back(new Space(get_space()) ); }
  }
  return elements;
}

bool is_hor (YAML::Node node) { return node["hor"].IsSequence(); }

Hor get_hor (YAML::Node node) { return Hor(get_boxes (node["hor"])); }

bool is_ver (YAML::Node node) { return node["ver"].IsSequence(); }

Ver get_ver (YAML::Node node) { return Ver(get_boxes (node["ver"])); }

bool is_space(YAML::Node node) { return has_key(node, "space"); }

Space get_space () { return Space(); }


// --------------------------------------------------------------------------
// Test


std::string get_file_contents(const char *filename)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in) {
        std::cerr << "could not open " << filename << std::endl;
        exit(1);
    }
    std::ostringstream contents;
    contents << in.rdbuf();
    return contents.str();
}


// https://github.com/jbeder/yaml-cpp/wiki/Tutorial
int check_rapid_yaml() 
{
  YAML::Node node = YAML::LoadFile("config.yaml");

  if (is_dial(node)) { get_dial(node).print(); }
  if (is_slider(node)) { get_slider(node).print(); }
  if (is_button(node)) { get_button(node).print(); }
  std::cout << node["hor"].IsSequence() << "\n";
  if (is_hor(node)) { std::cout << "Here\n"; get_hor(node).print(); }
  else { std::cout << "pre\n"; }
  return 0;
}




