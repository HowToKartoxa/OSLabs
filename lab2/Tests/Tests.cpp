#include <utils/array_algorithms.h>
#include <utils/array_data.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("MinMax regular", "[MinMax]") 
{
    ArrayData arr(5);
    arr[0] = 10;
    arr[1] = 5;
    arr[2] = 20;
    arr[3] = 1;
    arr[4] = 15;

    MinMax(&arr);

    REQUIRE(arr.min_indices.top() == 3);
    REQUIRE(arr.max_indices.top() == 2);
}

TEST_CASE("MinMax same element", "[MinMax]") 
{
    ArrayData arr(4);
    arr[0] = 5;
    arr[1] = 5;
    arr[2] = 5;
    arr[3] = 5;

    MinMax(&arr);

    REQUIRE(arr.min_indices.top() == 3);
    REQUIRE(arr.max_indices.top() == 3);
}

TEST_CASE("MinMax single element", "[MinMax]") 
{
    ArrayData arr(1);
    arr[0] = 10;

    MinMax(&arr);

    REQUIRE(arr.min_indices.top() == arr.max_indices.top());
    REQUIRE(arr.max_indices.top() == 0);
    REQUIRE(arr.min_indices.top() == 0);
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

TEST_CASE("MinMax multiple", "[MinMax]") 
{
    ArrayData arr(5);
    arr[0] = -100;
    arr[1] = -100;
    arr[2] = 8;
    arr[3] = 100;
    arr[4] = 100;

    MinMax(&arr);

    REQUIRE(arr.min_indices.size() == 2);
    REQUIRE(arr.max_indices.size() == 2);
    REQUIRE(arr.min_indices.top() == 1);
    REQUIRE(arr.max_indices.top() == 4);
    arr.min_indices.pop();
    arr.max_indices.pop();
    REQUIRE(arr.min_indices.top() == 0);
    REQUIRE(arr.max_indices.top() == 3);
}

