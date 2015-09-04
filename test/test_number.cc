#include <cmath>

#include "fixfmt.hh"
#include "gtest/gtest.h"

using namespace fixfmt;

TEST(pow10, basic) {
  ASSERT_EQ(     0.00001, pow10(-5));
  ASSERT_EQ(     0.1    , pow10(-1));
  ASSERT_EQ(     1.0    , pow10( 0));
  ASSERT_EQ(    10.0    , pow10( 1));
  ASSERT_EQ(100000.0    , pow10( 5));
}

TEST(round, basic) {
  ASSERT_EQ( 1.0, round( 1.0, 0));
  ASSERT_EQ( 1.0, round( 1.0, 1));
  ASSERT_EQ( 1.0, round( 1.0, 5));
  ASSERT_EQ(-1.0, round(-1.0, 0));
  ASSERT_EQ(-1.0, round(-1.0, 1));
  ASSERT_EQ(-1.0, round(-1.0, 5));

  ASSERT_EQ( 0.001, round( 0.00051, 3));
  ASSERT_EQ(-0.001, round(-0.00051, 3));
  // FIXME: Finish.
}

TEST(Number, long) {
  Number fmt{3};
  ASSERT_EQ(4, fmt.get_width());
  ASSERT_EQ("   0", fmt(    0));
  ASSERT_EQ("   0", fmt(   -0));
  ASSERT_EQ("   1", fmt(    1));
  ASSERT_EQ("  -1", fmt(   -1));
  ASSERT_EQ("   9", fmt(    9));
  ASSERT_EQ(" -11", fmt(  -11));
  ASSERT_EQ("  50", fmt(   50));
  ASSERT_EQ(" -99", fmt(  -99));
  ASSERT_EQ(" 100", fmt(  100));
  ASSERT_EQ("-101", fmt( -101));
  ASSERT_EQ(" 999", fmt(  999));
  ASSERT_EQ("-999", fmt( -999));
  ASSERT_EQ("####", fmt( 1000));
  ASSERT_EQ("####", fmt(-1000));
  ASSERT_EQ("####", fmt(98765));
  ASSERT_EQ("####", fmt(-9999));
}

TEST(Number, double) {
  Number fmt{2, 3};
  ASSERT_EQ(7, fmt.get_width());
  ASSERT_EQ("  0.000", fmt(   0.0    ));
  ASSERT_EQ("  0.000", fmt(  -0.0    ));
  ASSERT_EQ("  1.000", fmt(   1.0    ));
  ASSERT_EQ(" -1.000", fmt(  -1.0    ));
  ASSERT_EQ("  0.000", fmt(   0.0004 ));
  ASSERT_EQ(" -0.000", fmt(  -0.0004 ));
  ASSERT_EQ("  0.000", fmt(   0.0005 ));
  ASSERT_EQ(" -0.000", fmt(  -0.0005 ));
  ASSERT_EQ("  0.001", fmt(   0.00051));
  ASSERT_EQ(" -0.001", fmt(  -0.00051));
  ASSERT_EQ("  0.001", fmt(   0.0014 ));
  ASSERT_EQ(" -0.001", fmt(  -0.0014 ));
  ASSERT_EQ("  0.002", fmt(   0.0015 ));
  ASSERT_EQ(" -0.002", fmt(  -0.0015 ));
  ASSERT_EQ("  0.002", fmt(   0.00151));
  ASSERT_EQ(" -0.002", fmt(  -0.00151));
  ASSERT_EQ("  0.008", fmt(   0.0084 ));
  ASSERT_EQ("  0.008", fmt(   0.0085 ));
  ASSERT_EQ("  0.009", fmt(   0.0086 ));
  ASSERT_EQ(" -0.009", fmt(  -0.0094 ));
  ASSERT_EQ(" -0.010", fmt(  -0.0095 ));
  ASSERT_EQ(" -0.010", fmt(  -0.0096 ));
  ASSERT_EQ("  5.999", fmt(   5.9994 ));
  ASSERT_EQ("  5.999", fmt(   5.99949));
  ASSERT_EQ("  6.000", fmt(   5.9995 ));
  ASSERT_EQ("  6.000", fmt(   5.99951));
  ASSERT_EQ("-12.234", fmt( -12.2344 ));
  ASSERT_EQ("-12.234", fmt( -12.23449));
  ASSERT_EQ("-12.234", fmt( -12.2345 ));
  ASSERT_EQ("-12.235", fmt( -12.23451));
  ASSERT_EQ(" 43.499", fmt(  43.499  ));
  ASSERT_EQ(" 43.500", fmt(  43.4999 ));
  ASSERT_EQ(" 43.500", fmt(  43.5    ));
  ASSERT_EQ(" 43.502", fmt(  43.502  ));
  ASSERT_EQ(" 43.800", fmt(  43.7995 ));
  ASSERT_EQ(" 99.000", fmt(  99      ));
  ASSERT_EQ("-99.000", fmt( -99      ));
  ASSERT_EQ(" 99.999", fmt(  99.999  ));
  ASSERT_EQ("-99.999", fmt( -99.999  ));
  ASSERT_EQ(" 99.999", fmt(  99.9994 ));
  ASSERT_EQ("-99.999", fmt( -99.9994 ));
  ASSERT_EQ("#######", fmt(  99.9995 ));
  ASSERT_EQ("#######", fmt( -99.9995 ));
  ASSERT_EQ("#######", fmt( 100      ));
  ASSERT_EQ("#######", fmt(-100      ));
  ASSERT_EQ("#######", fmt( 999      ));
  ASSERT_EQ("#######", fmt(-999      ));
  ASSERT_EQ("    NaN", fmt( NAN      ));
  ASSERT_EQ("    NaN", fmt(-NAN      ));  // ain't no such thang
  ASSERT_EQ("    inf", fmt( INFINITY ));
  ASSERT_EQ("   -inf", fmt(-INFINITY ));
}

TEST(Number, exhaustive) {
  Number fmt{1, 2, ' ', '+'};
  ASSERT_EQ(5, fmt.get_width());

  for (int i = -9994; i < 9995; ++i) {
    double const val = 0.001 * i;
    char expected[6];
    snprintf(expected, sizeof(expected), "%+4.2f", val);
    // std::cerr << val << ": " << expected << " = " << fmt(val) << "\n";
    ASSERT_EQ(expected, fmt(val));
  }
}

TEST(Number, size) {
  ASSERT_EQ(" 4", Number{1}(  4));
  ASSERT_EQ("##", Number{1}( 42));
  ASSERT_EQ("##", Number{1}(999));

  ASSERT_EQ("  4", Number{2}(  4));
  ASSERT_EQ(" 42", Number{2}( 42));
  ASSERT_EQ("###", Number{2}(999));

  ASSERT_EQ("         4", Number{9}(   4));
  ASSERT_EQ("        42", Number{9}(  42));
  ASSERT_EQ("       999", Number{9}( 999));
  ASSERT_EQ("##########", Number{9}(1e+12));

}

TEST(Number, precision) {
  ASSERT_EQ("-123"      , Number(3, Number::PRECISION_NONE)(-123.45678));
  ASSERT_EQ("-123."     , Number(3, 0)(-123.45678));
  ASSERT_EQ("-123.5"    , Number(3, 1)(-123.45678));
  ASSERT_EQ("-123.46"   , Number(3, 2)(-123.45678));
  ASSERT_EQ("-123.457"  , Number(3, 3)(-123.45678));
  ASSERT_EQ("-123.4568" , Number(3, 4)(-123.45678));
  ASSERT_EQ("-123.45678", Number(3, 5)(-123.45678));
}

TEST(Number, pad) {
  ASSERT_EQ("  42.", Number(3, 0, ' ')( 42));
  ASSERT_EQ(" -42.", Number(3, 0, ' ')(-42));
  ASSERT_EQ(" 042.", Number(3, 0, '0')( 42));
  ASSERT_EQ("-042.", Number(3, 0, '0')(-42));
}

TEST(Number, sign) {
  ASSERT_EQ("     42.", Number(6, 0, ' ', '-')( 42));
  ASSERT_EQ("    -42.", Number(6, 0, ' ', '-')(-42));
  ASSERT_EQ(" 000042.", Number(6, 0, '0', '-')( 42));
  ASSERT_EQ("-000042.", Number(6, 0, '0', '-')(-42));

  ASSERT_EQ("    +42.", Number(6, 0, ' ', '+')( 42));
  ASSERT_EQ("    -42.", Number(6, 0, ' ', '+')(-42));
  ASSERT_EQ("+000042.", Number(6, 0, '0', '+')( 42));
  ASSERT_EQ("-000042.", Number(6, 0, '0', '+')(-42));

  ASSERT_EQ("    42.", Number(6, 0, ' ', ' ')( 42));
  ASSERT_EQ("#######", Number(6, 0, ' ', ' ')(-42));
  ASSERT_EQ("000042.", Number(6, 0, '0', ' ')( 42));
  ASSERT_EQ("#######", Number(6, 0, '0', ' ')(-42));
}

TEST(Number, nan) {
  ASSERT_EQ("N"     , Number(1, -1, ' ', ' ')(NAN));
  ASSERT_EQ("Na"    , Number(2, -1, ' ', ' ')(NAN));
  ASSERT_EQ("NaN"   , Number(3, -1, ' ', ' ')(NAN));
  ASSERT_EQ("   NaN", Number(2,  2, ' ', '-', "NaN")(NAN));
  ASSERT_EQ("   nan", Number(2,  2, ' ', '-', "nan")(NAN));
  ASSERT_EQ("NotANu", Number(2,  2, ' ', '-', "NotANumber")(NAN));
  ASSERT_EQ("  NotANumber", Number(12, -1, ' ', ' ', "NotANumber")(NAN));
}

TEST(Number, inf) {
  ASSERT_EQ("i"     , Number(1, -1, ' ', ' ')( INFINITY));
  ASSERT_EQ("in"    , Number(2, -1, ' ', ' ')( INFINITY));
  ASSERT_EQ("inf"   , Number(3, -1, ' ', ' ')( INFINITY));
  ASSERT_EQ("###"   , Number(3, -1, ' ', ' ')(-INFINITY));
  ASSERT_EQ(" inf"  , Number(3, -1, ' ', '-')( INFINITY));
  ASSERT_EQ("-inf"  , Number(3, -1, ' ', '-')(-INFINITY));
  ASSERT_EQ("+inf"  , Number(3, -1, ' ', '+')( INFINITY));
  ASSERT_EQ("-inf"  , Number(3, -1, ' ', '+')(-INFINITY));
  ASSERT_EQ("   inf", Number(2,  2, ' ', '-', "NaN", "inf")(INFINITY));
  ASSERT_EQ("   INF", Number(2,  2, ' ', '-', "NaN", "INF")(INFINITY));
  ASSERT_EQ(" infin", Number(2,  2, ' ', '-', "NaN", "infinity")(INFINITY));
  ASSERT_EQ("+infin", Number(2,  2, ' ', '+', "NaN", "infinity")(INFINITY));
  ASSERT_EQ("-infin", Number(2,  2, ' ', '-', "NaN", "infinity")(-INFINITY));
  ASSERT_EQ(
    "    infinity", Number(12, -1, ' ', ' ', "NaN", "infinity")(INFINITY));
  ASSERT_EQ(
    "   -infinity", Number(11, -1, ' ', '-', "NaN", "infinity")(-INFINITY));
  ASSERT_EQ(
    "############", Number(12, -1, ' ', ' ', "NaN", "infinity")(-INFINITY));
}

TEST(Number, types) {
  Number fmt{2, 3};
  ASSERT_EQ("-42.000", fmt((char) -42));
  ASSERT_EQ("-42.000", fmt((short) -42));
  ASSERT_EQ("-42.000", fmt((int) -42));
  ASSERT_EQ("-42.000", fmt((long) -42));
  ASSERT_EQ(" 42.000", fmt((unsigned char) 42));
  ASSERT_EQ(" 42.000", fmt((unsigned short) 42));
  ASSERT_EQ(" 42.000", fmt((unsigned int) 42));
  ASSERT_EQ(" 42.000", fmt((unsigned long) 42));
  ASSERT_EQ("-42.500", fmt((float) -42.5));
  ASSERT_EQ("-42.500", fmt((double) -42.5));
}

