#include <iostream>
#include <iomanip>
// #include <vector>
#include <filesystem>
#include <cassert>
#include "DataCruncher.h"

// 冒烟测试
// 辅助函数: 创建一个临时测试文件
void createTestFile(const std::string& name, const std::string& content) {
    std::ofstream ofs(name, std::ios::out | std::ios::trunc);
    if (ofs) {
        ofs << content;
        ofs.close();
    }
}

void runAutomatedTests() {
    std::cout << "--- 启动自动化测试序列 ---" << std::endl;

    // 测试1: 读取空文件是否崩溃？
    {
        createTestFile("empty.txt", "");
        MyVector<double> result = DataCruncher<double>::readFromFile("empty.txt");
        assert(result.empty());
        std::cout << "[Test 1] 空文件处理：通过 (未崩溃且结果为空)" << std::endl;
    }

    // 测试2: 全是非法字符时，返回是否为空？
    {
        createTestFile("new_junk.txt", "apple banana cherry");
        MyVector<double> result = DataCruncher<double>::readFromFile("new_junk.txt");
        assert(result.empty());
        std::cout << "[Test 2] 全非法字符处理：通过 (结果为空)" << std::endl;
    }

    // 测试3: 已知数据计算结果是否等于预期？
    {
        createTestFile("know.txt", "1.0 2.0 3.0");
        MyVector<double> raw = DataCruncher<double>::readFromFile("know.txt");
        MyVector<double> processed = DataCruncher<double>::processData(raw); // 1,2,3

        double avg = DataCruncher<double>::calculateAverage(processed);
        double median = DataCruncher<double>::calculateMedian(processed);
        
        // 浮点数比较要考虑误差
        assert(std::abs(avg - 2.0) < 1e-9);
        assert(std::abs(median - 2.0) < 1e-9);
        std::cout << "[Test 3] 已知数据计算：通过 (均值/中位数准确)" << std::endl;
    }

    std::cout << "--- 所有测试通过！开始执行主程序 ---\n" << std::endl;
}

int main() {

    runAutomatedTests();
    std::cout << "当前程序运行的实际路径是: " << std::filesystem::current_path() << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "       DataCruncher 分析系统 v1.0       " << std::endl;
    std::cout << "========================================" << std::endl;

    // 1. 读取数据 (使用我们之前调好的相对路径)
    std::string filePath = "../DataCruncher/data.txt";
    MyVector<double> rawData = DataCruncher<double>::readFromFile(filePath);

    if (rawData.empty()) {
        std::cerr << "[错误] 未能读取到有效数据，请检查路径或文件内容。" << std::endl;
        return 1;
    }

    // 2. 数据预处理 (去重并排序)
    MyVector<double> processedData = DataCruncher<double>::processData(rawData);

    // 3. 计算各项指标
    double avg = DataCruncher<double>::calculateAverage(processedData);
    double median = DataCruncher<double>::calculateMedian(processedData);
    double stdDev = DataCruncher<double>::calculateStandardDeviation(processedData);

    // 4. 格式化输出报告
    std::cout << std::fixed << std::setprecision(10); // 提高精度显示，观察微小差异
    std::cout << "原始数据量: " << rawData.size() << std::endl;
    std::cout << "去重后数量: " << processedData.size() << " (验证是否成功合并了微小差异值)" << std::endl;
    
    std::cout << "--------------------------------" << std::endl;
    std::cout << "平均值: " << avg << std::endl;
    std::cout << "标准差: " << stdDev << std::endl;
    
    std::cout << ">> 清洗后数据预览:" << std::endl;
    for (size_t i = 0; i < processedData.size(); ++i) {
        std::cout << processedData[i] << std::endl;
    }

    std::cout << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}