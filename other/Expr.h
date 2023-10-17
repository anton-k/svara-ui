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


