// YAML helpers

#pragma once
#include "../../libs/yaml-cpp/include/yaml-cpp/yaml.h"
#include <string>
#include <functional>
#include "Parser.h"

namespace Parser
{
  bool hasKey (YAML::Node node, std::string key);

  bool forKey (YAML::Node node, std::string key, std::function<void(YAML::Node)> go);

  void forObject (YAML::Node node, std::function<void(std::string key, YAML::Node)> go);

  double getDouble (YAML::Node node, double def);

  bool isDouble(YAML::Node node);

  int getInt (YAML::Node node, int def);

  bool isInt(YAML::Node node);

  std::string getString (YAML::Node node, std::string def);

  void forNodes(YAML::Node node, std::function<void(YAML::Node)> go);

  // domain specific funs

  Col getColor(YAML::Node node);
  Val<int> getValInt (YAML::Node node, int def);
  Val<double> getValDouble (YAML::Node node, double def);
  Val<std::string> getValString (YAML::Node node, std::string def);
  Val<Col> getValColor (YAML::Node node);

  void forInt(YAML::Node node, std::string key, std::function<void(int)> go);
  void forDouble(YAML::Node node, std::string key, std::function<void(double)> go);
  void forString(YAML::Node node, std::string key, std::function<void(std::string)> go);
  void forColor(YAML::Node node, std::string key, std::function<void(Col)> go);

  void forValInt(YAML::Node node, std::string key, std::function<void(Val<int>)> go);
  void forValDouble(YAML::Node node, std::string key, std::function<void(Val<double>)> go);
  void forValString(YAML::Node node, std::string key, std::function<void(Val<std::string>)> go);
  void forValColor(YAML::Node node, std::string key, std::function<void(Val<Col>)> go);
}



