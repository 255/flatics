#ifndef FLATICS_UTILITY_H
#define FLATICS_UTILITY_H

#include <cstdint>
#include <random>
#include <chrono>

#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
#define FLATICS_WINDOWS
#else
#define FLATICS_POSIX
#endif

namespace flatics {

  #ifdef FLATICS_WINDOWS
  #include <windows.h>
  inline void sleep_s(unsigned long s) {
    Sleep(s * 1000);
  }
  inline void sleep_ms(unsigned long ms) {
    Sleep(ms);
  }
  inline void sleep_us(unsigned long us) {
    Sleep(us/1000);
  }
  #else //POSIX style
  #include <unistd.h>
  inline void sleep_s(unsigned long s) {
    sleep(s);
  }
  inline void sleep_ms(unsigned long ms) {
    usleep(ms * 1000);
  }
  inline void sleep_us(unsigned long us) {
    usleep(us);
  }
  #endif

  // The rdtsc update frequency (uh, not rate) on my processor -- don't hard code this
  const unsigned int TICK_RATE = 3392200000;

  // Burn up the core for the specified amount of time (according to high_resolution_clock)
  inline double delay(double seconds) {
    using namespace std::chrono;
    double recorded_seconds;
    high_resolution_clock::time_point start = high_resolution_clock::now();
    while((recorded_seconds = duration_cast<duration<double> >(high_resolution_clock::now() - start).count()) < seconds);

    return recorded_seconds;
  }

  inline uint64_t combineUint32(uint32_t low, uint32_t high) {
    return (static_cast<uint64_t>(high) << 32) | low;
  }

  /* Read the TSC, return it as a 64-bit uint */
  inline uint64_t cycleCount() {
    uint32_t low, high;
    asm volatile("rdtsc" : "=a" (low), "=d" (high));
    return static_cast<uint64_t>(high) << 32 | low;
  }

  /* Lovingly borrowed from Intel's How to Benchmark Code Execution Times white paper */
  inline void startTiming(uint32_t& high, uint32_t& low) {
    // per Intel, CPUID blocks out-of-order execution -- call before and after
    asm volatile(
      "cpuid\n\t"
      "rdtsc\n\t"
      "mov %%edx, %0\n\t"
      "mov %%eax, %1"
      : "=r" (high), "=r" (low)
      :: "%rax", "%rbx", "%rcx", "%rdx"
    );
  }

  /* Lovingly borrowed from Intel's How to Benchmark Code Execution Times white paper */
  inline void stopTiming(uint32_t& high, uint32_t& low) {
    // rdtscp is a serializes execution
    asm volatile(
      "rdtscp\n\t"
      "mov %%edx, %0\n\t"
      "mov %%eax, %1\n\t"
      "cpuid"
      : "=r" (high), "=r" (low)
      :: "%rax", "%rbx", "%rcx", "%rdx"
    );
  }

  inline uint64_t cycleDifference(uint32_t start_high, uint32_t start_low, uint32_t end_high, uint32_t end_low) {
    return ((static_cast<uint64_t>(end_high) << 32) | end_low) - ((static_cast<uint64_t>(start_high) << 32) | start_low);
  }

  double estimateClockSpeed(double seconds = 1.0) {
    uint32_t hi_0, lo_0, hi_1, lo_1;

    startTiming(hi_0, lo_0);
    double recorded_time = delay(seconds);
    stopTiming(hi_1, lo_1);

    return static_cast<double>(cycleDifference(hi_0, lo_0, hi_1, lo_1)) / recorded_time;
  }

  template<typename Type>
  inline Type max(Type val1, Type val2) {
    return val2 > val1 ? val2 : val1;
  }

  // Constants
  const double EARTH_GRAVITY_ACCEL = 9.80665d;
  const double G = 6.6738480e-11d;

  // A random number for general use
  std::default_random_engine randGen;

  void init() {
    // TODO: set the random seed
  }
}

#endif //FLATICS_UTILITY_H
