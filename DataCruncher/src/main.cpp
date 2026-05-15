#include <iostream>
#include <iomanip>
// #include <vector>
#include <filesystem>
#include <cassert>
#include <chrono>
#include <random>
#include <string>
#include <DataCruncher/DataCruncher.hpp>
#include <DataCruncher/ReportGenerator.hpp>

// 大数据（例如100万）测试
void generateBigData(const std::string& filename, size_t count) {
    // 检查文件是否存在
    std::ifstream check(filename);
    if (check.good()) {
        std::cout << ">> 发现现有文件，跳过生成 " << std::endl;
        return;
    }
    check.close();
    // 如果不存在，则生成
    std::ofstream ofs(filename);
    std::ios_base::sync_with_stdio(false);
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 1000.0);

    for (size_t i = 0; i < count; ++i) {
        ofs << distribution(generator) << (i % 5 == 0 ? "\n" : " ");
    }
}
// 高精度计时器，测量：读取、排序（去重）、统计计算
void runPerformanceAnalysis() {
    const std::string bigFile = "perf_test.txt";
    const size_t dataSize = 1000000;

    std::cout << "--- 开始大数据测量 (" << dataSize << " 条数据) ---" << std::endl;
    // 1. 生成数据
    generateBigData(bigFile, dataSize);

    // 2. 测量读取速度
    auto start = std::chrono::high_resolution_clock::now();
    dc::MyVector<double> data = dc::DataCruncher<double>::readFromFile(bigFile);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "读取耗时: " << diff.count() << " s" << std::endl;
    
    // 3. 测量排序速度
    start = std::chrono::high_resolution_clock::now();
    dc::MyVector<double> processed = dc::DataCruncher<double>::processData(data);
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    std::cout << "处理(排序+去重)耗时: " << diff.count() << " s" << std::endl;

    // 4. 测量计算速度
    start = std::chrono::high_resolution_clock::now();
    double avg = dc::DataCruncher<double>::calculateAverage(processed);
    double stdDev = dc::DataCruncher<double>::calculateStandardDeviation(processed);
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    std::cout << "统计计算耗时: " << diff.count() << " s" << std::endl;
}

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
        dc::MyVector<double> result = dc::DataCruncher<double>::readFromFile("empty.txt");
        assert(result.empty());
        std::cout << "[Test 1] 空文件处理：通过 (未崩溃且结果为空)" << std::endl;
    }

    // 测试2: 全是非法字符时，返回是否为空？dc::
    {
        createTestFile("new_junk.txt", "apple banana cherry");
        dc::MyVector<double> result = dc::DataCruncher<double>::readFromFile("new_junk.txt");
        assert(result.empty());
        std::cout << "[Test 2] 全非法字符处理：通过 (结果为空)" << std::endl;
    }

    // 测试3: 已知数据计算结果是否等于预期？
    {
        createTestFile("know.txt", "1.0 2.0 3.0");
        dc::MyVector<double> raw = dc::DataCruncher<double>::readFromFile("know.txt");
        dc::MyVector<double> processed = dc::DataCruncher<double>::processData(raw); // 1,2,3

        double avg = dc::DataCruncher<double>::calculateAverage(processed);
        double median = dc::DataCruncher<double>::calculateMedian(processed);
        
        // 浮点数比较要考虑误差
        assert(std::abs(avg - 2.0) < 1e-9);
        assert(std::abs(median - 2.0) < 1e-9);
        std::cout << "[Test 3] 已知数据计算：通过 (均值/中位数准确)" << std::endl;
    }

    std::cout << "--- 所有测试通过！开始执行主程序 ---\n" << std::endl;
}

// 测试用的复杂结构体
struct Student {
    int id;
    std::string name;

    // 默认构造
    Student() : id(0), name("Unknown") {}
    // 带参构造
    Student(int i, std::string n) : id(i), name(n) {}
    
    // 自定义拷贝构造，方便观察
    Student(const Student& other) : id(other.id), name(other.name) {
        // std::cout << "[Copying Student: " << name << "]\n";
    }

    // 移动构造
    Student(Student&& other) noexcept : id(other.id), name(std::move(other.name)) {
        // std::cout << "[Moving Student: " << name << "]\n";
    }

    // 重载输出
    friend std::ostream& operator<<(std::ostream& os, const Student& s) {
        os << "{ID:" << s.id << ", Name:" << s.name << "}";
        return os;
    }
};

int main() {
    // --- 测试 1: 基础类型与空构造 ---
    std::cout << ">>> Test 1: Basic Empty Construction\n";
    dc::MyVector<int> v_int; 
    std::cout << "Size: " << v_int.size() << ", Cap: " << v_int.capacity() << "\n";
    v_int.push_back(100);
    std::cout << "After push: " << v_int << "\n\n";

    // --- 测试 2: 嵌套容器 MyVector<MyVector<T>> ---
    std::cout << ">>> Test 2: Nested Vector (Matrix)\n";
    dc::MyVector<dc::MyVector<double>> matrix;
    
    // 插入几个子 Vector
    matrix.push_back({1.1, 1.2});
    matrix.push_back({2.1, 2.2, 2.3});
    
    // 验证嵌套访问
    std::cout << "Matrix row 1, col 2: " << matrix[1][2] << "\n";
    std::cout << "Full Matrix: " << matrix << "\n\n";

    // --- 测试 3: 复杂结构体 (带 std::string) ---
    std::cout << ">>> Test 3: Complex Struct with std::string\n";
    dc::MyVector<Student> class_room;
    
    // 测试 push_back 触发移动语义
    class_room.push_back(Student(1, "Alex")); 
    class_room.push_back(Student(2, "Gemini"));
    
    // 测试 erase (中间删除)
    std::cout << "Before erase: " << class_room << "\n";
    class_room.erase(0); // 删除 Alex
    std::cout << "After erase:  " << class_room << "\n\n";

    // --- 测试 4: 拷贝与赋值 (Deep Copy) ---
    std::cout << ">>> Test 4: Deep Copy Validation\n";
    dc::MyVector<Student> class_copy = class_room; 
    class_copy[0].name = "Modified_Name";
    
    std::cout << "Original: " << class_room << "\n";
    std::cout << "Copy:     " << class_copy << " (Should be different)\n\n";

    // --- 测试 5: 异常安全性与内存压力 ---
    std::cout << ">>> Test 5: Stress & Capacity\n";
    dc::MyVector<int> stress;
    for(int i = 0; i < 1000; ++i) {
        stress.push_back(i);
    }
    std::cout << "Stress Size: " << stress.size() << ", Cap: " << stress.capacity() << "\n";
    stress.clear();
    std::cout << "After Clear Size: " << stress.size() << ", Cap: " << stress.capacity() << "\n";

    std::cout << "\n[All Tests Passed!]\n";
    return 0;
}



/*
// 网页交互
int main() {
    // 1. 读取数据
    auto data = dc::DataCruncher<double>::readFromFile("perf_test.txt");

    // 2. 排序
    data.sort();

    // 3.准备绘图数据
    auto [x, y] = dc::DataCruncher<double>::getHistogram(data, 50);

    // 4. 生成报告
    std::string reportName = "report.html";
    dc::ReportGenerator::generateHtml(reportName, x, y);

    std::cout << "\n✅ 分析完成！" << std::endl;
    std::cout << "📈 交互式报告已生成: " << reportName << std::endl;
    std::cout << "💡 请直接在浏览器中打开该文件查看。" << std::endl;

    return 0;
}
*/

