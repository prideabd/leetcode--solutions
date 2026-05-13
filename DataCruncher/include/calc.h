#ifndef CALC_H
#define CALC_H

#include <vector>

// 声明: 接受一个数组，返回一个排好序且去重的数组
std::vector<int> processData(std::vector<int> input);

// 计算平均值
double calculateAverage(const std::vector<int>& data);

#endif