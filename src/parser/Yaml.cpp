#include "Yaml.h"

namespace Parser
{

double getDouble (YAML::Node node, double def) 
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

bool isDouble(YAML::Node node)
{
  if (node.IsScalar()) {
    try {
      std::stod(node.as<std::string>());
      return true;
    } catch (const std::invalid_argument &) {                                      
    } catch (const std::out_of_range &) {                                          
    }
  }
  return false;
}

int getInt (YAML::Node node, int def) 
{
  int init = def;

  if (node.IsScalar()) {
    try {
      init = std::stoi(node.as<std::string>());
    } catch (const std::invalid_argument &) {                                      
    } catch (const std::out_of_range &) {                                          
    }
  }
  return init;
}

bool isInt(YAML::Node node)
{
  if (node.IsScalar()) {
    try {
      std::stoi(node.as<std::string>());
      return true;
    } catch (const std::invalid_argument &) {                                      
    } catch (const std::out_of_range &) {                                          
    }
  }
  return false;
}

std::string getString (YAML::Node node, std::string def) 
{
  std::string init = def;
  if (node.IsScalar()) {
      init = node.as<std::string>();
  }
  return init;
}
  
void forNodes(YAML::Node node, std::function<void(YAML::Node)> go)
{
  for (std::size_t i=0; i < node.size(); i++) {
    go(node[i]);
  }    
}

bool hasKey (YAML::Node node, std::string key) { return node[key].IsDefined(); }

bool forKey (YAML::Node node, std::string key, std::function<void(YAML::Node)> go) 
{
  bool ok = hasKey(node, key);
  
  if (ok) { 
    go(node[key]); 
  }
  return ok;
}


void forObject (YAML::Node node, std::function<void(std::string key, YAML::Node)> go)
{
  for(YAML::const_iterator it=node.begin();it!=node.end();++it) {
    go(it->first.as<std::string>(), it->second);
  }
}

Col getColor(YAML::Node node, Col def)
{
  return Col(getString(node, def.val));
}

void forInt(YAML::Node node, std::string key, std::function<void(int)> go)
{
  forKey(node, key, [&go](auto x) { go(getInt(x, 0));});
}

void forDouble(YAML::Node node, std::string key, std::function<void(double)> go)
{
  forKey(node, key, [&go](auto x) { go(getDouble(x, 0));});
}

void forString(YAML::Node node, std::string key, std::function<void(std::string)> go)
{
  forKey(node, key, [&go](auto x) { go(getString(x, ""));});
}

void forColor(YAML::Node node, std::string key, std::function<void(Col)> go)
{
  forKey(node, key, [&go](auto x) { go(getColor(x, Col("blue")));});
}

}


