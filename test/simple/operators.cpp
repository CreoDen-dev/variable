#include <gtest/gtest.h>

#include <variable.hpp>

using variable::Var;

TEST(simple, Operators) {
  Var x = 1;

  ASSERT_TRUE(x == 1);
  x++;
  ASSERT_TRUE(x == 2);
  x--;
  ASSERT_TRUE(x == 1);
  ++x;
  ASSERT_TRUE(x == 2);
  --x;
  ASSERT_TRUE(x == 1);

  x = 0;
  ASSERT_TRUE(x == 0);
  x += 10;
  ASSERT_TRUE(x == 10);
  x -= 10;
  ASSERT_TRUE(x == 0);

  x = 1;
  x *= 2;
  ASSERT_TRUE(x == 2);
  x /= 2;
  ASSERT_TRUE(x == 1);

  x = std::string("123");
  ASSERT_TRUE(x[0] == '1');

  for (auto i : x) {
    ASSERT_TRUE(x->find(i) != std::string::npos);
  }
}