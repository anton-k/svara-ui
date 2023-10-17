#pragma once
#include <algorithm>
#include <optional>

// functional types

using Proc = std::function<void()>;

template<typename T>
using Get = std::function<T()>;

template<typename T>
using Set = std::function<void(T)>;

template<typename A, typename B>
using Fun = std::function<B(A)>;

template<typename A>
using Endo = std::function<A(A)>;

template<typename A>
using Maybe = std::optional<A>;

// setter

template<typename T>
Set<T> appendSet(Set<T> a, Set<T> b) {
  return [a, b](T val) {
    a(val);
    b(val);
  };
}

inline Proc appendProc(Proc a, Proc b) {
  return [a, b] () {
    a();
    b();
  };
}

template<typename T>
Set<T> emptySet() {
  return [](T val) { (void) val; };
}

template<typename T>
Set<T> filter(Set<T> setter, Fun<T,bool> cond) {
  return [setter, cond](T val) { if (cond(val)) { setter(val); } };
}

inline Proc filter(Proc setter, Get<bool> cond) {
  return [setter, cond]() { if (cond()) { setter(); } };
}

// map

template<typename A, typename B>
Get<B> map(Get<A> g, Fun<A,B> f) {
  return [g,f]() { f(g()); };
}

template<typename A, typename B>
Set<B> cmap(Set<A> setter, Fun<B,A> f) {
  return [setter,f](A val) { setter(f(val)); };
}

template<typename A, typename B>
Maybe<B> map(Maybe<A> opt, Fun<A,B> f) {
  if (opt.has_value()) {
    return std::optional<B>(f(opt.value()));
  } else {
    return std::optional<B>();
  }
}

// Cat

template<typename A, typename B, typename C>
Fun<A,C> compose(Fun<A,B> g, Fun<B,C> f) {
  return [g,f](auto val) { f(g(val)); };
}

template<typename A>
Fun<A,A> identity() {
  return [](A a) { return a; };
}

// forEach

template<typename T>
void forEach(Maybe<T> &opt, Set<T> f) {
  if (opt.has_value()) {
    f(opt.value());
  }
}

template<typename T>
void forEach(std::vector<T> &v, Set<T> f) {
  std::for_each(v.begin(), v.end(), f);
}

// optional

template<typename A>
Maybe<A> alternative(Maybe<A> a, Maybe<A> b) {
  if (a.has_value()) {
    return a;
  } else {
    return b;
  }
}
