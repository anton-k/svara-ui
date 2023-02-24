#pragma once
#include "../../libs/yaml-cpp/include/yaml-cpp/yaml.h"

/*
int check_rapid_yaml();

class Box
{
  public:
    Box() {}
    virtual void print() {};
};


class Hor : public Box
{
  public:
    Hor() {}
    Hor (std::vector<Box*> v) { elements = v; }

    void print() override
    {
      std::cout << "Hor(\n";
      std::for_each(elements.begin(), elements.end(), [](auto box) { std::cout << "  "; box->print(); });
      std::cout << ")\n";
    }

  private:
    std::vector<Box*> elements;
};

class Ver : public Box
{
  public:
    Ver() {}
    Ver (std::vector<Box*> v) { elements = v; }

    void print() override
    {
      std::cout << "Ver(\n";
      std::for_each(elements.begin(), elements.end(), [](auto box) { std::cout << "  "; box->print(); });
      std::cout << ")\n";
    }

  private:
    std::vector<Box*> elements;
};


class Space : public Box
{
  public:
    Space() {}
    void print () override
    {
      std::cout << "Space\n";
    }
};

bool is_hor (YAML::Node node);
Hor get_hor (YAML::Node node);
bool is_ver (YAML::Node node);
Ver get_ver (YAML::Node node);
bool is_space (YAML::Node node);
Space get_space ();
*/
