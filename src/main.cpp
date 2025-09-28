#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>

double memory_in_mb(size_t elements) {
    return (elements * sizeof(double)) / (1024.0 * 1024.0);
}

double compute_with_storage(double x, int depth) {
    std::vector<double> intermediates(depth + 1);
    intermediates[0] = x;
    for (int i = 1; i <= depth; i++) {
        intermediates[i] = intermediates[i - 1] * intermediates[i - 1] + 1.0;
    }
    std::cout << "Storage used: "
              << std::fixed << std::setprecision(2)
              << memory_in_mb(intermediates.size())
              << " MB\n";
    return intermediates[depth];
}

double compute_with_checkpoints(double x, int depth, int checkpoint_interval, double &mem_mb, double &time_s) {
    std::vector<double> checkpoints;
    checkpoints.reserve(depth / checkpoint_interval + 2);
    checkpoints.push_back(x);

    auto start = std::chrono::high_resolution_clock::now();

    int i = 0;
    while (i < depth) {
        double val = checkpoints.back();

        int steps = std::min(checkpoint_interval, depth - i);
        for (int j = 0; j < steps; j++) {
            val = val * val + 1.0;
        }
        checkpoints.push_back(val);
        i += steps;
    }

    auto end = std::chrono::high_resolution_clock::now();
    time_s = std::chrono::duration<double>(end - start).count();
    mem_mb = memory_in_mb(checkpoints.size());
    return checkpoints.back();
}

double compute_with_recompute(double x, int depth) {
    double val = x;
    for (int i = 0; i < depth; i++) {
        val = val * val + 1.0;
    }
    std::cout << "Storage used: ~0 MB (just scalars)\n";
    return val;
}

int main() {

    int depth = 2000000000;
    double x = 0.001;

    auto start = std::chrono::high_resolution_clock::now();
    double r1 = compute_with_storage(x, depth);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "With storage: " << r1 << " in "
              << std::chrono::duration<double>(end - start).count() << "s\n\n";

    std::cout << "K,Memory_MB,Time_s\n";

    // Sweep over different checkpoint intervals
    for (int K : {10, 50, 100, 500, 1000, 5000, 10000, 50000}) {
        double memory, time;
        double r = compute_with_checkpoints(x, depth, K, memory, time);
        std::cout << K << "," << memory << "," << time << "\n";
    }

    /*
    start = std::chrono::high_resolution_clock::now();
    double r2 = compute_with_recompute(x, depth);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "With recompute: " << r2 << " in "
              << std::chrono::duration<double>(end - start).count() << "s\n\n";*/
}

