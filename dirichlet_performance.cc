#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>

#ifdef OMP_ENABLED
#include "omp.h"
#endif

#include <algorithm>
#include <iostream>
#include <array>
#include <iterator>
#include <random>
#include <vector>
#include <chrono>
#include <numeric>

using namespace std;

#define ITER 170000 * 7
#define VEC_SIZE 380

struct ThreadData {
  int sum;
  char padding[64];  // add padding to avoid false sharing
};

vector<double> GenerateRandomVector() {
  random_device rnd_device;
  mt19937 mersenne_engine{rnd_device()};
  uniform_int_distribution<int> dist{1, 52};

  auto gen = [&dist, &mersenne_engine]() {
    return static_cast<double>(dist(mersenne_engine));
  };

  vector<double> vec(VEC_SIZE);
  generate(begin(vec), end(vec), gen);
  return vec;
}

void draw() {
  vector<double> vec = GenerateRandomVector();
  gsl_rng* r_RNG = gsl_rng_alloc(gsl_rng_mt19937);
#ifdef OMP_ENABLED
  const int num_threads = omp_get_max_threads();
#else
  const int num_threads = 1;
#endif
  std::vector<ThreadData> thread_data(num_threads);

  using milli = std::chrono::milliseconds;
  auto start = std::chrono::high_resolution_clock::now();
#ifdef OMP_ENABLED
  std::cout << "OpenMP enabled" << std::endl;
#pragma omp parallel for
#endif
  for (int i = 0; i < ITER; ++i) {
    std::vector<double> results(vec.size());
    gsl_ran_dirichlet(r_RNG, vec.size(), vec.data(),
                      results.data());
#ifdef OMP_ENABLED
  const int idx = omp_get_max_threads();
#else
  const int idx = 0;
#endif

    thread_data[idx].sum = results[42];
  }
  auto finish = std::chrono::high_resolution_clock::now();
  std::cout << ITER << " gsl_ran_dirichlet executions took "
            << std::chrono::duration_cast<milli>(finish - start).count()
            << " milliseconds\n";
  std::cout << thread_data[0].sum << std::endl;
}

int main(int argc, char** argv) {
  draw();
}
