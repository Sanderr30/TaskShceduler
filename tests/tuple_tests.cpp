#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cmath>
#include <string>
#include "hlprs_std/tuple.h"
#include "hlprs_std/invoke.h"
#include "hlprs_std/apply.h"
#include "hlprs_std/any.h"
#include "scheduler.h"

using namespace dts;


TEST(TupleTests, BasicTupleFunctionality) {
    Tuple<int, float, std::string> tuple(52, 3.14, "aaaaaaaaaaaaaaaaaaa");

    EXPECT_EQ(Get<0>(tuple), 52);
    EXPECT_FLOAT_EQ(Get<1>(tuple), 3.14f);
    EXPECT_EQ(Get<2>(tuple), "aaaaaaaaaaaaaaaaaaa");

    Get<0>(tuple) = 100;
    EXPECT_EQ(Get<0>(tuple), 100);
}


TEST(TupleTests, TupleSize) {
    Tuple<int, float, std::string> tuple(52, 3.14, "aaaaaaaaaaaaaaaaaaa");
    EXPECT_EQ(TupleSize<decltype(tuple)>::value, 3);
}


TEST(TupleTests, Empty) {
    Tuple<int, float, std::string> tuple(52, 3.14, "aaaaaaaaaaaaaaaaaaa");
    EXPECT_EQ(TupleSize<decltype(tuple)>::value, 3);
}