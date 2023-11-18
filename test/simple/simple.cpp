#include <variable.hpp>

#include <gtest/gtest.h>

#include <string>

using variable::Var;

// Can be passed to function
template <auto W = [] {}>
void Foo(std::type_identity_t<Var<W>> y) {
  // can change type inside functions
  if constexpr (std::is_integral_v<decltype(y.Get())>) {
    y = std::string(y);
  } else {
    y = 100500;
  }
}

TEST(simple, Simple) {
  Var x = 1;
  ASSERT_TRUE(x == 1);

  x = std::string("123");
  x += "456";
  // can call member functions of current type
  ASSERT_EQ(x->size(), 6);
  ASSERT_EQ(x[1], '2');

  std::string str = x;

  // assign different type
  x = 3.14;

  // x.Get() -> double
  Foo(x.Get());

  // assign var to var
  Var y = x;
  // prints 3.14
  ASSERT_TRUE(y == double{3.14});

  // can pass to templated functions
  y = "hi 42";
  Foo(y);
  y = 42;
  Foo(y);

  // can use operators with var
  Var z = (x = 1) + (y = 2);
  ASSERT_TRUE(z == 3);
}