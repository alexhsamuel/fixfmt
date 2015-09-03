#include "gtest/gtest.h"
#include "string.hh"

using namespace fixfmt;

TEST(String, basic) {
  String fmt{10};
  ASSERT_EQ(10, fmt.get_width());

  ASSERT_EQ("Hello.    ", fmt("Hello."));
  ASSERT_EQ("Hello, ...", fmt("Hello, world!"));
}

// FIXME: More tests.
