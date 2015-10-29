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

TEST(string_length, basic) {
  ASSERT_EQ(string_length(""), 0);
  ASSERT_EQ(string_length("x"), 1);
  ASSERT_EQ(string_length("xxx"), 3);
  ASSERT_EQ(string_length("-x-x-x-x-x-"), 11);
  ASSERT_EQ(string_length("\u2026"), 1);
  ASSERT_EQ(string_length("x\u2026x"), 3);
  ASSERT_EQ(string_length("\u2026...\u2026\u2026"), 6);
  ASSERT_EQ(string_length("\x1b[m"), 0);
  ASSERT_EQ(string_length("\x1b[32m\u2502"), 1);
  ASSERT_EQ(string_length("\u2502\x1b[m"), 1);
  ASSERT_EQ(string_length("\x1b[32m\u2502\x1b[m"), 1);
  ASSERT_EQ(string_length(" \x1b[32m\u2502\x1b[m "), 3);
}

TEST(pad, basic) {
  string const s = "Hello, world!";
  ASSERT_EQ(pad(s, 10), s);
  ASSERT_EQ(pad(s, 13), s);
  ASSERT_EQ(pad(s, 14), "Hello, world! ");
  ASSERT_EQ(pad(s, 20), "Hello, world!       ");
  ASSERT_EQ(pad(s, 14, "~"), "Hello, world!~");
  ASSERT_EQ(pad(s, 20, "~"), "Hello, world!~~~~~~~");
  ASSERT_EQ(pad(s, 20, " ", true), "       Hello, world!");
  ASSERT_EQ(pad(s, 20, "-", true), "-------Hello, world!");
  ASSERT_EQ(pad(s, 20, "-|"), "Hello, world!-|-|-|-");
  ASSERT_EQ(pad(s, 20, "<->", true), "<-><-><Hello, world!");
  ASSERT_EQ(pad(s, 20, "\u2026.", true),
            "\u2026.\u2026.\u2026.\u2026Hello, world!");
}

TEST(center, basic) {
  string const s = "Hello, world!";
  ASSERT_EQ(center(s, 10), s);
  ASSERT_EQ(center(s, 13), s);
  ASSERT_EQ(center(s, 20), "    Hello, world!   ");
  ASSERT_EQ(center(s, 20, " " , 0.0 ), "Hello, world!       ");
  ASSERT_EQ(center(s, 20, " " , 0.25), "  Hello, world!     ");
  ASSERT_EQ(center(s, 20, " " , 0.49), "   Hello, world!    ");
  ASSERT_EQ(center(s, 20, " " , 0.75), "     Hello, world!  ");
  ASSERT_EQ(center(s, 20, " " , 1.0 ), "       Hello, world!");
  ASSERT_EQ(center(s, 20, "-" , 0.25), "--Hello, world!-----");
  ASSERT_EQ(center(s, 20, "+-", 0.25), "+-Hello, world!+-+-+");
}

TEST(elide, basic) {
  string const s = "Hello, world!";
  ASSERT_EQ(elide(s, 10), "Hello, wo\u2026");
  ASSERT_EQ(elide(s, 12), "Hello, worl\u2026");
  ASSERT_EQ(elide(s, 13), s);
  ASSERT_EQ(elide(s, 20), s);
  ASSERT_EQ(elide(s, 10, ELLIPSIS, 0.0 ), "\u2026o, world!");
  ASSERT_EQ(elide(s, 10, ELLIPSIS, 0.51), "Hello\u2026rld!");
  ASSERT_EQ(elide(s, 10, ELLIPSIS, 0.8 ), "Hello, \u2026d!");
}

