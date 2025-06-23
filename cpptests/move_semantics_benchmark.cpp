#include <benchmark/benchmark.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// 测试用的简单类
class TestObject {
   private:
    std::vector<int> data;
    std::string name;

   public:
    // 默认构造函数
    TestObject()
        : data(1000, 42), name("TestObject_" + std::to_string(rand())) {}

    // 拷贝构造函数
    TestObject(const TestObject& other) : data(other.data), name(other.name) {
        // 模拟一些额外的拷贝开销
    }

    // 移动构造函数
    // noexcept 表示该函数不会抛出异常：
    // 执行编译时检查，如果一个表达式被声明为不抛出任何异常，则返回 true
    TestObject(TestObject&& other) noexcept
        : data(std::move(other.data)), name(std::move(other.name)) {
        // 移动操作通常很快
    }

    // 拷贝赋值运算符
    TestObject& operator=(const TestObject& other) {
        if (this != &other) {
            data = other.data;
            name = other.name;
        }
        return *this;
    }

    // 移动赋值运算符
    TestObject& operator=(TestObject&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
            name = std::move(other.name);
        }
        return *this;
    }

    // 获取数据大小
    size_t size() const { return data.size(); }
};

// 创建测试对象的工厂函数
TestObject createTestObject() { return TestObject(); }

// 测试1: 拷贝构造 vs 移动构造
static void BM_CopyConstruction(benchmark::State& state) {
    for (auto _ : state) {
        TestObject original;
        TestObject copy(original);  // 拷贝构造
        benchmark::DoNotOptimize(copy);
    }
}

static void BM_MoveConstruction(benchmark::State& state) {
    for (auto _ : state) {
        TestObject original;
        TestObject moved(std::move(original));  // 移动构造
        benchmark::DoNotOptimize(moved);
    }
}

// 测试2: 拷贝赋值 vs 移动赋值
static void BM_CopyAssignment(benchmark::State& state) {
    TestObject target;
    for (auto _ : state) {
        TestObject source;
        target = source;  // 拷贝赋值
        benchmark::DoNotOptimize(target);
    }
}

static void BM_MoveAssignment(benchmark::State& state) {
    TestObject target;
    for (auto _ : state) {
        TestObject source;
        target = std::move(source);  // 移动赋值
        benchmark::DoNotOptimize(target);
    }
}

// 测试3: 函数返回值优化 (RVO vs 移动)
static void BM_ReturnValue_Copy(benchmark::State& state) {
    for (auto _ : state) {
        TestObject obj = createTestObject();  // 依赖RVO或移动
        benchmark::DoNotOptimize(obj);
    }
}

// 测试4: 容器操作中的移动语义
static void BM_VectorPushBack_Copy(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<TestObject> vec;
        for (int i = 0; i < 100; ++i) {
            TestObject obj;
            vec.push_back(obj);  // 拷贝
        }
        benchmark::DoNotOptimize(vec);
    }
}

static void BM_VectorPushBack_Move(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<TestObject> vec;
        for (int i = 0; i < 100; ++i) {
            TestObject obj;
            vec.push_back(std::move(obj));  // 移动
        }
        benchmark::DoNotOptimize(vec);
    }
}

static void BM_VectorEmplaceBack(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<TestObject> vec;
        for (int i = 0; i < 100; ++i) {
            vec.emplace_back();  // 原地构造
        }
        benchmark::DoNotOptimize(vec);
    }
}

// 测试5: 字符串移动语义
static void BM_StringCopy(benchmark::State& state) {
    std::string large_string(100000, 'A');
    for (auto _ : state) {
        std::string copy = large_string;  // 拷贝
        benchmark::DoNotOptimize(copy);
    }
}

static void BM_StringMove(benchmark::State& state) {
    for (auto _ : state) {
        std::string large_string(100000, 'A');
        std::string moved = std::move(large_string);  // 移动
        benchmark::DoNotOptimize(moved);
    }
}

// 测试6: unique_ptr 移动
static void BM_UniquePtrCopy(benchmark::State& state) {
    for (auto _ : state) {
        auto ptr1 = std::make_unique<TestObject>();
        // 注意: unique_ptr 不能拷贝，这里演示的是通过引用传递后重新创建
        auto ptr2 = std::make_unique<TestObject>(*ptr1);
        benchmark::DoNotOptimize(ptr2);
    }
}

static void BM_UniquePtrMove(benchmark::State& state) {
    for (auto _ : state) {
        auto ptr1 = std::make_unique<TestObject>();
        auto ptr2 = std::move(ptr1);  // 移动
        benchmark::DoNotOptimize(ptr2);
    }
}

// 注册基准测试
BENCHMARK(BM_CopyConstruction);
BENCHMARK(BM_MoveConstruction);
BENCHMARK(BM_CopyAssignment);
BENCHMARK(BM_MoveAssignment);
BENCHMARK(BM_ReturnValue_Copy);
BENCHMARK(BM_VectorPushBack_Copy);
BENCHMARK(BM_VectorPushBack_Move);
BENCHMARK(BM_VectorEmplaceBack);
BENCHMARK(BM_StringCopy);
BENCHMARK(BM_StringMove);
BENCHMARK(BM_UniquePtrCopy);
BENCHMARK(BM_UniquePtrMove);

// 主函数
BENCHMARK_MAIN();