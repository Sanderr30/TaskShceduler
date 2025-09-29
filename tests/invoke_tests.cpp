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


TEST(InvokeTests, InvokeFunction) {
    auto func = [](int a, int b) { return a + b; };
    EXPECT_EQ(Invoke(func, 2, 3), 5);
}


TEST(InvokeTests, InvokeMemberFunction) {
    struct TestClass {
        int multiply(int a, int b) const { return a * b; }
    };

    TestClass obj;
    EXPECT_EQ(Invoke(&TestClass::multiply, obj, 3, 4), 12);
}


TEST(InvokeTests, InvokeMemberFunctionPointer) {
    struct TestClass {
        int value = 10;
        int add(int a) const { return value + a; }
    };

    TestClass obj;
    EXPECT_EQ(Invoke(&TestClass::add, &obj, 5), 15);
}