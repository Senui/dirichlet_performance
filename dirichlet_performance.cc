#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>

#include "omp.h"

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
  const int num_threads = omp_get_max_threads();

  std::vector<int> sums(num_threads);

  using milli = std::chrono::milliseconds;
  auto start = std::chrono::high_resolution_clock::now();
#ifdef OMP_FOO
  std::cout << "OpenMP enabled" << std::endl;
#pragma omp parallel for
#endif
  for (int i = 0; i < ITER; ++i) {
    std::vector<double> results(vec.size());
    gsl_ran_dirichlet(r_RNG, vec.size(), vec.data(),
                      results.data());
    sums[omp_get_thread_num()] = results[42];
  }
  auto finish = std::chrono::high_resolution_clock::now();
  std::cout << ITER << " gsl_ran_dirichlet executions took "
            << std::chrono::duration_cast<milli>(finish - start).count()
            << " milliseconds\n";
  std::cout << sums[0] << std::endl;
}

int main(int argc, char** argv) {
  draw();
}
