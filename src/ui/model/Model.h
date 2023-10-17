#pragma once
#include <vector>
#include <set>
#include <functional>
#include <map>
#include <string>
#include <csound.hpp>
#include <plog/Log.h>
#include "../../general/Fun.h"

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

// Callback with zero arguments
class Procedure
{
  public:
    Procedure() { funs = std::vector<std::function<void()>>(); }
    Procedure (std::function<void()> f) { funs = std::vector<std::function<void()>>(); funs.push_back(f); };

    void apply() const
    {
      std::for_each(funs.begin(), funs.end(), [](auto f) { f(); } ) ;
    }

    void append (Procedure that)
    {
      funs.insert(funs.end(), that.funs.begin(), that.funs.end() );
    }

    void append (std::function<void()> f)
    {
      funs.push_back(f);
    }

    std::function<void()> toFunction() const
    {
      return [this] { this->apply();};
    }

  private:
    std::vector<std::function<void()>> funs;
};

// We can a append callbacks and they are accumulated
template<typename T>
class Callback
{
  public:
    Callback<T>() { funs = std::vector<std::function<void(T)>>(); }
    Callback<T> (std::function<void(T)> f) { funs = std::vector<std::function<void(T)>>(); funs.push_back(f); };

    void apply(T val) const
    {
      std::for_each(funs.begin(), funs.end(), [val](auto f) { f(val); } ) ;
    }

    void append(Callback<T> that)
    {
      funs.insert(funs.end(), that.funs.begin(), that.funs.end() );
    }

    void append(std::function<void(T)> f)
    {
      funs.push_back(f);
    }

    Procedure toProcedure(T val) const
    {
      auto setter = [this,val] { this->apply(val); };
      return Procedure(setter);
    }

    std::function<void(T)> toFunction() const
    {
      return [this] (T val) { this->apply(val); };
    }

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
    Var<T>(): val(), update(), name(""), hasDebug(false) {}
    Var<T>(T init, std::string name, bool hasDebug = false);
    static Var<T>* makeRange(T init, T min, T max);

    T get();
    void set(T);
    void appendCallback(Callback<T>* call);

  private:
    T val;
    Callback<T>* update;
    std::string name;
    bool hasDebug;
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

    void appendCallback(std::string name, Callback<T>* call);

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
    void insertInt(std::string name, int init, bool needDebug);
    void insertIntRange(std::string name, int init, int min, int max);
    void insertDouble(std::string name, double init, bool needDebug);
    void insertDoubleRange(std::string name, double init, double min, double max);
    void insertString(std::string name, std::string init, bool needDebug);

    // get variable type
    Type getType(std::string name);

    // get variable value
    int getInt(std::string name);
    double getDouble(std::string name);
    std::string getString(std::string name);

    // set variable with value
    void setInt(std::string name, int v);
    void setDouble(std::string name, double v);
    void setString(std::string name, std::string v);

    // append callback to variable
    void appendCallbackInt(std::string name, Callback<int>* call);
    void appendCallbackDouble(std::string name, Callback<double>* call);
    void appendCallbackString(std::string name, Callback<std::string>* call);

    void appendSetter(std::string name, Procedure* call);
    void appendSetterInt (std::string name, std::string key, int val);
    void appendSetterDouble (std::string name, std::string key, double val);
    void appendSetterString (std::string name, std::string key, std::string val);

    // debug
    void printInt(std::string name);
    void printInts();

  private:
    void appendSetter(std::string name, std::function<void(void)> call);
    Vars<int> ints;
    Vars<double> doubles;
    Vars<std::string> strings;
    std::map<std::string, Enum*> enums;
};

class Chan {
  public:
    Chan(): name("") {}
    Chan(std::string _name): name(_name) {}
    std::string name;

    friend bool operator == (Chan const& l, Chan const& r) noexcept
    {
      return l.name == r.name;
    };

    friend bool operator < (Chan const& l, Chan const& r) noexcept
    {
      return l.name < r.name;
    };
};

// Expressions that can have dependencies on readable channels
template<typename T>
class Expr {
  public:
    Expr() {};
    Expr(T val):
      getter([val] () { return val; }),
      chans(std::set<Chan>())
    {};

    Expr(std::function<T()> _getter, std::set<Chan> _chan):
      getter(_getter),
      chans(_chan)
    {}

    T apply() const
    {
      return getter();
    }

    std::set<Chan> getChans() const
    {
      return chans;
    }

    template <typename A, typename B>
    static Expr<B> ap(Expr<std::function<B(A)>> f, Expr<A> a)
    {
      std::set<Chan> resChans;
      resChans.insert(resChans.end(), f.chans.begin(), f.chans.end());
      resChans.insert(resChans.end(), a.chans.begin(), a.chans.end());
      auto resGetter = [f,a] { return f.getter()(a.getter()); };
      return Expr<B>(resGetter, resChans);
    }

/*
    template <typename A, typename B, typename C>
    static Expr<C> lift2 (Expr<std::function<C(A,B)>> f, Expr<A> a, Expr<B> b)
    {
      std::set<Chan> resChans;
      resChans.insert(resChans.end(), f.chans.begin(), f.chans.end());
      resChans.insert(resChans.end(), a.chans.begin(), a.chans.end());
      resChans.insert(resChans.end(), b.chans.begin(), b.chans.end());

      auto resGetter = [f,a,b] { return f.getter()(a.getter(), b.getter()); };
      return Expr<B>(resGetter, resChans);
    }

    template <typename A, typename B, typename C, typename D>
    static Expr<D> lift3 (Expr<std::function<D(A,B,C)>> f, Expr<A> a, Expr<B> b, Expr<C> c)
    {
      std::set<Chan> resChans;
      resChans.insert(resChans.end(), f.chans.begin(), f.chans.end());
      resChans.insert(resChans.end(), a.chans.begin(), a.chans.end());
      resChans.insert(resChans.end(), b.chans.begin(), b.chans.end());
      resChans.insert(resChans.end(), c.chans.begin(), c.chans.end());

      auto resGetter = [f,a,b,c] { return f.getter()(a.getter(), b.getter(), c.getter()); };
      return Expr<B>(resGetter, resChans);
    }

    template <typename A, typename B>
    static Expr<B> lifts (Expr<std::function<B(std::vector<A>)>> f, std::vector<Expr<A>> as)
    {
      std::set<Chan> resChans;
      std::for_each(as.begin(), as.end(), [&resChans] (auto a) {
        resChans.insert(resChans.end(), a.chans.begin(), a.chans.end());
      });

      auto resGetter = [f,as] {
        std::vector<A> args;
        for_each(as.begin(), as.end(), [&args](auto a) {
          args.push_back(a.getter());
        });
        return f.getter()(args) ;
      };
      return Expr<B>(resGetter, resChans);
    }
*/

  private:
    std::function<T()> getter;
    std::set<Chan> chans;
};

Expr<int> readIntChan(Chan chan, State* state);
Expr<double> readDoubleChan(Chan chan, State* state);
Expr<std::string> readStringChan(Chan chan, State* state);

template <typename A, typename B>
Expr<B> map(std::function<B(A)> f, Expr<A> a)
{
  return Expr<B>([a,f] { return f(a.apply()); }, std::set<Chan>(a.getChans()));
}

// Interface for Csound engine. Which actions UI can perform
class CsdModel {
  public:
    // return setter of control channel
    virtual Set<MYFLT> setChannel(std::string name) = 0;

    // return setter of string channel
    virtual Set<std::string> setStringChannel(std::string name) = 0;

    // return getter of control channel
    virtual Get<MYFLT> getChannel(std::string name) = 0;

    // return getter of control channel
    virtual Get<std::string> getStringChannel(std::string name) = 0;

    // schedule a note
    virtual void readScore(std::string &sco) = 0;
};

// Mock csd implementation does nothing
class MockCsdModel : public CsdModel {
  public:
    Set<MYFLT> setChannel(std::string name) override {
      return [name](MYFLT val) {
        PLOG_INFO << "[csd] set control channel " << name << " to: " << val;
      };
    }
    Set<std::string> setStringChannel(std::string name) override {
      return [name](std::string val) {
        PLOG_INFO << "[csd] set string channel " << name << " to: " << val;
      };
    }

    Get<MYFLT> getChannel(std::string _name) override {
      return []() { return 0; };
    }

    Get<std::string> getStringChannel(std::string _name) override {
      return []() { return ""; };
    }

    void readScore(std::string &sco) override {
      PLOG_INFO << "[csd] read score: " << sco;
    };
};

// Real Csound interface implementation
class RealCsdModel : public CsdModel {
  public:
    RealCsdModel(Csound* _csound): csound(_csound) {}

    Set<MYFLT> setChannel(std::string name) override {
      MYFLT *channelPtr;
      csound->GetChannelPtr(channelPtr, name.c_str(), CSOUND_INPUT_CHANNEL | CSOUND_CONTROL_CHANNEL);

      Set<MYFLT> setter = [channelPtr](MYFLT val) {
        *channelPtr = val;
      };
      return setter;
    }

    Set<std::string> setStringChannel(std::string name) override {
      return [this, name](std::string val) {
        this->csound->SetStringChannel(name.c_str(), (char *) val.c_str());
      };
    }

    Get<MYFLT> getChannel(std::string name) override {
      MYFLT *channelPtr;
      csound->GetChannelPtr(channelPtr, name.c_str(), CSOUND_OUTPUT_CHANNEL | CSOUND_CONTROL_CHANNEL);

      Get<MYFLT> getter = [channelPtr]() {
        return *channelPtr;
      };
      return getter;
    }

    Get<std::string> getStringChannel(std::string name) override {
      return [this, name]() {
        char* result = {};
        this->csound->GetStringChannel(name.c_str(), result);
        return std::string(result);
      };
    }

    void readScore(std::string &sco) override {
      PLOG_INFO << "[csd] read score: " << sco;
      csound->ReadScore(sco.c_str());
    };

  private:
    Csound* csound;
};
