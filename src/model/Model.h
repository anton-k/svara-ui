#pragma once
#include <vector>
#include <functional>
#include <map>
#include <string>

int check_model();

enum class Type { Int, Double, String };

// ---------------------------------------------------------------------------
// Values

class Val
{
  public:
    virtual Type getType() = 0;
    // virtual Val* getVal() = 0;

    double getDouble();
    int getInt();
    // TODO
    // getString();
};

class DoubleVal : public Val
{
  public:
    DoubleVal () { val = 0; }
    DoubleVal (double v) { val = v; };

    Type getType() override;
    double getVal() { return val; };

  private:
    double val;
};

class IntVal : public Val
{
  public:
    IntVal () { val = 0; }
    IntVal (int v) { val = v; };

    Type getType() override;
    int getVal() { return val; };

  private:
    int val;
};

// ---------------------------------------------------------------------------
// Callbacks


// We can a append callbacks and they are accumulated
template<class T>
class Callback
{
  public:
    Callback<T>() { funs = std::vector<std::function<void(T)>>(); }
    Callback<T> (std::function<void(T)> f) { funs = std::vector<std::function<void(T)>>(); funs.push_back(f); };
    void apply(T val);

    void append (Callback<T> that);
    void append (std::function<void(T)> f);

  private:
    std::vector<std::function<void(T)>> funs;
};

// ---------------------------------------------------------------------------
// Var

template <class T>
class Var
{
  enum Type { Int, Double, String };

  public:
    Var<T>(): val(), update() {}
    Var<T>(T init);
    Var<T>(T init, std::function<void(T)> setter);
    static Var<T>* makeRange(T init, T min, T max);

    T get();
    void set(T);
    void appendCallback(std::function<void(T)> call);

  private:
    T val;
    Callback<T> update;
};

// ---------------------------------------------------------------------------
// State


template <class T>
class Vars
{
  public:
    Vars(): vars() {}
    Vars(T def) { defaultVal = def; } ;

    void insert(std::string, Var<T>*);

    T get(std::string name);

    void set(std::string name, T v);

    void appendCallback(std::string name, std::function<void(T)> call);

    void print();

    bool member(std::string);

  private:
    T defaultVal;
    std::map<std::string, Var<T>*> vars;
};

class Enum
{
  public:
    Enum();
    void insert(std::string tag);
    int lookup(std::string tag);
    bool member(std::string tag);

  private:
    std::map<std::string, int> tags;
};

class State
{
  public:


    State(): ints(Vars<int>(0)), doubles(Vars<double>(0)), strings(Vars<std::string>("")) {}

    // register new variable
    void insertInt(std::string name, int init);
    void insertIntRange(std::string name, int init, int min, int max);
    void insertDouble(std::string name, double init);
    void insertDoubleRange(std::string name, double init, double min, double max);
    void insertString(std::string name, std::string init);

    // get variable value
    int getInt(std::string name);
    double getDouble(std::string name);
    std::string getString(std::string name);

    // set variable with value
    void setInt(std::string name, int v);
    void setDouble(std::string name, double v);
    void setString(std::string name, std::string v);

    // append callback to variable
    void appendCallbackInt(std::string name, std::function<void(int)> call);
    void appendCallbackDouble(std::string name, std::function<void(double)> call);
    void appendCallbackString(std::string name, std::function<void(std::string)> call);

    void appendSetter(std::string name, std::function<void(void)> call);
    void appendSetterInt (std::string name, std::string key, int val);
    void appendSetterDouble (std::string name, std::string key, double val);
    void appendSetterString (std::string name, std::string key, std::string val);

    // debug
    void printInt(std::string name);
    void printInts();

  private:
    Vars<int> ints;
    Vars<double> doubles;
    Vars<std::string> strings;
    std::map<std::string, Enum*> enums;
};
