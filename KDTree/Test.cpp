#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <array>
#include <random>

#include "KDTree.h"
#include "Types.h"

namespace sps {
    static std::vector<sps::V3f> testData = {
        V3f{ 0.0f, 0.0f, 0.0f },
        V3f{ 1.0f, 0.0f, 0.0f },
        V3f{ 0.0f, 1.0f, 0.0f },
        V3f{ 0.0f, 0.0f, 1.0f },
        V3f{ 1.0f, 1.0f, 0.0f },
        V3f{ 1.0f, 0.0f, 1.0f },
        V3f{ 0.0f, 1.0f, 1.0f },
        V3f{ 1.0f, 1.0f, 1.0f },
    };

    static std::vector<sps::V3f> generateTestData(u32 pointCount, std::default_random_engine& engine) {
        std::vector<sps::V3f> result;
        result.reserve(pointCount);

        std::uniform_real_distribution<float> distribution(-10000.0f, 10000.0f);
        for (auto i = 0; i < pointCount; ++i) {
            result.emplace_back(V3f{ distribution(engine), distribution(engine), distribution(engine) });
        }
        return result;
    }

    TEST_CASE("KD Tree Simple")
    {
        KDTree tree = KDTree::buildTree(testData);

        {
            sps::V3f outsideMin{ -1.0f, -1.0f, -1.0f };
            sps::V3f outsideMax{ -0.5f, -0.5f, -0.5f };
            std::vector<sps::V3f> searchResult;
            tree.rangeSearch(outsideMin, outsideMax, searchResult);
            REQUIRE(searchResult.empty());
        }
        {
            sps::V3f completeMin{ -1.0f, -1.0f, -1.0f };
            sps::V3f completeMax{ 2.5f, 2.5f, 2.5f };
            std::vector<sps::V3f> searchResult;
            tree.rangeSearch(completeMin, completeMax, searchResult);
            REQUIRE(searchResult.size() == 8);
        }
        {
            sps::V3f halfMin{ -1.0f, -1.0f, -1.0f };
            sps::V3f halfMax{ 1.5f, 0.5f, 1.5f };
            std::vector<sps::V3f> searchResult;
            tree.rangeSearch(halfMin, halfMax, searchResult);
            REQUIRE(searchResult.size() == 4);
        }
    }

    TEST_CASE("KD Tree Generated")
    {
        constexpr u32 pointCount = 100000;
        std::default_random_engine engine(42);
        auto data = generateTestData(pointCount, engine);
        KDTree tree = KDTree::buildTree(data);

        sps::V3f min{ -5000.0f, -5000.0f, -5000.0f };
        sps::V3f max{ 5000.0f, 5000.0f, 5000.0f };
        std::vector<sps::V3f> searchResult;
        tree.rangeSearch(min, max, searchResult);

        std::vector<V3f> expectedResult;
        expectedResult.reserve(pointCount / 2);
        for (const V3f& point : data) {
            if ((min.x <= point.x && point.x <= max.x) &&
                (min.y <= point.y && point.y <= max.y) &&
                (min.z <= point.z && point.z <= max.z)) {
                expectedResult.emplace_back(point);
            }
        }
        REQUIRE(expectedResult.size() == searchResult.size());

        for (const V3f& searchPoint : searchResult) {
            bool found = false;
            for (const V3f& expectedPoint : expectedResult) {
                if (searchPoint.x == expectedPoint.x && searchPoint.y == expectedPoint.y && searchPoint.z == expectedPoint.z) {
                    found = true;
                    break;
                }
            }
            REQUIRE(found);
        }
    }

    void benchmarkNaive(u32 pointCount, Catch::Benchmark::Chronometer& meter) {
        std::default_random_engine engine(42);
        auto data = generateTestData(pointCount, engine);

        sps::V3f min{ -5000.0f, -5000.0f, -5000.0f };
        sps::V3f max{ 5000.0f, 5000.0f, 5000.0f };

        meter.measure([&]() {
            std::vector<V3f> expectedResult;
            expectedResult.reserve(pointCount / 2);
            for (const V3f& point : data) {
                if ((min.x <= point.x && point.x <= max.x) &&
                    (min.y <= point.y && point.y <= max.y) &&
                    (min.z <= point.z && point.z <= max.z)) {
                    expectedResult.emplace_back(point);
                }
            }
            });
    };

    TEST_CASE("Naive Benchmarks")
    {
        BENCHMARK_ADVANCED("100 points")(Catch::Benchmark::Chronometer meter)
        {
            benchmarkNaive(100, meter);
        };

        BENCHMARK_ADVANCED("10'000 points")(Catch::Benchmark::Chronometer meter)
        {
            benchmarkNaive(10'000, meter);
        };

        BENCHMARK_ADVANCED("1'000'000 points")(Catch::Benchmark::Chronometer meter)
        {
            benchmarkNaive(1'000'000, meter);
        };
    }

    void benchmarkKDTree(u32 pointCount, Catch::Benchmark::Chronometer& meter) {
        std::default_random_engine engine(42);
        auto data = generateTestData(pointCount, engine);

        sps::V3f min{ -5000.0f, -5000.0f, -5000.0f };
        sps::V3f max{ 5000.0f, 5000.0f, 5000.0f };

        auto tree = KDTree::buildTree(data);

        meter.measure([&]() {
            std::vector<V3f> expectedResult;
            expectedResult.reserve(pointCount / 2);
            tree.rangeSearch(min, max, expectedResult);
            });
    };

    TEST_CASE("KD Tree Benchmarks")
    {
        BENCHMARK_ADVANCED("100 points")(Catch::Benchmark::Chronometer meter)
        {
            benchmarkKDTree(100, meter);
        };

        BENCHMARK_ADVANCED("10'000 points")(Catch::Benchmark::Chronometer meter)
        {
            benchmarkKDTree(10'000, meter);
        };

        BENCHMARK_ADVANCED("1'000'000 points")(Catch::Benchmark::Chronometer meter)
        {
            benchmarkKDTree(1'000'000, meter);
        };
    }
}