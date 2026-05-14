#include "calc.h"
#include <algorithm>
#include <numeric>

MyVector<int> processData(const MyVector<int>& data) {
    MyVector<int> input = data;
    // 1.排序
    std::sort(input.begin(), input.end());
    // 2.去重
    auto it = std::unique(input.begin(), input.end());
    input.erase(it, input.end());
    return input;
}

double calculateAverage(const MyVector<int>& data) {
    if (data.empty()) {
        return 0.0;
    }
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}

double calculateMedian(MyVector<int> data) {
    if (data.empty()) {
        return 0.0;
    }
    std::sort(data.begin(), data.end());
    size_t size = data.size();
    if (size % 2 == 0) {
        return (data[size / 2 - 1] + data[size / 2]) / 2.0;
    } else {
        return data[size / 2];
    }
}