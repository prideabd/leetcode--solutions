#include <iostream>
#include <vector>
#include "calc.h"

int main() {
    // 模拟一组杂乱的原始数据
    MyVector<int> rawData = {9, 1, 5, 2, 9, 3, 1, 10, 5};
    // MyVector<int> rawData = {1, 2, 3, 4};
    
    std::cout << "DataCruncher 启动..." << std::endl;
    
    // 调用引擎处理数据
    MyVector<int> cleanData = processData(rawData);
    
    std::cout << "原始数据大小: " << rawData.size() << std::endl;
    std::cout << "原始数据: " << rawData << std::endl;
    std::cout << "处理后数据: " << cleanData << std::endl;
    // std::cout << "处理后数据: ";
    // for (int n : cleanData) {
    //     std::cout << n << " ";
    // }
    // std::cout << std::endl;
    
    std::cout << "平均值: " << calculateAverage(cleanData) << std::endl;
    std::cout << "中位数: " << calculateMedian(cleanData) << std::endl;
    
    return 0;
}