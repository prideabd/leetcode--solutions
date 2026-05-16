#include <iostream>
#include <iomanip>
// #include <vector>
#include <filesystem>
#include <cassert>
#include <chrono>
#include <random>
#include <string>
#include "DataCruncher.hpp"
#include "ReportGenerator.hpp"

// 测试用的复杂结构体
struct Student {
    int id;
    std::string name;

    // 默认构造
    Student() : id(0), name("Unknown") {
        std::cout << "[Student] 默认构造: " << name << " (" << this << ")\n";
    }
    // 带参构造
    Student(int i, std::string n) : id(i), name(n) {
        std::cout << "[Student] 构造: " << name << " (" << this << ")\n";
    }
    
    // 自定义拷贝构造，方便观察
    Student(const Student& other) : id(other.id), name(other.name) {
        std::cout << "[Student] 拷贝构造: " << name << " (" << this << ")\n";
    }

    // 移动构造
    Student(Student&& other) noexcept : id(other.id), name(std::move(other.name)) {
        std::cout << "[Student] 移动构造: " << name << " (" << this << ")\n";
    }

    // 析构函数（新增：用来验证 destroy_elements 是否正确工作）
    ~Student() {
        std::cout << "[Student] 析构销毁: " << name << " (" << this << ")\n";
    }

    // 重载输出
    friend std::ostream& operator<<(std::ostream& os, const Student& s) {
        os << "{ID:" << s.id << ", Name:" << s.name << "}";
        return os;
    }
};

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

// --- 测试 1: 基础类型与空构造 ---
void test_basic_construction() {
    std::cout << ">>> Test 1: Basic Empty Construction\n";
    dc::MyVector<int> v_int; 
    std::cout << "Size: " << v_int.size() << ", Cap: " << v_int.capacity() << "\n";
    v_int.push_back(100);
    std::cout << "After push: " << v_int << "\n\n";
}

// --- 测试 2: 嵌套容器 MyVector<MyVector<T>> ---
void test_nested_vector() {
    std::cout << ">>> Test 2: Nested Vector (Matrix)\n";
    dc::MyVector<dc::MyVector<double>> matrix;
    
    matrix.push_back({1.1, 1.2});
    matrix.push_back({2.1, 2.2, 2.3});
    
    std::cout << "Matrix row 1, col 2: " << matrix[1][2] << "\n";
    std::cout << "Full Matrix: " << matrix << "\n\n";
}

// --- 测试 3: 复杂结构体 (带 std::string) & erase ---
void test_complex_struct_and_erase() {
    std::cout << ">>> Test 3: Complex Struct with std::string\n";
    dc::MyVector<Student> class_room;
    
    class_room.push_back(Student(1, "Alex")); 
    class_room.push_back(Student(2, "Gemini"));
    
    std::cout << "Before erase: " << class_room << "\n";
    class_room.erase(0); 
    std::cout << "After erase:  " << class_room << "\n\n";
}

// --- 测试 4: 拷贝与赋值 (Deep Copy) ---
void test_deep_copy() {
    std::cout << ">>> Test 4: Deep Copy Validation\n";
    dc::MyVector<Student> class_room;
    class_room.push_back(Student(2, "Gemini")); // 准备一个基础数据
    
    dc::MyVector<Student> class_copy = class_room; 
    class_copy[0].name = "Modified_Name";
    
    std::cout << "Original: " << class_room << "\n";
    std::cout << "Copy:     " << class_copy << " (Should be different)\n\n";
}

// --- 测试 5: 异常安全性与内存压力 ---
void test_stress_and_clear() {
    std::cout << ">>> Test 5: Stress & Capacity\n";
    dc::MyVector<int> stress;
    for(int i = 0; i < 1000; ++i) {
        stress.push_back(i);
    }
    std::cout << "Stress Size: " << stress.size() << ", Cap: " << stress.capacity() << "\n";
    stress.clear();
    std::cout << "After Clear Size: " << stress.size() << ", Cap: " << stress.capacity() << "\n\n";
}

// --- 测试 6: resize() 核心逻辑 ---
void test_resize() {
    std::cout << "========================================\n";
    std::cout << ">>> 开始测试: resize() \n";
    std::cout << "========================================\n";

    dc::MyVector<Student> vec;
    std::cout << "1. 初始状态 -> Size: " << vec.size() << ", Cap: " << vec.capacity() << "\n\n";

    std::cout << "--- 产生一个原型对象 fill_stu ---\n";
    Student fill_stu(99, "Backup"); 
    std::cout << "--------------------------------\n";
    
    std::cout << "\n[动作] vec.resize(3, fill_stu) -> 扩容并用原型拷贝填充\n";
    vec.resize(3, fill_stu);
    std::cout << "结果 -> Size: " << vec.size() << ", Cap: " << vec.capacity() << "\n";
    std::cout << "内容: " << vec << "\n\n";

    std::cout << "[动作] vec.resize(5) -> 进一步扩容，使用默认构造填充\n";
    vec.resize(5);
    std::cout << "结果 -> Size: " << vec.size() << ", Cap: " << vec.capacity() << "\n";
    std::cout << "内容: " << vec << "\n\n";

    std::cout << "[动作] vec.resize(2) -> 尺寸截断到 2（应该触发 3 次析构，但 Cap 不变）\n";
    vec.resize(2);
    std::cout << "结果 -> Size: " << vec.size() << ", Cap: " << vec.capacity() << "\n";
    std::cout << "内容: " << vec << "\n\n";
}

// --- 测试 7: shrink_to_fit() 瘦身逻辑 ---
void test_shrink_to_fit() {
    std::cout << "========================================\n";
    std::cout << ">>> 开始测试: shrink_to_fit() \n";
    std::cout << "========================================\n";

    // 为了让 shrink_to_fit 有效，我们先构造一个有闲置空间的状态
    dc::MyVector<Student> vec;
    vec.reserve(5);
    vec.push_back(Student(10, "Stay1"));
    vec.push_back(Student(20, "Stay2"));

    std::cout << "1. 压缩前状态 -> Size: " << vec.size() << ", Cap: " << vec.capacity() << "\n";
    
    if (vec.capacity() > vec.size()) {
        std::cout << "\n[动作] vec.shrink_to_fit() -> 释放闲置内存，压缩地皮\n";
        std::cout << "(这会触发有效对象的重新搬迁与老家析构)\n";
        std::cout << "-------------------------------------\n";
        vec.shrink_to_fit();
        std::cout << "-------------------------------------\n";
        std::cout << "压缩后状态 -> Size: " << vec.size() << ", Cap: " << vec.capacity() << "\n";
        std::cout << "内容: " << vec << "\n\n";
    } else {
        std::cout << "当前无闲置内存，无需压缩。\n\n";
    }
}

// --- 测试 8: emplace_back 对比 push_back ---
void test_emplace_vs_push() {
    std::cout << "========================================\n";
    std::cout << ">>> 开始测试: push_back vs emplace_back \n";
    std::cout << "========================================\n";

    dc::MyVector<Student> vec;
    vec.reserve(4); 

    std::cout << "\n[测试] 使用传统 push_back 插入:\n";
    std::cout << "------------------------------------\n";
    vec.push_back(Student(1, "Push_Way"));
    std::cout << "------------------------------------\n";

    std::cout << "\n[测试] 使用现代 emplace_back 插入:\n";
    std::cout << "------------------------------------\n";
    vec.emplace_back(2, "Emplace_Way");
    std::cout << "------------------------------------\n";

    std::cout << "\n内容: " << vec << "\n\n";
}

int main() {
    // runPerformanceAnalysis(); // 高性能检测
    // runAutomatedTests(); // 冒烟测试
    std::cout << "========================================\n";
    std::cout << ">>> 启动运行 MyVector 自动化测试集 <<<\n";
    std::cout << "========================================\n\n";

    // 💡 提示：在 VS Code 中，把光标停留在不需要的测试行上，按 Ctrl+/ 即可秒开/秒关！
    
    // test_basic_construction();      // 测试 1
    // test_nested_vector();           // 测试 2
    // test_complex_struct_and_erase(); // 测试 3
    // test_deep_copy();               // 测试 4
    // test_stress_and_clear();        // 测试 5
    // test_resize();                  // 测试 6
    // test_shrink_to_fit();           // 测试 7
    
    test_emplace_vs_push();            // 测试 8（当前专注于测试它）

    std::cout << "========================================\n";
    std::cout << ">>> 所有的前台测试项执行完毕 <<<\n";
    std::cout << "========================================\n";
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

