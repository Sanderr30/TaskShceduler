#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>
#include <string>

#include "any_tests.cpp"
#include "tuple_tests.cpp"
#include "invoke_tests.cpp"


#include "hlprs_std/tuple.h"
#include "hlprs_std/invoke.h"
#include "hlprs_std/apply.h"
#include "hlprs_std/any.h"

#include "scheduler.h"

using namespace dts;



TEST(ApplyTests, ApplyWithTuple) {
    auto func = [](int a, float b, const std::string& c) {
        return std::to_string(a) + " " + std::to_string(b) + " " + c;
    };

    Tuple<int, float, std::string> tuple(52, 3.14, "aaaaaaaaaaaaaaaaaaa");
    EXPECT_EQ(Apply(func, tuple), "52 3.140000 aaaaaaaaaaaaaaaaaaa");
}



TEST(SchedulerTests, BasicTaskExecution) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](int a, int b) { return a + b; }, 2, 3);
    auto id2 = scheduler.add([](int a, int b) { return a * b; }, 4, scheduler.getFutureResult<int>(id1));

    scheduler.executeAll();

    EXPECT_EQ(scheduler.getResult<int>(id1), 5);
    EXPECT_EQ(scheduler.getResult<int>(id2), 20);
}


TEST(SchedulerTests, TaskWithMultipleDependencies) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](float a, float b) { return a + b; }, 1.0f, 2.0f);
    auto id2 = scheduler.add([](float a, float b) { return a * b; }, 3.0f, 4.0f);
    auto id3 = scheduler.add([](float a, float b, float c) { return a + b + c; },
                             scheduler.getFutureResult<float>(id1),
                             scheduler.getFutureResult<float>(id2),
                             5.0f);

    scheduler.executeAll();

    EXPECT_FLOAT_EQ(scheduler.getResult<float>(id1), 3.0f);
    EXPECT_FLOAT_EQ(scheduler.getResult<float>(id2), 12.0f);
    EXPECT_FLOAT_EQ(scheduler.getResult<float>(id3), 20.0f);
}


TEST(SchedulerTests, TaskWithMemberFunction) {
    struct TestClass {
        float add(float a) const { return a + value; }
        float value = 10.0f;
    };

    TTaskScheduler scheduler;
    TestClass obj;

    auto id1 = scheduler.add([](float a, float b) { return a * b; }, 2.0f, 3.0f);
    auto id2 = scheduler.add(&TestClass::add, obj, scheduler.getFutureResult<float>(id1));

    scheduler.executeAll();

    EXPECT_FLOAT_EQ(scheduler.getResult<float>(id1), 6.0f);
    EXPECT_FLOAT_EQ(scheduler.getResult<float>(id2), 16.0f);
}


TEST(SchedulerTests, MoreThanTwoArguments) {
    auto func = [](int a, int b, int c, int d) { return a + b + c + d; };
    TTaskScheduler scheduler;

    auto id = scheduler.add(func, 1, 2, 3, 4);
    scheduler.executeAll();

    EXPECT_EQ(scheduler.getResult<int>(id), 10);
}


TEST(SchedulerTests, MixedFuturesAndValues) {
    auto add = [](int a, int b, int c) { return a + b + c; };
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](int x) { return x * 2; }, 5);
    auto id2 = scheduler.add([](int x) { return x + 1; }, 7);
    auto id3 = scheduler.add(add, scheduler.getFutureResult<int>(id1), 10, scheduler.getFutureResult<int>(id2));

    scheduler.executeAll();

    EXPECT_EQ(scheduler.getResult<int>(id1), 10);
    EXPECT_EQ(scheduler.getResult<int>(id2), 8);
    EXPECT_EQ(scheduler.getResult<int>(id3), 28);
}



TEST(SchedulerTests, MemberFunctionViaNonConstRef) {
    struct TestClass {
        int mul(int a) { return a * factor++; }
        int factor = 2;
    };

    TTaskScheduler scheduler;
    TestClass obj;

    auto id = scheduler.add(&TestClass::mul, std::ref(obj), 5);
    scheduler.executeAll();

    EXPECT_EQ(scheduler.getResult<int>(id), 10);
    EXPECT_EQ(obj.factor, 3);
}


TEST(SchedulerTests, MemberFunctionViaConstRef) {
    struct TestClass {
        int sum(int a, int b) const { return a + b + base; }
        int base = 3;
    };

    const TestClass obj;
    TTaskScheduler scheduler;

    auto id = scheduler.add(&TestClass::sum, std::cref(obj), 4, 5);
    scheduler.executeAll();

    EXPECT_EQ(scheduler.getResult<int>(id), 12);
}