#ifndef CALC_H
#define CALC_H

#include "MyVector.h"

// 声明: 接受一个数组，返回一个排好序且去重的数组
MyVector<int> processData(const MyVector<int>& data);

// 计算平均值
double calculateAverage(const MyVector<int>& data);

// 计算中位数
double calculateMedian(MyVector<int> data);

#endif