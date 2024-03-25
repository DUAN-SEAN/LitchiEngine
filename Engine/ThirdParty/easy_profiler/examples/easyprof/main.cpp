#define BUILD_WITH_EASY_PROFILER
#define USING_EASY_PROFILER
#include <iostream>
#include <chrono>
#include <easy/profiler.h>
#include <easy/easy_protocol.h>
#include <iostream>
#include <glog/logging.h>

#include <thread>

void bar() {
  EASY_FUNCTION(0xfff080aa); // Function block with custom ARGB color
  std::this_thread::sleep_for(
      std::chrono::milliseconds(10)); // sleep for 1 second
}

void baz() {
  EASY_FUNCTION(); // Function block with default color == Amber100
  std::this_thread::sleep_for(
      std::chrono::milliseconds(20)); // sleep for 1 second
}

void foo() {
  EASY_FUNCTION(profiler::colors::Magenta); // Magenta block with name "foo"

  EASY_BLOCK("Calculating sum"); // Begin block with default color == Amber100
  int sum = 0;
  for (int i = 0; i < 10; ++i) {
    EASY_BLOCK(
        "Addition",
        profiler::colors::Red); // Scoped red block (no EASY_END_BLOCK needed)
    sum += i;
  }
  EASY_END_BLOCK; // End of "Calculating sum" block

  EASY_BLOCK("Calculating multiplication",
             profiler::colors::Blue500); // Blue block
  int mul = 1;
  for (int i = 1; i < 11; ++i)
    mul *= i;
  // EASY_END_BLOCK; // This is not needed because all blocks are ended on
  // destructor when closing braces met
  std::this_thread::sleep_for(
      std::chrono::milliseconds(100)); // sleep for 1 second
  bar();
  baz();
}

int main() {
  profiler::startListen();
  EASY_PROFILER_ENABLE; // Enable profiler
  int i = 0;
  while (true) {
    foo();
    bar();
    baz();
    i++;
    std::cout << "this is a info call" << std::endl;

//    if (i > 10000) {
//      break;
//    }
  }
  profiler::stopListen();
}