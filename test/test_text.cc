#include "fixfmt/text.hh"
#include "gtest/gtest.h"

using namespace fixfmt;

TEST(utf8_length, basic) {
  ASSERT_EQ(utf8_length(""), 0);
  ASSERT_EQ(utf8_length("x"), 1);
  ASSERT_EQ(utf8_length("Hello, world!"), 13);
  ASSERT_EQ(utf8_length("\u2026"), 1);
  ASSERT_EQ(utf8_length("x\u2026x"), 3);
  ASSERT_EQ(utf8_length("\u2026...\u2026\u2026"), 6);
}

TEST(pad, basic) {
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

