#include "fixfmt/text.hh"
#include "gtest/gtest.h"

using namespace fixfmt;

TEST(pad, test0) {
  string const s = "Hello, world!";
  ASSERT_EQ(pad(s, 10), s);
  ASSERT_EQ(pad(s, 13), s);
  ASSERT_EQ(pad(s, 14), "Hello, world! ");
  ASSERT_EQ(pad(s, 20), "Hello, world!       ");
  ASSERT_EQ(pad(s, 14, '~'), "Hello, world!~");
  ASSERT_EQ(pad(s, 20, '~'), "Hello, world!~~~~~~~");
  ASSERT_EQ(pad(s, 20, ' ', true), "       Hello, world!");
  ASSERT_EQ(pad(s, 20, '-', true), "-------Hello, world!");
}
