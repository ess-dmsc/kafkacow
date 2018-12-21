#include <gtest/gtest.h>

std::string BinDirectory;

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  BinDirectory = argv[0];
  return RUN_ALL_TESTS();
}
