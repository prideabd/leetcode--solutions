#include <iostream>
#include <vector>
#include "calc.h"

int main() {
    // 模拟一组杂乱的原始数据
    std::vector<int> rawData = {9, 1, 5, 2, 9, 3, 1, 10, 5};
    
    std::cout << "DataCruncher 启动..." << std::endl;
    
    // 调用引擎处理数据
    std::vector<int> cleanData = processData(rawData);
    
    std::cout << "原始数据大小: " << rawData.size() << std::endl;
    std::cout << "处理后数据: ";
    for (int n : cleanData) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    
    std::cout << "平均值: " << calculateAverage(cleanData) << std::endl;
    
    return 0;
}