#include <format>
#include <vector>
#include <utility>
#include <string>
#include "kaizen.h"

volatile int sink = 0;

// Case 1: Independent Arithmetic Operations
void independent_arithmetic(int iterations) {
    int a = 0, b = 0, c = 0;
    for (int i = 0; i < iterations; ++i) {
        a += i;         
        b += i * 2;    
        c += i - 1;   
    }
    sink = a + b + c;
}

// Case 2: Independent Memory Accesses
void independent_memory(int iterations, std::vector<int> array) {
    for (int i = 0; i < iterations; ++i) {
        sink = array[i % 100];
        sink = array[(i + 1) % 100];
        sink = array[(i + 2) % 100];
    }
}

// Case 3: Mixed Latency Operations
void mixed_latency(int iterations, std::vector<int> array) {
    int a = 0;
    for (int i = 0; i < iterations; ++i) {
        a += i;              
        sink = array[i % 100];
        sink += i * 2;
    }
}

// Case 4: Sequential Dependent Operations
void sequential_dependent(int iterations, std::vector<int> array) {
    int result = 0;
    for (int i = 0; i < iterations; ++i) {
        result += i;          
        result = array[result % 100];
        sink = result * 2;
    }
}

template<typename F, typename... Args>
auto measure_time(F&& func, Args&&... args) {
    zen::timer t;

    t.start();
    func(std::forward<Args>(args)...);
    t.stop();
    return t.duration<zen::timer::usec>().count();
}

int parse_args(int argc, char** argv) {
    zen::cmd_args args(argv, argc);
    if (!args.is_present("--iters")) {
        zen::log(zen::color::yellow(std::format("Warning: --iters option is not provided using the default value {}", 5000000)));
        return {5000000};
    }
    return {std::stoi(args.get_options("--iters")[0])};
}

int main(int argc, char* argv[]) {
    int iterations = parse_args(argc, argv);

    std::vector<int> array(100);
    zen::generate_random(array, array.size());

    auto time1 = measure_time(independent_arithmetic, iterations);
    auto time2 = measure_time(independent_memory, iterations, array);
    auto time3 = measure_time(mixed_latency, iterations, array);
    auto time4 = measure_time(sequential_dependent, iterations, array);

    double diff1 = static_cast<double>(time4 - time1) / time4 * 100.0;
    double diff2 = static_cast<double>(time4 - time2) / time4 * 100.0;
    double diff3 = static_cast<double>(time4 - time3) / time4 * 100.0;

    zen::print("\n=== Performance Results ===\n");
    zen::print(std::format("{:<30} {:>8} {:<15} {:>10}\n", 
                                "Case Description", "Time (us)", "ROB Status", "% Diff"));
    zen::log(std::string(65, '-'));
    zen::print(std::format("{:<30} {:>8} {:<15} {:>9.1f}%\n", 
                                "1. Independent Arithmetic", time1, "Triggered", diff1));
    zen::print(std::format("{:<30} {:>8} {:<15} {:>9.1f}%\n", 
                                "2. Independent Memory", time2, "Triggered", diff2));
    zen::print(std::format("{:<30} {:>8} {:<15} {:>9.1f}%\n", 
                                "3. Mixed Latency", time3, "Triggered", diff3));
    zen::print(std::format("{:<30} {:>8} {:<15} {:>10}\n", 
                                "4. Sequential Dependent", time4, "Not Triggered", "-"));
    zen::log(std::string(65, '-'));


    return 0;
}