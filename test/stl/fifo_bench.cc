#include <deque>
#include <list>

#include <benchmark/benchmark.h>

using namespace benchmark;

template<template<typename T> class C>
static inline void fifo_push_front_bench(State &state) {
  const auto n = state.range(0);
  for (auto _ : state) {
    C<int> fifo;
    for (int i = 0; i < n; ++i)
      fifo.push_front(1);
  }
}

template<template<typename T> class C>
static inline void fifo_push_back_bench(State &state) {
  const auto n = state.range(0);
  for (auto _ : state) {
    C<int> fifo;
    for (int i = 0; i < n; ++i)
      fifo.push_front(1);
  }
}

template<template<typename T> class C>
static inline void fifo_pop_front_bench(State &state) {
  C<int> fifo;
  const int n = state.range(0);
  for (auto _ : state) {
    state.PauseTiming();
    for (int i = 0; i < n; ++i) {
      fifo.push_front(i);
    }
    state.ResumeTiming();
    for (int i = 0; i < n; ++i) {
      fifo.pop_front();
    }
  }
}

template<template<typename T> class C>
static inline void fifo_pop_back_bench(State &state) {
  C<int> fifo;
  const int n = state.range(0);
  for (auto _ : state) {
    state.PauseTiming();
    for (int i = 0; i < n; ++i) {
      fifo.push_back(i);
    }
    state.ResumeTiming();
    for (int i = 0; i < n; ++i) {
      fifo.pop_back();
    }
  }
}

template<typename T>
using Deque = std::deque<T>;

template<typename T>
using List = std::list<T>;

static inline void deque_push_back(State &state) { fifo_push_back_bench<Deque>(state); }
static inline void deque_pop_back(State &state) { fifo_pop_back_bench<Deque>(state); }
static inline void deque_push_front(State &state) { fifo_push_front_bench<Deque>(state); }
static inline void deque_pop_front(State &state) { fifo_pop_front_bench<Deque>(state); }

static inline void list_push_back(State &state) { fifo_push_back_bench<List>(state); }
static inline void list_pop_back(State &state) { fifo_pop_back_bench<List>(state); }
static inline void list_push_front(State &state) { fifo_push_front_bench<List>(state); }
static inline void list_pop_front(State &state) { fifo_pop_front_bench<List>(state); }

#define FIFO_BENCHMARK(func) \
  BENCHMARK(func)->Name(#func)->RangeMultiplier(10)->Range(1, 1000000)

FIFO_BENCHMARK(deque_push_back);
FIFO_BENCHMARK(deque_push_front);
FIFO_BENCHMARK(deque_pop_back);
FIFO_BENCHMARK(deque_pop_front);
FIFO_BENCHMARK(list_push_back);
FIFO_BENCHMARK(list_push_front);
FIFO_BENCHMARK(list_pop_back);
FIFO_BENCHMARK(list_pop_front);