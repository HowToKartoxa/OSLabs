#include <utils/array_algorithms.h>
#include <utils/array_data.h>

#include <catch2/catch_test_macros.hpp>

#if defined(USE_WINAPI) or defined(USE_STD_THREAD)

TEST_CASE("MinMax regular", "[MinMax]") 
{
    ArrayData arr(5);
    arr[0] = 10;
    arr[1] = 5;
    arr[2] = 20;
    arr[3] = 1;
    arr[4] = 15;

    MinMax(&arr);

    REQUIRE(arr.min_index == 3);
    REQUIRE(arr.max_index == 2); 
}

TEST_CASE("MinMax same element", "[MinMax]") 
{
    ArrayData arr(4);
    arr[0] = 5;
    arr[1] = 5;
    arr[2] = 5;
    arr[3] = 5;

    MinMax(&arr);

    REQUIRE(arr.min_index == 0);
    REQUIRE(arr.max_index == 0);
}

TEST_CASE("MinMax single element", "[MinMax]") 
{
    ArrayData arr(1);
    arr[0] = 10;

    MinMax(&arr);

    REQUIRE(arr.min_index == arr.max_index);
    REQUIRE(arr.max_index == 0);
    REQUIRE(arr.min_index == 0);
}

TEST_CASE("Average regular", "[Average]") 
{
    ArrayData arr(3);
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;

    Average(&arr); 

    REQUIRE(arr.average == 20);
}

TEST_CASE("Average single element", "[Average]") 
{
    ArrayData arr(1);
    arr[0] = 42;

    Average(&arr);

    REQUIRE(arr.average == 42);
}

#endif

