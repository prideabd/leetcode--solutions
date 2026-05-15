#ifndef DATACRUNCHER_H
#define DATACRUNCHER_H
#include <fstream>
#include <sstream>
#include <type_traits>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstdlib>
#include <DataCruncher/MyVector.hpp>

namespace dc {
    // 模版DataCruncher
    template <typename T>
    class DataCruncher {
    public:
        // 1. 静态读取函数：负责获取数据
        static MyVector<T> readFromFile(const std::string& filename) {
            MyVector<T> result;
            // result.reserve(1000000); // 内存预留，优化读取
            // 1. 打开文件流
            std::ifstream inputFile(filename);

            // 2.检查文件是否成功打开
            if (!inputFile) {
                std::cerr << "错误: 无法打开文件" << filename << std::endl;
                return result;
            }

            //  3.读取循环
            std::string word;
            // 优化读取，from_chars在macos上无法处理double，采用strtod
            while (inputFile >> word) {
                char* endPtr = nullptr;
                // 转化字符串，
                double val = std::strtod(word.c_str(), &endPtr);
                // 检查是否完整, endPtr 应该指向字符串的末尾 (\0)
                if (endPtr != word.c_str() && *endPtr == '\0') {
                    result.push_back(static_cast<T>(val));
                } else {
                    std::cout << "警告：跳过非法数据 [" << word << "]" << std::endl;
                }
            }
            /*
            // 读取: file -> string -> stringstream -> double
            while (inputFile >> word) { // 1.先将其当作字符串单词读取
                std::stringstream ss(word);
                T tempValue;

                // 2.尝试将单词转为类型T
                if (ss >> tempValue && ss.eof()) {
                    result.push_back(tempValue);
                } else {
                    // std::streampos pos = inputFile.tellg(); // 获取当前读取位置
                    std::cout << "警告：跳过非法数据 [" << word << "]" << std::endl;
                }
            }*/
            inputFile.close();
            return result;
        }

        // 2. 静态处理函数：负责去重、排序等逻辑
        //    由于现在有double，那么直接使用unique会导致错误
        static MyVector<T> processData(const MyVector<T>& data) {
            if (data.empty()) {
                return data;
            }
            MyVector<T> sortedData = data;
            // 1.排序
            std::sort(sortedData.begin(), sortedData.end());
            // 2.去重
            MyVector<T> uniqueData;
            uniqueData.push_back(sortedData[0]);
            for (size_t i = 1; i < sortedData.size(); ++i) {
                if constexpr (std::is_floating_point<T>::value) {
                    if (std::abs(sortedData[i] - sortedData[i - 1]) > 1e-9) {
                        uniqueData.push_back(sortedData[i]);
                    }
                } else {
                    if (sortedData[i] != sortedData[i - 1]) {
                        uniqueData.push_back(sortedData[i]);
                    }
                }
            }
            return uniqueData;
        }

        // 3. 统计函数：计算平均值
        static double calculateAverage(const MyVector<T>& data){
            if (data.empty()) {
                return 0.0;
            }
            double sum = std::accumulate(data.begin(), data.end(), 0.0);
            return sum / data.size();
        }

        // 4. 统计函数：计算中位数
        static double calculateMedian(MyVector<T> data) { // 注意：这里用传值，因为排序会改动数据
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

        // 5. 计算标准差
        static double calculateStandardDeviation(const MyVector<T>& data) {
            // 1. 安全检查
            if (data.size() < 2) {
                return 0.0;
            }
            // 2.获取平均数
            double avg = calculateAverage(data);

            // 3.计算离差平方和
            double sumOfSquares = 0.0;
            for (size_t i = 0; i < data.size(); ++i) {
                double diff = data[i] - avg;
                sumOfSquares += diff * diff;
            }

            // 4.计算方差并开根号
            double variance = sumOfSquares / data.size();
            return std::sqrt(variance);
        }
        
        // 6. 计算直方图数据：将数据分成 numBins 个区间
        static std::pair<std::vector<double>, std::vector<double>> 
        getHistogram(const MyVector<T>& data, int numBins) {
            if (data.size() == 0) return {};

            T minVal = data[0]; // 假设已经排序
            T maxVal = data[data.size() - 1];
            double range = static_cast<double>(maxVal - minVal);
            double binWidth = range / numBins;

            std::vector<double> bins(numBins);
            std::vector<double> x_axis(numBins);

            for (size_t i = 0; i < data.size(); ++i) {
                int binIdx = static_cast<int>((data[i] - minVal) / binWidth);
                if (binIdx >= numBins) binIdx = numBins - 1;
                bins[binIdx]++;
            }

            for (int i = 0; i < numBins; ++i) {
                x_axis[i] = minVal + i * binWidth;
            }

            return {x_axis, bins};
        }
    };
}

#endif