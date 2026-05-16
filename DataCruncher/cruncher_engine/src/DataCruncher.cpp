// #include "DataCruncher.h"
// #include <fstream>
// #include <iostream>
// #include <algorithm>
// #include <numeric>
// #include <cmath>

// MyVector<int> DataCruncher::readFromFile(const std::string& filename) {
//     MyVector<int> result;
//     // 1. 打开文件流
//     std::ifstream inputFile(filename);

//     // 2.检查文件是否成功打开
//     if (!inputFile) {
//         std::cerr << "错误: 无法打开文件" << filename << std::endl;
//         return result;
//     }

//     //  3.读取循环
//     int tempValue;
//     while (true) {
//         if (inputFile >> tempValue) {
//             result.push_back(tempValue);
//         } else if (inputFile.eof()) {
//             //读取到文件的末尾，退出
//             break;
//         } else {
//             // 遇到无法解析为整数的内容
//             inputFile.clear(); // 重置流状态，否则流会不再读取任何东西
//             std::string junk;
//             inputFile >> junk; // 将坏内容读取到junk中进行处理
//             std::cerr << "警告：跳过非法数据 [" << junk << "]" << std::endl;
//             // 循环继续，读取下一个数据
//         }
//     }
//     inputFile.close();
//     return result;
// }

// MyVector<int> DataCruncher::processData(const MyVector<int>& data) {
//     MyVector<int> input = data;
//     // 1.排序
//     std::sort(input.begin(), input.end());
//     // 2.去重
//     auto it = std::unique(input.begin(), input.end());
//     input.erase(it, input.end());
//     return input;
// }

// double DataCruncher::calculateAverage(const MyVector<int>& data) {
//     if (data.empty()) {
//         return 0.0;
//     }
//     double sum = std::accumulate(data.begin(), data.end(), 0.0);
//     return sum / data.size();
// }

// double DataCruncher::calculateMedian(MyVector<int> data) {
//     if (data.empty()) {
//         return 0.0;
//     }
//     std::sort(data.begin(), data.end());
//     size_t size = data.size();
//     if (size % 2 == 0) {
//         return (data[size / 2 - 1] + data[size / 2]) / 2.0;
//     } else {
//         return data[size / 2];
//     }
// }

// double DataCruncher::calculateStandardDeviation(const MyVector<int>& data) {
//     // 1. 安全检查
//     if (data.size() < 2) {
//         return 0.0;
//     }
//     // 2.获取平均数
//     double avg = calculateAverage(data);

//     // 3.计算离差平方和
//     double sumOfSquares = 0.0;
//     for (size_t i = 0; i < data.size(); ++i) {
//         double diff = data[i] - avg;
//         sumOfSquares += diff * diff;
//     }

//     // 4.计算方差并开根号
//     double variance = sumOfSquares / data.size();
//     return std::sqrt(variance);
// }

// template class DataCruncher<int>;
// template class DataCruncher<double>;