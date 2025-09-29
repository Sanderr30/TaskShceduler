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


TEST(AnyTests, StoreAndRetrieveValue) {
    Any any;

    any = 42;
    EXPECT_EQ(AnyCast<int>(any), 42);

    any = 3.14f;
    EXPECT_FLOAT_EQ(AnyCast<float>(any), 3.14f);

    any = std::string("Hello");
    EXPECT_EQ(AnyCast<std::string>(any), "Hello");
}


TEST(AnyTests, InvalidCastThrowsException) {
    Any any = 42;

    EXPECT_THROW(AnyCast<float>(any), std::bad_cast);
}


TEST(AnyTests, CopyConstruction) {
    Any any1 = 42;
    Any any2 = any1;


    EXPECT_EQ(AnyCast<int>(any2), 42);
}


TEST(AnyTests, MoveConstruction) {
    Any any1 = std::string("Hello");
    Any any2 = std::move(any1);

    EXPECT_EQ(AnyCast<std::string>(any2), "Hello");
}


TEST(AnyTests, CopyAssignment) {
    Any any1 = 42;
    Any any2;
    any2 = any1;

    EXPECT_EQ(AnyCast<int>(any2), 42);
}
