#include "calc.h"
#include <algorithm>
#include <numeric>

std::vector<int> processData(std::vector<int> input) {
    // 1.排序
    std::sort(input.begin(), input.end());
    // 2.去重
    auto it = std::unique(input.begin(), input.end());
    input.erase(it, input.end());
    return input;
}

double calculateAverage(const std::vector<int>& data) {
    if (data.empty()) {
        return 0.0;
    }
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}