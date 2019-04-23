#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <gtest/gtest.h>

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  // log errors during tests
  auto Logger = spdlog::stderr_color_mt("LOG");

  return RUN_ALL_TESTS();
}
