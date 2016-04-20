#include "gtest/gtest.h"
#include "fixfmt.hh"

using namespace fixfmt;

TEST(String, basic) {
  String fmt(10);
  ASSERT_EQ(10u, fmt.get_width());

  ASSERT_EQ("Hello.    ", fmt("Hello."));
  ASSERT_EQ("Hello, wo\u2026", fmt("Hello, world!"));
}

// FIXME: More tests.
