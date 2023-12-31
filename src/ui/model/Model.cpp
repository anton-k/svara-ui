#include "Model.h"
#include <fstream>
#include <iostream>
#include <plog/Log.h>

double Val::getDouble() 
{
  return static_cast<DoubleVal*>(this)->getVal();  
}

Type DoubleVal::getType() { return Type::Double; }
// double DoubleVal::getVal() { return this; }

Type IntVal::getType() { return Type::Int; }
// Val* IntVal::getVal() { return this; }

// ---------------------------------------------------------------------------
// Var

template <class T>
Var<T>::Var(T init, std::string str, bool debug) 
{
  val = init;
  update = new Callback<T>( [this](T v) { this->val = v; } );
  name = str;
  hasDebug = debug;
}

int within(int x, int min, int max )
{
  return ((x - min) % (max - min)) + min;
}

template <>
Var<int>* Var<int>::makeRange(int init, int min, int max) 
{
  Var<int>* res = new Var<int>();
  res->val = within(init, min, max);
  res->update->append(Callback<int>( [res,min,max](int v) { res->val = within(v, min, max); } ));
  return res;  
}

template <class T>
T Var<T>::get() 
{
  return val;
}

template <class T>
void Var<T>::set(T v)
{
  update->apply(v);
  if (hasDebug) {
    PLOG_INFO << "set-var: " << name << ": " << v;
  }
}

template <class T>
void Var<T>::appendCallback(Callback<T>* call)
{
  update->append(*call);
}

// ---------------------------------------------------------------------------
// Vars

template <class T>
void Vars<T>::insert(std::string name, Var<T>* v) 
{
  vars[name] = v;
}

template <class T>
T Vars<T>::get(std::string name)
{
  auto it = vars.find(name);
  if (it == vars.end()) {
    return defaultVal;
  } else {
    return it->second->get();
  }
}

template <class T>
void Vars<T>::set(std::string name, T val) 
{
  auto it = vars.find(name);
  if (it != vars.end()) {
    it->second->set(val);
  }
}

template <class T>
void Vars<T>::appendCallback(std::string name, Callback<T>* call)
{
  auto it = vars.find(name);
  if (it != vars.end()) {
    it->second->appendCallback(call);
  }
}

template<class T>
void Vars<T>::print()
{
  for ( auto it = vars.begin(); it != vars.end(); ++it  )
  {
    PLOG_DEBUG << it->first << " " << std::to_string(it->second->get()) << std::endl;
  } 
}

template <class T>
bool Vars<T>::member(std::string name)
{
  return vars.count(name);
}

// ---------------------------------------------------------------------------
// Enums

void Enum::insert(std::string tag)
{
  tags[tag] = tags.size();
}

int Enum::lookup(std::string tag)
{
  auto it = tags.find(tag);
  if (it != tags.end()) {
    return it->second;
  } else {
    return 0; // TODO: maybe better -1
  }
}

bool Enum::member(std::string tag)
{
  return tags.count(tag);
}

// ---------------------------------------------------------------------------
// State

Type State::getType(std::string name)
{
  if (ints.member(name)) {
    return Type::Int;
  } else if (doubles.member(name)) {
    return Type::Double;
  } else {
    return Type::String;
  }
}

// inserts

void State::insertInt(std::string name, int init, bool needDebug)
{
  ints.insert(name, new Var<int>(init, name, needDebug));
}
    
void State::insertIntRange(std::string name, int init, int min, int max)
{ 
  ints.insert(name, Var<int>::makeRange(init, min, max));
}

void State::insertDouble(std::string name, double init, bool needDebug)
{
  doubles.insert(name, new Var<double>(init, name, needDebug));
}

void State::insertString(std::string name, std::string init, bool needDebug)
{
  strings.insert(name, new Var<std::string>(init, name, needDebug));
}

// getters

int State::getInt(std::string name)
{
  return ints.get(name);
}

double State::getDouble(std::string name)
{
  return doubles.get(name);
}


std::string State::getString(std::string name)
{
  return strings.get(name);
}

// setters

void State::setInt(std::string name, int v)
{
  ints.set(name, v);
}

void State::setDouble(std::string name, double v)
{
  doubles.set(name, v);
}

void State::setString(std::string name, std::string v)
{
  strings.set(name, v);
}

// append callbacks

void State::appendCallbackInt(std::string name, Callback<int>* call)
{
  ints.appendCallback(name, call);
}

void State::appendCallbackDouble(std::string name, Callback<double>* call)
{
  doubles.appendCallback(name, call);
}

void State::appendCallbackString(std::string name, Callback<std::string>* call)
{
  strings.appendCallback(name, call);
}

void State::appendSetter(std::string name, Procedure* call) 
{
  appendSetter(name, call->toFunction());
}

void State::appendSetter(std::string name, std::function<void(void)> call) 
{
  if (ints.member(name)) {
    appendCallbackInt(name, new Callback<int>([this,call](auto x) { (void)x; call(); }));
  } else if (doubles.member(name)) {
    appendCallbackDouble(name, new Callback<double>([this,call](auto x) { (void)x; call(); } ));
  } else if (strings.member(name)) {
    appendCallbackString(name, new Callback<std::string>([this,call](auto x) { (void)x; call(); } ));
  }

}

void State::appendSetterInt(std::string name, std::string key, int val)
{
  appendSetter(name, [this, key,val] { this->setInt(key, val); });
}

void State::appendSetterDouble(std::string name, std::string key, double val)
{
  appendSetter(name, [this, key,val] { this->setDouble(key, val); });
}

void State::appendSetterString(std::string name, std::string key, std::string val)
{
  appendSetter(name, [this, key,val] { this->setString(key, val); });
}

// debug
void State::printInt(std::string name)
{
  PLOG_DEBUG << name << ": " << getInt(name) << "\n";
}

void State::printInts()
{
  ints.print();
}

// ---------------------------------------------------------------------------
// Check

int check_model() 
{
  Val* v1 = new DoubleVal(2);
  Val* v2 = new DoubleVal(2);

  Callback<double> print( [](double d) { PLOG_DEBUG << d << "\n"; });

  Callback<double> sayHi([](double d) { (void)d; PLOG_DEBUG << "Hi\n"; } );
  Callback<double> sayBoo([](double d) { (void)d; PLOG_DEBUG << "Boo\n"; } );

  double res = v1->getDouble() + v2->getDouble();
  print.append(sayHi);
  print.append(sayBoo);
  print.apply(res);
  
  State st;
  // x = 0
  // y = 0
  // z = x + y
  st.insertInt("x", 0, true);
  st.insertInt("y", 2, true);
  st.insertIntRange("z", 0, 0, 5);

  st.appendCallbackInt("x", new Callback<int>([&st](int v) { st.setInt("z", v + st.getInt("y")); }));
  st.appendCallbackInt("y", new Callback<int>([&st](int v) { st.setInt("z", v + st.getInt("x")); }));
  st.appendCallbackInt("z", new Callback<int>([&st](int v) { (void)v; PLOG_DEBUG << "Z was updated with " << st.getInt("z") << "\n"; }));

  st.printInts();
  st.setInt("x", 2);
  st.printInts();
  st.setInt("y", 4);
  st.printInts();

  Var<int> v(1, "v", true);
  v.set(2);
  v.set(4);
  
  Vars<int> vs;
  auto vx = new Var<int>(0, "vx");
  auto vy = new Var<int>(2, "vy");
  vs.insert("x", vx);
  vs.insert("y", vy);
  vs.print();
  vs.set("y", 10);
  vs.print();
  return 0;
}
