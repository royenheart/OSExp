#include <butil/containers/doubly_buffered_data.h>
#include <gflags/gflags.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <random>
#include <thread>
#include <unordered_map>
#include <vector>

// C++11 兼容的读写锁实现
class ReadWriteMutex {
   private:
    std::mutex mutex_;
    std::condition_variable reader_cv_;
    std::condition_variable writer_cv_;
    int reader_count_;
    bool writer_active_;
    int waiting_writers_;

   public:
    ReadWriteMutex()
        : reader_count_(0), writer_active_(false), waiting_writers_(0) {}

    void lock_shared() {  // 读锁
        std::unique_lock<std::mutex> lock(mutex_);
        reader_cv_.wait(
            lock, [this] { return !writer_active_ && waiting_writers_ == 0; });
        ++reader_count_;
    }

    void unlock_shared() {  // 释放读锁
        std::unique_lock<std::mutex> lock(mutex_);
        --reader_count_;
        if (reader_count_ == 0) {
            writer_cv_.notify_one();
        }
    }

    void lock() {  // 写锁
        std::unique_lock<std::mutex> lock(mutex_);
        ++waiting_writers_;
        writer_cv_.wait(
            lock, [this] { return !writer_active_ && reader_count_ == 0; });
        --waiting_writers_;
        writer_active_ = true;
    }

    void unlock() {  // 释放写锁
        std::unique_lock<std::mutex> lock(mutex_);
        writer_active_ = false;
        if (waiting_writers_ > 0) {
            writer_cv_.notify_one();
        } else {
            reader_cv_.notify_all();
        }
    }
};

// RAII 读锁
class SharedLock {
   private:
    ReadWriteMutex& mutex_;

   public:
    explicit SharedLock(ReadWriteMutex& mutex) : mutex_(mutex) {
        mutex_.lock_shared();
    }
    ~SharedLock() { mutex_.unlock_shared(); }
};

// RAII 写锁
class UniqueLock {
   private:
    ReadWriteMutex& mutex_;

   public:
    explicit UniqueLock(ReadWriteMutex& mutex) : mutex_(mutex) {
        mutex_.lock();
    }
    ~UniqueLock() { mutex_.unlock(); }
};

// 模拟一个配置数据结构
struct Config {
    std::unordered_map<std::string, std::string> settings;
    std::vector<int> feature_flags;
    int version;

    Config() : version(0) {
        // 初始化一些默认配置
        settings["host"] = "localhost";
        settings["port"] = "8080";
        settings["timeout"] = "30";
        feature_flags = {1, 0, 1, 0, 1};
    }

    // 模拟配置更新
    void update() {
        version++;
        settings["port"] = std::to_string(8080 + version % 1000);
        settings["timeout"] = std::to_string(30 + version % 60);
        for (auto& flag : feature_flags) {
            flag = (flag + 1) % 2;
        }
    }

    // 模拟配置读取操作
    std::string get_info() const {
        std::string info = "v" + std::to_string(version) +
                           " port:" + settings.at("port") +
                           " timeout:" + settings.at("timeout") +
                           " flags:" + std::to_string(feature_flags.size());
        return info;
    }
};

// 1. 使用 DoublyBufferedData 的版本
class DoublyBufferedConfig {
   private:
    butil::DoublyBufferedData<Config> data_;

   public:
    DoublyBufferedConfig() {}

    void update_config() {
        data_.Modify([](Config& config) {
            config.update();
            return true;  // 返回 true 表示修改成功
        });
    }

    std::string read_config() {
        butil::DoublyBufferedData<Config>::ScopedPtr ptr;
        if (data_.Read(&ptr) == 0) {
            return ptr->get_info();
        }
        return "read failed";
    }
};

// 2. 使用传统读写锁的版本
class MutexConfig {
   private:
    Config config_;
    mutable ReadWriteMutex mutex_;

   public:
    MutexConfig() {}

    void update_config() {
        UniqueLock lock(mutex_);
        config_.update();
    }

    std::string read_config() {
        SharedLock lock(mutex_);
        return config_.get_info();
    }
};

// 3. 使用普通互斥锁的版本
class SimpleMutexConfig {
   private:
    Config config_;
    mutable std::mutex mutex_;

   public:
    SimpleMutexConfig() {}

    void update_config() {
        std::lock_guard<std::mutex> lock(mutex_);
        config_.update();
    }

    std::string read_config() {
        std::lock_guard<std::mutex> lock(mutex_);
        return config_.get_info();
    }
};

// 性能测试函数
template <typename ConfigType>
void benchmark(const std::string& name, ConfigType& config, int num_readers,
               int num_writers, int duration_seconds) {
    std::atomic<long> read_count{0};
    std::atomic<long> write_count{0};
    std::atomic<bool> stop_flag{false};

    std::vector<std::thread> threads;

    // 启动读线程
    for (int i = 0; i < num_readers; ++i) {
        threads.emplace_back([&config, &read_count, &stop_flag]() {
            while (!stop_flag.load()) {
                volatile auto result = config.read_config();
                read_count.fetch_add(1);
                // 模拟一些CPU工作
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        });
    }

    // 启动写线程
    for (int i = 0; i < num_writers; ++i) {
        threads.emplace_back([&config, &write_count, &stop_flag]() {
            while (!stop_flag.load()) {
                config.update_config();
                write_count.fetch_add(1);
                // 写操作间隔稍长一些
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }

    // 运行指定时间
    auto start_time = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(duration_seconds));
    stop_flag.store(true);

    // 等待所有线程结束
    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);

    long total_reads = read_count.load();
    long total_writes = write_count.load();

    std::cout << "\n=== " << name << " ===\n";
    std::cout << "Duration: " << elapsed.count() << " ms\n";
    std::cout << "Readers: " << num_readers << ", Writers: " << num_writers
              << "\n";
    std::cout << "Total reads: " << total_reads << " ("
              << (total_reads * 1000.0 / elapsed.count()) << " reads/sec)\n";
    std::cout << "Total writes: " << total_writes << " ("
              << (total_writes * 1000.0 / elapsed.count()) << " writes/sec)\n";
    std::cout << "Read throughput: " << (total_reads * 1000.0 / elapsed.count())
              << " ops/sec\n";
}

// 延迟测试
template <typename ConfigType>
void latency_test(const std::string& name, ConfigType& config, int iterations) {
    std::vector<double> latencies;
    latencies.reserve(iterations);

    for (int i = 0; i < iterations; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        volatile auto result = config.read_config();
        auto end = std::chrono::high_resolution_clock::now();

        auto duration =
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        latencies.push_back(duration.count());
    }

    // 计算统计信息
    std::sort(latencies.begin(), latencies.end());
    double avg = std::accumulate(latencies.begin(), latencies.end(), 0.0) /
                 latencies.size();
    double p50 = latencies[static_cast<size_t>(latencies.size() * 0.5)];
    double p95 = latencies[static_cast<size_t>(latencies.size() * 0.95)];
    double p99 = latencies[static_cast<size_t>(latencies.size() * 0.99)];

    std::cout << "\n=== " << name << " Latency ===\n";
    std::cout << "Iterations: " << iterations << "\n";
    std::cout << "Average: " << avg << " ns\n";
    std::cout << "P50: " << p50 << " ns\n";
    std::cout << "P95: " << p95 << " ns\n";
    std::cout << "P99: " << p99 << " ns\n";
}

// 定义命令行参数
DEFINE_int32(duration, 5, "测试持续时间（秒）");
DEFINE_int32(num_readers, 8, "读线程数");
DEFINE_int32(num_writers, 2, "写线程数");
DEFINE_int32(latency_iterations, 10000, "延迟测试迭代次数");

int main(int argc, char* argv[]) {
    std::cout << "DoublyBufferedData Performance Test\n";
    std::cout << "===================================\n";

    // 初始化 gflags
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // 测试参数
    int duration = FLAGS_duration;
    int num_readers = FLAGS_num_readers;
    int num_writers = FLAGS_num_writers;
    int latency_iterations = FLAGS_latency_iterations;

    std::cout << "Testing with " << num_readers << " readers and "
              << num_writers << " writers for " << duration
              << " seconds each...\n";

    // 1. DoublyBufferedData 测试
    {
        DoublyBufferedConfig config;
        benchmark("DoublyBufferedData", config, num_readers, num_writers,
                  duration);
        latency_test("DoublyBufferedData", config, latency_iterations);
    }

    // 2. 读写锁测试
    {
        MutexConfig config;
        benchmark("Shared Mutex (RW Lock)", config, num_readers, num_writers,
                  duration);
        latency_test("Shared Mutex (RW Lock)", config, latency_iterations);
    }

    // 3. 普通互斥锁测试
    {
        SimpleMutexConfig config;
        benchmark("Simple Mutex", config, num_readers, num_writers, duration);
        latency_test("Simple Mutex", config, latency_iterations);
    }

    std::cout << "\n=== Summary ===\n";
    std::cout << "DoublyBufferedData advantages:\n";
    std::cout
        << "1. Lock-free reads: Readers never block each other or writers\n";
    std::cout
        << "2. Low read latency: No lock acquisition overhead for reads\n";
    std::cout
        << "3. High read throughput: Scales linearly with reader threads\n";
    std::cout << "4. Consistent performance: No lock contention issues\n";
    std::cout << "\nBest for scenarios with:\n";
    std::cout << "- High read/write ratio (many readers, few writers)\n";
    std::cout << "- Latency-sensitive read operations\n";
    std::cout << "- Configuration/metadata that updates infrequently\n";

    return 0;
}